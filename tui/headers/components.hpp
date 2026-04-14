#ifndef COMPONENTS
#define COMPONENTS

#include "base_elements.hpp"
#include <algorithm>
#include <array>
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
        : screen(screen), text(box_col + 1, box_row + (box_height / 2), box_width, border_color),
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
    size_t header_gap;

  public:
    Base_Element::Box box;
    Base_Element::HLine header_seperator;
    size_t divisions;
    COLOR color;
    int behavior = SUPPORTS_INPUT_TEXT | SUPPORTS_NAVIGATION;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    size_t gap = 0;

    Table(Renderer::Screen &screen, int col, int row, size_t width, size_t height, size_t divisions, size_t header_gap, COLOR color)
        : screen(screen), box(col, row, width, height, color, BOX), header_gap(header_gap),
          header_seperator(row + header_gap, col + 1, width - 2, color), color(color), divisions(divisions), headers(divisions, " "),
          rows(divisions, std::vector<std::string>(divisions, " ")) {
        component_col = col;
        component_row = row;
        component_width = width;
        component_height = height;
    }
    void draw() override {
        box.draw(screen);
        header_seperator.draw(screen);
        if (divisions == 1) {
            gap = (component_width) / 2;
        } else {
            gap = (component_width) / divisions;
        }
        for (int i = 1; i < divisions; i++) {
            Base_Element::VLine temp_v_line(component_row + 1, (i * gap) + component_col, component_height - 2, color);
            temp_v_line.draw(screen);
        }
        for (int header = 0; header < headers.size(); header++) {
            Base_Element::Text text(component_col + gap / 3 + (header * gap), component_row + (header_gap / 2), gap / 2, color);
            text.set_inner_text(headers[header]);
            text.draw(screen);
        }
        for (int row = 0; row < rows.size(); row++) {
            for (int r_value = 0; r_value < rows[row].size(); r_value++) {
                Base_Element::Text text(component_col + gap / 3 + (r_value * gap),
                                        component_row + (r_value + 1) * header_gap + (header_gap / 2), gap / 2, color);
                text.set_inner_text(rows[row][r_value]);
                text.draw(screen);
            }
        }
    }
    void fill_rows(std::string value, int row, int col) { rows[row][col] = value; }
    void fill_headers(std::string value, int col) { headers[col] = value; }
};
#endif
