#include"ros/ros.h"
#include"frmudpserver.h"
#include "frmudpserver.cpp"





int main1(int argc, char **argv)
{
    ros::init(argc, argv, "INS_publish");

    ros::NodeHandle param("~");
    frmUdpServer t1;
    t1.frmUdpServer1(param);


     //---test---
    ros::spin();

    printf("\n\t Map Build \n\n");
    return 0;
}

// int main (int argc, char** argv)
// {
//     ros::init(argc,argv,"INS_publish");//命名节点
//     ros::NodeHandle n;//创建句柄



//     ros::Publisher Ins_publish = n.advertise<sensor_msgs::Imu>("IMU_data",10);//创建publisher
//     ros::Publisher Nav_publish = n.advertise<sensor_msgs::NavSatFix>("Nav_data",10);
//     ros::Rate loop_rate(60);//每秒发送数据的频率

   /* while (ros::ok())
    {
        //发布IMU数据
        INS_data.header.stamp = ros::Time::now();
        INS_data.header.frame_id = "base_link";
        ROS_INFO("Publish Imu Info:Roll:%lf Pitch:%lf Heading:%lf",
                    INS_data.orientation.x, INS_data.orientation.y, INS_data.orientation.z);
        ROS_INFO("angular_Xrate:%lf angular_Yrate:%lf angular_Zrate:%lf",
                    INS_data.angular_velocity.x, INS_data.angular_velocity.y, INS_data.angular_velocity.z);
        ROS_INFO("Longitudinal_acceleration:%lf  Traverse_acceleration:%lf Down_acceleration:%lf\n",
                    INS_data.linear_acceleration.x, INS_data.linear_acceleration.y, INS_data.linear_acceleration.z);
        Ins_publish.publish(INS_data); //发布消息

        //发布定位数据
        Nav_data.header.stamp = ros::Time::now();
        Nav_data.header.frame_id = "base_link";
        ROS_INFO("Publish Navgation Info:Latitude:%lf  Longtitude:%lf  Altitude:%lf\n",
                    Nav_data.latitude, Nav_data.longitude, Nav_data.altitude);
        Nav_publish.publish(Nav_data); //发布消息
        //ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;*/


    


//}