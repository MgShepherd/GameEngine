#include "vk_swap_chain_management.h"
#include "result.h"
#include "result_utils.h"
#include "vk_utils.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

enum M_Result m_swap_chain_get_device_support(struct M_SwapChainSupport *swap_support, VkPhysicalDevice device,
                                              const struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  VkResult vk_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance->vk_surface, &swap_support->capabilities);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);

  swap_support->num_formats = 0;
  vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->vk_surface, &swap_support->num_formats, NULL);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);
  swap_support->formats = malloc(swap_support->num_formats * sizeof(VkSurfaceFormatKHR));
  return_result_if_null_clean(swap_support->formats, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              m_swap_chain_support_destroy, swap_support);
  vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->vk_surface, &swap_support->num_formats, swap_support->formats);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);

  swap_support->num_present_modes = 0;
  vk_result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->vk_surface, &swap_support->num_present_modes, NULL);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);
  swap_support->present_modes = malloc(swap_support->num_present_modes * sizeof(VkPresentModeKHR));
  return_result_if_null_clean(swap_support->present_modes, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              m_swap_chain_support_destroy, swap_support);
  vk_result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->vk_surface, &swap_support->num_present_modes, swap_support->present_modes);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);

  return result;
}

void m_swap_chain_support_destroy(struct M_SwapChainSupport *swap_support) {
  if (swap_support != NULL) {
    if (swap_support->formats != NULL) {
      free(swap_support->formats);
    }

    if (swap_support->present_modes != NULL) {
      free(swap_support->present_modes);
    }
  }
}
