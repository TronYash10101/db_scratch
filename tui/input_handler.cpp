#include "headers/input_handler.hpp"
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>

void InputHandlers::Stdin_Handler::control_byte_handle(char byte, Events &events) {

    if ((active_component->behavior & curr_state) == SUPPORTS_INPUT_TEXT) {
        if (TextBox *ptr = dynamic_cast<TextBox *>(active_component)) {
            std::string curr_text = ptr->get_inner_text();
            if (byte == 127 || byte == 7) {
                if (curr_text.size() != 0) {
                    curr_text.pop_back();
                    ptr->text.set_inner_text(curr_text);
                }
            } else if (isprint(byte)) {
                ptr->text.append_inner_text(std::string(1, byte));
            } else if (byte == '\n' && byte == '\r') {
                events.SUBMIT = true;
            }
        }
    }
}
void InputHandlers::Stdin_Handler::navigation_handle(Events &events) {
    char dir_key;

    if (sscanf(special_buff.data(), "\033[%c", &dir_key) != 1) {
        if (*special_buff.data() == '\n' || *special_buff.data() == '\r') {
            dir_key = *special_buff.data();
        }
    }

    if (Accordion *ptr = dynamic_cast<Accordion *>(active_component)) {
        if (dir_key == 'A' && ptr->which_selected != 0) {
            ptr->which_selected -= 1;
        } else if (dir_key == 'B' && ptr->which_selected < ptr->entry.size()) {
            ptr->which_selected += 1;
        } else if (ptr->which_selected < 0) {
            ptr->which_selected = 0;
        } else if (dir_key == '\n' || dir_key == '\r') {
            ptr->entry[ptr->which_selected].is_expanded = !ptr->entry[ptr->which_selected].is_expanded;
        }
    }
};

void InputHandlers::Stdin_Handler::mouse_byte_handle(Events &events) {
    int x = 0;
    int y = 0;
    int btn;
    char mstatus;
    MOUSE_BUTTON mbtn;
    MOUSE_STATUS mstat;
    sscanf(special_buff.data() + 3, "%d;%d;%d%c", &btn, &x, &y, &mstatus);
    /* 0: Left Button
       1: Middle Button (Scroll wheel click)
       2: Right Button
       32: Left Button + Drag
       33: Middle Button + Drag
       34: Right Button + Drag
       64: Scroll Up
       65: Scroll Down
       for future reference*/
    if (btn == 0) {
        mbtn = LEFT;
    } else if (btn == 1) {
        mbtn = MIDDLE;
    } else if (btn == 2) {
        mbtn = RIGHT;
    }

    if (mstatus == 'M') {
        mstat = PRESSED;
    } else if (mstatus == 'm') {
        mstat = RELEASED;
    }
    special_buff = "";
    for (int i = 0; i < curr_structure.components.size(); i++) {
        if ((curr_structure.components[i]->behavior & SUPPORTS_MOUSE) == SUPPORTS_MOUSE) {
            if ((x >= curr_structure.components[i]->component_col &&
                 x <= (curr_structure.components[i]->component_col + curr_structure.components[i]->component_width)) &&
                (y >= curr_structure.components[i]->component_row &&
                 y <= (curr_structure.components[i]->component_row + curr_structure.components[i]->component_height))) {
                active_component = curr_structure.components[i];
                break;
            }
        }
    }
}

void InputHandlers::Stdin_Handler::read(char byte, Events &events) noexcept {

    if ((byte == '\n' || byte == '\r')) {
        if (dynamic_cast<TextBox *>(active_component)) {
            events.SUBMIT = true; // TextBox + Enter = submit
        } else if (dynamic_cast<Accordion *>(active_component)) {
            special_buff.clear();
            special_buff += byte;
            navigation_handle(events); // Accordion + Enter = expand/collapse
        }
        return;
    }

    if ((byte == 7 || byte == 127 || byte == '\n' || isprint(byte)) && curr_state == SUPPORTS_INPUT_TEXT) {
        control_byte_handle(byte, events);
        curr_state = SUPPORTS_INPUT_TEXT;
        return;
    }

    if (byte == '\033') {
        special_buff.clear();
        curr_state = SUPPORTS_NAVIGATION | SUPPORTS_MOUSE;
        special_buff = byte;
        return;
    }

    if ((curr_state) == (SUPPORTS_NAVIGATION | SUPPORTS_MOUSE)) {
        special_buff += byte;
        if (*special_buff.data() == '\033' && byte == '[') {
            return;
        } else if (byte == '<') {
            curr_state = SUPPORTS_MOUSE;
            return;
        } else {
            curr_state = SUPPORTS_NAVIGATION;
            navigation_handle(events);
            return;
        }
    }

    if (curr_state == SUPPORTS_MOUSE) {
        special_buff += byte;
        if (byte == 'M' || byte == 'm') {
            mouse_byte_handle(events);
            curr_state = SUPPORTS_INPUT_TEXT;
        }
        return;
    }
}
