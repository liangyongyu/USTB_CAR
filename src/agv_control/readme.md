# agv 小车控制


## 环境搭建
Ubuntu 16.04 

QT 5.10 + qChart 安装过程中勾选qChart组件

glog + gflags 

    glog 和 gflag 安装方式
    sudo apt-get install libgflags-dev libgoogle-glog-dev 

cmake 

    sudo apt-get install cmake

    
linux can  

    can通信库

libboost-all-dev

    sudo apt-get install libboost-all-dev
    在使用websocket通信过程中使用了.boost version > 1.5


openssl

    OPENSSL_VERSION = 1.0.2g
    sudo apt-get install libssl-dev openssl libssl


# 缺少 opengl
    
    sudo apt-get install freeglut3-dev
    sudo apt-get install libglew-dev libsdl2-dev libsdl2-image-dev libglm-dev libfreetype6-dev


****
## 编译 安装
    
    mkdir build && cd build 
    cmake .. && make && make install

## 调试

## CAN消息格式说明

1、agv发送的CAN数据包为大端数据，注意对双字节数据格式的转换

2、内存数据中，大于1个字节的数据类型的起始地址只能是偶数字节地址
    比如short，int 类型首内存地址必须为偶数。

3、通信消息格式 通信波特率为500kpbs

## 发送 电机控制信息

0xC000101   CANMode | Direct1 | Direct2 | Torque1 | CRC
0xC000102   Torque2 | Torque3 | Torque4 | CRC


Direct1 控制前轮方向 输入有效值 -840 ~ +900
TorqueX 代表驱动电机的扭矩输入

发送消息中CRC 不需要

发送周期 <400ms

## 转向电机说明

向左转 输入 负值 
向左转 输入 正值

使用位置式PID


## 驱动电机控制

左侧电机 -> 前进方向 速度值为正

右侧电机 -> 前进方向 速度值为负   ？未确认

电机控制方式使用增量式PID


## AGV 发送的消息说明

1、机械角度 -- 无效
2、ROLL_CHECK -- 无效
3、


## 注意处理电机的错误消息！！！



## 转向模型，使用 Ackman转向

需要进行参数调整

## PID 参数文件
config.ini 文件
注意文件格式是固定的
**注意速度上限的输出

# ini 文件

    ini 文件中保存了配置文件
    需要放在编译的二进制文件目录下


# 调试问题总结

    1、数据大小端转换问题，不经过大小端转换的数据无法正常操作

    **2、右侧轮 转向问题
        4个轮子输出正扭矩时，agv前进，
        输出速度数值， 左侧轮输出-> 正值
                     右侧轮输出-> 负值

    **3、转向
        注意控制agv的整理速度 上限，
        转向时 单侧电机输出的速度可能会超过上限，单个轮的上限要比较高
    
    4、ws通信，
        注意程序的 程序的正常终止

    5、通信中断 操作 #TODO


    6、问题 ini文件 读取name 值出错 # TODO

    7、局域网传送视频
        使用opencv对视频进行编码（压缩），
        websocket bin传输
        解码和显示
        发送vector 需要 buf.data() 或者&buf[0] 做首地址
            
    8、编译 qt 出现多版本    
        卸载 多余的cmake文件


