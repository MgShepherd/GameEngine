#include "render_object.h"
#include "buffer_management.h"
#include "instance_private.h"
#include "result.h"
#include "result_utils.h"

enum M_Result m_render_object_create(const struct M_Vertex *vertices, uint32_t num_vertices, const uint32_t *indices,
                                     uint32_t num_indices, M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  instance->object.vertex_buf.num_elements = num_vertices;
  instance->object.index_buf.num_elements = num_indices;

  const VkDeviceSize vert_buf_size = sizeof(struct M_Vertex) * num_vertices;
  result = m_buffer_create(&instance->object.vertex_buf, instance, vertices, vert_buf_size,
                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  return_result_if_err(result);

  const VkDeviceSize index_buf_size = sizeof(uint32_t) * num_indices;
  result =
      m_buffer_create(&instance->object.index_buf, instance, indices, index_buf_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  return_result_if_err(result);

  return result;
}

void m_render_object_destroy(M_Instance *instance) {
  m_buffer_destroy(&instance->object.vertex_buf, instance);
  m_buffer_destroy(&instance->object.index_buf, instance);
}
