#ifndef IULIB_WIN_WAVE_IN_DEVICE_H__
#define IULIB_WIN_WAVE_IN_DEVICE_H__

#include <mmsystem.h>

struct Win_WaveInDevice : iu_WaveInDevice<Win_WaveInDevice>
{
    static char const WND_CLASS_NAME[];
    static HINSTANCE plugin_instance_;
    static HWND window_handle_;

    HWAVEIN device_handle_;
    WORD sample_bits_;
    WORD num_channels_;
    WAVEHDR buffers_[NUM_BUFFERS];

    // Static methods
    static int InitStatic_Impl();

    static void DestroyStatic_Impl();

    static unsigned int GetNumDevices();

    // Methods
    Win_WaveInDevice();

    int Start_Impl(unsigned int device, iu_WaveInDeviceConfig const& config);

    void Stop_Impl();

private:
    // Internal methods
    MMRESULT Open(UINT id, WAVEFORMATEX const& wf, float min_update_rate);

    void Close();
};

#endif
