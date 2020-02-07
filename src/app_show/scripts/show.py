#!/usr/bin/env python3
import rospy
import matplotlib.pyplot as plt
import numpy as np
from math import sin

class Show:
    def __init__(self):
        self.fun_A = 0
        self.fun_B = 1
        self.fun_C = 5
        self.X_GPS = 1
        self.Y_GPS = 1
        self.car_angle = 0  # 车轮转角的信息
        self.x = []  # 保存历史轨迹
        self.y = []  # 保存历史轨迹
        self.start_point = 0  # 轨迹起始点
        self.end_point = 10  # 轨迹末端点

    def Draw_loaction(self, start, end, x_num=50):
        plt.ion()
        plt.figure(1)
        plt.clf()
        x_axis = np.linspace(-x_num, x_num)
        # plt.xlim(start, end)
        plt.xlim(-10, 20)
        plt.ylim(-10, 20)
        if self.fun_B != 0:
            y_axis = -self.fun_A / self.fun_B * x_axis + (-self.fun_C / self.fun_B)
            plt.plot(x_axis, y_axis)
            plt.plot(self.X_GPS, self.Y_GPS, '*')
            plt.plot(self.x, self.y)
            plt.pause(0.000000001)
        else:
            # plt.axvline(int(-self.c/self.fun_A), 0, 100)  # 添加垂直直线
            # plt.plot([-x_num, x_num], [-self.fun_A/self.fun_B,-self.fun_A/self.fun_B])
            # plt.pause(0.000000001)
            pass
    def draw_sin(self, a = 5):
        plt.ion()
        plt.figure(1)
        plt.clf()
        x = np.linspace(0, 50, 1000)
        y = [a*sin(0.2*i) for i in x]
        plt.plot(x, y)
        plt.plot(self.X_GPS, self.Y_GPS, '*')
        plt.plot(self.x, self.y)
        plt.pause(0.001)
    def draw_camera(self):
        plt.ion()
        plt.figure(1)
        plt.clf()
        plt.plot(self.X_GPS, self.Y_GPS, '*')
        plt.plot(self.x, self.y)
        plt.pause(0.001)
# if __name__ == "__main__":
#     rospy.init_node("show_node")
#     graph = Show()
#     while not rospy.is_shutdown():
#         # graph.draw_sin()
#         graph.draw_camera()


