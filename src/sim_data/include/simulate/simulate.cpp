//
// Created by lyy on 2019/12/21.
//

#include "simulate.h"

loaction location_sim(float x, float y, double v, double heading, double turn) {
    loaction loaction_1;
    double cos_theat;
    double sin_theat;
    double tan_theat;
    double x_delta, y_delta, theat;

    cos_theat = cos(heading * M_PI / 180);
    sin_theat = sin(heading * M_PI / 180);
    tan_theat = -1 * tan(turn * M_PI / 180);//注意转弯角度的正负问题
    x_delta = cos_theat * v*Period;//x轴的位移量
    y_delta = sin_theat * v*Period;
    theat = tan_theat * v / Length_car;
    //        cout << "x:" << x_delta << "  Y:" << y_delta << endl;
//    cout<<"theat:"<<theat<<endl;
    loaction_1.x = x + x_delta;
    loaction_1.y = y + y_delta;

    if ((heading + theat) > 180) {
        loaction_1.heading = heading + theat - 360;
    } else if ((heading + theat) < -180) {
        loaction_1.heading = heading + theat + 360;
    } else {
        loaction_1.heading = heading + theat;

    }

//    if((heading + theat)<0){loaction_1.heading = heading + theat-360;}

    return loaction_1;
}
