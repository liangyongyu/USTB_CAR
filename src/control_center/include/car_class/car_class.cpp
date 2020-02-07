//
// Created by lyy on 2019/11/23.
//

#include "car_class.h"
#include "math.h"
/****************计数线程函数****************************/
extern bool Run;
extern int Time_count;
using namespace std;
void *Count_time_function(void *pth){

    while(Run)
    {
        Time_count++;
        usleep(20000);
        if(Time_count>100)
        {
            std::cout<<"手柄失联"<<std::endl;
            break;
        }
    }
    std::cout<<"over";
}

bool Parameter::Save_Paramenter(void)
{
    char *path = NULL;
    std::string root;
    //也可以将buffer作为输出参数
    path = getcwd(NULL, 0);
    root = path;
    free(path);
    root = root.append("/data/parameter.json");
    using json = nlohmann::json ;
    json j;
    j["angle_Kp"] = angle_Kp;
    j["angle_Ki"]= angle_Ki;
    j["angle_Kd"]= angle_Kd;
    j["distance_Kp"] = distance_Kp;
    j["distance_Ki"]= distance_Ki;
    j["distance_Kd"]= distance_Kd;
    std::ofstream fout(root);
    if(fout.is_open())
    {
        fout << std::setw(0) << j << std::endl;
        fout.close();
        std::cout<<root;
        return 1;
    } else{
        return 0;
    }
}
bool Parameter::Read_Paramenter(void)
{
    char *path = NULL;
    std::string root;
    //也可以将buffer作为输出参数
    path = getcwd(NULL, 0);
    root = path;
    puts(path);
    free(path);
    root = root.append("/data/parameter.json");
    using json = nlohmann::json;
    json obj_data;

    std::ifstream fin;
    fin.open(root);
//    std::ifstream fin(file);
    if (!fin.is_open())
    {
        std::cout << "open json file failed.";
    }
    fin>>obj_data;
    angle_Kp = obj_data["angle_Kp"];
    angle_Ki = obj_data["angle_Ki"];
    angle_Kd = obj_data["angle_Kd"];
    distance_Kp = obj_data["distance_Kp"];
    distance_Ki = obj_data["distance_Ki"];
    distance_Kd = obj_data["distance_Kd"];
    fin.close();
}
void Car::angle_pid(double angle_error){
//    turn_angle = turn_angle + angle_error*parameter.angle_Kp +
//                 (error_History.angle_history[1] - error_History.angle_history[0])*parameter.angle_Kd;//PID实现
    double sum = 0;
    sum = error_History.angle_history[2] + angle_error;
    turn_angle = angle_error*parameter.angle_Kp +
            (angle_error - error_History.angle_history[0])*parameter.angle_Kd+
            sum*parameter.angle_Ki;//PID实现
    if(abs(sum)<10){
        error_History.angle_history[2] = (float)sum;
    }

    error_History.angle_history[1] = error_History.angle_history[0];
    error_History.angle_history[0] = (float)angle_error;

    if(turn_angle>17){turn_angle = 17;}
    if(turn_angle<-17){turn_angle = -17;}
}
//
void Car::distance_pid(float distance) {
    float tempt = 0;
    float sum = 0;
    sum =  error_History.distance_history[2] + error_History.distance_history[1] + error_History.distance_history[0] + distance;
    tempt =  distance*parameter.distance_Kp +
            50*(distance - error_History.distance_history[0])*parameter.distance_Kd
            + sum*parameter.distance_Ki;//PID
    //保存偏差
    cout<<"微分量："<<(distance - error_History.distance_history[0])*50*parameter.distance_Kd<<endl;
    cout<<"积分量:"<<sum*parameter.distance_Ki<<endl;
//    error_History.distance_history[2] = error_History.distance_history[1];
    if(abs(sum)<10){
        error_History.distance_history[2] = sum;
    }

    error_History.distance_history[1] = error_History.distance_history[0];
    error_History.distance_history[0] = distance;
    turn_angle = tempt;
    if(turn_angle>17){turn_angle = 17;}
    if(turn_angle<-17){turn_angle = -17;}
}
void Car::distance_pid_camera(float distance, float speed) {
    float tempt = 0;
    float sum = 0;
    //camera_error[2]用来存储总的偏差和
    sum = error_History.camera_error[2] + distance;
    tempt =  distance*parameter.camera_Kp +
             50*(distance - error_History.distance_history[0])*parameter.camera_Kd
             + sum*parameter.camera_KI;//PID
    //保存偏差
    cout<<"相机微分量："<<(distance - error_History.distance_history[0])*50*parameter.distance_Kd;
    cout<<"    相机积分量:"<<sum*parameter.distance_Ki<<endl;
//    error_History.distance_history[2] = error_History.distance_history[1];
    if(abs(sum)<10){
        error_History.distance_history[2] = sum;
    }
    error_History.camera_error[1] = error_History.camera_error[0];
    error_History.camera_error[0] = distance;

    speed_car._4= speed_car._3 = speed_car._2 = speed_car._1 = abs(speed*30);

    turn_angle = tempt;
    if(turn_angle>17){turn_angle = 17;}
    if(turn_angle<-17){turn_angle = -17;}
    cout<<"速度设定："<< speed_car._4<<"     角度设定:"<<turn_angle<<endl;
}
void Car::Position_pid(double angle_error)//位置式pid
{
    float tempt_1;
    tempt_1 = angle_error*parameter.angle_Kp + (angle_error - error_History.angle_history[0])*parameter.angle_Kd;
    turn_angle = tempt_1;
}
/*************定义差速***********************************************************************/

