#ifndef RESULT_H
#define RESULT_H

enum M_Result {
  M_SUCCESS,
  M_WINDOW_INIT_ERR,
  M_VULKAN_INIT_ERR,
  M_MEMORY_ALLOC_ERR,
  M_FILE_READ_ERR,
};

const char *m_result_to_string(enum M_Result result);

void m_result_set_handler(enum M_Result (*handler)(enum M_Result code, const char *description));

enum M_Result m_result_process(enum M_Result code, const char *description);

#endif // !RESULT_H
