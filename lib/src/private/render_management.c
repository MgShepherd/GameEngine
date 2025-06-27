#include "render_management.h"
#include "device_management.h"
#include "instance_private.h"
#include "logger.h"
#include "m_utils.h"
#include "result_utils.h"
#include "sprite_private.h"
#include <assert.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

enum M_Result allocate_command_buffers(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  instance->renderer.command_buffers = malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->renderer.command_buffers, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  const VkCommandBufferAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = instance->renderer.command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  const VkResult vk_result =
      vkAllocateCommandBuffers(instance->device.vk_device, &allocate_info, instance->renderer.command_buffers);
  vk_return_result_if_err_clean(vk_result, m_renderer_destroy, instance);
  m_logger_info("Successfully allocated command buffers");

  return result;
}

enum M_Result create_sync_objects(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  const VkSemaphoreCreateInfo semaphore_create_info = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  const VkFenceCreateInfo fence_create_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  instance->renderer.image_available_semaphores = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->renderer.image_available_semaphores, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");
  instance->renderer.render_finished_semaphores = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->renderer.render_finished_semaphores, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");
  instance->renderer.in_flight_fences = malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->renderer.in_flight_fences, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  VkResult vk_result;
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vk_result = vkCreateSemaphore(instance->device.vk_device, &semaphore_create_info, NULL,
                                  &instance->renderer.image_available_semaphores[i]);
    vk_return_result_if_err(vk_result);
    vk_result = vkCreateSemaphore(instance->device.vk_device, &semaphore_create_info, NULL,
                                  &instance->renderer.render_finished_semaphores[i]);
    vk_return_result_if_err(vk_result);
    vk_result =
        vkCreateFence(instance->device.vk_device, &fence_create_info, NULL, &instance->renderer.in_flight_fences[i]);
    vk_return_result_if_err(vk_result);
  }

  return result;
}

enum M_Result m_renderer_prepare(const struct M_Instance *instance, uint32_t image_idx) {
  enum M_Result result = M_SUCCESS;
  const uint32_t current_frame = instance->renderer.current_frame;

  const VkCommandBufferBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };

  vk_return_result_if_err(vkBeginCommandBuffer(instance->renderer.command_buffers[current_frame], &begin_info));

  const VkClearValue clear_color = {
      .color = {.float32 = {instance->clear_color.r, instance->clear_color.g, instance->clear_color.b, 1.0f}}};
  const VkRenderPassBeginInfo render_begin_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = instance->pipeline.render_pass,
      .framebuffer = instance->swapchain.framebuffers[image_idx],
      .renderArea =
          {
              .offset = {.x = 0, .y = 0},
              .extent = instance->swapchain.extent,
          },
      .clearValueCount = 1,
      .pClearValues = &clear_color,
  };

  VkDeviceSize offsets[] = {0};

  vkCmdBeginRenderPass(instance->renderer.command_buffers[current_frame], &render_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(instance->renderer.command_buffers[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                    instance->pipeline.vk_pipeline);
  vkCmdBindDescriptorSets(instance->renderer.command_buffers[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          instance->pipeline.layout, 0, 1, &instance->uniforms.descriptor_sets[current_frame], 0, NULL);
  return result;
}

enum M_Result m_renderer_present(struct M_Instance *instance, uint32_t image_idx) {
  enum M_Result result = M_SUCCESS;
  const uint32_t current_frame = instance->renderer.current_frame;
  const VkPipelineStageFlags wait_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  const VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers = &instance->renderer.command_buffers[current_frame],
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &instance->renderer.image_available_semaphores[current_frame],
      .pWaitDstStageMask = &wait_flags,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &instance->renderer.render_finished_semaphores[current_frame],
  };

  VkResult vk_result = vkQueueSubmit(instance->device.graphics_queue, 1, &submit_info,
                                     instance->renderer.in_flight_fences[current_frame]);
  vk_return_result_if_err(vk_result);

  const VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &instance->renderer.render_finished_semaphores[current_frame],
      .swapchainCount = 1,
      .pSwapchains = &instance->swapchain.vk_swapchain,
      .pImageIndices = &image_idx,
  };

  vk_return_result_if_err(vkQueuePresentKHR(instance->device.present_queue, &present_info));
  instance->renderer.current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
  return result;
}

enum M_Result m_renderer_create(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  result =
      create_command_pool(&instance->renderer.command_pool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, instance);
  return_result_if_err(result);
  return_result_if_err(allocate_command_buffers(instance));
  return_result_if_err(create_sync_objects(instance));
  instance->renderer.currently_rendering = false;

  return result;
}
enum M_Result m_renderer_render_begin(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  const uint32_t current_frame = instance->renderer.current_frame;

  vk_return_result_if_err(vkWaitForFences(instance->device.vk_device, 1,
                                          &instance->renderer.in_flight_fences[current_frame], VK_TRUE, UINT64_MAX));
  vk_return_result_if_err(
      vkResetFences(instance->device.vk_device, 1, &instance->renderer.in_flight_fences[current_frame]));

  VkResult vk_result = vkAcquireNextImageKHR(instance->device.vk_device, instance->swapchain.vk_swapchain, UINT64_MAX,
                                             instance->renderer.image_available_semaphores[current_frame], NULL,
                                             &instance->renderer.current_image_idx);
  vk_return_result_if_err(vk_result);

  vk_return_result_if_err(vkResetCommandBuffer(instance->renderer.command_buffers[current_frame], 0));
  return_result_if_err(m_renderer_prepare(instance, instance->renderer.current_image_idx));

  return result;
}

enum M_Result m_renderer_render_end(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  vkCmdEndRenderPass(instance->renderer.command_buffers[instance->renderer.current_frame]);
  vk_return_result_if_err(vkEndCommandBuffer(instance->renderer.command_buffers[instance->renderer.current_frame]));
  return_result_if_err(m_renderer_present(instance, instance->renderer.current_image_idx));

  return result;
}

enum M_Result m_renderer_render_buffer(const struct M_Instance *instance, VkBuffer vertex_buf, VkBuffer index_buf,
                                       uint32_t num_indices) {
  enum M_Result result = M_SUCCESS;

  VkDeviceSize offsets[] = {0};
  const uint32_t current_frame = instance->renderer.current_frame;
  vkCmdBindVertexBuffers(instance->renderer.command_buffers[current_frame], 0, 1, &vertex_buf, offsets);
  vkCmdBindIndexBuffer(instance->renderer.command_buffers[current_frame], index_buf, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(instance->renderer.command_buffers[current_frame], num_indices, 1, 0, 0, 0);

  return result;
}

void m_renderer_destroy(struct M_Instance *instance) {
  if (instance->renderer.image_available_semaphores != NULL) {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      vkDestroySemaphore(instance->device.vk_device, instance->renderer.image_available_semaphores[i], NULL);
    free(instance->renderer.image_available_semaphores);
  }
  if (instance->renderer.render_finished_semaphores != NULL) {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      vkDestroySemaphore(instance->device.vk_device, instance->renderer.render_finished_semaphores[i], NULL);
    free(instance->renderer.render_finished_semaphores);
  }
  if (instance->renderer.in_flight_fences != NULL) {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      vkDestroyFence(instance->device.vk_device, instance->renderer.in_flight_fences[i], NULL);
    free(instance->renderer.in_flight_fences);
  }
  if (instance->renderer.command_pool != NULL) {
    vkDestroyCommandPool(instance->device.vk_device, instance->renderer.command_pool, NULL);
    instance->renderer.command_pool = NULL;
  }
  if (instance->renderer.command_buffers != NULL) {
    free(instance->renderer.command_buffers);
  }
}
