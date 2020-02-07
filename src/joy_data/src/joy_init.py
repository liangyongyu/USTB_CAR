#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 手柄话题发送文件
import rospy
import pygame
from joy_data.msg import joydata
if __name__ == "__main__":
    # 初始化ros节点
    rospy.init_node("Jos_data_pub")
    # 初始化消息的发送者
    pub = rospy.Publisher("Joy_data", joydata, queue_size=10)
    rate = rospy.Rate(50)
    pygame.init()  # 初始化pygame
    pygame.joystick.init()  # 初始化joystick
    data = joydata()  #  始化节点发送的消息对象
    while not rospy.is_shutdown():
        # print(pygame.event.get())
        # 得到joystick的数量 手柄的个数
        joystick_count = pygame.joystick.get_count()
        # print("手柄的个数: {}".format(joystick_count))
        if joystick_count != 0:
            pygame.event.get()
            for i in range(joystick_count):
                joystick = pygame.joystick.Joystick(i)
                joystick.init()
                # 从操作系统中获取控制器/joystick的名称
                name = joystick.get_name()
                data.axis_1 = -1*joystick.get_axis(5)  # 前进
                data.axis_2 = joystick.get_axis(0)  # 转弯
                data.button_1 = joystick.get_button(0)  # 停止按键
                data.button_2 = joystick.get_button(1)  # 开始按键
                data.button_3 = joystick.get_button(4)
                data.button_4 = joystick.get_button(3)
                data.button_5 = joystick.get_button(9)  # 右前放下面按键
                data.button_6 = joystick.get_button(7)  # 右前方上面按键
                data.button_7 = joystick.get_button(6)  # 左前方下面按键
                data.button_8 = joystick.get_button(8)  # 左前方上面按键
        else:
            print("手柄未连接")
        pub.publish(data)