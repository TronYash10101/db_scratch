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
    std::string text;

  public:
    Text(int col, int row, COLOR color, COMPONENT component, size_t font_size = 0) : style(component, color), text("") {
        // font size is currently not supported
        Element::col = col;
        Element::row = row;
        Element::width = width;
        Element::height = height;
        text.resize(1024);
    }
    void draw(Renderer::Screen &screen) override {
        for (int c = 0; c < text.size(); c++) {
            screen.at(std::string(1, text[c]), style, row, c + col);
        }
    }
    std::string get_inner_text() const { return text; }
    void set_inner_text(std::string inner_text) { text += inner_text; }
};
} // namespace Base_Element
