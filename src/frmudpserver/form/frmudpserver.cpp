#include "frmudpserver.h"
#include <ui_frmudpserver.h>
#include "quiwidget.h"
#include <cstdio>


frmUdpServer::frmUdpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmUdpServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}
void frmUdpServer:: frmUdpServer1(ros::NodeHandle param)
{   
  Ins_publish = param.advertise<sensor_msgs::Imu>("/IMU_data",10);//创建publisher
  Nav_publish = param.advertise<sensor_msgs::NavSatFix>("/Nav_data",10);
  Initial_data_publish = param.advertise<frmudpserver::Initial_data>("/Initial_Ins",1000);
        
}

frmUdpServer::~frmUdpServer()
{
    delete ui;
}

void frmUdpServer::initForm()
{
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(App::Intervals);
    ui->cboxData->addItems(App::Datas);
}

void frmUdpServer::initConfig()
{
    ui->ckHexSend->setChecked(App::HexSendUdpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(App::HexReceiveUdpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(App::AsciiUdpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(App::DebugUdpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(App::AutoSendUdpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(App::IntervalUdpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(App::UdpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(App::UdpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(App::UdpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    timer->setInterval(App::IntervalUdpServer);
    App::AutoSendUdpServer ? timer->start() : timer->stop();
}

void frmUdpServer::saveConfig()
{
    App::HexSendUdpServer = ui->ckHexSend->isChecked();
    App::HexReceiveUdpServer = ui->ckHexReceive->isChecked();
    App::AsciiUdpServer = ui->ckAscii->isChecked();
    App::DebugUdpServer = ui->ckDebug->isChecked();
    App::AutoSendUdpServer = ui->ckAutoSend->isChecked();
    App::IntervalUdpServer = ui->cboxInterval->currentText().toInt();
    App::UdpServerIP = ui->txtServerIP->text().trimmed();
    App::UdpServerPort = ui->txtServerPort->text().trimmed().toInt();
    App::UdpListenPort = ui->txtListenPort->text().trimmed().toInt();
    App::writeConfig();

    timer->setInterval(App::IntervalUdpServer);
    App::AutoSendUdpServer ? timer->start() : timer->stop();
}

void frmUdpServer::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (ui->ckShow->isChecked()) {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain->setTextColor(QColor("darkgreen"));
    } else {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("red"));
    }

    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}

void frmUdpServer::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;


    while (udpSocket->hasPendingDatagrams()) {
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(), data.size(), &host, &port);

        if (App::HexReceiveUdpServer) {
            buffer = QUIHelper::byteArrayToHexStr(data);
        } else if (App::AsciiUdpServer) {
            buffer = QUIHelper::byteArrayToAsciiStr(data);
        } else {
            buffer = QString(data);
        }

        QString ip = host.toString();
        if (ip.isEmpty()) {
            continue;
        }

        Initial_full_data.full_data = buffer.toStdString();
        Initial_data_publish.publish(Initial_full_data);

        //Gsof数据解析
        if (buffer.length() == 230)
        {   outFile.open("/home/agv/Desktop/path.txt",ios::app);
            if(outFile.fail()){
                cout<<"error\n";
            }
            // Imu融合状态
            QString Imu_alignment_status = buffer.mid(30,2);
            qDebug() << "Imu_alignment_status:" << Imu_alignment_status <<endl;
            // 设置输出精度；
            cout.precision(16);
            outFile.precision(16);
           //位置信息：Latitude、Longtitude、Altitude
            QString Latitude = buffer.mid(34,16);
              QByteArray ba_Lat=Latitude.toLatin1();
              char *c_Lat=ba_Lat.data();
              double val_Lat = frmUdpServer::str2double(c_Lat);
              Nav_data.latitude = val_Lat;
            cout << "Latitude: " << val_Lat << "    ";

            QString Longtitude = buffer.mid(50,16);
              QByteArray ba_Lot=Longtitude.toLatin1();
              char *c_Lot=ba_Lot.data();
              double val_Lot = frmUdpServer::str2double(c_Lot);
              Nav_data.longitude = val_Lot;
            cout << "Longitude: " << val_Lot <<"   ";

            QString Altitude = buffer.mid(66,16);
              QByteArray ba_Alt = Altitude.toLatin1();
              char *c_Alt=ba_Alt.data();
              double val_Alt = frmUdpServer::str2double(c_Alt);
              Nav_data.altitude = val_Alt;
            cout << "Altitude: " << val_Alt << endl;
           //速度信息
            QString North_velocity = buffer.mid(82,8);
              QByteArray ba_Nv=North_velocity.toLatin1();
              char *c_Nv=ba_Nv.data();
              double val_Nv = frmUdpServer::str2float(c_Nv);
            cout << "North Velocity: " << val_Nv << "   ";

            QString East_velocity = buffer.mid(90,8);
              QByteArray ba_Ev=East_velocity.toLatin1();
              char *c_Ev=ba_Ev.data();
              double val_Ev = frmUdpServer::str2float(c_Ev);
            cout << "East Velocity: " << val_Ev << "    ";

            QString Down_velocity = buffer.mid(98,8);
              QByteArray ba_Dv=Down_velocity.toLatin1();
              char *c_Dv=ba_Dv.data();
              double val_Dv = frmUdpServer::str2float(c_Dv);
            cout << "Down Velocity: " << val_Dv << "    ";

            QString Total_Speed = buffer.mid(106,8);
              QByteArray ba_TS=Total_Speed.toLatin1();
              char *c_TS=ba_TS.data();
              double val_TS = frmUdpServer::str2float(c_TS);
            cout << "Total_Speed: " << val_TS << endl;
           //方向信息
            QString Roll = buffer.mid(114,16);
              QByteArray ba_Roll = Roll.toLatin1();
              char *c_Roll=ba_Roll.data();
              double val_Roll = frmUdpServer::str2double(c_Roll);
              INS_data.orientation.x = val_Roll;
            cout << "Roll: " << val_Roll << "   ";

            QString Pitch = buffer.mid(130,16);
              QByteArray ba_Pitch= Pitch.toLatin1();
              char *c_Pitch=ba_Pitch.data();
              double val_Pitch = frmUdpServer::str2double(c_Pitch);
              INS_data.orientation.y = val_Pitch;
            cout << "Pitch: " << val_Pitch << " ";
            
            QString Heading = buffer.mid(146,16);
              QByteArray ba_Hd = Heading.toLatin1();
              char *c_Hd = ba_Hd.data();
              double val_Hd = frmUdpServer::str2double(c_Hd);
              INS_data.orientation.z = val_Hd;
            cout << "Heading: " << val_Hd << endl;
            //角速度
            QString Track_angle = buffer.mid(162,16);
              QByteArray ba_Ta=Track_angle.toLatin1();
              char *c_Ta=ba_Ta.data();
              double val_Ta = frmUdpServer::str2float(c_Ta);
            cout << "Track angle: " << val_Ta << "  ";

            QString angular_Xrate = buffer.mid(178,8);
              QByteArray ba_Xr=angular_Xrate.toLatin1();
              char *c_Xr=ba_Xr.data();
              double val_Xr = frmUdpServer::str2float(c_Xr);
              INS_data.angular_velocity.x = val_Xr;
            cout << "angular Xrate: " << val_Xr << "    ";

            QString angular_Yrate = buffer.mid(186,8);
              QByteArray ba_Yr=angular_Yrate.toLatin1();
              char *c_Yr=ba_Yr.data();
              double val_Yr = frmUdpServer::str2float(c_Yr);
              INS_data.angular_velocity.y = val_Yr;
            cout << "angular Yrate: " << val_Yr << "    ";


            QString angular_Zrate = buffer.mid(194,8);
              QByteArray ba_Zr=angular_Zrate.toLatin1();
              char *c_Zr=ba_Zr.data();
              double val_Zr = frmUdpServer::str2float(c_Zr);
              INS_data.angular_velocity.z = val_Zr;
            cout << "angular Zrate: " << val_Zr << endl;

            //加速度
            QString longitudinal_acceleration = buffer.mid(202,8);
              QByteArray ba_Lacc=longitudinal_acceleration.toLatin1();
              char *c_Lacc=ba_Lacc.data();
              double val_Lacc = frmUdpServer::str2float(c_Lacc);
              INS_data.linear_acceleration.x = val_Lacc;
            cout << "longitudinal acceleration: " << val_Lacc << "  ";

            QString Traverse_acceleration = buffer.mid(210,8);
              QByteArray ba_Tacc=Traverse_acceleration.toLatin1();
              char *c_Tacc=ba_Tacc.data();
              double val_Tacc = frmUdpServer::str2float(c_Tacc);
              INS_data.linear_acceleration.y = val_Tacc;
            cout << "Traverse acceleration: " << val_Tacc << "  ";

            QString Down_acceleration = buffer.mid(218,8);
              QByteArray ba_Dacc=Down_acceleration.toLatin1();
              char *c_Dacc=ba_Dacc.data();
              double val_Dacc = frmUdpServer::str2float(c_Dacc);
              INS_data.linear_acceleration.z = val_Dacc;
            cout << "Down acceleration: " << val_Dacc << endl;
            Nav_publish.publish(Nav_data);
            Ins_publish.publish(INS_data);
            outFile << "latitude: " << Nav_data.latitude << endl;
            outFile << "longitude: " << Nav_data.longitude << endl;
            outFile.close();



        } 

        QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(buffer);
        append(1, str);

        if (App::DebugUdpServer) {
            int count = App::Keys.count();
            for (int i = 0; i < count; i++) {
                if (App::Keys.at(i) == buffer) {
                    sendData(ip, port, App::Values.at(i));
                    break;
                }
            }
        }
    }
}

void frmUdpServer::sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    if (App::HexSendUdpServer) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else if (App::AsciiUdpServer) {
        buffer = QUIHelper::asciiStrToByteArray(data);
    } else {
        buffer = data.toLatin1();
    }

    udpSocket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);

}


float frmUdpServer::str2float(char* p)
{
    //char hexstr[9];
    char *pc=p;
    long int n=0;
    float * pf=(float*)&n;

    while(*pc)
    {
        if('0'<=*pc&&*pc<='9')
             *pc=*pc-'0';
        if('A'<=*pc&&*pc<='F')
            *pc= 10+*pc-'A';
        if('a'<=*pc&&*pc<='f')
             *pc= 10+*pc-'a';
        n=n*16+*pc;
        pc++;
     }
    return *pf;
}

double frmUdpServer::str2double(char* p)
{
    char *pc=p;
    long int n=0;
    double * pf=(double*)&n ;

    while(*pc)
    {
        if('0'<=*pc&&*pc<='9')
             *pc=*pc-'0';
        if('A'<=*pc&&*pc<='F')
            *pc= 10+*pc-'A';
        if('a'<=*pc&&*pc<='f')
             *pc= 10+*pc-'a';
        n=n*16+*pc;
        pc++;
     }
     return *pf;
}

void frmUdpServer::on_btnListen_clicked()
{



    if (ui->btnListen->text() == "监听") {
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
        bool ok = udpSocket->bind(QHostAddress::AnyIPv4, App::UdpListenPort);
#else
        bool ok = udpSocket->bind(QHostAddress::Any, App::UdpListenPort);
#endif



        if (ok) {
            ui->btnListen->setText("关闭");
            append(0, "监听成功");

    /*    int i = 10;
        while (i >= 0)
        {i--;
            outFile.open("/home/agv/Desktop/path.txt",ios::app);
            if(outFile.fail()){
                cout<<"error\n";
            }
         QString buffer = ("0228406de300003168081106f62c540201404405215f8a85cc405d1a3b0648802e4049f56db58d2782");

         Initial_full_data.full_data = buffer.toStdString();
         Initial_data_publish.publish(Initial_full_data);

         if (buffer.length() == 82)
         {
            QString Imu_alignment_status = buffer.mid(30,2);
            qDebug() << "Imu_alignment_status:" << Imu_alignment_status <<endl;
           //位置信息：Latitude、Longtitude、Altitude
            QString Latitude = buffer.mid(34,16);
              QByteArray ba_Lat=Latitude.toLatin1();
              char *c_Lat=ba_Lat.data();
              double val_Lat = frmUdpServer::str2double(c_Lat);
              Nav_data.latitude = val_Lat;
             cout.precision(20);
             outFile.precision(20);
            cout << "Latitude: " << val_Lat << "    ";

            QString Longtitude = buffer.mid(50,16);
              QByteArray ba_Lot=Longtitude.toLatin1();
              char *c_Lot=ba_Lot.data();
              double val_Lot = frmUdpServer::str2double(c_Lot);
              Nav_data.longitude = val_Lot;
            cout << "Longtitude: " << val_Lot <<"   ";

            QString Altitude = buffer.mid(66,16);
              QByteArray ba_Alt = Altitude.toLatin1();
              char *c_Alt=ba_Alt.data();
              double val_Alt = frmUdpServer::str2double(c_Alt);
              Nav_data.altitude = val_Alt;
            cout << "Altitude: " << val_Alt << endl;
            Nav_publish.publish(Nav_data);
            outFile << "latitude" << Nav_data.latitude << endl;
            outFile << "longitude" << Nav_data.longitude << endl;
            outFile.close();
          }
          else 
           qDebug() << "not gsof"; 
        }*/
           


        
     
        
        }
    } else {
        udpSocket->abort();
        ui->btnListen->setText("监听");
    }
}

void frmUdpServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    if (data.length() <= 0) {
        return;
    }

    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(STRDATETIME);
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(data.toUtf8());
        file.close();
    }

    on_btnClear_clicked();
}

void frmUdpServer::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmUdpServer::on_btnSend_clicked()
{
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(App::UdpServerIP, App::UdpServerPort, data);
}
