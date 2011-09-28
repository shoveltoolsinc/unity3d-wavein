#include "wave_in_device.h"

iu_WaveInDeviceConfig const iu_WaveInDeviceConfig::STEREO_16BIT_44100HZ = { 2, 16, 44100 };

template<class IMPL>
bool iu_WaveInDevice<IMPL>::is_initialized_;

template<class IMPL>
int iu_WaveInDevice<IMPL>::InitStatic()
{
    if(1 != IMPL::InitStatic_Impl())
        return 0;
    
    is_initialized_ = true;
    return 1;
}

template<class IMPL>
void iu_WaveInDevice<IMPL>::DestroyStatic()
{
    if(!is_initialized_)
        return;
    
    is_initialized_ = false;
    IMPL::DestroyStatic_Impl();
}

template<class IMPL>
iu_WaveInDevice<IMPL>::iu_WaveInDevice()
: is_running_(false),
callback_(NULL)
{
}

template<class IMPL>
int iu_WaveInDevice<IMPL>::Start(unsigned int device, iu_WaveInDeviceConfig const& config)
{
    if(is_running_)
        return 0;
    
    if(1 != static_cast<IMPL*>(this)->Start_Impl(device, config))
        return 0;
    
    is_running_ = true;
    return 1;
}

template<class IMPL>
void iu_WaveInDevice<IMPL>::Stop()
{
    if(!is_running_)
        return;
    
    is_running_ = false;
    static_cast<IMPL*>(this)->Stop_Impl();
}

template struct iu_WaveInDevice<iu_WaveInDeviceType>;
