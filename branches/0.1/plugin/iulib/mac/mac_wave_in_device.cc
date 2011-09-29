#include "../wave_in_device.h"

namespace
{
    AudioStreamBasicDescription AudioStreamBasicDescription_Helper(iu_WaveInDeviceConfig const& config, UInt32 fmt)
    {
        AudioStreamBasicDescription desc;
        memset(&desc, 0, sizeof(desc));
        desc.mFormatID = fmt;
        desc.mSampleRate = config.sample_rate_;
        desc.mChannelsPerFrame = config.num_channels_;
        desc.mBitsPerChannel = config.sample_bits_;
        desc.mBytesPerPacket = desc.mBytesPerFrame = desc.mChannelsPerFrame * desc.mBitsPerChannel / 8;
        desc.mFramesPerPacket = 1;
        desc.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
        return desc;
    }
    
    void AudioQueueInput(void *ud, AudioQueueRef inAQ, AudioQueueBufferRef inBuf, AudioTimeStamp const* inStartTime, UInt32 inNumberPacketDesc, AudioStreamPacketDescription const* inPacketDescs)
    {
        Mac_WaveInDevice *dev = static_cast<Mac_WaveInDevice*>(ud);
        assert(NULL != dev);
        if(NULL != dev)
        {
            dev->callback_->Data(inBuf->mAudioData, inBuf->mAudioDataByteSize, dev->sample_bits_, dev->num_channels_);
            if(dev->is_running_)
                AudioQueueEnqueueBuffer(inAQ, inBuf, 0, NULL);
        }
    }
}

int Mac_WaveInDevice::InitStatic_Impl()
{
    return 1;
}

void Mac_WaveInDevice::DestroyStatic_Impl()
{
}

unsigned int Mac_WaveInDevice::GetNumDevices()
{
    return 1;
}

Mac_WaveInDevice::Mac_WaveInDevice()
: iu_WaveInDevice<Mac_WaveInDevice>(),
queue_(NULL)
{
    memset(queue_buffers_, 0, sizeof(queue_buffers_));
}

int Mac_WaveInDevice::Start_Impl(unsigned int device, iu_WaveInDeviceConfig const& config)
{
    OSStatus err;
    
    for(;;)
    {
        if(0 != (err = Open(config, 20.0f)))
            break;

        if(0 != (err = AudioQueueStart(queue_, NULL)))
            break;
        
        return 1;
    }
    
    Close();
    return err;
}

void Mac_WaveInDevice::Stop_Impl()
{
    AudioQueueStop(queue_, true);
    
    Close();
}

OSStatus Mac_WaveInDevice::Open(iu_WaveInDeviceConfig const& config, float min_update_rate)
{
    AudioStreamBasicDescription const desc = AudioStreamBasicDescription_Helper(config, kAudioFormatLinearPCM);
    
    OSStatus err;
    for(;;)
    {
        if(0 != (err = AudioQueueNewInput(&desc, AudioQueueInput, this, NULL, kCFRunLoopCommonModes, 0, &queue_)))
            break;
        
        size_t const buffer_size = desc.mSampleRate * desc.mBytesPerFrame / min_update_rate;
        int num_buffers_set_up = 0;
        for(int i = 0; i < NUM_BUFFERS; i++)
        {
            if(0 != (err = AudioQueueAllocateBuffer(queue_, buffer_size, &queue_buffers_[i])))
                break;
            if(0 != (err = AudioQueueEnqueueBuffer(queue_, queue_buffers_[i], 0, NULL)))
                break;
            num_buffers_set_up++;
        }
        
        if(num_buffers_set_up < NUM_BUFFERS)
            break;
        
        sample_bits_ = desc.mBitsPerChannel;
        num_channels_ = desc.mChannelsPerFrame;
        return 0;
    }
    
    Close();
    return err;
}

void Mac_WaveInDevice::Close()
{
    if(queue_)
        AudioQueueDispose(queue_, true);
    queue_ = NULL;
}
