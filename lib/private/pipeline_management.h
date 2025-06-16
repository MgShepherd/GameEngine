#ifndef PIPELINE_MANAGEMENT_H
#define PIPELINE_MANAGEMENT_H

#include "instance.h"

enum M_Result m_pipeline_create(M_Instance *instance);
void m_pipeline_destroy(M_Instance *instance);

#endif // !PIPELINE_MANAGEMENT_H
