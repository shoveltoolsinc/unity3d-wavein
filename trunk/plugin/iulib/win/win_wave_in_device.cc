#include "../wave_in_device.h"

namespace
{
    WAVEFORMATEX WAVEFORMATEX_Helper(iu_WaveInDeviceConfig const& config, WORD fmt)
    {
        WAVEFORMATEX wf;
        ::ZeroMemory(&wf, sizeof(wf));
        wf.wFormatTag = fmt;
        wf.nChannels = config.num_channels_;
        wf.nSamplesPerSec = config.sample_rate_;
        wf.wBitsPerSample = config.sample_bits_;
        wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nChannels * wf.wBitsPerSample) / 8;
        wf.nBlockAlign = (wf.nChannels * wf.wBitsPerSample) / 8; 
        return wf;
    }

    LRESULT WINAPI WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if(MM_WIM_DATA == msg)
        {
            HWAVEIN device_handle = reinterpret_cast<HWAVEIN>(wparam);
            WAVEHDR *wave_hdr = reinterpret_cast<WAVEHDR*>(lparam);
            waveInAddBuffer(device_handle, wave_hdr, sizeof(*wave_hdr));
        }

        return ::DefWindowProc(hwnd, msg, wparam, lparam);
    }

    void CALLBACK WaveInProc(HWAVEIN hwi, UINT msg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
    {
        Win_WaveInDevice *dev = reinterpret_cast<Win_WaveInDevice*>(dwInstance);
        assert(NULL != dev);
        if(NULL == dev)
            return;

        if(WIM_DATA == msg)
        {
            WAVEHDR *hdr = reinterpret_cast<WAVEHDR*>(dwParam1);
            assert(NULL != hdr);
            if(NULL == hdr)
                return;
            if(hdr->dwBytesRecorded)
                dev->callback_->Data(hdr->lpData, hdr->dwBytesRecorded, dev->sample_bits_, dev->num_channels_);
            if(dev->is_running_)
                ::PostMessage(Win_WaveInDevice::window_handle_, MM_WIM_DATA, reinterpret_cast<WPARAM>(hwi), dwParam1);
        }
    }
}

char const Win_WaveInDevice::WND_CLASS_NAME[] = "WaveIn_Message";
HINSTANCE Win_WaveInDevice::plugin_instance_;
HWND Win_WaveInDevice::window_handle_;

int Win_WaveInDevice::InitStatic_Impl()
{
    ATOM atom = 0;
    for(;;)
    {
        if(NULL == (plugin_instance_ = ::GetModuleHandle(NULL)))
            break;

        WNDCLASS wc;
        ::ZeroMemory(&wc, sizeof(wc));
        {
            wc.lpfnWndProc = WindowProc;
            wc.hInstance = plugin_instance_;
            wc.lpszClassName = WND_CLASS_NAME;
        }

        if(0 == (atom = ::RegisterClass(&wc)))
            break;

        // Create window
        window_handle_ = ::CreateWindow(WND_CLASS_NAME, NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, 0, plugin_instance_, 0);
        if(NULL == window_handle_)
            break;

        return 1;
    }

    if(0 != atom)
        ::UnregisterClass(WND_CLASS_NAME, plugin_instance_);
    plugin_instance_ = NULL;
    return 0;
}

void Win_WaveInDevice::DestroyStatic_Impl()
{
    ::DestroyWindow(window_handle_);
    window_handle_ = NULL;

    ::UnregisterClass(WND_CLASS_NAME, plugin_instance_);
    plugin_instance_ = NULL;
}

unsigned int Win_WaveInDevice::GetNumDevices()
{
    return waveInGetNumDevs();
}

Win_WaveInDevice::Win_WaveInDevice()
: iu_WaveInDevice(),
device_handle_(NULL),
sample_bits_(0),
num_channels_(0)
{
    ::ZeroMemory(buffers_, sizeof(buffers_));
}

int Win_WaveInDevice::Start_Impl(unsigned int device, iu_WaveInDeviceConfig const& config)
{
    // TODO: expose update rate
    if(MMSYSERR_NOERROR != Open(device, WAVEFORMATEX_Helper(config, WAVE_FORMAT_PCM), 20.0f))
        return 0;

    // Start device
    if(MMSYSERR_NOERROR != waveInStart(device_handle_))
    {
        Close();
        return 0;
    }

    return 1;
}

void Win_WaveInDevice::Stop_Impl()
{
    // Request device stop
    waveInStop(device_handle_);

    Close();
}

MMRESULT Win_WaveInDevice::Open(UINT id, WAVEFORMATEX const& wf, float min_update_rate)
{
    MMRESULT res;
    for(;;)
    {
        // Test format
        if(MMSYSERR_NOERROR != (res = waveInOpen(NULL, id, &wf, 0, 0, WAVE_FORMAT_QUERY)))
            break;

        // Open device
        if(MMSYSERR_NOERROR != (res = waveInOpen(&device_handle_, id, &wf, reinterpret_cast<DWORD_PTR>(WaveInProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION)))
            break;

        // Set up buffers
        const DWORD buffer_size = static_cast<DWORD>(wf.nAvgBytesPerSec / min_update_rate);

        int num_buffers_set_up = 0;
        for(int i = 0; i < NUM_BUFFERS; i++)
        {
            buffers_[i].dwBufferLength = buffer_size;
            buffers_[i].lpData = new CHAR[buffer_size];
            if(MMSYSERR_NOERROR != (res = waveInPrepareHeader(device_handle_, &buffers_[i], sizeof(buffers_[i]))))
                break;
            if(MMSYSERR_NOERROR != (res = waveInAddBuffer(device_handle_, &buffers_[i], sizeof(buffers_[i]))))
                break;
            num_buffers_set_up++;
        }

        if(num_buffers_set_up < NUM_BUFFERS)
            break;

        sample_bits_ = wf.wBitsPerSample;
        num_channels_ = wf.nChannels;
        return res;
    }

    Close();
    return res;
}

void Win_WaveInDevice::Close()
{
    // Return buffers to application
    waveInReset(device_handle_);

    // Tear down buffers
    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        assert(0 == (buffers_[i].dwFlags & WHDR_INQUEUE));
        waveInUnprepareHeader(device_handle_, &buffers_[i], sizeof(buffers_[i]));
        delete [] buffers_[i].lpData;
    }
    ::ZeroMemory(buffers_, sizeof(buffers_));

    // Close device
    waveInClose(device_handle_);
    device_handle_ = NULL;
}