void Car::different_speed(int speed, float angle){
    int tempt = 60;
    speed_car._1 = speed_car._2 = speed_car._3 = speed_car._4 = speed;
    if(angle<0 && speed!=0){
        speed_car._1 = speed - tempt*angle/17;
        speed_car._3 = speed_car._1;}
    if(angle>0&&speed!=0){
        speed_car._2 = speed + 1*tempt*angle/17;
        speed_car._4 = speed_car._2;
    }


}
/********************************************电线关系判断*************************************************************************/
float Car::distance_compute(float x, float y, float coefficient_A, float coefficient_B, float coefficient_C)
{
    float tempt;
    tempt = abs(coefficient_A*x+coefficient_B*y+coefficient_C);
    tempt = tempt/sqrt(coefficient_A*coefficient_A+coefficient_B*coefficient_B);

    return tempt;
}
int Car::point_line_relation_1(float x, float y, float coefficient_A, float coefficient_B, float coefficient_C){
    if (distance_compute(x,y,coefficient_A,coefficient_B,coefficient_C)==0){
        return in_linne;
    }
    if(coefficient_B ==0 ){
        if(-coefficient_C/coefficient_A < x){ return under_line;}
        else{ return above_line;}
    }
    if(coefficient_A ==0){
        if(-coefficient_C/coefficient_B > y){ return under_line;}
        else{ return above_line;}
    }
    float tempt;
    tempt = -coefficient_A/coefficient_B*x-coefficient_C/coefficient_B;
    if(tempt > y){ return under_line;}
    else{ return above_line;}
}
int Car::point_line_relation_2(float x, float y, float coefficient_A, float coefficient_B, float coefficient_C) {
    /******1右转  0左转  */
    if (distance_compute(x,y,coefficient_A,coefficient_B,coefficient_C)==0){
        return in_linne;
    }
    if(coefficient_B ==0 ){
        if(-coefficient_C/coefficient_A < x){ return 1;}
        else{ return 0;}
    }

    if(coefficient_A ==0){
        if(-coefficient_C/coefficient_B > y){ return 1;}
        else{ return 0;}
    }

    float tempt;
    tempt = -coefficient_A/coefficient_B*x-coefficient_C/coefficient_B;
    if(tempt > y){ return 1;}
    else{ return 0;}
}
/****************************************构造函数和析构函数*******************************************************************************/
Car::~Car() {}
Car::Car() {
    Run = 1;
    Time_count = 0;
    Control_Mode = 0;
    speed_car._1 =speed_car._2=speed_car._3=speed_car._4=0;
    turn_angle = 0;
    location.X_GPS = 0;
    location.Y_GPS = 10.5;
    parameter.function_B= -1;
    parameter.function_A=  1;
    parameter.function_C= 10;
    Control_statue =2;



    pthread_create(&num_id,NULL,Count_time_function,NULL);

    sub_GPS = obj.subscribe<geometry_msgs::Pose2D>(AGV_name, 10, &Car::callback_Pose, this);
    sub_heading = obj.subscribe<sensor_msgs::Imu>(IMU_name, 10, &Car::callback_Heading, this);
    sub_Joy = obj.subscribe<joy_data::joydata>(Joy_name, 10, &Car::callback_joy, this);
    client_callpath = obj.serviceClient<Following_path::Path_para>("PATH");
    pub_txt_data = obj.advertise<joy_data::control>("AGV_ControlData", 10);
    pub_show_data = obj.advertise<control_center::show_data>("show_data_topic", 10);
    sub_sim = obj.subscribe<sim_data::sim_data>(Sim_name, 10, &Car::callback_sim, this);
    sub_camera = obj.subscribe<Darknet::person_box>("person_info", 10, &Car::callback_camera, this);

}

