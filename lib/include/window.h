#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

typedef struct M_Window M_Window;

M_Window *m_window_create(const char *title, int width, int height);
bool m_window_is_open(const M_Window *window);
void m_window_update(M_Window *window);
void m_window_destroy(M_Window *window);

#endif // WINDOW_H
