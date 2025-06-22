#ifndef M_INSTANCE_PRIVATE_H
#define M_INSTANCE_PRIVATE_H

#include "buffer_management.h"
#include "pipeline_management.h"
#include "render_management.h"
#include "render_object.h"
#include "vk_device_management.h"
#include "vk_swap_chain_management.h"
#include <vulkan/vulkan_core.h>

struct M_Instance {
  VkInstance vk_instance;
  VkDebugUtilsMessengerEXT vk_debug_messenger;
  struct M_Device device;
  VkSurfaceKHR vk_surface;
  struct M_SwapChain swapchain;
  struct M_Pipeline pipeline;
  struct M_Renderer renderer;
};

#endif // !M_INSTANCE_PRIVATE_H
