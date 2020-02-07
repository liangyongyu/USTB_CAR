//
// Created by lyy on 2019/11/23.
//
#include <iostream>
#include "car_class.h"
#include "trajectory.h"
#include "agv_control/feedback.h"
//Run,Time_count
using namespace std;


int main(int argc, char **argv)
{
    ros::init(argc, argv, "Control");
    ros::NodeHandle nh;
    ros::ServiceClient client = nh.serviceClient<agv_control::feedback>("car_info");//初始化服务信息
    agv_control::feedback req;
    Car car;
    ros::Rate rate(20);
    /**********************************/
    car.heading = -5;
    car.startpoint = 0;
    car.endpoint = 15;
    car.location.Y_GPS = 10;
    car.location.X_GPS = -0.50;
    int i=0;
    car.parameter.function_A =  0;
    car.parameter.function_C = -15;
    car.startpoint =0;
    car.endpoint =60;
    point_trajectory tempt_trajectory;
    /**********************************/
    while(ros::ok())
    {
        /*************获取当前车的实时信息*************************/
//        req.request.num = 1;
//        if (client.call(req))
//        {car.car_yaw = req.response.data;/*cout<<"车轮当前转角"<<car.car_yaw<<endl;*/}
//        else{ROS_ERROR("获取车辆实时信息服务失败");return 1;}
        /********************************************************/
//        if(car.Control_Mode==0){
//            car.joy_control();
//            cout<<"速度值1,3："<<car.speed_car._1<<"        速度值2,4："<<car.speed_car._2<<"      车坐标："<<car.location.X_GPS<<","<<car.location.Y_GPS<<"  转角："<<car.turn_angle<<endl;
//        } else if(car.Control_Mode ==1)
//        {
//            if(car.request_trajectory(car.endpoint)){
//                /**请求服务***/
//                cout<<"到达点附近结束"<<endl;
//                if(i==0){   car.parameter.function_A =  0;car.parameter.function_C = -15;  car.startpoint =0;  car.endpoint =60; }
//                if(i==1){   car.parameter.function_A = 3.75;car.parameter.function_C = -240;car.startpoint =60;  car.endpoint =80;}
//                if(i==2){   car.parameter.function_A = 0;car.parameter.function_C = 60;car.startpoint =60;  car.endpoint =-30;}
//                if(i==3){   car.parameter.function_A = -2.5;car.parameter.function_C = -15;car.startpoint =-30;  car.endpoint =0;}
//                if(i==3){   i=-1;}
//                i++;
//            }
//            car.find_line();
//            car.change_value();
//        } else if(car.Control_Mode == 2)
//        {
//            cout<<"摄像头跟踪参数信息：    ";
//            car.distance_pid_camera(car.error_History.camera_error[0]);
//            cout<<endl<<endl;
//        }

/************服务实现代码**********************/
//Following_path::Path_para srv;
/*********************测试代码********************************************************/
/***********************相机控制仿真***************/
//        cout<<"摄像头跟踪参数信息：    ";
//        car.distance_pid_camera(car.error_History.camera_error[0]);
//        cout<<endl<<endl;
        car.pure_control(10, 10);
        car.speed_car._3 = 1;
        cout<<"速度值1,3："<<car.speed_car._1<<"        速度值2,4："<<car.speed_car._2<<"      车坐标："<<car.location.X_GPS<<","<<car.location.Y_GPS<<"  转角："<<car.turn_angle<<endl;

        car.pub_data();

/***********************测试代码******************************************************/
//        if(Time_count>100){
//            car.pub_miss_data();//发送手柄丢失后的车辆控制指令
//            cout<<"手柄失联"<<endl;
//            return 0;
//        }
//        else{
//            car.pub_data();
//        }
        ros::spinOnce();
        rate.sleep();
    }



    return 0;
}
