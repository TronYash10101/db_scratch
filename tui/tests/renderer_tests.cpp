#include "../headers/renderer.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>

static termios original_term_setting;
enum CLICK { LEFT, RIGHT, MIDDLE };
enum MOUSE_STATUS { CLICKED, RELEASED };
struct mouseEvent {
    int x;
    int y;
    MOUSE_STATUS mouse_status;
    CLICK which_click;
};

bool set_raw_tty(int stdout_no) {

    struct termios raw_term_setting;

    if (tcgetattr(stdout_no, &original_term_setting) == -1) {
        throw std::runtime_error("ERROR GETTING ORIGINAL TERMINAL SETTING");
    }

    raw_term_setting.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw_term_setting.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
    // term_setting.c_oflag &= ~(OPOST);
    // term_setting.c_ospeed

    std::string enable_mouse = "\033[?1000h\033[?1006h";
    write(stdout_no, enable_mouse.data(), enable_mouse.size());

    if (tcsetattr(stdout_no, TCSAFLUSH, &raw_term_setting) < 0) {
        throw std::runtime_error("ERROR SETTING RAW TERMINAL SETTING");
    }
    return true;
}

bool reset_raw_tty(int stdout_no) {
    if (tcsetattr(stdout_no, TCSAFLUSH, &original_term_setting) < 0) {
        throw std::runtime_error("ERROR SETTING CANONICAL TERMINAL SETTING");
    }
    return true;
}

std::optional<mouseEvent> mouse_coords(std::string &s) {
    // SGR format: \033[<btn;x;yM  or  \033[<btn;x;ym
    if (s.size() < 6 || s.compare(0, 3, "\033[<") != 0)
        return std::nullopt;

    mouseEvent ev;
    int btn;
    char type;

    // sscanf is much safer than manual index incrementing for variable-length numbers
    if (sscanf(s.c_str(), "\033[<%d;%d;%d%c", &btn, &ev.x, &ev.y, &type) != 4) {
        return std::nullopt;
    }

    if (btn == 0)
        ev.which_click = LEFT;
    else if (btn == 1)
        ev.which_click = MIDDLE;
    else
        ev.which_click = RIGHT;

    ev.mouse_status = (type == 'M') ? CLICKED : RELEASED;
    return ev;
}

int main() {
    /* Renderer::Screen screen;

    Renderer::Box box1(1, 1, 190, 20, RED, BOX);
    Renderer::Text text1("hello", 5, 5, RED, TEXT);
    box1.draw(screen);
    text1.draw(screen);
    screen.Render(); */

    int out_fd = STDOUT_FILENO;
    int in_fd = STDIN_FILENO;

    if (!set_raw_tty(out_fd))
        return 1;

    char buffer[128];
    while (true) {
        ssize_t n = read(in_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0)
            continue;
        buffer[n] = '\0';

        if (buffer[0] == 3)
            break;

        std::string input(buffer, n);
        auto me = mouse_coords(input);

        if (me) {
            char out[64];
            int len = snprintf(out, sizeof(out), "\rX: %d, Y: %d (Btn: %d)   ", me->x, me->y, me->which_click);
            write(out_fd, out, len);
        } else {
            write(out_fd, buffer, sizeof(buffer));
        }
    }
}
