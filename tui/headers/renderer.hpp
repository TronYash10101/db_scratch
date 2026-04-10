#include "types.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sys/ioctl.h>

namespace Renderer {
struct Cell {
    std::string character;
    Style cell_style;
};

class Screen {
  private:
    struct winsize resolution;
    std::unique_ptr<Cell[]> buffer;

  public:
    Screen() {
        int stdout_no = fileno(stdout);
        if (ioctl(stdout_no, TIOCGWINSZ, &resolution) == -1 || resolution.ws_col == 0) {
            resolution.ws_row = 24;
            resolution.ws_col = 80;
        }
        buffer = std::make_unique<Cell[]>(resolution.ws_row * resolution.ws_col);
    }
    void at(std::string character, const Style style, int row, int col) {
        if (row < 0 || row >= resolution.ws_row || col < 0 || col >= resolution.ws_col) {
            throw std::out_of_range("Screen coordinates out of bounds!");
        }
        buffer[(resolution.ws_col * row) + col].character = character;
        buffer[(resolution.ws_col * row) + col].cell_style = style;
    }
    void Render();
};

class Component {
  protected:
    int col = 0;
    int row = 0;
    size_t width = 0;
    size_t height = 0;

  public:
    virtual void draw(Screen &screen) = 0;
};

class Box : public Component {
  protected:
    Style style;

  public:
    Box(int col, int row, size_t width, size_t height, COLOR color, COMPONENT component, bool is_bold = false) : style(component, color) {
        Component::col = col;
        Component::row = row;
        Component::width = width;
        Component::height = height;
        style.unique_prop.box.is_bold = is_bold;
    }

    void draw(Screen &screen) override {
        for (int r = row; r < row + height; r++) {
            if (r == row) {
                for (int c = col; c < col + width; c++) {
                    if (c == col) {
                        screen.at("╭", style, r, c);
                    } else if (c == (col + width) - 1) {
                        screen.at("╮", style, r, c);
                    } else {
                        screen.at("─", style, r, c);
                    }
                }
            } else if (r == (row + height) - 1) {
                for (int c = col; c < col + width; c++) {
                    if (c == col) {
                        screen.at("╰", style, r, c);
                    } else if (c == (col + width) - 1) {
                        screen.at("╯", style, r, c);
                    } else {
                        screen.at("─", style, r, c);
                    }
                }
            } else {
                for (int c = col; c < col + width; c++) {
                    if (c == col || c == (col + width) - 1) {
                        screen.at("│", style, r, c);
                    } else {
                        screen.at(" ", style, r, c);
                    }
                }
            }
        }
    }
};

class Text : public Component {
  protected:
    Style style;
    std::string text;

  public:
    Text(std::string text, int col, int row, COLOR color, COMPONENT component, size_t font_size = 0) : style(component, color), text(text) {
        // font size is currently not supported
        Component::col = col;
        Component::row = row;
        Component::width = width;
        Component::height = height;
    }
    void draw(Screen &screen) override {
        for (int c = 0; c < text.size(); c++) {
            screen.at(std::string(1, text[c]), style, row, c + col);
        }
    }
};

} // namespace Renderer
