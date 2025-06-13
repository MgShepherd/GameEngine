#ifndef VK_DEBUG_MESSENGER_HELPER_H
#define VK_DEBUG_MESSENGER_HELPER_H

#include <vulkan/vulkan_core.h>

void vk_debug_messenger_fill_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info);
enum M_Result vk_debug_messenger_create(VkDebugUtilsMessengerEXT *vk_debug_messenger, VkInstance instance);
void vk_debug_messenger_destroy(VkDebugUtilsMessengerEXT vk_debug_messenger, VkInstance instance);

#endif
