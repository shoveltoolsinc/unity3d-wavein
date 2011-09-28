#ifndef IULIB_MAC_WAVE_IN_DEVICE_H__
#define IULIB_MAC_WAVE_IN_DEVICE_H__

#include <AudioToolbox/AudioToolbox.h>

struct Mac_WaveInDevice : iu_WaveInDevice<Mac_WaveInDevice>
{
    AudioQueueRef queue_;
    AudioQueueBufferRef queue_buffers_[NUM_BUFFERS];
    int num_channels_;
    int sample_bits_;

    // Static implementation methods
    static int InitStatic_Impl();
    
    static void DestroyStatic_Impl();
    
    static unsigned int GetNumDevices();
    
    // Methods
    Mac_WaveInDevice();
    
    // Implementation methods
    int Start_Impl(unsigned int device, iu_WaveInDeviceConfig const& config);

    void Stop_Impl();

private:
    // Internal methods
    OSStatus Open(iu_WaveInDeviceConfig const& config, float min_update_rate);

    void Close();
};

#endif
