#include "src/catalog_manager/headers/schmea_manager.hpp"
#include "src/query_manager/headers/parser.hpp"
#include "src/query_manager/headers/planner.hpp"
#include "src/storage_manager/headers/index_writer.hpp"
#include "tui/headers/components.hpp"
#include "tui/headers/input_handler.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <poll.h>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <termios.h>
#include <type_traits>
#include <unistd.h>
#include <variant>
#include <vector>

enum RESPONSE_TYPE { FIRST_LOAD, SELECT_QUERY, INSERT_QUERY, CREATE_TABLE_QUERY, CREATE_SCHEMA_QUERY };
struct Request {
    std::string text_box_input;
};

struct Response {
    RESPONSE_TYPE query_type;
    std::optional<std::vector<access_methods_types::row_t>> results;
    std::optional<std::vector<schema::schema_attr>> schemas;
};

Response DB_Pipeline(std::string schema_name, schema::schema_manager &sch_ma, parser::Parser &parser,
                     buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods, Request &input,
                     bool first_load) {

    Response response_obj = {};
    index_write::root_struct curr_root = {};

    if (first_load) {
        response_obj.query_type = FIRST_LOAD;
        response_obj.schemas.emplace();
        sch_ma.get_schema(response_obj.schemas.value());
        return response_obj;
    }

    parser::token_iterator tok_it(input.text_box_input);
    parser_types::ASTResult ast = parser.grammer_check(tok_it, sch_ma, schema_name);

    if (parser_types::SCHEMA_AST *sch_ptr = std::get_if<parser_types::SCHEMA_AST>(&ast)) {
        sch_ma.create_schema(*sch_ptr);
        response_obj.query_type = CREATE_SCHEMA_QUERY;
        response_obj.schemas.emplace();
        sch_ma.get_schema(response_obj.schemas.value());

    } else if (parser_types::CREATE_TABLE_AST *ct_ast = std::get_if<parser_types::CREATE_TABLE_AST>(&ast)) {
        if (schema_name != "") {
            sch_ma.schema_create_table(schema_name, *ct_ast);
            response_obj.query_type = CREATE_TABLE_QUERY;
            response_obj.schemas.emplace();
            sch_ma.get_schema(response_obj.schemas.value());
        }
    } else if (parser_types::INSERT_AST *insert_ast = std::get_if<parser_types::INSERT_AST>(&ast)) {
        if (schema_name != "") {
            planner::insert_plan(buff_pool, access_methods, sch_ma, *insert_ast, curr_root, schema_name);
            response_obj.query_type = INSERT_QUERY;
        }

    } else if (parser_types::SELECT_AST *select_ast = std::get_if<parser_types::SELECT_AST>(&ast)) {
        if (schema_name != "") {
            response_obj.query_type = SELECT_QUERY;
            response_obj.results = planner::select_plan(buff_pool, access_methods, sch_ma, *select_ast, schema_name);
            response_obj.schemas.emplace();
            sch_ma.get_schema(response_obj.schemas.value());
        }
    }

    return response_obj;
}

void TUI_Pipeline(schema::schema_manager &sch_ma, parser::Parser &parser, buffer_manager::buffer_pool &buff_pool,
                  access_methods::Access_methods &access_methods, Structure &st, bool first_load) {

    Request input;
    input = Request{};
    // input.text_box_input = input_text;

    for (std::unique_ptr<TextBox> &textbox : st.textbox) {
        if (textbox->component_id == "query_input") {
            input.text_box_input = textbox->get_inner_text();
            break;
        }
    }

    std::string curr_active_schema = "";
    for (std::unique_ptr<Accordion> &ac : st.accordion) {
        if (ac->component_id == "schema_display" && ac->entry.size() != 0) {
            curr_active_schema = ac->entry[ac->which_selected].name;
            break;
        }
    }

    Response res = DB_Pipeline(curr_active_schema, sch_ma, parser, buff_pool, access_methods, input, first_load);
    int accord_idx = 0;
    if (res.schemas.has_value()) {
        for (accord_idx = 0; accord_idx < st.accordion.size(); accord_idx++) {
            if (st.accordion[accord_idx]->component_id == "schema_display") {
                for (int i = 0; i < res.schemas.value().size(); i++) {
                    st.accordion[accord_idx]->fill_entry(res.schemas.value()[i].schema_name, i);
                    for (int j = 0; j < res.schemas.value()[i].tables.size(); j++) {
                        st.accordion[accord_idx]->fill_childs(res.schemas.value()[i].tables[j].table_name, i);
                    }
                }
                break;
            }
        }
    }

    if (res.results.has_value()) {
        int table_idx = 0;
        for (int i = 0; i < st.table.size(); i++) {
            if (st.table[i]->component_id == "display_table") {
                st.table[i]->divisions = res.results.value().size();
                table_idx = i;
                break;
            }
        }
        int parent_selected = st.accordion[accord_idx]->which_selected;
        int child_selected = st.accordion[accord_idx]->entry[parent_selected].sub_child_selected;

        // remember to resize according to res.results (number of col) after setting divisions
        st.table[table_idx]->divisions = res.schemas.value()[parent_selected].tables[child_selected].columns.size();
        int num_cols = res.schemas.value()[parent_selected].tables[child_selected].columns.size();
        st.table[table_idx]->rows.assign(res.results.value().size(), std::vector<std::string>(num_cols, " "));
        st.table[table_idx]->headers.assign(num_cols, " ");

        int h = 0;

        for (h = 0; h < res.schemas.value()[parent_selected].tables[child_selected].columns.size(); h++) {
            st.table[table_idx]->fill_headers(res.schemas.value()[parent_selected].tables[child_selected].columns[h].column_name, h);
        }

        for (int r = 0; r < res.results.value().size(); r++) {
            for (int v = 0; v < res.results.value()[r].row.size(); v++) {
                std::visit(
                        [&](auto &&val) {
                            using T = std::decay_t<decltype(val)>;

                            if constexpr (std::is_same_v<int, T>) {
                                st.table[table_idx]->fill_rows(std::to_string(val), r, v);
                            } else if constexpr (std::is_same_v<std::string, T>) {
                                st.table[table_idx]->fill_rows(val, r, v);
                            } else if constexpr (std::is_same_v<float, T>) {
                                st.table[table_idx]->fill_rows(std::to_string(val), r, v);
                            }
                        },
                        (res.results.value()[r].row[v]));
            }
        }
    }
}