/************************************回调函数****************************************************/
//获取位置坐标
void Car::callback_Pose(const geometry_msgs::Pose2D::ConstPtr &msg){
    location.X_GPS = msg->x;
    location.Y_GPS = msg->y;
}
//车头回调函数，获取heading值-180～+180
void Car::callback_Heading(const sensor_msgs::Imu::ConstPtr &msg){
    heading = 90 - msg->orientation.z;
    if(heading <-180){heading = 360 + heading;}
}
//手柄回调函数，获取手柄数据
void Car::callback_joy(const joy_data::joydata::ConstPtr &msg){
    if(msg->button_2){Control_Mode = 1;}
    if(msg->button_1){Control_Mode = 0;}
    if(msg->button_3){Control_Mode = 2;}
    button._GO = msg->axis_1;
    button._angle = msg->axis_2;
//    button._3 = msg->button_3;
    button._4 = msg->button_4;
    button._5 = msg->button_5;
    button._6 = msg->button_6;
    button._7 = msg->button_7;
    button._8 = msg->button_8;

//    std::cout<<"手柄回调:"<<Time_count<<std::endl;
    Time_count=0 ;
}
//仿真结果获取回调函数
void Car::callback_sim(const sim_data::sim_data::ConstPtr &msg){
    location.X_GPS = msg->X_gps;
    location.Y_GPS = msg->Y_gps;
    heading = msg->heading;

}

