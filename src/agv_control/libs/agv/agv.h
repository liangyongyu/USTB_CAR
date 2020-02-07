#ifndef AGV_H
#define AGV_H


#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <math.h>
#include <string.h>

#include "controlcan.h"
#include "ros/ros.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
using namespace google;


#include "INIReader.h"
#include "joy_data/control.h"
#include "agv_control/feedback.h"


#define SISHORT signed short
#define INT32   signed int


#define AGV_RECE1_BASE  0xB520101       // CAN接收1 BASE
#define AGV_RECE2_BASE  0xB520201       // CAN接收1 BASE
#define AGV_RECE3_BASE  0x381           // CAN接受位置返回值


// 接收数据
void BytesSwap(BYTE *aData,int nBegin,int nSize);
// << -------------- 驱动错误代码  ----------------
struct AGV_ERR_CODE{
    union{
        USHORT CODE;  //无符号short类型   #define USHORT unsigned short int
        BYTE Data[2];//BYTE 无符号char   #define BYTE unsigned char
        struct{
            BYTE VBUS_OVER_CUR:1;   // VBus 过流
            BYTE VBUS_OVER_VOL:1;   // VBus 过压
            BYTE VBUS_DOWN_CUR:1;   // VBus 欠压
            BYTE SENSOR_ERR:1;      // 位置传感器错误
            BYTE PRE_CFG_ERR:1;     // 预驱传感器错误
            BYTE PRE_RO:1;          // 预驱保护
            BYTE MOS_FAULT:1;       // MOS管故障
            BYTE PVDD_OVER:1;       // 预驱过压
            BYTE PVDD_DOWN:1;       // 预驱欠压
            BYTE PRE_OVER_TEMP:1;   // 预驱过温
            BYTE HEART_SKIP:1;      // 握手跳帧（>20帧）
            BYTE COM_TIMEOUT:1;     // 握手超时（>2s）
            BYTE CMDID_ERR:1;       // CmdID 连续 100 次出错
            BYTE NC:3;
        };
    };
};

// << -------------- 接受数据 1 --------------
 struct AGV_MSG_REC1{
    UINT ID;
    UINT Updated;
    union{
        BYTE Data[8];
        struct{
            BYTE            CMD_ID;
            BYTE            PRE_STU;
            SISHORT         TORQUE_FBK;     // 当前输出扭矩反馈
            AGV_ERR_CODE    ERR_CODE;
            USHORT          ROLL_CHECK;
        };
    };
    void MsgSwap(){
        BytesSwap(Data,2,2);
        BytesSwap(Data,4,2);
        BytesSwap(Data,6,2);
    }
};

// << -------------------------------------- 接受数据 2 ----------------------
 struct AGV_MSG_REC2{
    UINT ID;
    UINT Updated;                       // 是否更新了
    union{
        BYTE Data[10];
        struct{
            BYTE        NC1;
            BYTE        CMD_ID;

            SISHORT     MOTOR_SPD;      // 单位 1 rpm
            USHORT      MOTOR_ANGLE;   // 电机机械角度（正数） 0x1=0 度， 0x4096=359.91 度
            BYTE        DIRECTON;
            BYTE        NC2;
            USHORT      ROLL_CHECK;
        };
    };
    void MsgSwap()
    {
        BytesSwap(Data, 2, 2);
        BytesSwap(Data, 4, 2);
        BytesSwap(Data, 8, 2);
    }
 };
struct AGV_MSG_REC3
{
    UINT ID;
    UINT     Updated;
    BYTE     Data[2];
    BYTE     DIRECTON;//判断转向电机的方向
    USHORT   Loaction;//转向电机的位置

};

struct AGV_MSG_SEND1{
    UINT ID;
    BYTE Updated;
    union{
        BYTE Data[10];
        struct{
            BYTE       NC;
            BYTE       CAN_MODE;       
            short      CANDirect1;   // short 必须在偶数字节上
            short      CANDirect2;  
            short      CANTorque1;
            BYTE       CRC;            
        };
    };
    void MsgSwap()
    {
        BytesSwap(Data, 2, 2);
        BytesSwap(Data, 4, 2);
        BytesSwap(Data, 6, 2);
    }
};


// #define AGV_SEND_ID2 0xC000102
 struct AGV_MSG_SEND2{
    UINT ID;
    BYTE Updated;
    union{
        BYTE Data[8];
        
        struct{
            // 4 个扭矩电机
            SISHORT   CANTorque2;//定义有符号数short  #define SISHORT signed short
            SISHORT   CANTorque3;
            SISHORT   CANTorque4;
            USHORT    CRC;
        };
    };
    // 大小端转换
    void MsgSwap()
    {
        BytesSwap(Data, 0, 2);
        BytesSwap(Data, 2, 2);
        BytesSwap(Data, 4, 2);
        BytesSwap(Data, 6, 2);
    }
};
#define AGV_CAN_MODE 0x01 //0x00非can控制；0x01 can控制
#define AGV_SEND_ID1 0xC000101 //can发送数据包1
#define AGV_SEND_ID2 0xC000102 //can发送数据包2
#include<iomanip>
// 发送消息实体
struct AGV_MSG_SEND{
    AGV_MSG_SEND1 msg1;
    AGV_MSG_SEND2 msg2;
};



/**
 * 驱动电机
 * 
*/
struct DirectMotor{

    std::string name;

    int MaxDirValue = 850;

    // pid 参数
    float kp;
    float ki;
    float kd;

    // 偏差信息
    float   err;
    float   perr;
    float   serr;   // sum
    float   angle_feedback = 0;//反馈车轮的角度
    float   output_volumne;
    
