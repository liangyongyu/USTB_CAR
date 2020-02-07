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

#ifndef __JOYSTICK_HH__
#define __JOYSTICK_HH__

#include <string>
#include <iostream>

#define JS_EVENT_BUTTON 0x01 // button pressed/released
#define JS_EVENT_AXIS 0x02   // joystick moved
#define JS_EVENT_INIT 0x80   // initial state of device

// 按键定义

class JoystickEvEvents
{
    // 输出坐标

    // protobuf 数据格式
};
#define INT32 signed int

struct JoystickMsg{
    INT32 AXIS[11];
    INT32 BTNs[15];
};


/**
 * Encapsulates all data relevant to a sampled joystick event.
 */

class JoystickEvent
{
  public:
    /** Minimum value of axes range */
    static const short MIN_AXES_VALUE = -32768;

    /** Maximum value of axes range */
    static const short MAX_AXES_VALUE = 32767;

    /**
   * The timestamp of the event, in milliseconds.
   */
    unsigned int time;

    /**
   * The value associated with this joystick event.
   * For buttons this will be either 1 (down) or 0 (up).
   * For axes, this will range between MIN_AXES_VALUE and MAX_AXES_VALUE.
   */
    short value;

    /**
   * The event type.
   */
    unsigned char type;

    /**
   * The axis/button number.
   */
    unsigned char number;

    /**
   * Returns true if this event is the result of a button press.
   */
    bool isButton()
    {
        return (type & JS_EVENT_BUTTON) != 0;
    }

    /**
   * Returns true if this event is the result of an axis movement.
   */
    bool isAxis()
    {
        return (type & JS_EVENT_AXIS) != 0;
    }

    /**
   * Returns true if this event is part of the initial state obtained when
   * the joystick is first connected to.
   */
    bool isInitialState()
    {
        return (type & JS_EVENT_INIT) != 0;
    }

    /**
   * The ostream inserter needs to be a friend so it can access the
   * internal data structures.
   */
    friend std::ostream &operator<<(std::ostream &os, const JoystickEvent &e);
};

/**
 * Stream insertion function so you can do this:
 *    cout << event << endl;
 */
std::ostream &operator<<(std::ostream &os, const JoystickEvent &e);

/**
 * Represents a joystick device. Allows data to be sampled from it.
 */
class Joystick
{
  private:
    void openPath(std::string devicePath, bool blocking = false);

    int _fd;
    typedef void (*CallbackFun)(void *arg);
    bool isOpen_ = false;
  public:
    // 消息
    JoystickMsg jsMsg;
    bool isOpen(){
        return isOpen_;
    }

    bool DeviceClose();
    bool DeviceOpen();
    Joystick();
    ~Joystick();

    void sample();

    // 多线程调用
    pthread_t hand_;
    static void *SampleHelper(void *arg)
    {
        Joystick *js = (Joystick *)arg;
        js->sample();
    }
    void *Parent = NULL;
    // 回调函数
    CallbackFun callBF;
    void SetCallBack(CallbackFun fun,void *arg){
        callBF = fun;
        Parent = arg;
    }
};

#endif