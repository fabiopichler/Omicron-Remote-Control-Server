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

#include "MainWindow.hpp"
#include "AboutDialog.hpp"

#include <QSettings>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QSysInfo>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QFontDatabase>
#include <QCloseEvent>

MainWindow::MainWindow(QObject *, QSettings *settings)
    : m_iniSettings(settings)
{
#ifdef Q_OS_WIN32
    m_useBluetooth = (QSysInfo::productVersion() == QStringLiteral(u"10")
                      || QSysInfo::productVersion() == QStringLiteral(u"11"));
#endif

    QFontDatabase::addApplicationFont(QStringLiteral(u":/fonts/NotoSans/NotoSans-Regular.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(u":/fonts/NotoSans/NotoSans-Bold.ttf"));

    QFile css(QStringLiteral(u":/style.css"));

    if (css.open(QFile::ReadOnly | QFile::Text))
    {
        qApp->setStyleSheet(css.readAll());
        css.close();
    }

    createWidgets();
    createLayouts();
    createEvents();

    setWindowIcon(QIcon(QStringLiteral(u":/app-logo.png")));
    setWindowTitle(QStringLiteral(u"Omicron Remote Control - Server"));
}

MainWindow::~MainWindow()
{
    QFontDatabase::removeAllApplicationFonts();
}

void MainWindow::createWidgets()
{
    m_menuBar = new QMenuBar;

    auto fileMenu = new QMenu(QStringLiteral(u"Arquivo"));
    m_hideAction = fileMenu->addAction(QStringLiteral(u"Ocultar a janela"));
    fileMenu->addSeparator();
    m_exitAction = fileMenu->addAction(QStringLiteral(u"Sair"));
    m_menuBar->addMenu(fileMenu);

    auto aboutMenu = new QMenu(QStringLiteral(u"Ajuda"));
    m_websiteAction = aboutMenu->addAction(QStringLiteral(u"Visitar o website oficial"));
    m_twitterAction = aboutMenu->addAction(QStringLiteral(u"Perfil no Twitter"));
    aboutMenu->addSeparator();
    m_aboutAction = aboutMenu->addAction(QStringLiteral(u"Sobre"));
    m_menuBar->addMenu(aboutMenu);

    createServerCheckBox();
    createSpinBoxes();

    m_statusLabel = new QLabel;
    m_listenersLabel = new QLabel;

    m_serverAutoStartCheckBox = new QCheckBox;
    m_serverAutoStartCheckBox->setText(QStringLiteral(u"Iniciar servidor ao abrir o programa"));
    m_serverAutoStartCheckBox->setChecked(m_iniSettings->value(QStringLiteral(u"Server/AutoStart"), true).toBool());

    m_showWinStartupCheckBox = new QCheckBox;
    m_showWinStartupCheckBox->setText(QStringLiteral(u"Abrir janela ao abrir o programa"));
    m_showWinStartupCheckBox->setChecked(m_iniSettings->value(QStringLiteral(u"MainWindow/ShowAtStartup"), true).toBool());

    m_hideWithCloseButtonCheckBox = new QCheckBox;
    m_hideWithCloseButtonCheckBox->setText(QStringLiteral(u"Ocultar janela com o botão fechar"));
    m_hideWithCloseButtonCheckBox->setChecked(m_iniSettings->value(QStringLiteral(u"MainWindow/HideWithCloseButton"), true).toBool());

    m_startServerButton = new QPushButton;
}

void MainWindow::createSpinBoxes()
{
    m_tcpPortLabel = new QLabel(QStringLiteral(u"Porta da Rede Local (TCP):"));

    m_tcpPortSpinBox = new QSpinBox;
    m_tcpPortSpinBox->setRange(0, 49151);
    m_tcpPortSpinBox->setSingleStep(1);
    m_tcpPortSpinBox->setValue(m_iniSettings->value(QStringLiteral(u"Server/TcpPort"), 4242).toInt());
}

void MainWindow::createServerCheckBox()
{
    m_tcpServerCheckBox = new QCheckBox;
    m_tcpServerCheckBox->setText(QStringLiteral(u"Rede local (TCP)"));
    m_tcpServerCheckBox->setChecked(m_iniSettings->value(QStringLiteral(u"Server/Tcp"), true).toBool());

    m_bluetoothServerCheckBox = new QCheckBox;
    m_bluetoothServerCheckBox->setText(QStringLiteral(u"Bluetooth"));
    m_bluetoothServerCheckBox->setChecked(m_iniSettings->value(QStringLiteral(u"Server/Bluetooth"), false).toBool());
#ifdef Q_OS_WIN32
    m_bluetoothServerCheckBox->setEnabled(m_useBluetooth);
#endif

    auto serverLayout = new QVBoxLayout;
    serverLayout->addWidget(m_tcpServerCheckBox);
    serverLayout->addWidget(m_bluetoothServerCheckBox);

    m_serverGroupBox = new QGroupBox(QStringLiteral(u"Usar conexão:"));
    m_serverGroupBox->setLayout(serverLayout);
}

void MainWindow::createLayouts()
{
    auto statusLayout = new QVBoxLayout;
    statusLayout->addWidget(m_statusLabel, 0, Qt::AlignHCenter);
    statusLayout->addWidget(m_listenersLabel, 0, Qt::AlignHCenter);

    auto statusGroupBox = new QGroupBox;
    statusGroupBox->setStyleSheet(QStringLiteral(u"QGroupBox { padding-top: 0em; margin-top: 0.2em; background:rgba(255,255,255,.1) }"));
    statusGroupBox->setLayout(statusLayout);

    auto serverLayout = new QVBoxLayout;
    serverLayout->addWidget(statusGroupBox);
    serverLayout->addWidget(m_serverGroupBox);
    serverLayout->addWidget(m_tcpPortLabel);
    serverLayout->addWidget(m_tcpPortSpinBox);
    serverLayout->addWidget(m_serverAutoStartCheckBox);
    serverLayout->addWidget(m_startServerButton);

    auto serverGroupBox = new QGroupBox(QStringLiteral(u"Servidor"));
    serverGroupBox->setLayout(serverLayout);

    auto mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(m_menuBar);
    mainLayout->addWidget(serverGroupBox);
    mainLayout->addWidget(m_showWinStartupCheckBox);
    mainLayout->addWidget(m_hideWithCloseButtonCheckBox);

    setLayout(mainLayout);
}

void MainWindow::createEvents()
{
    connect(m_hideAction, &QAction::triggered, this, &MainWindow::hide);
    connect(m_exitAction, &QAction::triggered, qApp, &QApplication::quit);

    connect(m_websiteAction, &QAction::triggered, [] {
        QDesktopServices::openUrl(QUrl(QStringLiteral(u"https://fabiopichler.net/")));
    });

    connect(m_twitterAction, &QAction::triggered, [] {
        QDesktopServices::openUrl(QUrl(QStringLiteral(u"https://twitter.com/FabioPichler")));
    });

    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onShowAboutDialog);

    connect(m_tcpServerCheckBox, &QCheckBox::toggled, [=](bool checked) {
        m_iniSettings->setValue(QStringLiteral(u"Server/Tcp"), checked);
    });

    connect(m_bluetoothServerCheckBox, &QCheckBox::toggled, [=](bool checked) {
        m_iniSettings->setValue(QStringLiteral(u"Server/Bluetooth"), checked);
    });

    connect(m_serverAutoStartCheckBox, &QCheckBox::toggled, [=](bool checked) {
        m_iniSettings->setValue(QStringLiteral(u"Server/AutoStart"), checked);
    });

    connect(m_showWinStartupCheckBox, &QCheckBox::toggled, [=](bool checked) {
        m_iniSettings->setValue(QStringLiteral(u"MainWindow/ShowAtStartup"), checked);
    });

    connect(m_hideWithCloseButtonCheckBox, &QCheckBox::toggled, [=](bool checked) {
        m_iniSettings->setValue(QStringLiteral(u"MainWindow/HideWithCloseButton"), checked);
    });

    connect(m_startServerButton, &QPushButton::clicked, this, &MainWindow::startButtonClicked);

    auto valueChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(m_tcpPortSpinBox, valueChanged, [=](int value) {
        m_iniSettings->setValue(QStringLiteral(u"Server/TcpPort"), value);
    });
}

void MainWindow::setServerRunning(bool running)
{
    m_statusLabel->setStyleSheet(running ? QStringLiteral(u"color: #0B5; font-weight: bold") : QStringLiteral(u"color: #F44"));
    m_statusLabel->setText(running ? QStringLiteral(u"Online") : QStringLiteral(u"Parado"));
    m_startServerButton->setText(running ? QStringLiteral(u"Parar") : QStringLiteral(u"Iniciar"));

    m_tcpServerCheckBox->setEnabled(!running);
    m_tcpPortSpinBox->setEnabled(!running);

#ifdef Q_OS_WIN32
    if (m_useBluetooth)
#endif
        m_bluetoothServerCheckBox->setEnabled(!running);
}

void MainWindow::setListeners(int count)
{
    m_listenersLabel->setText(QString(QStringLiteral(u"Clientes: %1")).arg(count));
}

void MainWindow::onShowAboutDialog()
{
    AboutDialog(this).exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_iniSettings->value(QStringLiteral(u"MainWindow/HideWithCloseButton"), true).toBool())
    {
        hide();
        event->ignore();
    }
    else
    {
        //event->accept();
        qApp->quit();
    }
}
