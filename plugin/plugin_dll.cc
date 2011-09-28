#include "plugin.h"

// Plugin interface
extern "C"
{
    int PLUGIN_DECL WaveIn_Init();
    void PLUGIN_DECL WaveIn_Destroy();
    unsigned int PLUGIN_DECL WaveIn_GetNumDevices();
    int PLUGIN_DECL WaveIn_StartDevice(unsigned int id, Plugin_CallbackFunc callback_func);
    void PLUGIN_DECL WaveIn_StopDevice();
}

namespace
{
    // Device
    iu_WaveInDeviceType s_Device;

    // Callback
    Plugin_CallbackFunc s_CallbackFunc = NULL;

    class Plugin_Callback : public iu_WaveInDeviceCallback
    {
    public:
        void Data(void const* data, unsigned int num_bytes, int bits_per_sample, int num_channels)
        {
            switch(bits_per_sample)
            {
            case 16:
                unsigned int const sample_bytes = bits_per_sample / 8;
                unsigned int const num_interleaved_samples = num_bytes / sample_bytes;
                if(s_CallbackFunc)
                    s_CallbackFunc(num_interleaved_samples, static_cast<int16_t const*>(data));
                break;
            }
        }
    } s_Callback;
}

int PLUGIN_DECL WaveIn_Init()
{
    int ret;
    if(1 != (ret = iu_WaveInDeviceType::InitStatic()))
        return ret;

    s_Device.SetCallback(&s_Callback);
    return 1;
}

void PLUGIN_DECL WaveIn_Destroy()
{
    s_Device.Stop();
    s_Device.SetCallback(NULL);

    iu_WaveInDeviceType::DestroyStatic();
}

unsigned int PLUGIN_DECL WaveIn_GetNumDevices()
{
    return s_Device.GetNumDevices();
}

int PLUGIN_DECL WaveIn_StartDevice(unsigned int id, Plugin_CallbackFunc callback_func)
{
    s_Device.Stop();

    s_CallbackFunc = callback_func;

    return s_Device.Start(id, iu_WaveInDeviceConfig::STEREO_16BIT_44100HZ);
}

void PLUGIN_DECL WaveIn_StopDevice()
{
    s_Device.Stop();
}
