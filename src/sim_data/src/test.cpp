//
// Created by lyy on 2020/1/13.
//
#include "simulate.h"
#include "control_center/show_data.h"
#include "ros/ros.h"
double heading = 0;
float x_gps;
float y_gps;
double turn_angle;
float speed;
int main(int argc, char **argv)
{
    loaction x;
    ros::init(argc, argv , "xxxx");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<control_center::show_data>("show_data_topic", 10);
    control_center::show_data msg1;
    ros::Rate rate(20);
    while(ros::ok()){
        x = location_sim(x_gps, y_gps, 1, heading, 15);
        x_gps = x.x;
        y_gps = x.y;
        heading = x.heading;

        msg1.x = x_gps;
        msg1.y = y_gps;
        msg1.angle = heading;
        msg1.A =1;
        msg1.B = 1;
        msg1.C =1;
        msg1.start =1;
        msg1.end = 1;
        pub.publish(msg1);
        cout<<x_gps<<"    "<<y_gps<<endl;
        rate.sleep();
    }

}
