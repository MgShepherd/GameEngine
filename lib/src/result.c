#include "result.h"
#include "logger.h"
#include <assert.h>
#include <stdio.h>

void default_handler(enum M_Result code, const char *description) {
  if (code != M_SUCCESS) {
    m_logger_error("Error %s: %s\n", m_result_to_string(code), description);
  }
}

void (*g_handler)(enum M_Result code, const char *description) = &default_handler;

const char *m_result_to_string(enum M_Result result) {
  switch (result) {
  case M_SUCCESS:
    return "SUCCESS";
  case M_VULKAN_INIT_ERR:
    return "VULKAN_INIT";
  case M_WINDOW_INIT_ERR:
    return "WINDOW_INIT";
  }
}

void m_result_set_handler(void (*handler)(enum M_Result code, const char *description)) {
  g_handler = handler;
}

void m_result_process(enum M_Result code, const char *description) {
  if (g_handler != NULL) {
    g_handler(code, description);
  }
}
