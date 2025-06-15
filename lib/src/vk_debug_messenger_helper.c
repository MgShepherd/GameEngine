#include "vk_debug_messenger_helper.h"
#include "logger.h"
#include "result.h"
#include "vk_utils.h"
#include <vulkan/vulkan_core.h>

void display_debug_callback_message(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const char *message) {
  switch (severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    m_logger_verbose("%s", message);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    m_logger_warn("%s", message);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    m_logger_error("%s", message);
  default:
    break;
  };
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                     VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                     const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                     void *user_data) {
  display_debug_callback_message(message_severity, callback_data->pMessage);
  return VK_FALSE;
}

VkResult debug_utils_messenger_create(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                      const VkAllocationCallbacks *allocator,
                                      VkDebugUtilsMessengerEXT *debug_messenger) {
  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL) {
    return func(instance, create_info, allocator, debug_messenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void debug_utils_messenger_destroy(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
                                   const VkAllocationCallbacks *allocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL) {
    func(instance, debug_messenger, allocator);
  }
}

void vk_debug_messenger_fill_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info) {
  *create_info = (VkDebugUtilsMessengerCreateInfoEXT){
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity =
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debug_callback,
      .pUserData = NULL,
  };
}

enum M_Result vk_debug_messenger_create(VkDebugUtilsMessengerEXT *vk_debug_messenger, VkInstance instance) {
  enum M_Result result = M_SUCCESS;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  vk_debug_messenger_fill_create_info(&create_info);
  result = process_vulkan_result(debug_utils_messenger_create(instance, &create_info, NULL, vk_debug_messenger));

  return result;
}

void vk_debug_messenger_destroy(VkDebugUtilsMessengerEXT vk_debug_messenger, VkInstance instance) {
  if (vk_debug_messenger != NULL && instance != NULL) {
    debug_utils_messenger_destroy(instance, vk_debug_messenger, NULL);
  }
}
