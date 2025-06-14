#include "instance.h"
#include "instance_private.h"
#include "logger.h"
#include "result.h"
#include "vk_debug_messenger_helper.h"
#include "vk_device_management.h"
#include "vk_instance_helper.h"
#include "window.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

enum M_Result m_instance_create(struct M_Instance **instance, const M_Window *window, const M_InstanceOptions *instance_options) {
  enum M_Result result = M_SUCCESS;

  *instance = malloc(sizeof(struct M_Instance));
  if (*instance == NULL) {
    result = m_result_process(M_MEMORY_ALLOC_ERR, "Unable to allocate required memory for M_Instance struct");
    goto instance_init_cleanup;
  }

  result = vk_instance_create(&(*instance)->vk_instance, instance_options);
  if (result != M_SUCCESS) {
    goto instance_init_cleanup;
  }

  if (instance_options->enable_debug) {
    result = vk_debug_messenger_create(&(*instance)->vk_debug_messenger, (*instance)->vk_instance);
    if (result != M_SUCCESS) {
      goto instance_init_cleanup;
    }
  }

  result = m_window_surface_create(window, *instance);
  if (result != M_SUCCESS) {
    goto instance_init_cleanup;
  }

  VkPhysicalDevice physical_device;
  result = vk_physical_device_find(&physical_device, *instance);
  if (result != M_SUCCESS) {
    goto instance_init_cleanup;
  }

  result = vk_device_create(*instance, physical_device);
  if (result != M_SUCCESS) {
    goto instance_init_cleanup;
  }

  m_logger_info("Successfully initialised M_Instance");

instance_init_cleanup:
  if (result != M_SUCCESS) {
    m_instance_destroy(*instance);
    *instance = NULL;
  }

  return result;
}

void m_instance_destroy(M_Instance *instance) {
  if (instance != NULL) {
    vk_device_destroy(instance->vk_device);
    instance->vk_device = NULL;
    m_window_surface_destroy(instance);
    instance->vk_surface = NULL;
    vk_debug_messenger_destroy(instance->vk_debug_messenger, instance->vk_instance);
    instance->vk_debug_messenger = NULL;
    vk_instance_destroy(instance->vk_instance);
    instance->vk_instance = NULL;
    free(instance);
  }
}
