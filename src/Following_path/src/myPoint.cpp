#include "myPoint.h"

//MyPoint::MyPoint(double xPos, double yPos, double alpha, ros::Time t)
//{
//  x = xPos;
//  y = yPos;
//  angle = alpha;
//  time = t;
//}
//
//MyPoint::~MyPoint()
//{
//}


double MyPoint::getAngle(MyPoint* target)
{
  //return atan2((target->y - y),(target->x - x));
}

double MyPoint::getDistance(MyPoint* target)
{
  //return sqrt((pow(target->y - y,2.0)) + (pow(target->x - x,2.0)));
}

vector <vector<float>> Path;
vector<float> variables;



void MyPoint::LonLat2Mercator(double Latitude, double Longitude, double arr[])
{
    double a = 6378.137;
    double e = 0.0818192;
    double k0 = 0.9996;
    double lamda;
    double lamda0;
    double v,A,T,C,S;
    double phi;
    double trans_x,trans_y;

    int  E0 = 500;
    int N0 = 0;
    int zoneNumber;
    double IPI = 0.0174532925199433333333;

    zoneNumber = floor(Longitude/6)+31;
    lamda0 = ((zoneNumber-1)*6-180+3)*IPI;
    lamda = Longitude*IPI;
    phi = Latitude*IPI;
    v = 1/sqrt(1-e*e*(sin(phi)*sin(phi)));
    A = (lamda-lamda0)*cos(phi);
    T = tan(phi)*tan(phi);
    C = e*e*cos(phi)*cos(phi)/(1-e*e);
    S = (1-e*e/4-3*pow(e,4)/64-5*pow(e,6)/256)*phi -
        (3*e*e/8+3*pow(e,4)/32+45*pow(e,6)/1024)*sin(2*phi) +
        (15*pow(e,4)/256+45*pow(e,6)/1024)*sin(4*phi) -
        35*pow(e,6)/3072*sin(6*phi);
    trans_x = (E0 + k0*a*v*(A+(1-T+C)*pow(A,3)/6 + (5-18*T+T*T)*pow(A,5)/120) - 444.4406636)*1000;
    trans_y = (N0 + k0*a*(S+v*tan(phi)*(A*A/2+(5-T+9*C+4*C*C)*pow(A,4)/24+(61-58*T+T*T)*pow(A,6)/720)) - 4426.899512)*1000;

    arr[0] = trans_x;
    arr[1] = trans_y;


}

void MyPoint::Nav_Info2coordinate(ros::NodeHandle n)
{
    MyPoint point;
    Nav_info = n.subscribe<sensor_msgs::NavSatFix>("/Nav_data", 1, boost::bind(&MyPoint::NavCallback, this, _1));
    Coordinate_xy = n.advertise<geometry_msgs::Pose2D>("/AGV_pose",10);
    send_path_service = n.advertiseService("send_path_info", point.PathCallback);

}

void MyPoint::NavCallback(const sensor_msgs::NavSatFix::ConstPtr &Nav_data)
{
    is_ready = true;
    MyPoint point;
    double arr[2];
    point.LonLat2Mercator(Nav_data->latitude, Nav_data->longitude, arr);

    AGV_pose.x = arr[0];
    AGV_pose.y = arr[1];

    cout << "x = " << AGV_pose.x << " ";
    cout << "y = " << AGV_pose.y << " ";

    Coordinate_xy.publish(AGV_pose);

    // glClear(GL_COLOR_BUFFER_BIT);  //用当前背景色填充窗口
    // glColor3f(1.0, 0.0, 0.0);   //设置当前的绘图RGB颜色

    // GLfloat sizes[2];   //保存绘制点的尺寸范围
    // GLfloat step = 1;       //保存绘制点尺寸的步长
    // GLfloat curSize;    //当前绘制的点的大小
    // glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
    // glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);

    // glPointSize(4);
    //       glBegin(GL_POINTS);

    //       glVertex2d(AGV_pose.x, AGV_pose.y);


    //       glEnd();
    //       glFlush();

}

bool MyPoint::PathCallback(Following_path::Path_paraRequest &request, 
                           Following_path::Path_paraResponse &response)
{
   ROS_INFO("callback activated");
   if (request.i < Path.size())
   {
       response.A = Path[request.i][0];
       response.B = Path[request.i][1];
       response.startpoint = Path[request.i][2];
       response.endpoint = Path[request.i][3];
   }
   else
   {
       ROS_INFO("You have arrived the terminal");
       return false;

    }

}

