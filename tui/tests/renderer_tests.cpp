#include "../headers/input_handler.hpp"
#include <cstdio>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>

int main() {
    Renderer::Screen screen;
    Structure st;

    // --- TextBox (top-left)
    TextBox textbox1(screen,
                     37,  // row
                     2,   // col
                     3,   // height
                     140, // width
                     MAGENTA);

    // --- Table (below textbox)
    Table table1(screen,
                 2,      // col
                 2,      // row
                 140,    // width
                 38 - 3, // height
                 3, 3, RED);

    // --- Accordion (right side)
    Accordion acc1(screen,
                   2 + 140 + 2, // col
                   2,           // row
                   43,          // width
                   38,          // height
                   2, RED);

    table1.fill_headers("header 1", 0);
    table1.fill_headers("header 2", 1);
    table1.fill_headers("header 3", 2);
    table1.fill_rows("sdf", 0, 0);
    table1.fill_rows("abc", 0, 1);
    table1.fill_rows("iqs", 1, 0);
    table1.fill_rows("prq", 1, 1);
    acc1.fill_entry("schema1", 0);
    acc1.fill_entry("schema2", 1);
    acc1.fill_childs("table1", 0);
    acc1.fill_childs("table2", 0);
    acc1.fill_childs("table3", 0);
    acc1.fill_childs("table1", 1);
    acc1.fill_childs("table2", 1);
    acc1.fill_childs("table3", 1);

    // acc1.expand(0, true);

    st.screen_components.push_back(std::make_unique<Table>(table1));
    st.screen_components.push_back(std::make_unique<TextBox>(textbox1));
    st.screen_components.push_back(std::make_unique<Accordion>(acc1));

    InputHandlers::Stdin_Handler input_handler(st);

    char in;
    write(STDOUT_FILENO, "\033[?25l", 6);
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
    write(STDOUT_FILENO, std::string(st.screen_components.size(), 1).data(), 1);
    while (1) {

        screen.reset_region(1, 1, screen.max_cols - 1, screen.max_rows - 1, {});

        int bytes_read = read(STDIN_FILENO, &in, 1);

        if (bytes_read <= 0)
            continue;

        if (in == 'q')
            break;

        input_handler.read(in);

        st.draw_structure();
        write(STDOUT_FILENO, "\033[H", 3);
        screen.Render();
    }
    write(STDOUT_FILENO, "\033[?25h", 6);
}
