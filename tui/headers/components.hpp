#ifndef COMPONENTS
#define COMPONENTS

#include "base_elements.hpp"
#include "renderer.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>
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
    std::string component_id;
    virtual void draw() {}
};

class TextBox : public Component {
  private:
    Renderer::Screen &screen;

  public:
    Base_Element::Text text;
    Base_Element::Box box;
    TextBox(Renderer::Screen &screen, int box_row, int box_col, size_t box_height, size_t box_width, COLOR border_color, std::string id)
        : screen(screen), text(box_col + 1, box_row + (box_height / 2), box_width, border_color),
          box(box_col, box_row, box_width, box_height, border_color) {
        behavior = SUPPORTS_MOUSE | SUPPORTS_INPUT_TEXT;
        component_id = id;
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
    void append_inner_text(std::string inner_text) { text.append_inner_text(inner_text); }
    void set_inner_text(std::string inner_text) { text.set_inner_text(inner_text); }
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
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    size_t gap = 0;

    Table(Renderer::Screen &screen, int col, int row, size_t width, size_t height, size_t divisions, size_t header_gap, COLOR color,
          std::string id)
        : screen(screen), box(col, row, width, height, color), header_gap(header_gap),
          header_seperator(row + header_gap, col + 1, width - 2, color), color(color), divisions(divisions), headers(divisions, " "),
          rows(divisions, std::vector<std::string>(divisions, " ")) {
        component_col = col;
        component_row = row;
        component_id = id;
        component_width = width;
        component_height = height;
        behavior = SUPPORTS_INPUT_TEXT | SUPPORTS_NAVIGATION;
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
                Base_Element::Text text(component_col + 2 + (r_value * gap),
                                        component_row + header_gap + (row * header_gap) + (header_gap / 2), gap / 2, color);
                text.set_inner_text(rows[row][r_value]);
                text.draw(screen);
            }
        }
    }
    void fill_rows(std::string value, int row, int col) { rows[row][col] = value; }
    void fill_headers(std::string value, int col) { headers[col] = value; }
};

class Accordion : public Component {
  private:
    struct accordion_entry {
        std::string name;
        bool is_expanded = false;
        int sub_child_selected = 0;
        std::vector<std::string> sub_childs;
        std::unordered_set<std::string> sub_childs_lookup;
    };
    Renderer::Screen &screen;

  public:
    std::vector<accordion_entry> entry;
    Base_Element::Box box;
    size_t entry_gap;
    COLOR default_color;
    int which_selected;

    Accordion(Renderer::Screen &screen, int col, int row, size_t width, size_t height, size_t entry_gap, COLOR color, std::string id)
        : box(col, row, width, height, color), screen(screen), entry_gap(entry_gap), default_color(color) {
        behavior = SUPPORTS_MOUSE | SUPPORTS_NAVIGATION;
        component_id = id;
        entry.resize(10, {});
        component_col = col;
        component_row = row;
        component_width = width;
        component_height = height;
        which_selected = 0;
    }
    void draw() override {
        box.draw(screen);
        size_t off = entry_gap;
        for (int i = 0; i < entry.size(); i++) {
            COLOR pcolor = default_color;
            if (which_selected >= 0 && i == which_selected) {
                pcolor = CYAN;
            }
            Base_Element::Text parent(component_col + 4, component_row + off, component_width - 4, pcolor);
            parent.set_inner_text(entry[i].name);
            parent.draw(screen);
            int k = 0;
            if (entry[i].is_expanded) {
                while (k < entry[i].sub_childs.size()) {
                    COLOR ccolor = default_color;
                    if (entry[i].sub_child_selected) {
                        ccolor = MAGENTA;
                    }
                    Base_Element::Text child(component_col + 4, component_row + off + (k + 1), component_width - 4, ccolor);
                    child.set_inner_text("   " + entry[i].sub_childs[k]);
                    child.draw(screen);
                    k++;
                }
            }
            off += (k + 1) + 1;
        }
    }
    void fill_entry(std::string value, int idx) { entry[idx].name = value; }
    void fill_childs(std::string value, int parent_idx) {
        entry[parent_idx].sub_childs.push_back(value);
        entry[parent_idx].sub_childs_lookup.insert(value);
    }
};
#endif