int name_histroy = 0;
int num = 0;
int num_1 = 0;
void Car::callback_camera(const Darknet::person_box::ConstPtr &msg){
    using namespace std;
    double distance_fd = msg->x;//error

    double height = msg->y;
    int name_id = msg->objectid;
    int flag_person = msg->flag_person;
    /******判断***********/
    if((name_histroy == 0)&&(flag_person == 10000))//
    {
        name_histroy = name_id;
    }
    if(flag_person == 10000 && name_histroy != 0 && (name_histroy == name_id))//有人+id相等未丢
    {
        error_History.camera_error[0] = distance_fd;
        camera_speed = msg->z;//deep
        num_1 = 0;
        num = 0;
    }
    //有人 + 以前di丢了
    if(flag_person == 0){
        num++;
        if(num > 500){
            error_History.camera_error[0] = 0;
            name_histroy = 0;
            num = 0;
        }
    } else if (flag_person == 10000 && name_histroy != name_id){
        num_1++;
        if(num_1 > 50){
            name_histroy = name_id;
            num_1 = 0;
        }
    }




}
/****************************调参函数****************************************************************/
void Car::change_value(void)//改参数
{
    if(Control_statue == 1)
    {
        if (button._3 == 1) {
            parameter.angle_Kp = parameter.angle_Kp + 0.001;
            button._3 = 0;
            if (parameter.angle_Kp < 0) { parameter.angle_Kp = 0; }

        }
        if (button._4 == 1) {
            parameter.angle_Kp = parameter.angle_Kp - 0.001;
            button._4 = 0;
            if (parameter.distance_Kp < 0) { parameter.distance_Kp = 0; }
        }
    }
    else if (Control_statue == 2)
    {
        if (button._3 == 1) {
            parameter.distance_Kp = parameter.distance_Kp + 0.001;
            button._3 = 0;
            if (parameter.distance_Kp < 0) { parameter.distance_Kp = 0; }

        }
        if (button._4 == 1) {
            parameter.distance_Kp = parameter.distance_Kp - 0.001;
            button._4 = 0;
            if (parameter.distance_Kp < 0) { parameter.distance_Kp = 0; }
        }
    }
}
/*****************************写文件函数********************************************************/
bool Car::write_control_data(){
    //获取当前工作目录的绝对路径
    char *path = NULL;
    std::string root;
    //也可以将buffer作为输出参数
    path = getcwd(NULL, 0);
    root = path;
    free(path);
    root = root.append("/data/control_data.json");
    using json = nlohmann::json ;
    json j;
    j["turn"]=turn_angle;
    j["speed_1"]=speed_car._1;
    j["speed_2"]=speed_car._2;
    j["speed_3"]=speed_car._3;
    j["speed_4"]=speed_car._4;
    std::ofstream fout(root);
    if(fout.is_open())
    {
        fout << std::setw(0) << j;
        fout.close();
//        std::cout<<root;
        return 1;
    } else{
        return 0;
    }
}
bool Car::write_control_txt(){
    using namespace std;
    char *path = NULL;
    std::string root;
    //也可以将buffer作为输出参数
    path = getcwd(NULL, 0);
    root = path;
    free(path);
    root = root.append("/data/data.txt");

    ofstream examplefile(root);
    if (examplefile.is_open()) {
        examplefile << "1"<<"\n";
        examplefile <<speed_car._1<<"\n";
        examplefile << speed_car._2<<"\n";
        examplefile <<speed_car._3<<"\n";
        examplefile <<speed_car._4<<"\n";
        examplefile <<turn_angle<<"\n";
        examplefile.close();
        cout<<"速度： "<<speed_car._1<<endl;
        cout<<"角度： "<<turn_angle<<endl;
        cout<<"写入OK"<<endl;
        return 1;
    }
    return 0;
}

