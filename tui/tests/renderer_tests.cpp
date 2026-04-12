#include "../headers/components.hpp"
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>

int main() {
    Renderer::Screen screen;

    /* Base_Element::Box box1(1, 1, 20, 20, RED, BOX);
    Base_Element::Text text1("hello", 1, 1, RED, TEXT);
    box1.draw(screen);
    text1.draw(screen); */

    TextBox textbox1(screen, 1, 1, 3, 25, RED);

    /* while (1) {
        // // This loop should recieve a exisiting strcture/layout
        // input_handler (collects stream from stdin and changes structure of given layout, takes structure (elements, components) as input)
        // re-draw current layout
        // render()
    } */

    char in;
    write(STDOUT_FILENO, "\033[?25l", 6);
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
    while (1) {
        int bytes_read = read(STDIN_FILENO, &in, 1);

        if (bytes_read <= 0)
            continue;

        if (in == 'q')
            break;

        // if (!isprint(in))
        //     continue;

        // only rendering if input handler emits
        textbox1.set_inner_text(in);

        textbox1.draw();
        write(STDOUT_FILENO, "\033[H", 3);
        usleep(16000);
        screen.Render();
    }
    write(STDOUT_FILENO, "\033[?25h", 6);
}
