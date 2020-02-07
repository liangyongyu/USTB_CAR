//
// Created by lyy on 2020/1/7.
//

#include "algorithm.h"

pure_pursuit_control::pure_pursuit_control(){}
pure_pursuit_control::~pure_pursuit_control(){}
double pure_pursuit_control::control_output(){
    double theat = 0;
    double tempt;
    tempt = 2*L_car*sin(Alpha_angle*M_PI/180.0)/L_distance;
    theat = 180.0*atan(tempt)/M_PI;
    if(theat > 17){theat = 17;}
    if(theat < -17){theat = -17;}
    return theat;
}
void pure_pursuit_control::update_data(double Alpha, double L_d){
    Alpha_angle = Alpha;
    L_distance = L_d;
}


