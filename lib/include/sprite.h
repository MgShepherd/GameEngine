#ifndef SPRITE_H
#define SPRITE_H

typedef struct M_Sprite M_Sprite;

struct M_Color {
  float r, g, b;
};

struct M_SpriteProperties {
  float x, y, width, height;
  struct M_Color color;
};

#include "instance.h"

enum M_Result m_sprite_create(M_Sprite **sprite, const M_Instance *instance,
                              const struct M_SpriteProperties *properties);
void m_sprite_destroy(M_Sprite *sprite, const M_Instance *instance);

#endif // !SPRITE_H