    float GoalPosition;
    // 转向的pid增量 计算
    void PositionalPid(){
        using namespace std;
        float tempt;
        err = GoalPosition - angle_feedback;
        if((err < 0.1)&&(err>-0.1)){err = 0;}
        serr = err + serr;

//        tempt = err * kp + kd * (err - perr);
//
//        output_volumne = output_volumne + tempt;
        cout<<"角度设定："<<GoalPosition<<"   角度反馈："<<angle_feedback<<"   偏差："<<err<<endl;
//        output_volumne = err * kp + kd * (err - perr) + ki*(err + perr + serr);
//        cout.setf(ios::fixed);
//        cout<<setprecision(3)<<GoalPosition<<",       "<<angle_feedback<<",       "<<err<<endl;

        output_volumne = GoalPosition*50 + serr*ki;
//        output_volumne = GoalPosition*50;
        perr = err;
        serr = perr + serr;

        if(output_volumne > 1000){
            output_volumne = 1000;
        }
        if(output_volumne < -1000){
            output_volumne = -1000;
        }
    }

};

/**
 * 方向电机
 * 
*/
struct DriverMotor{

    std::string name;
    bool Updated;
    BYTE PRE_STU;   // 一个字节    预驱状态,无符号char    #define BYTE unsigned char
    SISHORT TORQUE_FBK;//            #define SISHORT signed short
    // 状态
    AGV_ERR_CODE FBErrCode;  // 错误信息
    int FBTor;              // 扭矩反馈
    int FBSpeed;            // 速度反馈
    BYTE FBDir;             // 方向反馈
    // pid 参数
    float kp;
    float ki;
    float kd;
    float err;  // 本次
    float perr; // 上次
    float derr; // 上上次
    float output_volume;  // 扭矩输出

    // 输出限制 扭矩
    int MaxSpeed = 1300;   // e
    int DOutLimit = 200;    // 单次增量
    int OutLimit = 1300;     // 总输出限幅
    // 控制
    int GoalSpeed;  // 目标速度
    
    // 增量PID
    void IncrementalPid(){
        //速度限幅，防止超速
        if(GoalSpeed > MaxSpeed){
            GoalSpeed = MaxSpeed;
        }
        if(GoalSpeed < -MaxSpeed){
            GoalSpeed = -MaxSpeed;
        }
        //计算偏差
        err = GoalSpeed - FBSpeed;

        //增量式PID计算增量  err-本次偏差 perr-上次偏差 derr-上上次偏差
        float dout = kp * err + ki * ((err + perr + derr)/3)+ kd * (err  - perr);
        //计算单次增量，车速平滑递增或递减
        if(dout >DOutLimit){
            dout = DOutLimit;
        }
        else if(dout < -DOutLimit)
        {
            dout = -DOutLimit;
        }
        output_volume = output_volume + dout;
        //输出限幅
        if(output_volume >OutLimit){
            output_volume = OutLimit;
        }
        else if(output_volume < -OutLimit)
        {
            output_volume = -OutLimit;
        }
//        std::cout<<out<<std::endl;
        // LOG(INFO) << out;
        //保存误差,依次保存为history[1] history[2],便于后续计算增量
        derr = perr;
        perr = err;
    }


};

struct AgvMsg{
    void *pHandle;
    std::string name;
    double value;
};
typedef void (*CallbackFun)(void *arg);//函数指针，输入参数为指针
class AGVCar
{
private:

//    ros::Subscriber sub;
    /* data */
public:
    AGVCar(/* args */);
    ~AGVCar();
    DriverMotor driMotors[4];       //DriverMotor结构体,实例化为driMotors:0-3 驱动电机
    DirectMotor dirMotor;

    // 检验是否打开
    bool IsOpen(){

        return this->isOpen_;
    }  
    // ============= CAN 设备操作 =============
    bool DeviceOpen();
    void DeviceReset();
    void DeviceClose();

    // ============= 电机控制 =================
    void Control(int,int);
    void SpdControl(float iDir,int iSpd1,int iSpd2,int iSpd3,int iSpd4);//给定转角和四个电机的速度值
    void TorControl(int iDir,int iTor1,int iTor2,int iTor3,int iTor4);//给定tor数据值
    // 消息
    bool SendControlMsg(AGV_MSG_SEND msg);//发送消息返回bool类型的数据

    void MotorPidQuery(int motor,float &kp,float &ki,float &kd)//获取pid参数，motor为电机机号
    {
        kp = driMotors[motor].kp;
        ki = driMotors[motor].ki;
        kd = driMotors[motor].kd;
    }
    void MotorPidModify(int motor,float kp,float ki,float kd){
        driMotors[motor].kp = kp;
        driMotors[motor].ki = ki;
        driMotors[motor].kd = kd;
    }
    void MotorStatus(int motor,int &spd,int &tor){
        spd = driMotors[motor].FBSpeed;//速度反馈
        tor = driMotors[motor].FBTor;//扭矩反馈
    }
    void DirPidQuery(float &kp,float &ki,float &kd){
        kp = dirMotor.kp;
        ki = dirMotor.ki;
        kd = dirMotor.kd;
    }
    void DirPidModify(float kp,float ki,float kd){
        dirMotor.kp = kp;
        dirMotor.ki = ki;
        dirMotor.kd = kd;
        
    }
    void *hParent = NULL;//空指针
    CallbackFun callBack=NULL;
    void setCallBack(CallbackFun pFun,void* pArg)
    {
        hParent = pArg;
        callBack = pFun;
    }

private:
    //
    bool isOpen_ = false;
    bool keepRuning = false;
    pthread_t recePid;                  // 读取线程
    
    // 。
    static void *receHelper(void *arg)
    {
        ((AGVCar *)arg)->ReceThread();//
    }
    void ReceThread();
};

#endif