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

#include "MasterVolumePlugin.hpp"

#include <cmath>
#include <thread>

#ifdef __unix
  #include "linux/MasterVolume_Alsa.hpp"
#else
  #include "windows/MasterVolume_Win32.hpp"
#endif

ORC_PLUGIN_MAIN(MasterVolumePlugin)

MasterVolumePlugin::MasterVolumePlugin(QObject *parent)
    : PluginBase(QStringLiteral(u"master_volume"), parent)
    , m_stop(false)
    , m_isMuted(false)
    , m_volume(0.f)
{
    m_data = MasterVolumePlugin::onExec(QJsonObject());

    std::thread(&MasterVolumePlugin::run, this).detach();
}

MasterVolumePlugin::~MasterVolumePlugin()
{
    m_stop = true;
}

void MasterVolumePlugin::run()
{
    MasterVolume::monitor([=] {
        QJsonObject data = onExec(QJsonObject());

        if (m_data == data)
            return;

        m_data = data;

        emit update(getData());
    }, m_stop);
}

QJsonObject MasterVolumePlugin::onExec(const QJsonObject &data)
{
    float volume = MasterVolume::get();
    QString action = data[QStringLiteral(u"action")].toString();
    QString value = data[QStringLiteral(u"value")].toString();

    if (action == QStringLiteral(u"up")) {
        if (m_isMuted)
            volume = m_volume;

         volume += 0.01f;

        if (volume > 1.f)
            volume = 1.f;

        m_isMuted = false;
        MasterVolume::set(volume);

    } else if (action == QStringLiteral(u"down")) {
        if (m_isMuted)
            volume = m_volume;

        volume -= 0.01f;

        if (volume < 0.f)
            volume = 0.f;

        m_isMuted = false;
        MasterVolume::set(volume);

    } else if (action == QStringLiteral(u"set_level")) {
        volume = float(value.toInt()) / 100.f;

        if (volume < 0.f)
            volume = 0.f;

        else if (volume > 1.f)
            volume = 1.f;

        m_isMuted = false;
        MasterVolume::set(volume);

    } else if (action == QStringLiteral(u"mute")) {
        if (m_isMuted) {
            m_isMuted = false;
            volume = m_volume;

        } else {
            m_volume = volume;
            volume = 0.f;
            m_isMuted = true;
        }

        MasterVolume::set(volume);
    }

    QJsonObject responseJson;

    responseJson.insert(QStringLiteral(u"level"), static_cast<int>(std::round((m_isMuted ? m_volume : volume) * 100.f)));
    responseJson.insert(QStringLiteral(u"muted"), m_isMuted);

    return responseJson;
}
