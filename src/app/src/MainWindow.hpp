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

#pragma once

#include <QWidget>

class QSettings;
class QCheckBox;
class QVBoxLayout;
class QPushButton;
class QGroupBox;
class QSpinBox;
class QLabel;
class QMenuBar;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QObject *parent, QSettings *settings);
    ~MainWindow() override;

    void createWidgets();
    void createSpinBoxes();
    void createServerCheckBox();
    void createLayouts();
    void createEvents();

    void setServerRunning(bool running);
    void setListeners(int count);

    void onShowAboutDialog();

signals:
    void startButtonClicked();

private:
    void closeEvent(QCloseEvent *event) override;

    QSettings *m_iniSettings;
    QMenuBar *m_menuBar;

    QAction *m_hideAction,
        *m_exitAction,
        *m_websiteAction,
        *m_twitterAction,
        *m_aboutAction;

    QCheckBox *m_tcpServerCheckBox,
        *m_bluetoothServerCheckBox,
        *m_serverAutoStartCheckBox,
        *m_showWinStartupCheckBox,
        *m_hideWithCloseButtonCheckBox;

    QPushButton *m_startServerButton;
    QGroupBox *m_serverGroupBox;
    QSpinBox *m_tcpPortSpinBox;
    QLabel *m_tcpPortLabel, *m_statusLabel, *m_listenersLabel;

#ifdef Q_OS_WIN32
    bool m_useBluetooth;
#endif
};
