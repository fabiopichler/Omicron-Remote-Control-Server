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

#include "App.hpp"
#include "MainWindow.hpp"
#include "core/Config.hpp"
#include "core/Server.hpp"

#include <QJsonObject>
#include <QJsonDocument>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QMenu>
#include <QStandardPaths>
#include <QApplication>

App::App()
    : m_systemTrayIcon(nullptr)
    , m_iniSettings(nullptr)
    , m_window(nullptr)
    , m_server(new Server(this))
{
    config()->setPluginsPath(qApp->applicationDirPath());

    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    settingsPath.append(QStringLiteral(u"/Omicron Remote Control/Settings.ini"));

    m_iniSettings = new QSettings(settingsPath, QSettings::IniFormat);
    m_iniSettings->setIniCodec("UTF-8");

    initSystemTrayIcon();

    if (m_iniSettings->value(QStringLiteral(u"Server/AutoStart"), true).toBool())
        startButtonClicked();

    if (m_iniSettings->value(QStringLiteral(u"MainWindow/ShowAtStartup"), true).toBool())
        showWindow();
}

App::~App()
{
    delete m_systemTrayIcon;
    delete m_window;
    delete m_iniSettings;
    delete m_server;
}

void App::initSystemTrayIcon()
{
    m_systemTrayIcon = new QSystemTrayIcon(QIcon(QStringLiteral(u":/app-logo.png")), nullptr);

    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, this, &App::trayIconActivated);

    auto trayIconMenu = new QMenu;

    connect(trayIconMenu->addAction(QStringLiteral(u"Exibir/Ocultar Interface")), &QAction::triggered, this, &App::showWindow);
    trayIconMenu->addSeparator();
    connect(trayIconMenu->addAction(QStringLiteral(u"Sair")), &QAction::triggered, qApp, &QApplication::quit);

    m_systemTrayIcon->setContextMenu(trayIconMenu);
    m_systemTrayIcon->show();
}

void App::startButtonClicked()
{
    if (m_server->isRunning())
    {
        m_server->stop();
    }
    else
    {
        try
        {
            if (m_iniSettings->value(QStringLiteral(u"Server/Tcp"), true).toBool())
                m_server->startTcpServer(m_iniSettings->value(QStringLiteral(u"Server/TcpPort"), 4242).toInt());
        }
        catch (std::runtime_error err)
        {

        }

        try
        {
            if (m_iniSettings->value(QStringLiteral(u"Server/Bluetooth"), false).toBool())
                m_server->startBluetoothServer();
        }
        catch (std::runtime_error err)
        {

        }
    }
}

void App::showWindow()
{
    if (!m_window)
    {
        m_window = new MainWindow(this, m_iniSettings);
        m_window->setServerRunning(m_server->isRunning());
        m_window->setListeners(m_server->countListeners());

        connect(m_server, &Server::statusChanged, m_window, &MainWindow::setServerRunning);
        connect(m_server, &Server::updateListeners, m_window, &MainWindow::setListeners);
        connect(m_window, &MainWindow::startButtonClicked, this, &App::startButtonClicked);
    }

    if (!m_window->isHidden() && !m_window->isMinimized())
    {
        m_window->hide();
    }
    else
    {
        m_window->showNormal();
        m_window->activateWindow();
    }
}

void App::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            showWindow();
            break;

        case QSystemTrayIcon::DoubleClick:
            break;

        case QSystemTrayIcon::MiddleClick:
            if (m_window)
                m_window->activateWindow();
            break;

        case QSystemTrayIcon::Context:
            break;

        case QSystemTrayIcon::Unknown:
            break;
    }
}