/*******************************控制相关函数***********************************************************/
void Car::joy_control(void){
    float tempt = 300;
    if(Control_Mode==0){
        turn_history = turn_angle;
        turn_angle = button._angle*17;
        if(turn_angle - turn_history >= 2){
            turn_angle = turn_history + 2;
        } else if(turn_angle - turn_history <= -2){
            turn_angle = turn_history - 2;
        }
        speed_car._1 = button._GO*300;
        speed_car._2 = button._GO*300;
        speed_car._3 = button._GO*300;
        speed_car._4 = button._GO*300;

        if(button._GO>0&&turn_angle>0){speed_car._1 = speed_car._1 + tempt*turn_angle/17;speed_car._3 = speed_car._1;}
        if(button._GO>0&&turn_angle<0){speed_car._2 = speed_car._2 + -1*tempt*turn_angle/17;speed_car._4 = speed_car._2;}
        if(button._GO<0&&turn_angle>0){speed_car._1 = speed_car._1 - tempt*turn_angle/17;speed_car._3 = speed_car._1;}
        if(button._GO<0&&turn_angle<0){speed_car._2 = speed_car._2 + tempt*turn_angle/17;speed_car._4 = speed_car._2;}
    }

}
void Car::pub_data(void){
    using namespace std;
    joy_data::control msg;
    if(turn_angle - turn_history >3){
        turn_angle = turn_history +3;
    } else if (turn_angle - turn_history <-3){
        turn_angle = turn_history -3;
    }
    msg.link = 1;
    msg.turn = turn_angle;
    msg.moto_1_1 = speed_car._1;
    msg.moto_1_2 = speed_car._2;
    msg.moto_1_3 = speed_car._3;
    msg.moto_1_4 = speed_car._4;
    msg.mod = 1;
    turn_history = turn_angle;
    pub_txt_data.publish(msg);
    /**发送上位机显示数据*此数据也用于仿真*/
    control_center::show_data msg1;
    msg1.x = location.X_GPS;
    msg1.y = location.Y_GPS;
    msg1.angle = heading;
    msg1.A = parameter.function_A;
    msg1.B = parameter.function_B;
    msg1.C = parameter.function_C;
    msg1.start = startpoint;
    msg1.end = endpoint;
    msg1.spd = speed_car._3;
    pub_show_data.publish(msg1);
}
void Car::pub_miss_data(void){
    using namespace std;
    joy_data::control msg;
    msg.link = 1;
    msg.turn = 0;
    msg.moto_1_1 = 0;
    msg.moto_1_2 = 0;
    msg.moto_1_3 = 0;
    msg.moto_1_4 = 0;
    msg.mod = 1;
    pub_txt_data.publish(msg);
}
/******控制情况判断*********/
void Car::Control_condition_1(float angle_car_line){//angle_car_line车头和轨迹的夹角,只有值没有正负之分
    float tempt = 0;
    float tempt_1 = 0;
    using namespace std;
    int tempt_3 = 1;
    if(abs(min_distance)>=2){tempt_3 = 0;}
    else{tempt_3 = 1;}

    /***********情形1：车头方向和轨迹夹角过大，只控制车头方向，纵向控制***************车里轨迹很近或者夹角大于80度车头矫***车头角度**/
    if(angle_car_line >= 90){Control_statue = 1;return;}
    /************情形2：车头角度低于设定阈值度数时，不对车头方向做控制，只对横向距离做控制************车头角度限定范围内，横向距离控制*/
    if(Control_statue == 1 && ((min_distance >=1 && angle_car_line<= 40)||(min_distance<1 && angle_car_line<= 15)))
        Control_statue = 2;
    /************情形3：车头角度正常，距离正常,*****************************************车头角度范围内，距离设定之内，控制车头***/
}
void Car::Control_condition_2(float angle_car_line) {
    float tempt = angle_car_line;
    using namespace std;
    /***********情形1：车头方向和轨迹夹角过大，只控制车头方向，纵向控制***************车里轨迹很近或者夹角大于80度车头矫***车头角度**/
    if(min_distance >=1 && tempt >= 90){Control_statue = 1;return;}
    /************情形2：车头角度低于设定阈值度数时，不对车头方向做控制，只对横向距离做控制************车头角度限定范围内，横向距离控制*/
    if(Control_statue == 1 && ((min_distance >=1 && tempt<= 40)||(min_distance<1 && tempt<= 15)))
        Control_statue = 2;
    /************情形3：车头角度正常，距离正常,*****************************************车头角度范围内，距离设定之内，控制车头***/

}
void Car::output_control_value_1(void){
    using namespace std;
    //获取车辆信息
    int point_condition;
    float temp_1 = 0;//计算偏差
    double temp_2 = 0;//模式三对应偏差
    float angle_car_line = 0;//车头和轨迹的夹角,只有值没有正负之分
    float distance_error;
    double angle_tempt = 0;//angle_tempt:轨迹与x轴夹角,有正负+90～~90

    min_distance = distance_compute(location.X_GPS, location.Y_GPS,parameter.function_A,
                                    parameter.function_B, parameter.function_C);
    point_condition = point_line_relation_1(location.X_GPS, location.Y_GPS,
                                          parameter.function_A, parameter.function_B, parameter.function_C);
    /******车头和轨迹的夹角大小******只有值，没有正负之分************************************************************************************************************/
    if(parameter.function_B != 0&& parameter.function_A!=0){//A,B均不为0 ，斜率存在
        angle_car_line = abs(heading - (atan(-parameter.function_A / parameter.function_B) * 180 / M_PI));
    }
    else if(parameter.function_B ==0)//a不为0，，b为0  垂直与x轴
    {
        angle_car_line = abs(90 -abs(heading));
    } else if(parameter.function_A==0)//a为0，b不为0  垂直于y轴
    {
        angle_car_line =  abs(heading);
    }
    /***************控制情形判断*********************************************************************/
    Control_condition_1(angle_car_line);
    /************最小距离pid************注意调整减去的数值大小******/
    if(Control_statue == 1)//控制车头方向
    {
        /***左右 转向情况区别**************************************************/
        if(parameter.function_B !=0){angle_tempt = (atan(-parameter.function_A / parameter.function_B) * 180 / M_PI);}
        else if (parameter.function_B == 0){angle_tempt = 90;}
        else if (parameter.function_A ==0){angle_tempt = 0;}//angle_tempt:轨迹与x轴夹角
        if(heading <= angle_tempt){//车头往右偏多控制左打
          /*计算偏差*/
            temp_1 = -1*abs(angle_tempt - 30 - heading)/180;
        }else
        {//车头往左偏多，控制右打
            temp_1 = abs(angle_tempt - 30 - heading)/180;
        }
        angle_pid(temp_1);//直接控制产生输出
        //只对车头方向进行控制，其中设定车头方向是：与轨迹夹角为30度

        cout<<"Control_statue:"<<Control_statue<<endl;
        cout<<"距离偏差"<<min_distance<<"   纵向PID输入偏差："<<temp_1<<"    设定转角"<<turn_angle<<"    车坐标(x,y):"<<location.X_GPS<<","
        <<location.Y_GPS<<"   车头角度："<<heading<<"  车头和轨迹的夹角大小:"<<angle_car_line<<"  KP:"<<parameter.angle_Kp<<endl;
    }
    else if(Control_statue == 2)//根据车辆
    {
        //已经确定车不在轨迹上，根据距离对车进行pid转向控制
        if(point_condition == under_line){distance_error = -min_distance;}
        else{distance_error = min_distance;}
        distance_pid(distance_error);
        cout<<"PID输入距离偏差："<<distance_error<<"   设定转角："<<turn_angle
        <<"   车线角度："<<angle_car_line<<"      车坐标(x,y):"<<location.X_GPS<<","<<location.Y_GPS;
        cout<<"     车头角度:"<<heading<<endl;
    }
    /**差速处理***/
    different_speed(100, turn_angle);//差速处理
//    cout<<"左轮速度："<<speed_car._1<<"        右轮速度："<<speed_car._2<<endl<<endl;
    if(turn_angle>17){turn_angle = 17;}
    if(turn_angle<-17){turn_angle = -17;}
}
void Car::output_control_value_2(void){
    using namespace std;
    //获取车辆信息
    int point_condition;
    float temp_1 = 0;//计算偏差
    float angle_car_line = 0;//车头和轨迹的夹角,只有值没有正负之分
    float distance_error;
    double angle_tempt = 0;//angle_tempt:轨迹与x轴夹角,有正负+90～~90
    min_distance = distance_compute(location.X_GPS, location.Y_GPS,parameter.function_A,
                                    parameter.function_B, parameter.function_C);
    point_condition = point_line_relation_2(location.X_GPS, location.Y_GPS,
                                          parameter.function_A, parameter.function_B, parameter.function_C);
    /******车头和轨迹的夹角大小******只有值，没有正负之分**************1右转  0左转***************************************************************/
    if(parameter.function_B != 0&& parameter.function_A!=0){//A,B均不为0 ，斜率存在
        angle_car_line = abs(heading - (atan(-parameter.function_A / parameter.function_B) * 180 / M_PI));
        if (heading > 0) {
            angle_car_line=abs(180 + (atan(-parameter.function_A / parameter.function_B) * 180 / M_PI) - heading);
        }
        if (heading < 0) {
            angle_car_line=(360 + heading) - (180 + (atan(-parameter.function_A / parameter.function_B) * 180 / M_PI));
            }
    }
    else if(parameter.function_B ==0)// 垂直与x轴
    {
        angle_car_line = abs(abs(heading) -90);

    } else if(parameter.function_A==0)//a为0，b不为0  垂直于y轴
    {
        angle_car_line = abs(180-abs(heading));
    }
    /***************控制情形判断*********************************************************************/
    Control_condition_2(angle_car_line);
    /************最小距离pid************注意调整减去的数值大小******/
    if(Control_statue == 1)//只控制车头方向
    {
        /***左右 转向情况区别**************************************************/
        if(parameter.function_B !=0){angle_tempt = 180-abs((atan(-parameter.function_A / parameter.function_B) * 180 / M_PI));}
        else if (parameter.function_B == 0){angle_tempt = 90;}
        else if (parameter.function_A ==0){angle_tempt = 0;}//angle_tempt:轨迹与x轴夹角
        if(heading <= angle_tempt){//车头往右偏多控制左打
            /*计算偏差*/
            temp_1 = -1*abs(angle_tempt - 30 - heading)/180;
        }
        else
        {
            //车头往左偏多，控制右打
            temp_1 = abs(angle_tempt - 30 - heading)/180;
        }
        angle_pid(temp_1);//直接控制产生输出

        cout<<"angle_tempt:"<<angle_tempt<<endl;
        cout<<"pid控制量"<<temp_1<<endl;
        //只对车头方向进行控制，其中设定车头方向是：与轨迹夹角为30度
        cout<<"距离偏差"<<min_distance<<"   纵向PID输入偏差："<<temp_1<<"    设定转角"<<turn_angle<<"    车坐标(x,y):"<<location.X_GPS<<","
            <<location.Y_GPS<<"   车头角度："<<heading<<"  车头和轨迹的夹角大小:"<<angle_car_line<<"  KP:"<<parameter.angle_Kp<<endl;
    }
    else if(Control_statue == 2)//根据车辆
    {
        //已经确定车不在轨迹上，根据距离对车进行pid转向控制
        if(point_condition == under_line){distance_error = -min_distance;}
        else{distance_error = min_distance;}
        distance_pid(distance_error);

        cout<<"PID输入距离偏差："<<distance_error<<"   设定转角："<<turn_angle
            <<"   车线角度："<<angle_car_line<<"      车坐标(x,y):"<<location.X_GPS<<","<<location.Y_GPS<<endl;
    }

    /**差速处理***/
    different_speed(100, turn_angle);//差速处理
    if(turn_angle>17){turn_angle = 17;}
    if(turn_angle<-17){turn_angle = -17;}
}
void Car::find_line(void)//找到车辆的轨迹方向
{
    using namespace std;
    float coordinate[2][2];
    float  vector_line[1][2] = {0};//轨迹的向量
    /***计算方向矢量***/
    coordinate[0][0]=startpoint;
    coordinate[0][1]=parameter.function_A*startpoint + parameter.function_C;
    coordinate[1][0]=endpoint;
    coordinate[1][1]=parameter.function_A*endpoint + parameter.function_C;
    vector_line[0][0] = coordinate[1][0] - coordinate[0][0];
    vector_line[0][1] = coordinate[1][1] - coordinate[0][1];
    /**********************************************************************/
    if((vector_line[0][0] >0)||(vector_line[0][0] ==0 && vector_line[0][1]>0))//轨迹的方向为
    {
        output_control_value_1();
        cout<<"东"<<endl;
    } else if ((vector_line[0][0]<0)||((vector_line[0][0] ==0 && vector_line[0][1]<0)))
    {
        cout<<"西"<<endl;
        output_control_value_2();
    }
}

