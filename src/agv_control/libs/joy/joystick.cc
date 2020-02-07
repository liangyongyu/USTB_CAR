// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright Drew Noakes 2013-2016

#include "joystick.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>
#include "unistd.h"

using namespace std;


Joystick::Joystick()
{
    

}
bool Joystick::DeviceOpen(){

    isOpen_ = false;
    for(int b_ = 0;b_ < 15;b_++){
        jsMsg.BTNs[b_] = 0;
    }
    for(int a_ = 0;a_ < 11;a_++){
        jsMsg.AXIS[a_] = 0;
    }
    _fd = open("/dev/input/js0",  O_RDONLY );
    if (_fd >= 0)
    {
        char data[8];
        for(int a = 0;a < 25;a++) 
        {
            read(_fd,data,8);
        }
        isOpen_ = true;
        pthread_create(&hand_,NULL,Joystick::SampleHelper,(void *)this);        
    
    }
    return _fd >= 0;
}

bool Joystick::DeviceClose()
{
    if(!isOpen_)
    {
        return true;
    }
    isOpen_ = false;

    close(_fd);
    
    return true;
}
// 从 手柄读取按键数值
void Joystick::sample()
{
    JoystickEvent event = JoystickEvent();
    int cout = 0;
    while (isOpen_)
    {
        usleep(1500);
        // 读取数据
        int bytes = read(_fd, &event, sizeof(event));

        // 终端
        if (bytes == -1)
        {
            std::cout << "terminal" <<std::endl;
            isOpen_ = false;
            break;    
        }
            // continue;
        // 进行赋值
        if (event.isButton())
        {
            // cout <<"btn" <<(int)event.number<< endl;//
            jsMsg.BTNs[event.number] = event.value;
        }
        else if (event.isAxis())
        {
            jsMsg.AXIS[event.number] = event.value;
        }
        
        if(Parent != NULL){
            callBF(Parent);
        }
    }
}

// Button 0 is up
// Button 1 is up
// Button 2 is up
// Button 3 is up
// Button 4 is up
// Button 5 is up
// Button 6 is up
// Button 7 is up
// Button 8 is up
// Button 9 is up
// Button 10 is up
// Button 11 is up
// Button 12 is up
// Button 13 is up
// Button 14 is up
// Axis 0 is at position 0
// Axis 1 is at position 0
// Axis 2 is at position 0
// Axis 3 is at position -32767
// Axis 4 is at position -32767
// Axis 5 is at position 0
// Axis 6 is at position -32767
// Axis 7 is at position -32767
// Axis 8 is at position 0
// Axis 9 is at position 0
// Axis 10 is at position -7095

Joystick::~Joystick()
{
    isOpen_ = false;
    pthread_join(hand_,NULL);
    pthread_cancel(hand_);
    
    Parent = NULL;
    close(_fd);
    // std::cout << "123" << std::endl;
    
    
}

std::ostream &operator<<(std::ostream &os, const JoystickEvent &e)
{
    os << "type=" << static_cast<int>(e.type)
       << " number=" << static_cast<int>(e.number)
       << " value=" << static_cast<int>(e.value);
    return os;
}
