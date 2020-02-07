#include "ros/ros.h"
#include "iostream"
#include "simulate.h"
#include "sim_data/sim_data.h"
#include "control_center/show_data.h"
#include "joy_data/control.h"
using namespace std;


double heading;
float x_gps;
float y_gps;
double turn_angle;
float speed;
int flag_1 = 0;
int flag_2 = 0;

void call_back(const control_center::show_data::ConstPtr &msg){
//    x_gps = msg->x;
//    y_gps = msg->y;
//    heading = msg->angle;
    flag_1++;
}
void call_back_1(const joy_data::control::ConstPtr &msg){
    turn_angle = msg->turn;
    flag_2++;
}

int main(int argc, char**argv){
    ros::init(argc, argv,"Simulate_node");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<sim_data::sim_data>("sim_data_topic", 10);
    ros::Subscriber sub = nh.subscribe("show_data_topic", 10, call_back);
    ros::Subscriber sub_1 = nh.subscribe("AGV_ControlData", 10, call_back_1);
    ros::Rate rate(20);
    sim_data::sim_data data;
    loaction x;
    while(ros::ok()){

        if(flag_1>0 && flag_2>0) {
            x = location_sim(x_gps, y_gps, 1, heading, turn_angle);
            x_gps = x.x;
            y_gps = x.y;
            heading = x.heading;
            data.X_gps = x_gps;
            data.Y_gps = y_gps;
            data.heading = heading;
            data.speed = speed;
            pub.publish(data);
            flag_1 = 0;
            flag_2 = 0;
        }

        cout<<"车仿真坐标位置："<<x.x<<","<<x.y<<"    车头角度："<<heading<<"    车轮设定转角:"<<turn_angle<<endl;


        ros::spinOnce();
        rate.sleep();
        pub.publish(data);
    }
    return 0;
}