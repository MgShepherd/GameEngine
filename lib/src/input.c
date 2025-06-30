#include "input.h"
#include "GLFW/glfw3.h"
#include "logger.h"
#include "result.h"
#include "result_utils.h"
#include "window_private.h"

#include <stdlib.h>

const unsigned short NUM_KEYS = 60;
static const uint32_t NUM_BLOCKS = 3;
static const unsigned short KEY_BLOCK_STARTS[] = {44, 65, 256};
static const unsigned short KEY_BLOCK_ENDS[] = {61, 92, 269};

static bool *keystates;

enum M_KeyCode m_key_from_glfw(int glfw_code) {
  for (uint32_t i = 0; i < NUM_BLOCKS; i++) {
    if (glfw_code >= KEY_BLOCK_STARTS[i] && glfw_code <= KEY_BLOCK_ENDS[i]) {
      enum M_KeyCode m_code = glfw_code - KEY_BLOCK_STARTS[i];
      for (uint32_t j = 0; j < i; j++) {
        m_code += KEY_BLOCK_ENDS[j] - KEY_BLOCK_STARTS[j] - 1;
      }
      return m_code;
    }
  }

  return M_KEY_UNSUPPORTED;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  enum M_KeyCode m_key = m_key_from_glfw(key);
  if (m_key == M_KEY_UNSUPPORTED) {
    m_logger_error("Unsupported key pressed: %d", key);
    return;
  }
  if (keystates == NULL) {
    m_logger_error("Attempted to process keys before input manager is initialised");
    return;
  }

  if (action == GLFW_PRESS) {
    keystates[m_key] = true;
  } else if (action == GLFW_RELEASE) {
    keystates[m_key] = false;
  }
}

enum M_Result m_input_init(const struct M_Window *window) {
  enum M_Result result = M_SUCCESS;
  glfwSetKeyCallback(window->glfw_window, key_callback);
  keystates = calloc(sizeof(bool), NUM_KEYS);
  return_result_if_null(keystates, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  return result;
}

bool m_key_is_pressed(enum M_KeyCode code) {
  if (keystates == NULL) {
    m_logger_error("Unable to detect if key was pressed due to input manager not being initialised");
    return false;
  };

  if (code >= NUM_KEYS || code < 0) {
    m_logger_error("Requested pressed status of an undefined key code: %d", code);
    return false;
  }

  return keystates[code];
}
