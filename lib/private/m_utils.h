#ifndef M_UTILS_H
#define M_UTILS_H

#include "instance.h"
#include <vulkan/vulkan_core.h>

enum M_Result process_vulkan_result(VkResult result);

enum M_Result create_command_pool(VkCommandPool *command_pool, VkCommandPoolCreateFlagBits flags,
                                  const struct M_Instance *instance);

#endif
