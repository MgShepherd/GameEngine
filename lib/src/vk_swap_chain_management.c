#include "vk_swap_chain_management.h"
#include "result.h"
#include "vk_utils.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

// TODO: Use this macro to replace some existing logic
#define get_device_support_handle_vk_result(func) \
  result = process_vulkan_result(func);           \
  if (result != M_SUCCESS) {                      \
    goto swap_chain_support_cleanup;              \
  }

enum M_Result m_swap_chain_get_device_support(struct M_SwapChainSupport *swap_support, VkPhysicalDevice device, const struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  get_device_support_handle_vk_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance->vk_surface, &swap_support->capabilities));

  swap_support->num_formats = 0;
  get_device_support_handle_vk_result(vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->vk_surface, &swap_support->num_formats, NULL));
  swap_support->formats = malloc(swap_support->num_formats * sizeof(VkSurfaceFormatKHR));
  if (swap_support->formats == NULL) {
    result = m_result_process(M_MEMORY_ALLOC_ERR, "Unable to allocate memory required for swap supports");
    goto swap_chain_support_cleanup;
  }

  get_device_support_handle_vk_result(vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->vk_surface, &swap_support->num_formats, swap_support->formats));
  swap_support->num_present_modes = 0;
  get_device_support_handle_vk_result(vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->vk_surface, &swap_support->num_present_modes, NULL));
  swap_support->present_modes = malloc(swap_support->num_present_modes * sizeof(VkPresentModeKHR));
  if (swap_support->present_modes == NULL) {
    result = m_result_process(M_MEMORY_ALLOC_ERR, "Unable to allocate memory required for present modes");
    goto swap_chain_support_cleanup;
  }

  get_device_support_handle_vk_result(vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->vk_surface, &swap_support->num_present_modes, swap_support->present_modes));

swap_chain_support_cleanup:
  if (result != M_SUCCESS)
    m_swap_chain_support_destroy(swap_support);
  return result;
}

void m_swap_chain_support_destroy(struct M_SwapChainSupport *swap_support) {
}
