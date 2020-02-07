//
// Created by agv on 2019/11/12.
//

#include "myPoint.h"
#include "ros/ros.h"

int main(int argc, char **argv)
{
    MyPoint point;

    ros::init(argc, argv, "path");
    ros::NodeHandle n;

    point.Nav_Info2coordinate(n);

    ros::AsyncSpinner spinner(2); // Use 4 threads
    spinner.start();


    ros::Rate r(50); // 10 hz



    glutInit(&argc, argv);
    //设置窗口的缓存和颜色模型
    //下面指定的是：窗口使用单个缓存并且使用RGB颜色模型来设定颜色值。
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    //设置窗口左上角的位置
    glutInitWindowPosition(400, 200);
    //设置窗口的宽高
    glutInitWindowSize(800, 600);
    glutCreateWindow("the path");


    //设置投影类型：正投影
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //观察参数：x坐标值从0到200，y是从0到150
    gluOrtho2D(-110.0, 60.0, -60.0, 60.0);

    glutDisplayFunc(point.read_path);
    //设置显示窗口的背景为白色。参数顺序为：红、绿、蓝、透明度。
    glClearColor(1.0, 1.0, 1.0, 0.0);


    glutMainLoop();
    ros::waitForShutdown();



    system("pause");
    return 0;

}