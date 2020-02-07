#ifndef MSG_H
#define MSG_H



namespace CarMSG{
    
    typedef enum{
        CarSTOP,
        CarCRTL,
        CarSPD,
        CarTOR,
        CarLINE,
        // 方向PID
        CarDirPIDQ,
        CarDirPIDM,
        // 驱动PID
        CarDriPIDQ,
        CarDriPIDM,
    }MsgType;
}


#endif