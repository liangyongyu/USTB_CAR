/**
 * 2019年01月03日18:28:38
 * create by ycsmile
 * 
 * 
*/
//


/**
 * 字节交换
 * 
*/


#include "agv.h"
#include <unistd.h>

typedef enum
{
    FIND,
    RESET,
    OPEN,
    INIT,
    CLEAR,
    START,
    CLOSE,

}DeviceOpCode;
#define Vairba2Str(x) (#x)
bool DeviceErr(DeviceOpCode method,INT32 code)
{
    // LOG(INFO) << code;
    if (code == 1)
    {
        return true;
    }
    std::string mthS;
    switch(method){
        case FIND:
            LOG(INFO) <<"未发现设备";
            return false;
            break;
        case RESET:
            mthS = Vairba2Str(RESET);
            break;
        case OPEN:
            mthS = Vairba2Str(OPEN);
            break;
        case INIT:
            mthS = Vairba2Str(INIT);
            break;
        case CLEAR:
            mthS = Vairba2Str(CLEAR);
            break;
            
        case START:
            mthS = Vairba2Str(START);
            break;
        case CLOSE:
            mthS = Vairba2Str(CLOSE);
            break;
    }
    LOG(INFO) << mthS << ((code==0)?" 操作失败":" 设备不存在或USB掉线");
    return false;
}

bool getIniPath(std::string iniPath,std::string &asbIniPath)
{
    int MAXBUFSIZE = 2048;
    char buf[ MAXBUFSIZE ];
	int count;
 
	count = readlink( "/proc/self/exe", buf, MAXBUFSIZE );
 
	if ( count < 0 || count >= MAXBUFSIZE )
	{ 
		printf( "Failed\n" );
        return false;
	} 
    char *pathend;
    pathend = strrchr(buf,'/');
    pathend++;
    buf[ count ] = '\0';
    asbIniPath = std::string(buf,pathend) + iniPath;
    return true;
}

void DriverMotorParasFormat(DriverMotor mot){

    LOG(INFO) <<"name " << mot.name;
    LOG(INFO) <<"kp " << mot.kp;
    LOG(INFO) <<"ki " << mot.ki;
    LOG(INFO) <<"kd " << mot.kd;
    LOG(INFO) <<"MaxSpeed " << mot.MaxSpeed;
    LOG(INFO) <<"OutLimit " << mot.OutLimit;

}
//extern float M1iDir;
//extern int M1iSpd1 ;
//extern int M1iSpd2 ;
//extern int M1iSpd3 ;
//extern int M1iSpd4 ;
//void AGVCar::callback(const joy_data::controlConstPtr &msg){
//    M1iDir = msg->turn;
//    M1iSpd1 = msg->moto_1_1;
//    M1iSpd2 = msg->moto_1_2;
//    M1iSpd3 = msg->moto_1_3;
//    M1iSpd4 = msg->moto_1_4;
//}

