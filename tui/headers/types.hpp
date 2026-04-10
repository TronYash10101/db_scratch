#include <iostream>
#include <variant>

enum COLOR {
    RED,
    GREEN,
    BLUE,
};
enum BORDER_OUTLINE { PLAIN, DASHED, DOTTED };
enum COMPONENT { TEXT, BOX };

struct Style {
    COLOR color;
    COMPONENT comp;

    union UniqueProp {
        struct Box {
            bool is_bold;
            // BORDER_OUTLINE outline;
        } box;

        struct Text {
            bool is_bold;
        } text;
    } unique_prop;
    Style() : color(RED), comp(TEXT){};
    Style(COMPONENT c, COLOR color) : comp(c), color(color) {

        switch (comp) {
        case TEXT:
            unique_prop.text.is_bold = false;
            break;
        case BOX:
            unique_prop.box.is_bold = false;
            // BORDER_OUTLINE outline;
            break;
        }
    }
};
