#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <vulkan/vulkan_core.h>

typedef struct M_Window M_Window;

enum M_Result m_window_create(M_Window **window, const char *title, int width, int height);
bool m_window_is_open(const M_Window *window);
void m_window_update(M_Window *window);
void m_window_destroy(M_Window *window);

#include "instance.h"

enum M_Result m_window_surface_create(const M_Window *window, M_Instance *instance);
void m_window_surface_destroy(M_Instance *instance);

#endif // WINDOW_H
