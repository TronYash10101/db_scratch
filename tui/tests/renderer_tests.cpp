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

    TextBox textbox1(screen, 40, 1, 3, 185, BLUE);
    Table table1(screen, 1, 1, 185, 39, 3, 3, RED);

    table1.fill_headers("header 1", 0);
    table1.fill_headers("header 2", 1);
    table1.fill_headers("header 3", 2);
    table1.fill_rows("sdf", 0, 0);
    table1.fill_rows("abc", 0, 1);

    st.screen_components.push_back(std::make_unique<Table>(table1));
    st.screen_components.push_back(std::make_unique<TextBox>(textbox1));

    InputHandlers::Stdin_Handler input_handler(st);

    char in;
    write(STDOUT_FILENO, "\033[?25l", 6);
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
    while (1) {
        int bytes_read = read(STDIN_FILENO, &in, 1);

        if (bytes_read <= 0)
            continue;

        if (in == 'q')
            break;

        input_handler.read(in);

        // only rendering if input handler emits

        st.draw_structure();
        write(STDOUT_FILENO, "\033[H", 3);
        screen.Render();
    }
    write(STDOUT_FILENO, "\033[?25h", 6);
}
