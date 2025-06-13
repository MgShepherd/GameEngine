#include "vk_utils.h"

#include "result.h"
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

enum M_Result process_vulkan_result(VkResult result) {
  if (result != VK_SUCCESS) {
    return m_result_process(M_VULKAN_INIT_ERR, string_VkResult(result));
  }

  return M_SUCCESS;
}
