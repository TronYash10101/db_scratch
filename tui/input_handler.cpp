#include "headers/input_handler.hpp"
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>

void InputHandlers::Stdin_Handler::control_byte_handle(char byte) {
    if (byte == 3) {
        std::string stop_msg = "\n Process Stopped \n";
        write(STDOUT_FILENO, stop_msg.data(), stop_msg.size());
        kill(getpid(), SIGINT);
    }

    for (int i = 0; i < curr_structure.screen_components.size(); i++) {
        if ((curr_structure.screen_components[i].get()->behavior & curr_state) == curr_state) {
            if (TextBox *ptr = dynamic_cast<TextBox *>(active_component)) {
                if (byte == 127 || byte == 7) {
                    std::string curr_text = ptr->get_inner_text();
                    if (curr_text.size() != 0) {
                        curr_text.pop_back();
                        if (!curr_text.empty()) {
                            curr_text.pop_back();
                            ptr->text.set_inner_text(curr_text);
                        }
                        ptr->text.set_inner_text(curr_text);
                    }
                    break;
                } else if (byte == '\n') {
                    ptr->text.append_inner_text(std::string(1, '\n'));
                } else if (isprint(byte)) {
                    ptr->text.append_inner_text(std::string(1, byte));
                }
            }
        }
    }
}

void InputHandlers::Stdin_Handler::mouse_byte_handle() {
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
    for (int i = 0; i < curr_structure.screen_components.size(); i++) {
        if ((curr_structure.screen_components[i]->behavior & SUPPORTS_MOUSE) == SUPPORTS_MOUSE) {
            if ((x >= curr_structure.screen_components[i]->component_col &&
                 x <= (curr_structure.screen_components[i]->component_col + curr_structure.screen_components[i]->component_width)) &&
                (y >= curr_structure.screen_components[i]->component_row &&
                 y <= (curr_structure.screen_components[i]->component_row + curr_structure.screen_components[i]->component_height))) {
                active_component = curr_structure.screen_components[i].get();
                break;
            }
        }
    }
}

void InputHandlers::Stdin_Handler::read(char byte) noexcept {

    // write(STDOUT_FILENO, "here", 4);
    if ((byte == 7 || byte == 127 || byte == '\n' || byte == 3 || isprint(byte)) && curr_state == SUPPORTS_INPUT_TEXT) {
        control_byte_handle(byte);
        curr_state = SUPPORTS_INPUT_TEXT;
        return;
    }

    if (byte == '\033') {
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
            return;
        }
    }
    if (curr_state == SUPPORTS_MOUSE) {
        special_buff += byte;
        if (byte == 'M' || byte == 'm') {
            mouse_byte_handle();
            curr_state = SUPPORTS_INPUT_TEXT;
        }
        return;
    }

    if (curr_state == SUPPORTS_NAVIGATION) {
        // esc_byte_handle() later
        curr_state = SUPPORTS_INPUT_TEXT;
        return;
    }
}