int Car::request_trajectory(float end_point_x){
    float y;
    y = -1*parameter.function_A/parameter.function_B*end_point_x - parameter.function_C/parameter.function_B;
    float tempe_1 = sqrt(pow((location.X_GPS - end_point_x),2)+pow((location.Y_GPS - y), 2));
    if(tempe_1<1){
        return 1;//到达位置请求新轨迹
    } else{
        return 0;//未达位置
    }
}
/*以车为坐标系，车头方向为y轴，计算轨迹点在车坐标系的位置，以及Alpha的度数信息
 */
double Car::get_data_purecontrol(float x, float y) {//获取purecontrol的数据
    //计算的alpha在-180~+180之间，heading在-180~+180之间
    float coordinate[2][2];
    //轨迹点coordinate[0],后车轴坐标coordinate[1]
    double theat;//顺时针旋转角度
    double tempt_x;//轨迹点转后坐标
    double tempt_y;//轨迹点转后坐标
    double alpha;//轨迹点和车头的夹角alpha
    //具体赋值
    theat = heading;//坐标系转换到后车轴，y轴方向为车头方向
    coordinate[0][0] = x;
    coordinate[0][1] = y;
    coordinate[1][0]=location.X_GPS;
    coordinate[1][1]=location.Y_GPS;

    /*************坐标系变换************************************/
    tempt_x = (coordinate[0][0]-coordinate[1][0])*sin(theat*M_PI/180)-(coordinate[0][1]-coordinate[1][1])*cos(theat*M_PI/180);
    tempt_y = (coordinate[0][0]-coordinate[1][0])*cos(theat*M_PI/180)+(coordinate[0][1]-coordinate[1][1])*sin(theat*M_PI/180);
    alpha = 90 - acos(tempt_x/sqrt(pow(tempt_x,2)+pow(tempt_y,2)))*180/M_PI;
    /**********************************************************************/
    return alpha;//角度 返回车头为y轴，后车轴与点的夹角，左为负值，右为正值
}
/*
 * (x,y)轨迹点的坐标
 * look_ahead_distance是前视距离最小值，和速度有关
 * */
