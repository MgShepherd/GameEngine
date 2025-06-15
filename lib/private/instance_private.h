#ifndef M_INSTANCE_PRIVATE_H
#define M_INSTANCE_PRIVATE_H

#include "vk_device_management.h"
#include "vk_swap_chain_management.h"
#include <vulkan/vulkan_core.h>

struct M_Instance {
  VkInstance vk_instance;
  VkDebugUtilsMessengerEXT vk_debug_messenger;
  struct M_Device device;
  VkSurfaceKHR vk_surface;
  struct M_SwapChain swapchain;
};

#endif // !M_INSTANCE_PRIVATE_H
