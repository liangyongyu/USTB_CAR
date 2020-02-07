#include "frmudpserver.h"
#include "quiwidget.h"
#include <ros/ros.h>

 

int main(int argc, char *argv[] )
{
     QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    QFont font;
    font.setFamily(QUIConfig::FontName);
    font.setPixelSize(QUIConfig::FontSize);
    a.setFont(font);

    //设置编码以及加载中文翻译文件
    QUIHelper::setCode();
    QUIHelper::setTranslator(":/qt_zh_CN.qm");
    QUIHelper::setTranslator(":/widgets.qm");
    QUIHelper::initRand();

    App::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    App::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    App::readConfig();
    App::readSendData();
    App::readDeviceData();

    frmUdpServer w;

    ros::init(argc, argv, "INS_publish");
    ros::NodeHandle param;

    w.frmUdpServer1(param);
    w.INS_data.header.stamp = ros::Time::now();
    w.INS_data.header.frame_id = "base_link";
    w.Nav_data.header.stamp = ros::Time::now();
    w.Nav_data.header.frame_id = "base_link";
    ros::Rate r(10); // 10 hz
  
    ros::spinOnce();     //轮转一次,返回
    r.sleep();        //休眠


    w.setWindowTitle(QString("From_Udp_Server").arg(QUIHelper::getLocalIP()));
    w.show();
    
  
    return a.exec();
}