AGVCar::AGVCar(/* args */)//AGVCar构造函数
{
//    sub = obj.subscribe<joy_data::control>("AGV_ControlData", 10, &AGVCar::callback, this);
    // 初始化参数
    memset(driMotors,0,sizeof(DriverMotor)*4);      // 清除数据
    memset(&dirMotor,0,sizeof(DirectMotor));
    // 获取程序所在路径

    using namespace std;
    char *path = NULL;
    std::string root;
    //也可以将buffer作为输出参数
    path = getcwd(NULL, 0);
    root = path;
    root.append("/data/config.ini");
    cout<<root;
    free(path);

    // 加载参数
    std::string cndif = root;
    std::string absIniPath;
    char ini[1024];
    if(getIniPath(cndif,absIniPath)){

        LOG(INFO) << absIniPath;
        INIReader iniReader(root);
        if (iniReader.ParseError() < 0) {
            LOG(INFO) << "无法加载" << absIniPath;
            return;
        }
        LOG(INFO) << "加载参数中...";
        int speedLimit = iniReader.GetInteger("car","speedLimit",0);
        int torLimit = iniReader.GetInteger("car","torLimit",0);
        int doutlimit = iniReader.GetInteger("car","doutlimit",0);
        // 电机1
        // driMotors[0].name = iniReader.Get("Motor1","name","0");
        driMotors[0].kp = iniReader.GetReal("Motor1","kp",0);
        driMotors[0].ki = iniReader.GetReal("Motor1","ki",0);
        driMotors[0].kd = iniReader.GetReal("Motor1","kd",0);

        driMotors[0].MaxSpeed = speedLimit;
        driMotors[0].OutLimit = torLimit;
        driMotors[0].DOutLimit = doutlimit;


        // 电机2
        // driMotors[1].name = iniReader.Get("Motor2","name","0");
        driMotors[1].kp = iniReader.GetReal("Motor2","kp",0);
        driMotors[1].ki = iniReader.GetReal("Motor2","ki",0);
        driMotors[1].kd = iniReader.GetReal("Motor2","kd",0);

        driMotors[1].MaxSpeed = speedLimit;
        driMotors[1].OutLimit = torLimit;
        driMotors[1].DOutLimit = doutlimit;
        // 电机3
        // driMotors[2].name = iniReader.Get("Motor3","name","0");
        driMotors[2].kp = iniReader.GetReal("Motor3","kp",0);
        driMotors[2].ki = iniReader.GetReal("Motor3","ki",0);
        driMotors[2].kd = iniReader.GetReal("Motor3","kd",0);

        driMotors[2].MaxSpeed = speedLimit;
        driMotors[2].OutLimit = torLimit;
        driMotors[2].DOutLimit = doutlimit;

        // 电机4
        // driMotors[3].name = iniReader.Get("Motor4","name","0");
        driMotors[3].kp = iniReader.GetReal("Motor4","kp",0);
        driMotors[3].ki = iniReader.GetReal("Motor4","ki",0);
        driMotors[3].kd = iniReader.GetReal("Motor4","kd",0);

        driMotors[3].MaxSpeed = speedLimit;
        driMotors[3].OutLimit = torLimit;
        driMotors[3].DOutLimit = doutlimit;
        // DriverMotorParasFormat(driMotors[0]);
        // DriverMotorParasFormat(driMotors[1]);
        // DriverMotorParasFormat(driMotors[2]);
        // DriverMotorParasFormat(driMotors[3]);

        //dirMotor.name = iniReader.Get("Direct","name","0");

//        dirMotor.MaxDirValue = iniReader.GetReal("Direct","dirMax",0);
        dirMotor.kp = iniReader.GetReal("Direct","kp",0);
        dirMotor.ki = iniReader.GetReal("Direct","ki",0);
        dirMotor.kd = iniReader.GetReal("Direct","kd",0);

        LOG(INFO) << dirMotor.kp;
        LOG(INFO) << dirMotor.ki;
        LOG(INFO) << dirMotor.kd;

    }
}

AGVCar::~AGVCar()
{
    DeviceClose();
}

/**
 * 打开设备
 *
*/
bool AGVCar::DeviceOpen(){

    INT32 ret;
    if(isOpen_){
        return isOpen_;
    }
    isOpen_ = false;
    //
    if(!DeviceErr(OPEN,VCI_OpenDevice(VCI_USBCAN2, 0, 0)))
    {
        DeviceErr(RESET, VCI_ResetCAN(VCI_USBCAN2, 0, 0));
        usleep(100000); //延时100ms。
        DeviceErr(CLOSE, VCI_CloseDevice(VCI_USBCAN2, 0));
        return false;
    }
    VCI_INIT_CONFIG config;
    config.AccCode = 0;
    config.AccMask = 0xFFFFFFFF;
    config.Filter = 1;              //接收所有帧

    // 波特率 为 500k
    config.Timing0 = 0x00;          /*波特率125 Kbps  0x03  0x1C*/
    config.Timing1 = 0x1C;

    // config.Timing0 = 0x01;          /*波特率125 Kbps  0x03  0x1C*/
    // config.Timing1 = 0x1C;

    // config.Timing0 = 0x03;          /*波特率125 Kbps  0x03  0x1C*/
    // config.Timing1 = 0x1C;

    config.Mode = 0;                //0:正常模式;2:回环

    if(!DeviceErr(INIT,VCI_InitCAN(VCI_USBCAN2, 0, 0,  &config))){
        return false;
    }
    if(!DeviceErr(CLEAR,VCI_ClearBuffer(VCI_USBCAN2, 0, 0))){
        return false;
    }
    if(!DeviceErr(START,VCI_StartCAN(VCI_USBCAN2, 0, 0))){
        return false;
    }



    // 发送
    // 创建线程进行接收
    isOpen_ = true;
    keepRuning = true;
    pthread_create(&recePid,NULL,AGVCar::receHelper,(void *)this);
    return true;
}
/**
 * Reset设备
 *
*/
void AGVCar::DeviceReset(){

}
/**
 * Close设备
 * 
*/
void AGVCar::DeviceClose(){
    if(! isOpen_){
        return;
    }
    keepRuning = false;
    pthread_cancel(recePid);
    pthread_join(recePid,NULL);
    DeviceErr(RESET, VCI_ResetCAN(VCI_USBCAN2, 0, 0));
    usleep(10000); //延时100ms。
    DeviceErr(CLOSE, VCI_CloseDevice(VCI_USBCAN2, 0));
    usleep(10000); //延时100ms。

    isOpen_ = false;

}

