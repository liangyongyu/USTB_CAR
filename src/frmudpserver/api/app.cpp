#include "app.h"
#include "quiwidget.h"

QString App::ConfigFile = "config.ini";
QString App::SendFileName = "send.txt";
QString App::DeviceFileName = "device.txt";
bool App::HexSendUdpServer = false;
bool App::HexReceiveUdpServer = false;
bool App::AsciiUdpServer = false;
bool App::DebugUdpServer = false;
bool App::AutoSendUdpServer = false;
int App::IntervalUdpServer = 1000;
int App::UdpListenPort = 6060;
QString App::UdpServerIP = "196.128.0.200";
int App::UdpServerPort = 6000;

void App::readConfig()
{
    if (!checkConfig()) {
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("UdpServerConfig");
    App::HexSendUdpServer = set.value("HexSendUdpServer").toBool();
    App::HexReceiveUdpServer = set.value("HexReceiveUdpServer").toBool();
    App::AsciiUdpServer = set.value("AsciiUdpServer").toBool();
    App::DebugUdpServer = set.value("DebugUdpServer").toBool();
    App::AutoSendUdpServer = set.value("AutoSendUdpServer").toBool();
    App::IntervalUdpServer = set.value("IntervalUdpServer").toInt();
    App::UdpServerIP = set.value("UdpServerIP").toString();
    App::UdpServerPort = set.value("UdpServerPort").toInt();
    App::UdpListenPort = set.value("UdpListenPort").toInt();
    set.endGroup();
}

void App::writeConfig()
{
    QSettings set(App::ConfigFile, QSettings::IniFormat);


    set.beginGroup("UdpServerConfig");
    set.setValue("HexSendUdpServer", App::HexSendUdpServer);
    set.setValue("HexReceiveUdpServer", App::HexReceiveUdpServer);
    set.setValue("DebugUdpServer", App::DebugUdpServer);
    set.setValue("AutoSendUdpServer", App::AutoSendUdpServer);
    set.setValue("IntervalUdpServer", App::IntervalUdpServer);
    set.setValue("UdpServerIP", App::UdpServerIP);
    set.setValue("UdpServerPort", App::UdpServerPort);
    set.setValue("UdpListenPort", App::UdpListenPort);
    set.endGroup();
}

void App::newConfig()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#endif
    writeConfig();
}

bool App::checkConfig()
{
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(App::ConfigFile);
    if (file.size() == 0) {
        newConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            newConfig();
            return false;
        }
    } else {
        newConfig();
        return false;
    }

    return true;
}

QStringList App::Intervals = QStringList();
QStringList App::Datas = QStringList();
QStringList App::Keys = QStringList();
QStringList App::Values = QStringList();

void App::readSendData()
{
    //读取发送数据列表
    App::Datas.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg(App::SendFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                App::Datas.append(line);
            }
        }

        file.close();
    }
}

void App::readDeviceData()
{
    //读取转发数据列表
    App::Keys.clear();
    App::Values.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg(App::DeviceFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                App::Keys.append(key);
                App::Values.append(value);
            }
        }

        file.close();
    }
}
