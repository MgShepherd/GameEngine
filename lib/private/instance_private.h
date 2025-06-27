#ifndef M_INSTANCE_PRIVATE_H
#define M_INSTANCE_PRIVATE_H

#include "buffer_management.h"
#include "device_management.h"
#include "pipeline_management.h"
#include "render_management.h"
#include "render_object.h"
#include "swap_chain_management.h"
#include "uniform_management.h"
#include <vulkan/vulkan_core.h>

struct M_Instance {
  VkInstance vk_instance;
  VkDebugUtilsMessengerEXT vk_debug_messenger;
  struct M_Device device;
  VkSurfaceKHR vk_surface;
  struct M_SwapChain swapchain;
  struct M_Pipeline pipeline;
  struct M_Renderer renderer;
  struct M_Uniforms uniforms;
  struct M_Color clear_color;
};

#endif // !M_INSTANCE_PRIVATE_H
