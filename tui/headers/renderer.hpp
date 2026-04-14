#ifndef RENDERER
#define RENDERER

#include "types.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace Renderer {
struct Cell {
    std::string character = " ";
    Style cell_style;
};
enum ClickType { LEFT, MIDDLE, RIGHT };
enum MouseStatus { CLICKED, RELEASED };

struct mouseEvent {
    int x;
    int y;
    ClickType which_click;
    MouseStatus mouse_status;
};

class Screen {
  private:
    struct winsize resolution;
    std::unique_ptr<Cell[]> buffer;
    struct termios original_term;

    bool set_raw_mode() {
        struct termios raw_term;
        if (tcgetattr(STDIN_FILENO, &raw_term) < 0) {
            return false;
        }
        original_term = raw_term;

        raw_term.c_lflag &= ~(ICANON | ECHO | IEXTEN);
        raw_term.c_iflag &= ~(IXON | ICRNL);
        raw_term.c_oflag &= ~(OPOST);

        raw_term.c_cc[VMIN] = 1; // is blocking, add poll later on STDIN
        raw_term.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_term) < 0) {
            return false;
        }
        std::string enable_mouse = "\033[?1000h\033[?1006h";
        write(STDOUT_FILENO, enable_mouse.data(), enable_mouse.size());
        return true;
    }

    void reset_raw_mode() {
        std::string disable_mouse = "\033[?1000l\033[?1006l";
        write(STDOUT_FILENO, disable_mouse.data(), disable_mouse.size());

        if (tcsetattr(STDIN_FILENO, TCSANOW, &original_term) < 0) {
            throw std::runtime_error("ERROR SETTING TERMINAL TO CANNONICAL MODE");
        }
        std::string out_message = "Reset";
        write(STDOUT_FILENO, out_message.data(), out_message.size());
    }

  public:
    Screen() {
        int stdout_no = fileno(stdout);
        if (ioctl(stdout_no, TIOCGWINSZ, &resolution) == -1 || resolution.ws_col == 0) {
            resolution.ws_row = 24;
            resolution.ws_col = 80;
        }
        buffer = std::make_unique<Cell[]>(resolution.ws_row * resolution.ws_col);
        if (set_raw_mode() == false) {
            throw std::runtime_error("ERROR SETTING TERMINAL IN RAW MODE");
        };
    }
    void at(std::string character, const Style style, int row, int col) {
        if (row < 0 || row >= resolution.ws_row || col < 0 || col >= resolution.ws_col) {
            throw std::out_of_range("Screen coordinates out of bounds!");
        }
        buffer[(resolution.ws_col * row) + col].character = character;
        buffer[(resolution.ws_col * row) + col].cell_style = style;
    }
    void Render();

    ~Screen() { reset_raw_mode(); }
};

} // namespace Renderer

#endif
