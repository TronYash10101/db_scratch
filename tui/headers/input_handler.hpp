#ifndef INPUT_HANDLERS
#define INPUT_HANDLERS
#include "structure.hpp"
#include <cctype>
#include <csignal>
#include <iostream>
#include <unistd.h>

namespace InputHandlers {

struct Events {
    bool SUBMIT = false;
    // can have more events later (mouse event)
};
class Stdin_Handler {
  private:
    Structure &curr_structure;
    Component *active_component = curr_structure.textbox[0].get();
    int curr_state = SUPPORTS_INPUT_TEXT;
    std::string special_buff;

  public:
    Stdin_Handler(Structure &curr_structure) : curr_structure(curr_structure), special_buff(""){};
    void control_byte_handle(char byte, Events &events);
    void esc_byte_handle();
    void mouse_byte_handle(Events &events);
    void navigation_handle(Events &events);
    void read(char byte, Events &events) noexcept;
};
} // namespace InputHandlers
#endif