int main() {
    std::filesystem::path heap_filepath = std::filesystem::current_path() / "heap.bin";
    std::filesystem::path index_filepath = std::filesystem::current_path() / "index.bin";
    std::filesystem::path schema_filepath = std::filesystem::current_path() / "schema_file.bin";

    std::ofstream file1(index_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file2(heap_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file3(schema_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    file1.close();
    file2.close();
    file3.close();

    Renderer::Screen screen;
    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);
    access_methods::Access_methods access_methods;
    schema::schema_manager sch_ma(schema_filepath);
    parser::Parser parser;

    // --- TextBox (top-left)
    TextBox textbox1(screen,
                     37,  // row
                     2,   // col
                     3,   // height
                     140, // width
                     MAGENTA, "query_input");

    // --- Table (below textbox)
    Table table1(screen,
                 2,      // col
                 2,      // row
                 140,    // width
                 38 - 3, // height
                 1, 3, RED, "display_table");

    // --- Accordion (right side)
    Accordion acc1(screen,
                   2 + 140 + 2, // col
                   2,           // row
                   43,          // width
                   38,          // height
                   3, RED, "schema_display");
    InputHandlers::Events events = {};

    Structure st;
    st.table.push_back(std::make_unique<Table>(table1));
    st.components.push_back(st.table.back().get());

    st.textbox.push_back(std::make_unique<TextBox>(textbox1));
    st.components.push_back(st.textbox.back().get());

    st.accordion.push_back(std::make_unique<Accordion>(acc1));
    st.components.push_back(st.accordion.back().get());

    InputHandlers::Stdin_Handler input_handler(st);

    char in;
    write(STDOUT_FILENO, "\033[?25l", 6);
    write(STDOUT_FILENO, "\033[2J\033[H", 7);

    TUI_Pipeline(sch_ma, parser, buff_pool, access_methods, st, true);
    struct pollfd stdin_poll = {STDIN_FILENO, POLLIN, 0};

    // can later have more (note this contains pointers to structs of pollfd)
    size_t nfds = 1;
    struct pollfd poll_table[nfds];
    poll_table[0] = stdin_poll;

    while (1) {
        int ready_fds = poll(poll_table, nfds, 16);

        if (ready_fds > 0) {
            for (int fd = 0; fd < nfds; fd++) {
                if (poll_table[fd].revents & POLLIN) {

                    screen.reset_region(1, 1, screen.max_cols - 1, screen.max_rows - 1, {});

                    int bytes_read = read(STDIN_FILENO, &in, 1);
                    if (bytes_read <= 0)
                        continue;

                    if (in == 3)
                        return 0;

                    input_handler.read(in, events);

                    if (events.SUBMIT) {
                        TUI_Pipeline(sch_ma, parser, buff_pool, access_methods, st, false);
                    }
                }
            }
        } else if (ready_fds < 0) {
            throw std::runtime_error("SOME ERROR IN MAIN LOOP");
        }

        st.draw_structure();
        write(STDOUT_FILENO, "\033[H", 3);
        screen.Render();

        events = {};
    }
    write(STDOUT_FILENO, "\033[?25h", 6);
    return 0;
}
