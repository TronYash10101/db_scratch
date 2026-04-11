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

    TextBox textbox1(screen, 1, 1, 3, 40, RED);

    /* while (1) {
        // // This loop should recieve a exisiting strcture/layout
        // input_handler (collects stream from stdin and changes structure of given layout, takes structure (elements, components) as input)
        // re-draw current layout
        // render()
    } */

    std::string in;
    in.resize(50);
    while (1) {
        int bytes_read = read(STDIN_FILENO, in.data(), in.size());

        if (*in.data() == 'q') {
            break;
        }
        textbox1.set_inner_text(in);

        textbox1.draw();
        screen.Render();
    }
}
