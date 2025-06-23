#include "render_object.h"
#include "buffer_management.h"
#include "instance_private.h"
#include "result.h"
#include "result_utils.h"

enum M_Result m_render_object_create(M_RenderObject *object, const M_Instance *instance,
                                     const struct M_Vertex *vertices, uint32_t num_vertices, const uint32_t *indices,
                                     uint32_t num_indices) {
  enum M_Result result = M_SUCCESS;

  object->vertex_buf.num_elements = num_vertices;
  object->index_buf.num_elements = num_indices;

  const VkDeviceSize vert_buf_size = sizeof(struct M_Vertex) * num_vertices;
  result = m_buffer_create_and_allocate(&object->vertex_buf, instance, vertices, vert_buf_size,
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  return_result_if_err(result);

  const VkDeviceSize index_buf_size = sizeof(uint32_t) * num_indices;
  result = m_buffer_create_and_allocate(&object->index_buf, instance, indices, index_buf_size,
                                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  return_result_if_err(result);

  return result;
}

void m_render_object_destroy(M_RenderObject *object, const M_Instance *instance) {
  m_buffer_destroy(&object->vertex_buf, instance);
  m_buffer_destroy(&object->index_buf, instance);
}
