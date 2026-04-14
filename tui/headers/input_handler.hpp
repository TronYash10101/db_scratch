#ifndef INPUT_HANDLERS
#define INPUT_HANDLERS
#include "structure.hpp"
#include <cctype>
#include <csignal>
#include <iostream>
#include <unistd.h>

namespace InputHandlers {

class Stdin_Handler {
  private:
    Structure &curr_structure;
    Component *active_component = curr_structure.screen_components[0].get();
    int curr_state = SUPPORTS_INPUT_TEXT;
    std::string special_buff;

  public:
    Stdin_Handler(Structure &curr_structure) : curr_structure(curr_structure), special_buff(""){};
    void control_byte_handle(char byte);
    void esc_byte_handle();
    void mouse_byte_handle();
    void read(char byte) noexcept;
};
} // namespace InputHandlers
#endif
