#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include "buffer_management.h"
#include "instance.h"
#include "shader_management.h"
#include <stdint.h>

typedef struct M_RenderObject M_RenderObject;

struct M_RenderObject {
  struct M_Buffer vertex_buf;
  struct M_Buffer index_buf;
};

enum M_Result m_render_object_create(const struct M_Vertex *vertices, uint32_t num_vertices, const uint32_t *indices,
                                     uint32_t num_indices, M_Instance *instance);
void m_render_object_destroy(M_Instance *instance);

#endif // !RENDER_OBJECT_H