/**
 * 大小端数据转换
 * 
*/

bool AGVCar::SendControlMsg(AGV_MSG_SEND msg)
{
    // 大小端转换
    msg.msg1.MsgSwap();
    msg.msg2.MsgSwap();

    UINT32 ret;
    VCI_CAN_OBJ send[2];

    send[0].ID = msg.msg1.ID;
    send[0].SendType = 0;
    send[0].RemoteFlag = 0;
    send[0].ExternFlag = 1;
    send[0].DataLen = 8;
    // <<-----------------------------------------
    // short 必须在偶数字节上
    mempcpy(send[0].Data,&msg.msg1.Data[1],8);

    send[1].ID = msg.msg2.ID;
    send[1].SendType = 0;
    send[1].RemoteFlag = 0;
    send[1].ExternFlag = 1;
    send[1].DataLen = 8;
    mempcpy(send[1].Data,msg.msg2.Data,8);

    // 发送数据
    if( (ret = VCI_Transmit(VCI_USBCAN2, 0, 0, send, 2)) ==2){
        return true;
    }
    return false;
}

/**
 * 接收线程
 * 
*/
void AGVCar::ReceThread(){

    int temp = 0;
    int recLen = 0;
    int idErr = 0;          // ID 差值
    int dataLen = 0;
    int deal = 0;
    VCI_CAN_OBJ rec[200]; //接收缓存，设为3000为佳。
    BYTE recData[8];
    int dir = 1;
    //
    if(keepRuning)
    {
        std::cout<<"数据接受进程被调用"<<std::endl;
    }
    while(keepRuning)
    {
        //调用接收函数，如果有数据，进行数据处理显示。
        if ((recLen = VCI_Receive(VCI_USBCAN2, 0, 0, rec, 200, 100)) > 0) //返回实际读取的帧数为recLen
        {
            for(int r_=0;r_ < recLen;r_++){
                // 提取接收到数据
                VCI_CAN_OBJ curRec = rec[r_]; //rec是CAN信息帧，长度缓存200,curRec最后就是收到的数据
                // 判断接收ID
                if(abs( idErr = (curRec.ID - AGV_RECE1_BASE)) < 4 )  // CAN接收1， AGV_RECE1_BASE是0xB520101 idErr是ID 差值
                {
                    // 提起数据
                    AGV_MSG_REC1 rec1;              //接受数据结构体 1
                    memcpy(rec1.Data,curRec.Data,curRec.DataLen);
                    rec1.MsgSwap(); // 字节交换
                    
                    dir = (idErr % 2 ==0)?1:-1;

                    driMotors[idErr].PRE_STU = rec1.PRE_STU;
                    driMotors[idErr].FBTor = dir * rec1.TORQUE_FBK;
                    driMotors[idErr].FBErrCode = rec1.ERR_CODE;

                    // 发送Tor反馈
                    AgvMsg *msg = new AgvMsg();
                    msg->pHandle = hParent;
                    msg->name = "M" + std::to_string(idErr+1) +"Out";
                    msg->value = driMotors[idErr].FBTor;
                    callBack((void *) msg);
                }
                else if(abs( idErr = (curRec.ID - AGV_RECE2_BASE)) < 4 )  // CAN接收1
                {
                    AGV_MSG_REC2 rec2;
                    memcpy(&rec2.Data[1],&curRec.Data[0],6);
                    memcpy(&rec2.Data[8],&curRec.Data[6],2);
                    rec2.MsgSwap();

                    dir = (idErr % 2 ==0)?1:-1;
                    driMotors[idErr].FBSpeed = dir * rec2.MOTOR_SPD;
                    driMotors[idErr].FBDir = dir * rec2.DIRECTON;
                    
                    AgvMsg *msg = new AgvMsg();
                    msg->pHandle = hParent;
                    msg->name = "M" + std::to_string(idErr+1) +"Spd";
                    msg->value = driMotors[idErr].FBSpeed;

                    callBack((void *) msg);
                }
                else if (curRec.ID == AGV_RECE3_BASE)
                {
//                    LOG(ERROR) << "0x381接受成功";

                    unsigned short int temp;
                    int out;
                    float angle;
                    temp =curRec.Data[5];
                    temp = temp<<8;
                    temp = temp + curRec.Data[4];
                    out = temp;
                    if(out > 60000)  {out =-(65535 - out);}
//                    std::cout<<"temp:"<<temp<<std::endl;
                    if(out>0){angle = (float)out*(17.0/901);}
                    else if(out<0){angle = (float)out*(17.0/831);}
                    dirMotor.angle_feedback = angle;
//                    std::cout<<"转角度数："<<angle<<std::endl;
//                    std::cout<<"转角度数："<<dirMotor.angle_feedback<<std::endl;
                }


//                LOG(ERROR) << "接受成功";
//                std::cout<<"0x"<<std::hex<<curRec.ID<<std::endl;
            }

        }
        // LOG(INFO) << temp++;
        usleep(500);
        // VCI_CAN_OBJ send;

        // send.ID = AGV_RECE2_BASE;
        // send.SendType = 0;
        // send.RemoteFlag = 0;
        // send.ExternFlag = 1;
        // send.DataLen = 8;

        // send.Data[0] = 0;
        // send.Data[1] = 0x01;
        // send.Data[2] = 0;
        // send.Data[3] = 0;

        // send.Data[4] = 0;
        // send.Data[5] = 0;
        // send.Data[6] = 0;
        // send.Data[7] = 0;

        // VCI_Transmit(VCI_USBCAN2, 0, 0, &send, 1);

    }
}

