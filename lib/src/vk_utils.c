#include "vk_utils.h"

#include "instance_private.h"
#include "result.h"
#include "result_utils.h"
#include "vk_device_management.h"
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

enum M_Result process_vulkan_result(VkResult result) {
  if (result != VK_SUCCESS) {
    return m_result_process(M_VULKAN_INIT_ERR, string_VkResult(result));
  }

  return M_SUCCESS;
}

enum M_Result create_command_pool(VkCommandPool *command_pool, VkCommandPoolCreateFlagBits flags,
                                  struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  const struct M_QueueFamilyIndices queue_families =
      vk_physical_device_get_queue_families(instance->device.physical_device, instance);

  const VkCommandPoolCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = flags,
      .queueFamilyIndex = queue_families.graphics,
  };

  const VkResult vk_result = vkCreateCommandPool(instance->device.vk_device, &create_info, NULL, command_pool);
  vk_return_result_if_err(vk_result);

  return result;
}
