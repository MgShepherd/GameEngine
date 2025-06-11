#ifndef INSTANCE_H
#define INSTANCE_H

#include "window.h"

typedef struct M_Instance M_Instance;

enum M_Result m_instance_create(M_Instance **instance, const char *app_name);
void m_instance_destroy(M_Instance *instance);

#endif // !INSTANCE_H
