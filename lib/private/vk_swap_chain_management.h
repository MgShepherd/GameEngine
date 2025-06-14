#ifndef SWAP_CHAIN_MANAGEMENT_H
#define SWAP_CHAIN_MANAGEMENT_H

#include "instance_private.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

struct M_SwapChainSupport {
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR *formats;
  uint32_t num_formats;
  VkPresentModeKHR *present_modes;
  uint32_t num_present_modes;
};

enum M_Result m_swap_chain_get_device_support(struct M_SwapChainSupport *swap_support, VkPhysicalDevice device, const struct M_Instance *instance);
void m_swap_chain_support_destroy(struct M_SwapChainSupport *swap_support);

#endif // !SWAP_CHAIN_MANAGEMENT_H
