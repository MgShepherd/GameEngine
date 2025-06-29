#include "input.h"
#include "GLFW/glfw3.h"
#include "logger.h"
#include "result.h"
#include "window_private.h"

const unsigned short STANDARD_KEYS_START = 44;
const unsigned short STANDARD_KEYS_END = 92;
const unsigned short ADDITIONAL_KEYS_START = 256;
const unsigned short ADDITIONAL_KEYS_END = 269;

enum M_KeyCode m_key_from_glfw(int glfw_code) {
  if (glfw_code >= STANDARD_KEYS_START && glfw_code <= STANDARD_KEYS_END) {
    return (enum M_KeyCode)(glfw_code - STANDARD_KEYS_START);
  }

  if (glfw_code >= ADDITIONAL_KEYS_START && glfw_code <= ADDITIONAL_KEYS_END) {
    return (enum M_KeyCode)(glfw_code - ADDITIONAL_KEYS_START + STANDARD_KEYS_END - STANDARD_KEYS_START);
  }

  return M_KEY_UNSUPPORTED;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  enum M_KeyCode m_key = m_key_from_glfw(key);
  m_logger_info("Pressed key %d with scancode %d and m code %d", key, scancode, m_key);
}

void m_input_init(const M_Window *window) { glfwSetKeyCallback(window->glfw_window, key_callback); }

bool m_key_is_pressed(const M_Window *window, enum M_KeyCode code) { return true; }
