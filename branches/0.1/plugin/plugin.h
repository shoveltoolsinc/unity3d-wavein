#ifndef PLUGIN_H__
#define PLUGIN_H__

#include "iulib/wave_in_device.h"

#if (IU_WINDOWS)
#   define PLUGIN_DECL __stdcall
#else
#   define PLUGIN_DECL
#endif

// Callback
typedef void (PLUGIN_DECL *Plugin_CallbackFunc)(int num_samples, int16_t const *samples);

#endif