void Car::pure_control(float x , float y, float look_ahead_distance){
    pure_pursuit_control p_;
    Alpha = get_data_purecontrol(x,y);//求Alpha角
    l_d = sqrt(pow(location.X_GPS-x, 2) + pow(location.Y_GPS-y, 2)); //前视距离
    if(l_d>look_ahead_distance){
        l_d =look_ahead_distance;
    }
    p_.update_data(Alpha, l_d);//pure pursuit计算角度
    cout<<"Alpha:"<<Alpha<<"   l-D:"<<l_d<<endl;
    turn_angle = p_.control_output();
}
/*改进的pure puresuit算法
 * 加入积分作用消除静差
 * */
void Car::pure_control_1(float x , float y, float look_ahead_distance){
    pure_pursuit_control p_;
    Alpha = get_data_purecontrol(x,y);//求Alpha角
    l_d = sqrt(pow(location.X_GPS-x, 2) + pow(location.Y_GPS-y, 2)); //前视距离
    error_History.pure_error_l[2] = error_History.pure_error_l[2] + error_History.pure_error_l[1];
    error_History.pure_error_l[1] = error_History.pure_error_l[0];
    error_History.pure_error_l[0] = l_d * sin(Alpha);
    cout<<"偏差:"<<0.01*error_History.pure_error_l[2]<<endl;
    if(l_d>look_ahead_distance){
        l_d =look_ahead_distance;
    }
    p_.update_data(Alpha, l_d);//pure pursuit计算角度
    cout<<"Alpha:"<<Alpha<<"   l-D:"<<l_d<<endl;
    turn_angle = p_.control_output() - 0.01*error_History.pure_error_l[2];
}















