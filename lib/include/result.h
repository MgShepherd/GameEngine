#ifndef RESULT_H
#define RESULT_H

enum M_Result {
  M_SUCCESS,
  M_WINDOW_INIT_ERR,
  M_VULKAN_INIT_ERR,
  M_MEMORY_ALLOC_ERR
};

const char *m_result_to_string(enum M_Result result);

void m_result_set_handler(void (*handler)(enum M_Result code, const char *description));

void m_result_process(enum M_Result code, const char *description);

#endif // !RESULT_H
