//
// Created by lyy on 2019/12/18.
//

#include "trajectory.h"
int trajectory[3][3]={{0,5},{5,-10},{10,0}};


point_trajectory send_trajectory(int i){
    point_trajectory tempt;
    switch(i) {
        case 0:
            tempt.x1 = trajectory[0][0];
            tempt.x2 = trajectory[0][1];
            break;
        case 1:
            tempt.x1 = trajectory[1][0];
            tempt.x2 = trajectory[1][1];
            break;
        case 2:
            tempt.x1 = trajectory[2][0];
            tempt.x2 = trajectory[2][1];
            break;
    }

    return tempt;
}