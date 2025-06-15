#ifndef RESULT_UTILS_H
#define RESULT_UTILS_H

#define return_result_if_null(var, code, message)                                                                      \
  if (var == NULL) {                                                                                                   \
    result = m_result_process(code, message);                                                                          \
    return result;                                                                                                     \
  }

#define return_result_if_null_clean(var, code, message, cleanup_func, ...)                                             \
  if (var == NULL) {                                                                                                   \
    result = m_result_process(code, message);                                                                          \
    cleanup_func(__VA_ARGS__);                                                                                         \
    return result;                                                                                                     \
  }

#define return_result_if_err_clean(result, cleanup_func, ...)                                                          \
  if (result != M_SUCCESS) {                                                                                           \
    cleanup_func(__VA_ARGS__);                                                                                         \
    return result;                                                                                                     \
  }

#define return_false_if_err(result)                                                                                    \
  if (result != M_SUCCESS) {                                                                                           \
    return false;                                                                                                      \
  }

#define return_null_if_null(var, code, message)                                                                        \
  if (var == NULL) {                                                                                                   \
    m_result_process(code, message);                                                                                   \
    return NULL;                                                                                                       \
  }

#define vk_return_false_if_err(result)                                                                                 \
  if (result != VK_SUCCESS) {                                                                                          \
    return false;                                                                                                      \
  }

#define vk_return_result_if_err(result)                                                                                \
  if (result != VK_SUCCESS) {                                                                                          \
    return process_vulkan_result(result);                                                                              \
  }

#define vk_return_result_if_err_clean(result, cleanup_func, ...)                                                       \
  if (result != VK_SUCCESS) {                                                                                          \
    cleanup_func(__VA_ARGS__);                                                                                         \
    return process_vulkan_result(result);                                                                              \
  }

#define vk_return_null_if_err(result)                                                                                  \
  if (result != VK_SUCCESS) {                                                                                          \
    process_vulkan_result(result);                                                                                     \
    return NULL;                                                                                                       \
  }

#endif // !RESULT_UTILS_H