/**
 * ACM 转向
 * 
 * 
*/
// AGV 小车参数
#define AGV_Length   (1030.0 - 254.0)/1000.0       // 长度
#define AGV_Width   (632.0 - 254.0/2)/1000.0       // 宽度

#define AGV_Length_HALF (AGV_Length/2)
#define AGV_Width_HALF (AGV_Width/2)

#define AGV_Length_DOU (AGV_Length * AGV_Length)

#define AGV_AngleRate (25.0/32768.0 * 3.1415 / 180.0) // 最大转向角度 15°
#define AGV_SpeedRate (200.0/32768.0)
#define AGV_DirectRate (850.0/32768.0)



void AGVCar::SpdControl(float iDir,int iSpd1,int iSpd2,int iSpd3,int iSpd4)//给定目标速度,idir为车轮的转角
{

    if(iDir>17){iDir = 17;}
    if(iDir<-17){iDir = -17;}
    dirMotor.GoalPosition = iDir;
    dirMotor.PositionalPid();
//    std::cout<<"Kp:"<<dirMotor.kp<<"     KD:"<<dirMotor.kd<<"     KI:"<<dirMotor.ki<<std::endl;
//    std::cout<<"output:"<<dirMotor.output_volumne<<std::endl;

    // 目标速度
    driMotors[0].GoalSpeed = (int)iSpd1;
    driMotors[1].GoalSpeed = (int)iSpd2;
    driMotors[2].GoalSpeed = (int)iSpd3;
    driMotors[3].GoalSpeed = (int)iSpd4;
    // 进行pid计算

    // 驱动电机PID
    driMotors[0].IncrementalPid();
    driMotors[1].IncrementalPid();
    driMotors[2].IncrementalPid();
    driMotors[3].IncrementalPid();

    // 转向电机PID
//    std::cout<<driMotors[0].output_volume<<std::endl;
    if(driMotors[0].FBErrCode.VBUS_DOWN_CUR){LOG(ERROR) << "电池没点，注意注意";}
    if(driMotors[1].FBErrCode.VBUS_DOWN_CUR){LOG(ERROR) << "电池没点，注意注意";}
    if(driMotors[2].FBErrCode.VBUS_DOWN_CUR){LOG(ERROR) << "电池没点，注意注意";}
    if(driMotors[3].FBErrCode.VBUS_DOWN_CUR){LOG(ERROR) << "电池没点，注意注意";}

    AGV_MSG_SEND send;
    send.msg1.ID = AGV_SEND_ID1;
    send.msg2.ID = AGV_SEND_ID2;
    send.msg1.CAN_MODE = AGV_CAN_MODE;
    send.msg1.CANDirect1 = dirMotor.output_volumne;//转向电机

    send.msg1.CANDirect2 = 0;
    send.msg1.CANTorque1 = driMotors[0].output_volume;//车轮0
    send.msg1.CRC = 0x0A;
    send.msg2.CANTorque2 = driMotors[1].output_volume;//车轮1
    send.msg2.CANTorque3 = driMotors[2].output_volume;//车轮2
    send.msg2.CANTorque4 = driMotors[3].output_volume;//车轮3
//    send.msg2.CANTorque4 = 0x00ff;//车轮3
    send.msg2.CRC = 0x5555;

    // 发送数据
    if(!SendControlMsg(send))
    {
        // 发送数据失败
    }
}

