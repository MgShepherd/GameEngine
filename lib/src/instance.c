#include "instance.h"
#include "buffer_management.h"
#include "instance_private.h"
#include "logger.h"
#include "pipeline_management.h"
#include "render_management.h"
#include "render_object.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "uniform_management.h"
#include "vk_debug_messenger_helper.h"
#include "vk_device_management.h"
#include "vk_instance_helper.h"
#include "vk_swap_chain_management.h"
#include "window.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

enum M_Result m_instance_create(struct M_Instance **instance, const M_Window *window,
                                const M_InstanceOptions *instance_options) {
  enum M_Result result = M_SUCCESS;

  *instance = malloc(sizeof(struct M_Instance));
  return_result_if_null_clean(*instance, M_MEMORY_ALLOC_ERR, "Unable to allocate memory", m_instance_destroy,
                              *instance);
  result = vk_instance_create(&(*instance)->vk_instance, instance_options);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  if (instance_options->enable_debug) {
    result = vk_debug_messenger_create(&(*instance)->vk_debug_messenger, (*instance)->vk_instance);
    return_result_if_err_clean(result, m_instance_destroy, *instance);
  }

  result = m_window_surface_create(window, *instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = vk_physical_device_find(*instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = vk_device_create(*instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = m_swap_chain_create(*instance, window);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = m_pipeline_create(*instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = m_swap_chain_framebuffers_create(*instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = m_renderer_create(*instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  result = m_uniforms_create(*instance);
  return_result_if_err_clean(result, m_instance_destroy, *instance);

  (*instance)->clear_color = instance_options->clear_color;

  m_logger_info("Successfully initialised M_Instance");

  return result;
}

void m_instance_set_clear_color(M_Instance *instance, struct M_Color color) { instance->clear_color = color; }

void m_instance_destroy(M_Instance *instance) {
  vkDeviceWaitIdle(instance->device.vk_device);
  if (instance != NULL) {
    m_uniforms_destroy(instance);
    m_renderer_destroy(instance);
    m_pipeline_destroy(instance);
    m_swap_chain_destroy(instance);
    vk_device_destroy(&instance->device);
    m_window_surface_destroy(instance);
    instance->vk_surface = NULL;
    vk_debug_messenger_destroy(instance->vk_debug_messenger, instance->vk_instance);
    instance->vk_debug_messenger = NULL;
    vk_instance_destroy(instance->vk_instance);
    instance->vk_instance = NULL;
    free(instance);
  }
}
