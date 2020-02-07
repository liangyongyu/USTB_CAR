#!/usr/bin/env python3
from show import *
import rospy
from control_center.msg import show_data
num = 0
def call_back_show(msg):
    global num
    num = num + 1
    host.X_GPS = msg.x
    host.Y_GPS = msg.y
    host.car_angle = msg.angle
    if(num == 5):
        host.x.append(host.X_GPS)
        host.y.append(host.Y_GPS)
        num = 0
    host.fun_A = msg.A
    host.fun_B = msg.B
    host.fun_C = msg.C
    host.start_point = msg.start
    host.end_point = msg.end

    if len(host.x) > 2000:
        del host.x[0]
        del host.y[0]



if __name__ == "__main__":
    rospy.init_node("show_node")
    rospy.Subscriber("show_data_topic", show_data, call_back_show, queue_size=10)
    host = Show()
    rate = rospy.Rate(10)
    while not rospy.is_shutdown():
        # host.Draw_loaction(host.start_point, host.end_point)
        # host.draw_sin(5)
        host.draw_camera()
        rate.sleep()
