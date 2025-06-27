#ifndef M_RENDER_OBJECT_H
#define M_RENDER_OBJECT_H

#include "buffer_management.h"
#include "instance.h"
#include "shader_management.h"
#include <stdint.h>

typedef struct M_RenderObject M_RenderObject;

struct M_RenderObject {
  struct M_Buffer vertex_buf;
  struct M_Buffer index_buf;
};

enum M_Result m_render_object_create(M_RenderObject *object, const M_Instance *instance,
                                     const struct M_Vertex *vertices, uint32_t num_vertices, const uint32_t *indices,
                                     uint32_t num_indices);
void m_render_object_destroy(M_RenderObject *object, const M_Instance *instance);

#endif
