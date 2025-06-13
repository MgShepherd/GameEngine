#include "instance.h"
#include "GLFW/glfw3.h"
#include "logger.h"
#include "result.h"
#include "vk_instance_helper.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

struct M_Instance {
  VkInstance vk_instance;
};

enum M_Result m_instance_create(struct M_Instance **instance, const M_InstanceOptions *instance_options) {
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

  m_logger_info("Successfully initialised M_Instance");

instance_init_cleanup:
  if (result != M_SUCCESS) {
    m_instance_destroy(*instance);
  }

  return result;
}

void m_instance_destroy(M_Instance *instance) {
  if (instance != NULL) {
    vk_instance_destroy(instance->vk_instance);
    free(instance);
    instance = NULL;
  }
}
