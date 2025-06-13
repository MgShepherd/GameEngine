#ifndef M_INSTANCE_PRIVATE_H
#define M_INSTANCE_PRIVATE_H

#include <vulkan/vulkan_core.h>

struct M_Instance {
  VkInstance vk_instance;
  VkDebugUtilsMessengerEXT vk_debug_messenger;
  VkDevice vk_device;
  VkSurfaceKHR vk_surface;
};

#endif // !M_INSTANCE_PRIVATE_H
