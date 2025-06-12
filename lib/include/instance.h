#ifndef INSTANCE_H
#define INSTANCE_H

#include "window.h"

typedef struct {
  bool enable_debug;
  const char *app_name;
} M_InstanceOptions;

typedef struct M_Instance M_Instance;

enum M_Result m_instance_create(M_Instance **instance, const M_InstanceOptions *instance_options);
void m_instance_destroy(M_Instance *instance);

#endif // !INSTANCE_H
