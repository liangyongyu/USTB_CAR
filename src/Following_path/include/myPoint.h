#ifndef MYPOINT_H
#define MYPOINT_H

#include <unistd.h>
#include <math.h>
#include "ros/ros.h"
#include "iostream"
#include "fstream"
#include <cmath>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <geometry_msgs/Pose2D.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/NavSatFix.h>
#include <Following_path/Path_para.h>
#include <stdio.h>
#include <vector>

#include <stdlib.h>
#include <string>


#include <GL/glut.h>


using  namespace std;

static string pop ="path4.txt";


class Point{
public:
    Point(float x=0, float y=0) :x(x), y(y) {}
    float GetX() const {return x;}
    float GetY() const {return y;}
private:
    float x,y;
};

class MyPoint
{
public:

  /* Constructor:
   * xPos   Position in x.
   * yPos   Position in y.
   * alpha  Robot rotation.
   * t      Time of measurement
   */
//  MyPoint(double xPos, double yPos);
//
//  ~MyPoint();

  /* Returns angle between this and target point.
   */
  double getAngle(MyPoint* target);

  /* Returns distance between this and target point.
   */
  double getDistance(MyPoint* target);

  /*Transform the Latitude and Longitude information to utm coordinate*/
  void LonLat2Mercator(double Latitude, double Longitude, double arr[]);

  // Get the row of path file
  int getNumberOfEdges();

  // fit the points of path to line
  float lineFit(const Point points[], int nPoint);

  //Subscriber and callback function
  void Nav_Info2coordinate(ros::NodeHandle n);
  void NavCallback(const sensor_msgs::NavSatFix::ConstPtr &Nav_data);
  static bool PathCallback(Following_path::Path_paraRequest &request,
                    Following_path::Path_paraResponse &response);


  static void read_path(void);
  static void display_path(void);
  void draw_path(double x, double y, bool is_static_point);


//ROS init
public:
  ros::Subscriber Nav_info;
  ros::Publisher Coordinate_xy;
  ros::ServiceServer send_path_service;
  geometry_msgs::Pose2D AGV_pose;



//variables
public:
  double x;       // Position in x.
  double y;       // Position in y.
  ros::Time time; // Time, when the position was measured.
  double angle;   // Robot's angle.
  bool is_ready = false;

};


extern MyPoint point;
extern vector<vector<float> > Path;
extern vector<float> variables;

#endif
