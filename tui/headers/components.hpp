#include "base_elements.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Component {
  protected:
    std::vector<std::unique_ptr<Base_Element::Element>> elements;

  public:
    virtual void draw() {}
};

class TextBox : public Component {
  private:
    Renderer::Screen &screen;
    Base_Element::Text *text;

  public:
    TextBox(Renderer::Screen &screen, int box_row, int box_col, size_t box_height, size_t box_width, COLOR border_color) : screen(screen) {
        auto box = std::make_unique<Base_Element::Box>(box_col, box_row, box_width, box_height, border_color, BOX);
        auto text = std::make_unique<Base_Element::Text>(box_col + 1, box_row + (box_height / 2), border_color, TEXT);

        this->text = text.get();

        elements.push_back(std::move(box));
        elements.push_back(std::move(text));
    }

    void draw() override {
        for (std::unique_ptr<Base_Element::Element> &ele : elements) {
            ele->draw(screen);
        }
    }

    std::string get_inner_text() const { return text->get_inner_text(); }
    void set_inner_text(std::string inner_text) { text->set_inner_text(inner_text); }
};