void AGVCar::TorControl(int iDir,int iTor1,int iTor2,int iTor3,int iTor4)
{
    //发送数据
    AGV_MSG_SEND send;
    send.msg1.ID = AGV_SEND_ID1;
    send.msg2.ID = AGV_SEND_ID2;
    send.msg1.CAN_MODE = AGV_CAN_MODE;

    send.msg1.CANDirect1 = iDir;
    send.msg1.CANDirect2 = 0;
    send.msg1.CANTorque1 = iTor1;
    send.msg1.CRC = 0x0A;

    send.msg2.CANTorque2 = iTor2;
    send.msg2.CANTorque3 = iTor3;
    send.msg2.CANTorque4 = iTor4;
    send.msg2.CRC = 0x5555;

    // 发送数据
    if(!SendControlMsg(send))
    {// 发送数据失败
    }
}

void AGVCar::Control(int iDirect,int iSpeed)//两个参数：参数1表示左右转为角度       参数2表示 速度(*200/32768)
{
    using std::cout;
    using std::endl;
    float dir = 1.0f;  // iDirect 大于0 代表右转
    if(iDirect<0){
        iDirect = -iDirect; 
        dir = -1.0;     // 左转
    }

    float angel = iDirect * AGV_AngleRate;      // 转向角度 弧度
    float speed =  iSpeed * AGV_SpeedRate;      // 
    float direct = dir * iDirect * AGV_DirectRate;

    // 计算转向角度比例
    // 采用 通分法
    float tanA = tan(angel);

    float R = sqrt(AGV_Length_DOU + pow(AGV_Length_HALF * tanA,2));
    float receR = 1.0/R;

    float R1 = sqrt(pow(AGV_Length + dir * AGV_Width_HALF * tanA,2) + pow(AGV_Length * tanA,2));
    float R2 = sqrt(pow(AGV_Length - dir * AGV_Width_HALF * tanA,2) + pow(AGV_Length * tanA,2));
    //后轮差速：车长 + dir*车宽*角度  dir根据左右确定为正负1
    float R3 = AGV_Length  + dir * AGV_Width_HALF * tanA;
    float R4 = AGV_Length  - dir * AGV_Width_HALF * tanA;

    float spd1 = R1  * receR *  speed;
    float spd2 = R2  * receR *  speed;
    float spd3 = R3  * receR *  speed;
    float spd4 = R4  * receR *  speed;

    // 目标速度    将速度转换为整形变量,给avg_car的四个轮子,四个轮子分别计算当前PID
    driMotors[0].GoalSpeed = (int)spd1;
    driMotors[1].GoalSpeed = (int)spd2;
    driMotors[2].GoalSpeed = (int)spd3;
    driMotors[3].GoalSpeed = (int)spd4;

    

    // 驱动电机PID
    driMotors[0].IncrementalPid();
    driMotors[1].IncrementalPid();
    driMotors[2].IncrementalPid();
    driMotors[3].IncrementalPid();

    // 转向电机PID
    // direct
    //转向目标值给定
    dirMotor.GoalPosition = direct;

    dirMotor.PositionalPid();

    //消息结构体实例化为send
    AGV_MSG_SEND send;

    send.msg1.ID = AGV_SEND_ID1;
    send.msg2.ID = AGV_SEND_ID2;

    send.msg1.CAN_MODE = AGV_CAN_MODE;

    send.msg1.CANDirect1 = dirMotor.output_volumne;
    send.msg1.CANDirect2 = 0;
    send.msg1.CANTorque1 = driMotors[0].output_volume;
    send.msg1.CRC = 0x0A;

    send.msg2.CANTorque2 = driMotors[1].output_volume;
    send.msg2.CANTorque3 = driMotors[2].output_volume;
    send.msg2.CANTorque4 = driMotors[3].output_volume;
    send.msg2.CRC = 0x5555;

    // 发送数据
    if(!SendControlMsg(send))
    {
        // 发送数据失败
    }
}

void BytesSwap(BYTE *aData,int nBegin,int nSize){

    if (nSize < 1)
    {
        return;
    }
    if (nSize % 2 != 0)
    {
        return;
    }
    BYTE temp;
    for (int n_ = 0; n_ < (nSize >> 1); n_++)
    {
        temp = aData[nBegin + n_];
        aData[nBegin + n_] = aData[nBegin + nSize - 1 - n_];
        aData[nBegin + nSize - 1 - n_] = temp;
    }
}
