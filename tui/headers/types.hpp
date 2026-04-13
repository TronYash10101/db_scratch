#include <iostream>
#include <variant>
#include <vector>

enum COLOR {
    RED,
    GREEN,
    BLUE,
};
enum BORDER_OUTLINE { PLAIN, DASHED, DOTTED };
enum COMPONENT { TEXT, BOX, LINE };

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
        struct Line {
            bool is_bold;
        } line;

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
        case LINE:
            unique_prop.box.is_bold = false;
            // BORDER_OUTLINE outline;
            break;
        }
    }
};

enum MOUSE_BUTTON { LEFT, MIDDLE, RIGHT };
enum MOUSE_STATUS { PRESSED, RELEASED };
struct mouseCoords {
    MOUSE_BUTTON mouse_btn;
    MOUSE_STATUS mouse_status;
    int x;
    int y;
};
