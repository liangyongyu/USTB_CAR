/**
 *  client and server 
 * 
 * 控制器Server
 * 
*/

// ========================= json ===============================
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ========================= google ===============================
#include <gflags/gflags.h>
#include <glog/logging.h>
using namespace google;

#include <unistd.h> // getuid
#include <signal.h>

#include "agv.h"
#include "msgtype.h"
// server 参数

// 创建一个WebSocket Server
#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>
#include <pthread.h>
#include "ros/ros.h"
#include "agv_control/feedback.h"
//***********************************************
#include <fstream>
#include <vector>
void readTxt();
void read_data(string file);
//************************************************
void *AgvControl(void *arg);
void ServerMsgHandler(json jmsg);
/**
 * check application is running as sudo
 * 
*/
bool CheckRootUser(){
    if( getuid()){
        return false;
    }
    return true;
}

class telemetry_server {
public:
    typedef websocketpp::connection_hdl connection_hdl;
    typedef websocketpp::server<websocketpp::config::asio> server;

    telemetry_server() : m_count(0)
    {
        // set up access channels to only log interesting things
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.set_access_channels(websocketpp::log::alevel::access_core);
        m_endpoint.set_access_channels(websocketpp::log::alevel::app);

        // Initialize the Asio transport policy
        m_endpoint.init_asio();

        // Bind the handlers we are using
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;
        using websocketpp::lib::bind;
        m_endpoint.set_open_handler(bind(&telemetry_server::on_open,this,_1));
        m_endpoint.set_close_handler(bind(&telemetry_server::on_close,this,_1));
    
        m_endpoint.set_message_handler(bind(&telemetry_server::on_message,this,_1,_2));
        // m_endpoint.set_http_handler(bind(&telemetry_server::on_http,this,_1));
    }
    // 读取消息
    void on_message(connection_hdl hdl, server::message_ptr msg)
    {
        LOG(INFO) << msg->get_payload();
        // LOG(INFO) << msg->get_opcode();
        // broadcast("rece");
        ServerMsgHandler(json::parse( msg->get_payload()));
    }

    void broadcast(std::string msg){
        // 广播消息
        con_list::iterator it;
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            m_endpoint.send(*it,msg,websocketpp::frame::opcode::text);
        }
    }

    void run(uint16_t port) {
        std::stringstream ss;
        m_endpoint.get_alog().write(websocketpp::log::alevel::app,ss.str());

        // listen on specified port
        m_endpoint.listen(port);

        // Start the server accept loop
        m_endpoint.start_accept();

        // Set the initial timer to start telemetry
        // set_timer();

        // Start the ASIO io_service run loop

        
        try {
            // m_endpoint.run();
            // 启动一个线程
            asio_thread = websocketpp::lib::thread(&server::run, &m_endpoint);
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        }
    }

    void close(){
        // 终止 进程
        
        // 终止所有链接
        // con_list::iterator it;
        // for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            // ((connection_hdl*)(it))->close();
        // }
        // asio_thread.detach();
        m_endpoint.stop();
        // asio_thread.native_handle();
        // 等待接结束
        asio_thread.join();
        // TerminateThread(asio_thread.native_handle())
    }

    void on_open(connection_hdl hdl) {
        m_connections.insert(hdl);
    }

    void on_close(connection_hdl hdl) {
        m_connections.erase(hdl);
        LOG(ERROR) << " 链接中断";
    }
private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;
    
    server m_endpoint;
    con_list m_connections;
    server::timer_ptr m_timer;

    websocketpp::lib::thread asio_thread;
    
    std::string m_docroot;
    
    // Telemetry data
    uint64_t m_count;
};

uint16_t port = 9002;
telemetry_server s;


AGVCar *agv = new AGVCar();     //实例化对象AGV

bool keepRunning = false;
pthread_t crtlPid;              //声明线程的ID  typedef unsigned long int pthread_t;
                                //定义无符号长整型数 crtlPid为线程ID

int ContrlMod = 1;//控制模式确定
// ===============================
// 模式0的输入参数
int M0iDir = 0;
int M0iSpd = 0;
// ===============================
// 模式1的输入参数
float M1iDir = 0;
int M1iSpd1 = 0;
int M1iSpd2 = 0;
int M1iSpd3 = 0;
int M1iSpd4 = 0;
// ===============================
// 模式2的输入参数
int M2iDir = 0;
int M2iTor1 = 0;
int M2iTor2 = 0;
int M2iTor3 = 0;
int M2iTor4 = 0;

