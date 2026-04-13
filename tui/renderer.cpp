#include "headers/renderer.hpp"

void Renderer::Screen::Render() {
    int stdout_no = fileno(stdout);
    size_t out_buff_size = (resolution.ws_row * resolution.ws_col) + 512;
    std::string out_buff;
    out_buff.reserve(out_buff_size);

    // later this can recieve max height to render, instead of doing full available height
    for (int r = 0; r < resolution.ws_row; r++) {
        for (int c = 0; c < resolution.ws_col; c++) {
            Cell cell = buffer[(resolution.ws_col * r) + c];
            struct Style style = cell.cell_style;

            // set common properties
            if (style.color == RED) {
                out_buff += "\033[31m";
            } else if (style.color == GREEN) {
                out_buff += "\033[32m";
            } else if (style.color == BLUE) {
                out_buff += "\033[33m";
            }

            // set unique properties
            if (style.comp == TEXT) {
                if (style.unique_prop.text.is_bold)
                    out_buff += "\033[1m";
            } else if (style.comp == BOX) {
                if (style.unique_prop.box.is_bold) {
                    out_buff += "\033[1m";
                }
            }
            out_buff += cell.character;
            out_buff += "\033[0m";
        }
        if (r != resolution.ws_row - 1) {
            out_buff += "\r\n";
        } else {
            out_buff += "\r";
        }
    }

    size_t byte_written = 0;
    while (byte_written < out_buff.size()) {
        size_t written = write(stdout_no, out_buff.data() + byte_written, out_buff.size() - byte_written);

        if (written == -1) {
            throw std::runtime_error("ERROR WHILE WRITING TO STDOUT");
        }

        byte_written += written;
    }
}
