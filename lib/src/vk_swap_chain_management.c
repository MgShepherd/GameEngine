#include "vk_swap_chain_management.h"
#include "GLFW/glfw3.h"
#include "instance_private.h"
#include "result.h"
#include "result_utils.h"
#include "vk_device_management.h"
#include "vk_utils.h"
#include "window_private.h"
#include <assert.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

uint32_t choose_surface_format_idx(const struct M_SwapChainSupport *swap_support) {
  for (uint32_t i = 0; i < swap_support->num_formats; i++) {
    if (swap_support->formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        swap_support->formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return i;
    }
  }
  return 0;
}

VkPresentModeKHR choose_present_mode(const struct M_SwapChainSupport *swap_support) {
  for (uint32_t i = 0; i < swap_support->num_present_modes; i++) {
    if (swap_support->present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return swap_support->present_modes[i];
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t clamp_between_values(int val, uint32_t min, uint32_t max) {
  if (val < 0 || val < min)
    return min;
  if (val > max)
    return max;
  return val;
}

VkExtent2D choose_extent(const struct M_SwapChainSupport *swap_support, const M_Window *window) {
  if (swap_support->capabilities.currentExtent.width != UINT32_MAX) {
    return swap_support->capabilities.currentExtent;
  }

  int width, height;
  glfwGetFramebufferSize(window->glfw_window, &width, &height);
  VkExtent2D extent;
  extent.width = clamp_between_values(width, swap_support->capabilities.minImageExtent.width,
                                      swap_support->capabilities.maxImageExtent.width);
  extent.height = clamp_between_values(height, swap_support->capabilities.minImageExtent.height,
                                       swap_support->capabilities.maxImageExtent.height);

  return extent;
}

enum M_Result get_swapchain_images(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  VkResult vk_result = vkGetSwapchainImagesKHR(instance->device.vk_device, instance->swapchain.vk_swapchain,
                                               &instance->swapchain.num_images, NULL);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_destroy, instance);
  instance->swapchain.images = malloc(instance->swapchain.num_images * sizeof(VkImage));
  return_result_if_null_clean(instance->swapchain.images, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              m_swap_chain_destroy, instance);
  vk_result = vkGetSwapchainImagesKHR(instance->device.vk_device, instance->swapchain.vk_swapchain,
                                      &instance->swapchain.num_images, instance->swapchain.images);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_destroy, instance);

  return result;
}

enum M_Result get_swapchain_image_views(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  instance->swapchain.image_views = malloc(instance->swapchain.num_images * sizeof(VkImageView));
  return_result_if_null_clean(instance->swapchain.images, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              m_swap_chain_destroy, instance);

  for (uint32_t i = 0; i < instance->swapchain.num_images; i++) {
    VkImageViewCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = instance->swapchain.images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = instance->swapchain.format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
            },
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    VkResult vk_result =
        vkCreateImageView(instance->device.vk_device, &create_info, NULL, &instance->swapchain.image_views[i]);
    vk_return_result_if_err_clean(vk_result, m_swap_chain_destroy, instance);
  }

  return result;
}

enum M_Result m_swap_chain_get_device_support(struct M_SwapChainSupport *swap_support, VkPhysicalDevice device,
                                              const struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  VkResult vk_result =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance->vk_surface, &swap_support->capabilities);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);

  swap_support->num_formats = 0;
  vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->vk_surface, &swap_support->num_formats, NULL);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);
  swap_support->formats = malloc(swap_support->num_formats * sizeof(VkSurfaceFormatKHR));
  return_result_if_null_clean(swap_support->formats, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              m_swap_chain_support_destroy, swap_support);
  vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->vk_surface, &swap_support->num_formats,
                                                   swap_support->formats);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);

  swap_support->num_present_modes = 0;
  vk_result =
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->vk_surface, &swap_support->num_present_modes, NULL);
  vk_return_result_if_err_clean(vk_result, m_swap_chain_support_destroy, swap_support);
  swap_support->present_modes = malloc(swap_support->num_present_modes * sizeof(VkPresentModeKHR));
  return_result_if_null_clean(swap_support->present_modes, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              m_swap_chain_support_destroy, swap_support);
  vk_result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->vk_surface, &swap_support->num_present_modes,
                                                        swap_support->present_modes);
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

enum M_Result m_swap_chain_create(struct M_Instance *instance, VkPhysicalDevice physical_device,
                                  const M_Window *window) {
  assert(instance != NULL && instance->vk_surface != NULL && instance->vk_instance != NULL && window != NULL);
  enum M_Result result = M_SUCCESS;

  struct M_SwapChainSupport swap_support;
  return_result_if_err(m_swap_chain_get_device_support(&swap_support, physical_device, instance));

  const uint32_t format_idx = choose_surface_format_idx(&swap_support);
  const VkPresentModeKHR present_mode = choose_present_mode(&swap_support);
  const VkExtent2D extent = choose_extent(&swap_support, window);

  uint32_t swap_image_count = swap_support.capabilities.minImageCount + 1;
  if (swap_support.capabilities.maxImageCount > 0 && swap_image_count > swap_support.capabilities.maxImageCount) {
    swap_image_count = swap_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swap_create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = instance->vk_surface,
      .minImageCount = swap_image_count,
      .imageFormat = swap_support.formats[format_idx].format,
      .imageColorSpace = swap_support.formats[format_idx].colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = swap_support.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .oldSwapchain = NULL,
      .clipped = VK_TRUE,
  };

  const struct M_QueueFamilyIndices queue_indices = vk_physical_device_get_queue_families(physical_device, instance);
  assert(queue_indices.present != UINT32_MAX && queue_indices.graphics != UINT32_MAX);

  uint32_t indices[] = {queue_indices.present, queue_indices.graphics};
  if (queue_indices.graphics != queue_indices.present) {
    swap_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swap_create_info.queueFamilyIndexCount = NUM_REQUIRED_QUEUES;
    swap_create_info.pQueueFamilyIndices = indices;
  } else {
    swap_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  vk_return_result_if_err(
      vkCreateSwapchainKHR(instance->device.vk_device, &swap_create_info, NULL, &instance->swapchain.vk_swapchain));

  instance->swapchain.extent = extent;
  instance->swapchain.format = swap_support.formats[format_idx].format;

  return_result_if_err(get_swapchain_images(instance));
  return_result_if_err(get_swapchain_image_views(instance));

  return result;
}

void m_swap_chain_destroy(M_Instance *instance) {
  assert(instance != NULL && instance->device.vk_device != NULL);
  if (instance->swapchain.image_views != NULL) {
    for (uint32_t i = 0; i < instance->swapchain.num_images; i++) {
      vkDestroyImageView(instance->device.vk_device, instance->swapchain.image_views[i], NULL);
    }
    free(instance->swapchain.image_views);
  }
  if (instance->swapchain.images != NULL) {
    free(instance->swapchain.images);
  }
  if (instance->swapchain.vk_swapchain != NULL) {
    vkDestroySwapchainKHR(instance->device.vk_device, instance->swapchain.vk_swapchain, NULL);
  }
  instance->swapchain.images = NULL;
  instance->swapchain.vk_swapchain = NULL;
}