int Timeout= 0;
using namespace std;
//agv控制函数
void *AgvControl(void *arg)
{
    std::cout<<"启用kongzhi线程"<<std::endl;
    ros::Rate rate(100);
    while(keepRunning)
    {
        if(!agv->IsOpen())//检查avg是否打开，返回bool类型数值
        {
            keepRunning = false;
            LOG(ERROR) <<  "异常:CAN中断链接";
            break;
        }
        //三种控制模式 给定速度和方向 | 给定方向和四个车轮目标速度 | 给定方向和四个车轮的转矩
        switch(ContrlMod)
        {
            case 0:agv->Control(M0iDir,M0iSpd);break;
            case 1:agv->SpdControl(M1iDir,M1iSpd1,M1iSpd2,M1iSpd3,M1iSpd4);break;//给定五个参数值并控制：dir m1 m2 m3 m4
            case 2:agv->TorControl(M2iDir,M2iTor1,M2iTor2,M2iTor3,M2iTor4);break;//转矩控制
        }
        ros::spinOnce();
        usleep(5000);  // 延时20ms,确定控制周期50hz,参数为us
//        cout<<"执行"<<endl;
//        rate.sleep();

    }
    std::cout<<"AgvControl执行结束"<<std::endl;
}

void AgcCallBack(void *arg){
    AgvMsg *msg = (AgvMsg *)arg;
    json jmsg;
    jmsg["typ"] = CarMSG::CarLINE;
    jmsg["msg"]["nam"] = msg->name;
    jmsg["msg"]["val"] = msg->value;
    s.broadcast(jmsg.dump());//json.dump 存储数据

}

void ServerMsgHandler(json jmsg){

    Timeout = 0;

    // // 处理消息
    switch((CarMSG::MsgType)(jmsg["typ"])){
        case  CarMSG::CarSTOP:
            ContrlMod = 0;
            M0iDir = 0;
            M0iSpd = 0;
            break;
        case  CarMSG::CarCRTL:    // 方向和
            ContrlMod = 0;
            M0iDir = jmsg["msg"]["dir"];
            M0iSpd = jmsg["msg"]["spd"];
            break;
        case  CarMSG::CarSPD:    // 方向和
            ContrlMod = 1;
            M1iDir = jmsg["msg"]["dir"];
            M1iSpd1 = jmsg["msg"]["spd1"];
            M1iSpd2 = jmsg["msg"]["spd2"];
            M1iSpd3 = jmsg["msg"]["spd3"];
            M1iSpd4 = jmsg["msg"]["spd4"];
            break;
        case  CarMSG::CarTOR:    // 方向和
            ContrlMod = 2;
            M2iDir = jmsg["msg"]["dir"];
            M2iTor1 = jmsg["msg"]["tor1"];
            M2iTor2 = jmsg["msg"]["tor2"];
            M2iTor3 = jmsg["msg"]["tor3"];
            M2iTor4 = jmsg["msg"]["tor4"];
            break;
        // pid操作
        case CarMSG::CarDirPIDQ:    // 查询 方向 pid
            {//
                float kp,ki,kd;
                json jmsgD;
                agv->DirPidQuery(kp,ki,kd);

                jmsgD["typ"] = CarMSG::CarDirPIDQ;
                jmsgD["msg"]["kp"] = kp;
                jmsgD["msg"]["ki"] = ki;
                jmsgD["msg"]["kd"] = kd;
                s.broadcast(jmsgD.dump());

            }
            break;

        case CarMSG::CarDirPIDM:    // 修改 方向 pid
            {
                float kp,ki,kd;
                kp = jmsg["msg"]["kp"] ;
                ki = jmsg["msg"]["ki"] ;
                kd = jmsg["msg"]["kd"] ;
                agv->DirPidModify(kp,ki,kd);
                LOG(INFO) << "修改 Dir Pid" << kp << " " << ki << " " << kd;
            }
            break;
        case CarMSG::CarDriPIDQ:    // 查询 驱动 pid
            {
                float kp,ki,kd;
                float mid;
                json jmsgM;
                mid = (int)jmsg["msg"]["mot"];
                agv->MotorPidQuery(mid,kp,ki,kd);

                jmsgM["typ"] = CarMSG::CarDirPIDQ;
                jmsgM["msg"]["mot"] = mid;
                jmsgM["msg"]["kp"] = kp;
                jmsgM["msg"]["ki"] = ki;
                jmsgM["msg"]["kd"] = kd;
                s.broadcast(jmsgM.dump());
            }
            break;

        case CarMSG::CarDriPIDM:    // 修改 驱动 pid
            //
            {
                float kp,ki,kd;
                float mid;
                mid = (int)jmsg["msg"]["mot"];
                kp = jmsg["msg"]["kp"];
                ki = jmsg["msg"]["ki"];
                kd = jmsg["msg"]["kd"];
                agv->MotorPidModify(mid,kp,ki,kd);
                LOG(INFO) << "修改 Dri" << mid << " " << kp << " " << ki << " " << kd;
            }
            break;
    }
}



