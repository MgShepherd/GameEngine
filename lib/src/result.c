#include "result.h"
#include "logger.h"
#include <assert.h>
#include <stdio.h>

enum M_Result default_handler(enum M_Result code, const char *description) {
  if (code != M_SUCCESS) {
    m_logger_error("Error %s: %s\n", m_result_to_string(code), description);
  }
  return code;
}

enum M_Result (*g_handler)(enum M_Result code, const char *description) = &default_handler;

const char *m_result_to_string(enum M_Result result) {
  switch (result) {
  case M_SUCCESS:
    return "SUCCESS";
  case M_VULKAN_INIT_ERR:
    return "VULKAN_INIT";
  case M_WINDOW_INIT_ERR:
    return "WINDOW_INIT";
  case M_MEMORY_ALLOC_ERR:
    return "MEMORY_ALLOCATION";
  }
}

void m_result_set_handler(enum M_Result (*handler)(enum M_Result code, const char *description)) {
  g_handler = handler;
}

enum M_Result m_result_process(enum M_Result code, const char *description) {
  if (g_handler != NULL) {
    return g_handler(code, description);
  }

  return M_SUCCESS;
}
