//
// Created by lyy on 2020/1/7.
//

#ifndef SRC_ALGORITHM_H
#define SRC_ALGORITHM_H

#include "car_class.h"
#include "math.h"

#define L_car 7.76 //定义车轴距长度为7.76米
/******************计算所需要的数据***********************************/
class calculate_data{
public:

private:
    double yaw;//偏航角
};
/***************pure pursuit追踪算法实现******只需要调用update函数更新private变量************************/
class pure_pursuit_control{
public:
    pure_pursuit_control();
    ~pure_pursuit_control();
    double control_output();//return:车轮转角-17~+17度
    void update_data(double _Alpha = 0, double _L_d = 4);//更新默认参数
private:
    double Alpha_angle;//车和点之间的角度偏差-180-180度
    double L_distance;//前视距离:车后轴和点的距离
};



































#endif //SRC_ALGORITHM_H