// 程序终止
void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        keepRunning = false;
    }
    LOG(WARNING) << "Terminated by Ctrl+C signal.";
}
//****************************************************
void readTxt()
{
    using namespace std;
    char *path = NULL;
    std::string root;
    //也可以将buffer作为输出参数
    path = getcwd(NULL, 0);
    root = path;
    root.append("/data/data.txt");
//    cout<<root;
    free(path);

    ifstream infile;
    infile.open(root.data()); //将文件流对象与文件连接起来
    while(!infile.is_open()) //若失败,则输出错误消息,并终止程序运行
    {
        cout<<"打开data失败";
        infile.open(root.data()); //将文件流对象与文件连接起来
    }

    float c[6]={0};
    int i = 0;
    while (!infile.eof())// 逐个字符读入忽略空格与回车直到结束
    {
        infile >>c[i];
//        cout<<c[i]<<endl;
        i++;
        if(i>5){break;}
    }


    infile.close(); //关闭文件输入流
    ContrlMod = c[0];
    if(M1iSpd1 != c[1]) {M1iSpd1 = c[1];}
    if(M1iSpd2 != c[2]) {M1iSpd2 = c[2];}
    if(M1iSpd3 != c[3]) {M1iSpd3 = c[3];}
    if(M1iSpd4 != c[4]) {M1iSpd4 = c[4];}
    if(M1iDir != c[5])  {M1iDir = c[5];}

}
void read_data(string file)
{
    float angle_temp;
    int speed[4];
    json obj_data;
    std::ifstream fin(file, std::ios::binary);
//    std::ifstream fin(file);
    if (!fin.is_open())
    {
        std::cout << "open json file failed.";

    }
    fin>>obj_data;
    fin.close();
    angle_temp = obj_data["angle"];
    speed[0] = obj_data["speed_1"];
    speed[1] = obj_data["speed_2"];
    speed[2] = obj_data["speed_3"];
    speed[3] = obj_data["speed_4"];
    if(speed[0] !=  M1iSpd1){M1iSpd1 = speed[0];}
    if(speed[1] !=  M1iSpd2){M1iSpd2 = speed[1];}
    if(speed[2] !=  M1iSpd3){M1iSpd3 = speed[2];}
    if(speed[3] !=  M1iSpd4){M1iSpd4 = speed[3];}
    if(angle_temp != M1iDir){M1iDir = angle_temp;}
}
/****************创建接收线程**************************/
void callback(const joy_data::controlConstPtr&msg){
    using namespace std;
    M1iDir = msg->turn;
    M1iSpd1 = msg->moto_1_1;
    M1iSpd2 = msg->moto_1_2;
    M1iSpd3 = msg->moto_1_3;
    M1iSpd4 = msg->moto_1_4;
    cout<<"收到msg"<<endl;
}
//*********************************************

bool info_call_back(agv_control::feedback::Request &req,
        agv_control::feedback::Response &res){
    if(req.num ==1){
        res.data =  agv->dirMotor.angle_feedback;
    }
    return true;
}
int main(int argc, char *argv[])
{
    ros::init(argc, argv,"Agv_Car");
    ros::NodeHandle obj;
    ros::Subscriber sub= obj.subscribe<joy_data::control>("AGV_ControlData", 10, &callback);
    ros::ServiceServer service = obj.advertiseService("car_info", info_call_back);
    std::cout<<"开始执行控制代码"<<std::endl;

    FLAGS_log_dir = "./log";       // 日志输出位置
    // 最低等级
    FLAGS_logtostderr = true;      //设置日志消息是否转到标准输出而不是日志文件
    FLAGS_alsologtostderr = true;  //设置日志消息除了日志文件之外是否去标准输出
    FLAGS_colorlogtostderr = true; //设置记录到标准输出的颜色消息（如果终端支持）

    ParseCommandLineFlags(&argc, &argv, true);//解析命令行
    InitGoogleLogging(argv[0]); //初始化
    signal(SIGINT, sig_handler);    // 注册信号
    //创建线程
    if(!agv->DeviceOpen()){
        LOG(ERROR) << "打开设备失败";
        return 0;
    }
    // 启动控制线程
    agv->setCallBack(AgcCallBack,NULL);

    keepRunning = true;
    // 启动线程，调用AgvControl函数
    pthread_create(&crtlPid,NULL,AgvControl,NULL);

    s.run(port);
    LOG(INFO) << "Running";
//    ros::spin();
    // 等待程序结束
    pthread_join(crtlPid,NULL);//无符号长整型数 crtlPid
    // 安全退出程序
    std::cout<<"控制结束"<<std::endl;
    agv->DeviceClose();
    // 关闭server
    s.close();
    usleep(20000);
    // 关闭






    return 0;
}


