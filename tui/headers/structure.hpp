#include "components.hpp"
#include <unistd.h>
#include <vector>

struct Structure {
    std::vector<std::unique_ptr<Component>> screen_components;
    uint8_t element_count = 0;

    void draw_structure() {
        for (int i = 0; i < screen_components.size(); i++) {
            screen_components[i].get()->draw();
        }
    }
};