int MyPoint::getNumberOfEdges()
{
    ifstream fin(pop,ios::in);
    if(!fin)
    {
        cerr<<"can not open file"<<endl;
        return -1;
    }


    char c;
    int lineCnt=0;
    while(fin.get(c))
    {
        if(c=='\n')
            lineCnt++;
    }
    fin.close();

    return lineCnt;
}

float MyPoint::lineFit(const Point points[], int nPoint) //友元函数体
{
    float avgX = 0, avgY = 0;    //定义变量
    float lxx = 0, lyy = 0, lxy = 0;
    for (int i = 0; i < nPoint; i++)   //计算X、Y的平均值
    {
        avgX += points[i].GetX() / nPoint;
        avgY += points[i].GetY() / nPoint;
    }
    for (int i = 0; i < nPoint; i++)   //计算Lxx、Lyy和Lxy
    {
        lxx += (points[i].GetX() - avgX) * (points[i].GetX() - avgX);
        lyy += (points[i].GetY() - avgY) * (points[i].GetY() - avgX);
        lxy += (points[i].GetX() - avgX) * (points[i].GetY() - avgY);
    }
    cout << "This line can be fitted by y=ax+b." << endl;
    cout << "a=" << lxy / lxx;  //输出回归系数a
    cout << " b=" << avgY - lxy * avgX / lxx << endl;   //输出回归系数b

    variables.push_back(lxy / lxx);
    variables.push_back(avgY - lxy * avgX / lxx);
    variables.push_back(points[0].GetX());
    variables.push_back(points[nPoint-1].GetX());

    Path.push_back(variables);

    variables.clear();

    return float(lxy / sqrt(lxx * lyy)); //返回相关系数r
}


void MyPoint::read_path(void)
{
    MyPoint point;
    ifstream infile;



    int row = point.getNumberOfEdges();
    double buf[row][3];      //临时保存读取出来的文件内容
    double coordinate[row][2];
    double arr[2];
    cout << row << endl;

    infile.open(pop);
    if (infile.is_open())          //文件打开成功,说明曾经写入过东西
    {
        while (infile.good())
        {
            for (int i = 0; i < row && !infile.eof(); i++)//定义行循环
            {
                for (int j = 0; j < 3; j++)               //定义列循环
                {
                    infile >> buf[i][j];                  //读取一个值（空格、制表符、换行隔开）就写入到矩阵中，行列不断循环进行
                }
            }
        }
        infile.close();
    }


    for (int k = 0; k < row; k++)
    {
        if (!(buf[k][0] == 0) && !(buf[k][1]) == 0)
        {
            point.LonLat2Mercator(buf[k][0], buf[k][1], arr);
            coordinate[k][0] = arr[0];
            coordinate[k][1] = arr[1];


            cout.precision(5);
            cout << coordinate[k][0] << " ";
            cout << coordinate[k][1] << endl;
        }
    }



    for (int m = 0; m < (row/1000)*1000; m += 100)
    {
        Point p[100] = {0};
        for (int n = m, a = 0; n < 100 + m, a < 100; n++, a++)
        {
            p[a] = {Point(coordinate[n][0], coordinate[n][1])};

        }
        float r = point.lineFit(p, 100);    //进行线性回归计算
        cout << "Line coefficient r=" << r << endl; //输出相关系数

    }
    Point p[(row - (row / 1000) * 1000)] = {0};
    for (int q = (row / 1000) * 1000, a = 0; q < row, a < (row - (row / 1000) * 1000) ; q++, a++)
    {

        p[a] = {Point(coordinate[q][0], coordinate[q][1])};
    }
    float r = point.lineFit(p, (row - (row / 1000) * 1000));    //进行线性回归计算
    cout << "Line coefficient r=" << r << endl; //输出相关系数


    glClear(GL_COLOR_BUFFER_BIT);  //用当前背景色填充窗口
    glColor3f(1.0, 0.0, 0.0);   //设置当前的绘图RGB颜色

    GLfloat sizes[2];   //保存绘制点的尺寸范围
    GLfloat step = 1;       //保存绘制点尺寸的步长
    GLfloat curSize;    //当前绘制的点的大小
    glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
    glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);

    //通过点绘制
    for (int i = 0; i < row; i++)  //点的绘制
    {
          glPointSize(2);
          glBegin(GL_POINTS);

          glVertex2d(coordinate[i][0], coordinate[i][1]);


          glEnd();
          glFlush();
          curSize += step;
    }

}
// void  MyPoint::display_path(void)
// {
//     MyPoint point;
//     if(!is_ready)
//     {
//         point.read_path();
//     }
    
// }


