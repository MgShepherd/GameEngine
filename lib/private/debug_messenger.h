#ifndef M_DEBUG_MESSENGER_H
#define M_DEBUG_MESSENGER_H

#include <vulkan/vulkan_core.h>

void m_debug_messenger_fill_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info);
enum M_Result m_debug_messenger_create(VkDebugUtilsMessengerEXT *vk_debug_messenger, VkInstance instance);
void m_debug_messenger_destroy(VkDebugUtilsMessengerEXT vk_debug_messenger, VkInstance instance);

#endif
