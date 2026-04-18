#include "components.hpp"
#include <memory>
#include <unistd.h>
#include <vector>

struct Structure {

    std::vector<Component *> components; // used by mouse handling

    std::vector<std::unique_ptr<TextBox>> textbox;
    std::vector<std::unique_ptr<Table>> table;
    std::vector<std::unique_ptr<Accordion>> accordion;

    void draw_structure() {
        for (std::unique_ptr<TextBox> &t : textbox) {
            t->draw();
        }
        for (std::unique_ptr<Table> &tb : table) {
            tb->draw();
        }
        for (std::unique_ptr<Accordion> &a : accordion) {
            a->draw();
        }
    }
};
