#ifndef IULIB_WAVE_IN_DEVICE_H__
#define IULIB_WAVE_IN_DEVICE_H__

#include "iulib.h"

struct iu_WaveInDeviceConfig
{
    static iu_WaveInDeviceConfig const STEREO_16BIT_44100HZ;

    int const num_channels_;
    int const sample_bits_;
    int const sample_rate_;
};

class iu_WaveInDeviceCallback
{
public:
    virtual ~iu_WaveInDeviceCallback() {}
    
    virtual void Data(void const* data, unsigned int num_bytes, int bits_per_sample, int num_channels) = 0;
};

template<class IMPL>
struct iu_WaveInDevice
{
    static int const NUM_BUFFERS = 3;
    static bool is_initialized_;
    bool is_running_;
    iu_WaveInDeviceCallback *callback_;
    
    // Static methods
    static int InitStatic();
    
    static void DestroyStatic();

    // Methods
    iu_WaveInDevice();

    int Start(unsigned int device, iu_WaveInDeviceConfig const& config);

    void Stop();

    inline void SetCallback(iu_WaveInDeviceCallback *callback)
    {
        callback_ = callback;
    }
};

#if (IU_WINDOWS)
#   include "win/win_wave_in_device.h"
typedef Win_WaveInDevice iu_WaveInDeviceType;
#elif (IU_MACOS)
#   include "mac/mac_wave_in_device.h"
typedef Mac_WaveInDevice iu_WaveInDeviceType;
#endif

#endif
