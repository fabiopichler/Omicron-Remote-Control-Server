/*-------------------------------------------------------------------------------

Copyright (c) 2023 Fábio Pichler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------------------------------------------------------*/

#include "MasterVolume_Win32.hpp"
#include "AudioEndpointVolumeCallback.hpp"

#include <thread>

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include <QDebug>

IAudioEndpointVolume *EndpointVolume_Init();

float MasterVolume::get()
{
    IAudioEndpointVolume *endpointVolume = EndpointVolume_Init();

    float currentVolume = 0;

    endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

    endpointVolume->Release();
    CoUninitialize();

    return currentVolume;
}

void MasterVolume::set(float volume)
{
    IAudioEndpointVolume *endpointVolume = EndpointVolume_Init();

    endpointVolume->SetMasterVolumeLevelScalar(volume, nullptr);

    endpointVolume->Release();
    CoUninitialize();
}

void MasterVolume::monitor(const std::function<void()> &func, const bool &stop)
{
    IAudioEndpointVolume *endpointVolume = EndpointVolume_Init();

    AudioEndpointVolumeCallback *volumeCallback = new AudioEndpointVolumeCallback(std::move(func));
    endpointVolume->RegisterControlChangeNotify(volumeCallback);

    while (!stop)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    endpointVolume->UnregisterControlChangeNotify(volumeCallback);
    endpointVolume->Release();
    volumeCallback->Release();

    CoUninitialize();
}

IAudioEndpointVolume *EndpointVolume_Init()
{
    CoInitialize(nullptr);

    IMMDeviceEnumerator *deviceEnumerator = nullptr;

    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID *>(&deviceEnumerator));

    IMMDevice *defaultDevice = nullptr;

    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = nullptr;

    IAudioEndpointVolume *endpointVolume = nullptr;

    defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<LPVOID *>(&endpointVolume));
    defaultDevice->Release();
    defaultDevice = nullptr;

    return endpointVolume;
}
