#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 小车最终段接受信息文件
# AGV车辆信息保存文件
import rospy
import os
from joy_data.msg import control
root = os.path.abspath(os.path.join(os.getcwd(), "../../.."))+"/src/data/data.txt"

# print(root)

def callback_function(joy_data):
    filename = root
    # print(root)
    # 判断手柄是否失连，是则将速度 角度预设为0
    # 失联手柄之后，车丢失保险，所有设置为0
    # print joy_data.link
    if not joy_data.link:
        joy_data.moto_1_1 = 0
        joy_data.moto_1_2 = 0
        joy_data.moto_1_3 = 0
        joy_data.moto_1_4 = 0
    # 写入txt文件，控制代码自动读取控制变量
    try:
        with open(root, 'w') as file_obj:
            file_obj.write(str(joy_data.mod) + "\n")
            file_obj.write(str(joy_data.moto_1_1) + "\n")
            file_obj.write(str(joy_data.moto_1_2) + "\n")
            file_obj.write(str(joy_data.moto_1_3) + "\n")
            file_obj.write(str(joy_data.moto_1_4) + "\n")
            file_obj.write(str(joy_data.turn))
            print("turn:" + str(joy_data.turn))
            # print("motors:" + str(joy_data.moto_1_1) + "  " + str(joy_data.moto_1_2))
            # print("motors:" + str(joy_data.moto_1_3) + "  " + str(joy_data.moto_1_4))

    except IOError:
        print("写入文件错误")

if __name__ == "__main__":
    rospy.init_node("AGV_Control_sub")
    sub = rospy.Subscriber("AGV_ControlData", control, callback_function, queue_size=10)
    rospy.spin()
