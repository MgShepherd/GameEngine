#ifndef M_SWAP_CHAIN_MANAGEMENT_H
#define M_SWAP_CHAIN_MANAGEMENT_H

#include "instance.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

struct M_SwapChainSupport {
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR *formats;
  uint32_t num_formats;
  VkPresentModeKHR *present_modes;
  uint32_t num_present_modes;
};

struct M_SwapChain {
  VkSwapchainKHR vk_swapchain;
  VkImage *images;
  VkImageView *image_views;
  uint32_t num_images;
  VkFormat format;
  VkExtent2D extent;
  VkFramebuffer *framebuffers;
};

enum M_Result m_swap_chain_get_device_support(struct M_SwapChainSupport *swap_support, VkPhysicalDevice device,
                                              const struct M_Instance *instance);
void m_swap_chain_support_destroy(struct M_SwapChainSupport *swap_support);

enum M_Result m_swap_chain_create(M_Instance *instance, const M_Window *window);
void m_swap_chain_destroy(M_Instance *instance);

enum M_Result m_swap_chain_framebuffers_create(M_Instance *instance);

#endif
