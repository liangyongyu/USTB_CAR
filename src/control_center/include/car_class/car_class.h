//
// Created by lyy on 2019/11/23.
//

#ifndef SRC_CAR_CLASS_H
#define SRC_CAR_CLASS_H

#include <cmath>
#include "json.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include "pthread.h"
/*******************************************************/
#include "ros/ros.h"
#include "geometry_msgs/Pose2D.h"
#include "sensor_msgs/Imu.h"
#include "joy_data/joydata.h"
#include "joy_data/control.h"
#include "Following_path/Path_para.h"
#include "control_center/show_data.h"
#include "algorithm.h"
#include "sim_data/sim_data.h"
#include "Darknet/person_box.h"
/*****************************************************/


#define above_line 1  //右转
#define under_line 0 //左转
#define in_linne 3//在直线下面左转，直线上面右转
#define AGV_name "AGV_pose"
#define IMU_name "IMU_data"
#define Joy_name "Joy_data"
#define Sim_name "sim_data_topic"
using namespace std;
void *Count_time_function(void *pth);//线程函数声明
//存放速度值
struct Speed_car{
    int _1 = 0;
    int _2 = 0;
    int _3 = 0;
    int _4 = 0;
};
//存放所有的参数信息
struct Parameter{
    float angle_Kp = 4.5;
    float angle_Ki = 1.0;
    float angle_Kd = 2.2;
    float distance_Kp = 6.5;
    float distance_Kd = 7.0;
    float distance_Ki = 0.0;
    float function_A = 0;
    float function_B = 0;
    float function_C = 0;
    float camera_Kp = 8.5;
    float camera_KI = 0.2;
    float camera_Kd = 0.1;
    bool Save_Paramenter(void);//保存参数
    bool Read_Paramenter(void);//保存参数
};
//存放所有的位置信息
struct Location{
    float X_GPS;
    float Y_GPS;
};
//存放所有偏差信息
struct Error_history{
    float angle_history[3] = {0};
    float distance_history[3] = {0};
    float pure_error_l[3] = {0};
    float camera_error[3] = {0};
};

/***********存放按键数据*****************************/
struct Button{
    float _GO = 0;
    float _angle = 0;
    bool _1;
    bool _2;
    bool _3;
    bool _4;
    bool _5;
    bool _6;
    bool _7;
    bool _8;
};
bool Run;//Run=0时，手柄失联，车stop
int Time_count;
/*****************************************************/
class Car
{
private:
    double Alpha;//纯追踪算法的Alpha角
    double l_d;//纯追踪算法前视距离
    double goal_x;
    double goal_y;//纯追踪算法输入的轨迹点
public:
    Parameter parameter;
    Speed_car speed_car;//车的速度值
    Button button;//手柄的按键
    Error_history error_History;//偏差信息
    float Set_speed=0.1;
    double car_yaw = 0;
    float turn_angle;//车的转弯角度
    float turn_history = 0;
    float obstacle_distance;//障碍物最小距离
    double heading = 0;//车头角度，与正东夹角
    float destination[2];//车俩目的地坐标
    float request_liner;
    float min_distance;//定义车到轨迹的最小距离
    Location location;
    int Control_Mode;//定义车的控制模式，0为手柄控制，1为自动驾驶
    int Control_statue;//
    unsigned int count_num_statue = 0;
    float startpoint = 0;
    float endpoint = -10;
    float camera_speed;
    /***********创建检测线程*********************/
    pthread_t num_id;
    /***************定义ros函数**********************/
    Car();
    ~Car();
    void callback_Pose(const geometry_msgs::Pose2D::ConstPtr &msg);//获取位置
    void callback_Heading(const sensor_msgs::Imu::ConstPtr &msg);//获取车头角度
    void callback_joy(const joy_data::joydata::ConstPtr &msg);//手柄控制
    void callback_sim(const sim_data::sim_data::ConstPtr &msg);//仿真会调
    void callback_camera(const Darknet::person_box::ConstPtr &msg);//相机跟踪

    void pub_data(void);//发出数据
    void pub_miss_data(void);//手柄丢失发送数据
    /****************定义非ros函数************************/
    //计算距离
    float distance_compute(float x, float y, float coefficient_A, float coefficient_B, float coefficient_C);
    //判断位置关系
    int point_line_relation_1(float x, float y, float coefficient_A, float coefficient_B, float coefficient_C);
    int point_line_relation_2(float x, float y, float coefficient_A, float coefficient_B, float coefficient_C);
    void joy_control(void);//手柄控制程序
    void Control_condition_1(float angle_car_line);//判断控制状态
    void Control_condition_2(float angle_car_line);//判断控制状态
    void output_control_value_1(void);//输出控制量
    void output_control_value_2(void);//输出控制量
    void angle_pid(double angle_error);//角度增量式pid
    void Position_pid(double angle_error);//位置式pid
    void distance_pid(float distance);
    void different_speed(int speed, float angle);
    void change_value(void);//改参数
    void find_line(void);
    int request_trajectory(float end_point_x);
    void distance_pid_camera(float distance, float spd = 0);//利用相机进行pid调节车头方向
    /******************写文件函数************************************************************************************/
    bool write_control_data();  //写入成功返回1
    bool write_control_txt();
    /***********纯追踪算法*******************/
    double get_data_purecontrol(float path_x = 0, float path_y = 0);//获取purecontrol的数据,返回角度值alpha
    void pure_control(float path_x = 0, float path_y = 0, float l_d=1);
    void pure_control_1(float path_x = 0, float path_y = 0, float l_d=1);

    /**********************************************************/
private:
    //定义ros需要的成员
    ros::NodeHandle obj;
    ros::Subscriber sub_GPS;
    ros::Subscriber sub_IMU;
    ros::Subscriber sub_Joy;
    ros::Subscriber sub_heading;
    ros::Publisher  pub_txt_data;
    ros::ServiceClient client_callpath;
    ros::Publisher pub_show_data;
    ros::Subscriber sub_sim;
    ros::Subscriber sub_camera;
};








#endif //SRC_CAR_CLASS_H
