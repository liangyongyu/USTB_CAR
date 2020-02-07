//
// Created by lyy on 2019/12/21.
//

#ifndef SRC_SIMULATE_H
#define SRC_SIMULATE_H
#include "math.h"
#include "iostream"

#define Length_car 0.776
#define Period 0.02
using namespace std;


/*保存位置信息,构造结构体函数
 * (x,y)位置坐标
 * heading：车头方向——y+轴负
 * */
struct loaction{
    double x,y;
    double heading;
};

/*(x,y)车的坐标
 * speed:车速
 * heading:车头方向——y+轴负
 * turn：车轮的设定转角值
 * */
loaction location_sim(float x, float y, double speed, double heading, double turn);



















#endif //SRC_SIMULATE_H
