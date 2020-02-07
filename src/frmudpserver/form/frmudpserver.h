#ifndef FRMUDPSERVER_H
#define FRMUDPSERVER_H

#include <QWidget>
#include <QtNetwork>
#include <string>
#include <iostream>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/NavSatFix.h>
#include"ros/ros.h"
#include <fstream>
#include <frmudpserver/Initial_data.h>

using namespace std;



namespace Ui {
class frmUdpServer;
}

class frmUdpServer : public QWidget
{
    Q_OBJECT

public:
    explicit frmUdpServer(QWidget *parent = 0);
       ~frmUdpServer();
    
 public:
     void frmUdpServer1(ros::NodeHandle param);

private:
    Ui::frmUdpServer *ui;    

    QUdpSocket *udpSocket;
    QTimer *timer;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void append(int type, const QString &data, bool clear = false);

    void readData();
    void sendData(const QString &ip, int port, const QString &data);

private slots:
    void on_btnListen_clicked();
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
public:
    static float str2float(char* p);
    static double str2double(char* p);
public:
    ros::Publisher Ins_publish;
    ros::Publisher Nav_publish;
    ros::Publisher Initial_data_publish;


    sensor_msgs::Imu INS_data;
    sensor_msgs::NavSatFix Nav_data;
    frmudpserver::Initial_data Initial_full_data;
    // 文件处理
    std::ofstream outFile;



};
#endif // FRMUDPSERVER_H
