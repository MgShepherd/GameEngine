#ifndef INSTANCE_H
#define INSTANCE_H

#include "types.h"
#include <stdbool.h>

typedef struct {
  bool enable_debug;
  const char *app_name;
  struct M_Color clear_color;
} M_InstanceOptions;

typedef struct M_Instance M_Instance;

#include "sprite.h"
#include "window.h"

enum M_Result m_instance_create(struct M_Instance **instance, const M_Window *window,
                                const M_InstanceOptions *instance_options);
void m_instance_destroy(M_Instance *instance);

void m_instance_set_clear_color(M_Instance *instance, struct M_Color color);

enum M_Result m_instance_update(M_Instance *instance, M_Sprite **sprites, uint32_t num_sprites);

#endif // !INSTANCE_H
