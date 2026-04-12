#include "renderer.hpp"
#include <iostream>

namespace Base_Element {
class Element {
  protected:
    int col = 0;
    int row = 0;
    size_t width = 0;
    size_t height = 0;

  public:
    virtual void draw(Renderer::Screen &screen) = 0;
};

class Box : public Element {
  protected:
    Style style;

  public:
    Box(int col, int row, size_t width, size_t height, COLOR color, COMPONENT component, bool is_bold = false) : style(component, color) {
        Element::col = col;
        Element::row = row;
        Element::width = width;
        Element::height = height;
        style.unique_prop.box.is_bold = is_bold;
    }

    void draw(Renderer::Screen &screen) override {
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

class Text : public Element {
  protected:
    Style style;
    std::string inner_text;
    size_t max_text_width;
    size_t view_offset;

  public:
    Text(int col, int row, size_t max_text_width, COLOR color, COMPONENT component, size_t font_size = 0)
        : style(component, color), inner_text(""), max_text_width(max_text_width), view_offset(0) {
        // font size is currently not supported
        Element::col = col;
        Element::row = row;
    }

    void draw(Renderer::Screen &screen) override {
        for (int c = view_offset; c < inner_text.size(); c++) {
            if ((c % (max_text_width - 1)) == 0) {
                ++view_offset;
            }
            screen.at(std::string(1, inner_text[c]), style, row, (c + col) - view_offset);
        }
    }
    std::string get_inner_text() const { return inner_text; }
    void set_inner_text(char inner_text) { this->inner_text += inner_text; }
};
} // namespace Base_Element
