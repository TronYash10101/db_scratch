#ifndef BASE_ELEMENTS
#define BASE_ELEMENTS

#include "renderer.hpp"
#include <iostream>
#include <unistd.h>

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
    Box(int col, int row, size_t width, size_t height, COLOR color, bool is_bold = false) : style(BOX, color) {
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

  public:
    size_t max_text_width;
    size_t view_offset;
    Text(int col, int row, size_t max_text_width, COLOR color, size_t font_size = 0)
        : style(TEXT, color), inner_text(""), max_text_width(max_text_width), view_offset(0) {
        // font size is currently not supported
        Element::col = col;
        Element::row = row;
    }

    void draw(Renderer::Screen &screen) override {
        size_t len = inner_text.size();

        if (len > max_text_width) {
            view_offset = len % max_text_width;
            if (view_offset == 0) {
                view_offset = max_text_width;
            }
            view_offset = len - view_offset;
        }
        for (size_t c = 0; c < max_text_width; c++) {
            if (c + view_offset < len) {
                screen.at(std::string(1, inner_text[c + view_offset]), style, row, col + c);
            } /* else {
                screen.at(" ", style, row, col);
            } */
        }
    }
    std::string get_inner_text() const { return inner_text; }
    void append_inner_text(std::string text) { inner_text += text; }
    void set_inner_text(std::string text) { inner_text = text; }
};

class VLine : public Element {
  protected:
    Style style;

  public:
    VLine(int row, int col, int height, COLOR color, bool is_bold = false) {
        Element::row = row;
        Element::col = col;
        Element::height = height;
        style.color = color;
        style.comp = LINE;
        style.unique_prop.text.is_bold = is_bold;
    }

    void draw(Renderer::Screen &screen) {
        for (int r = row; r < row + height; r++) {

            if (style.unique_prop.line.is_bold) {
                screen.at("┃", style, r, col);
            } else {
                screen.at("│", style, r, col);
            }
        }
    }
};
class HLine : public Element {
  protected:
    Style style;

  public:
    HLine(int row, int col, int width, COLOR color, bool is_bold = false) {
        Element::row = row;
        Element::col = col;
        Element::width = width;
        style.color = color;
        style.comp = LINE;
        style.unique_prop.text.is_bold = is_bold;
    }

    void draw(Renderer::Screen &screen) {
        for (int c = col; c < col + width; c++) {
            if (style.unique_prop.line.is_bold) {
                screen.at("━", style, row, c);
            } else {
                screen.at("─", style, row, c);
            }
        }
    }
};

} // namespace Base_Element

#endif
