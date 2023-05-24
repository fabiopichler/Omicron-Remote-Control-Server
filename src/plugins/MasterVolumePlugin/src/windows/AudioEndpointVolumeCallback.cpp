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

#include "AudioEndpointVolumeCallback.hpp"

AudioEndpointVolumeCallback::AudioEndpointVolumeCallback(const std::function<void()> &func)
    : m_refCount(1)
    , m_function(std::move(func))
{

}

HRESULT STDMETHODCALLTYPE AudioEndpointVolumeCallback::QueryInterface(REFIID riid, void **ppvObject)
{
    if (riid == IID_IUnknown || riid == __uuidof(IAudioEndpointVolumeCallback))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();

        return S_OK;
    }

    *ppvObject = nullptr;

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE AudioEndpointVolumeCallback::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE AudioEndpointVolumeCallback::Release()
{
    ULONG ref = InterlockedDecrement(&m_refCount);

    if (ref == 0)
        delete this;

    return ref;
}

HRESULT STDMETHODCALLTYPE AudioEndpointVolumeCallback::OnNotify(AUDIO_VOLUME_NOTIFICATION_DATA *)
{
    if (m_function)
        m_function();

    return S_OK;
}
