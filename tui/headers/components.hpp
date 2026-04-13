#ifndef COMPONENTS
#define COMPONENTS

#include "base_elements.hpp"
#include <algorithm>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum BEHAVIOR {
    SUPPORT_NONE = 0,
    SUPPORTS_INPUT_TEXT = 1 << 0, // text box
    SUPPORTS_NAVIGATION = 1 << 1, // item navigator
    SUPPORTS_MOUSE = 1 << 2,      // item navigator, text box
};

class Component {
  protected:
    // std::vector<std::unique_ptr<Base_Element::Element>> elements;

  public:
    int component_col;
    int component_row;
    int component_width;
    int component_height;
    int behavior = 0;
    virtual void draw() {}
};

class TextBox : public Component {
  private:
    Renderer::Screen &screen;

  public:
    Base_Element::Text text;
    Base_Element::Box box;
    TextBox(Renderer::Screen &screen, int box_row, int box_col, size_t box_height, size_t box_width, COLOR border_color)
        : screen(screen), text(box_col + 1, box_row + (box_height / 2), box_width, border_color, TEXT),
          box(box_col, box_row, box_width, box_height, border_color, BOX) {
        behavior = SUPPORTS_MOUSE | SUPPORTS_INPUT_TEXT;
        component_col = box_col;
        component_row = box_row;
        component_width = box_width;
        component_height = box_height;
    }

    void draw() override {
        box.draw(screen);
        text.draw(screen);
    }

    std::string get_inner_text() const { return text.get_inner_text(); }
    // void append_inner_text(std::string inner_text) { text.append_inner_text(inner_text); }
    // void set_inner_text(std::string inner_text) { text.set_inner_text(inner_text); }
};

class Table : public Component {
  private:
    Renderer::Screen &screen;

  public:
    Base_Element::Box box;
    Base_Element::HLine header_seperator;
    size_t verticle_seperator;
    COLOR color;

    Table(Renderer::Screen &screen, int col, int row, size_t width, size_t height, size_t verticle_seperator, COLOR color)
        : screen(screen), box(col, row, width, height, color, BOX), header_seperator(row + 3, col, width, color), color(color),
          verticle_seperator(verticle_seperator) {
        component_col = col;
        component_row = row;
        component_width = width;
        component_height = height;
    }
    void draw() override {
        box.draw(screen);
        header_seperator.draw(screen);
        for (int i = 1; i <= verticle_seperator; i++) {
            Base_Element::VLine temp_v_line(component_row, 2 * component_col, component_height, color);
            temp_v_line.draw(screen);
        }
    }
};
#endif
