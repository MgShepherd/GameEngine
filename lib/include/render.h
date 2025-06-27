#ifndef M_RENDER_H
#define M_RENDER_H

#include "instance.h"
#include "sprite.h"

enum M_Result m_render_begin(M_Instance *instance);
enum M_Result m_render_end(M_Instance *instance);

enum M_Result m_render_sprite(const M_Instance *instance, const M_Sprite *sprite);
enum M_Result m_render_sprites(const M_Instance *instance, M_Sprite **sprites, uint32_t num_sprites);

#endif // !M_RENDER_H
