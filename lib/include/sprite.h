#ifndef SPRITE_H
#define SPRITE_H

typedef struct M_Sprite M_Sprite;

#include "instance.h"

enum M_Result m_sprite_create(M_Sprite **sprite, const M_Instance *instance);
void m_sprite_destroy(M_Sprite *sprite, const M_Instance *instance);

#endif // !SPRITE_H
