# socket_thread_pycpp
socket thread python send and cpp listen


# 前言

本文在ubuntu16.04下实现了python向c++发送数据的socket通信，同时在两个进程中分别开启两个线程，主线程负责处理数据，子线程负责socket通信。（我也是刚开始学，用比较粗糙的方法实现python和c++通信，可能加上线程锁的相关内容会更加严谨，希望大家多多交流指正。）
| |python call.py  |C++ listen.cpp  |
|--|--|--|
|主线程  |随机产生数据  |获取数据并打印|
|子线程  |socket发送  |socket接收|



# 一、Socket和多线程
[Socket原理讲解](https://blog.csdn.net/pashanhu6402/article/details/96428887)
[C++多线程并发（一）--- 线程创建与管理](https://blog.csdn.net/m0_37621078/article/details/104909834)
[浅谈C++中的多线程（一）](https://blog.csdn.net/lizun7852/article/details/88753218)

# 二、效果演示
python发送的数据格式为 `str(double，double，double)`以`,`隔开，C++接收后存入`vector<double>`中。
![在这里插入图片描述](https://img-blog.csdnimg.cn/0be6962aaba34a628ea21d58755b3ddb.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBARnIwbWRlZXBzZWE=,size_20,color_FFFFFF,t_70,g_se,x_16#pic_center)
# 三、代码
程序是在同一台机器的两个终端执行的，如果要在不同机器实现通信，需要两台机器在同一个网段内，再修改代码中的ip地址。


```Python call.py```



```python
import socket
import time
import _thread
import random

def socket_send(name):
    while(1):
        try:
            global msg #使用全局变量进行数据传递
            p = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            p.connect(('127.0.0.1',1234))
            p.send(msg.encode('utf-8'))
            p.close() 
            time.sleep(0.5)
        except:
            # print ("Connection refused")
            continue
list=[0.123124,2.2145,3.023489]#初始化list防止子线程第一次跑不了
strn=str(list[0])
for i in range(1,len(list)):
    strn=strn+','+str(list[i])
msg = strn
name=1
_thread.start_new_thread(socket_send,(name,))#开启多线程
while 1:
    list.clear()
    for i in range(0,3):
        list.append(round(random.random(),6))
    strn=str(list[0])
    for i in range(1,len(list)):
        strn=strn+','+str(list[i])#将数据用,隔开便于C++获取
    msg = strn
    print(msg)
    time.sleep(0.5)
```

```C++ listen.cpp```

```cpp
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <vector>
#include <iterator>  // for std::ostream_iterator
#include <algorithm> // for std::copy
using namespace std;
//打印vector<double>
void print_(vector<double> *p)
{
    for (std::vector<double>::const_iterator i = p->begin(); i != p->end(); i++)
    {
        std::cout << *i << " ";
    }
    cout << endl;
}
//socket监听
void socket_listen(vector<double> *p, int *flag)
{
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));           //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                     //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
    serv_addr.sin_port = htons(1234);                   //端口
    bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    //进入监听状态，等待用户发起请求
    while (1)
    {
        listen(serv_sock, 5);
        
        //接收客户端请求
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (*flag)
        {
            cout << "listen..." << endl;
            //向客户端发送数据
            char str[50];
            recv(clnt_sock, str,49, 0);
            // cout << "get:" << str << endl;
            const char *delim = ",";
            char *re;
            re = strtok(str, delim);
            while (re != NULL)
            {
                // cout << re << endl;
                p->push_back(strtod(re, NULL));
                re = strtok(NULL, delim);
            }
            
        }
        else
        {
            cout << "wait for flag ..." << endl;
            continue;
        }
    }
}
//socket获取数据并存入vector<double>中
void get_socket_data(vector<double> *p,vector<double> &q)
{
    while (1)
    {
        if (p->size() == 3)
        {
            // print_(p);
            q=*p;
            p->clear();
            break;
        }
        else
        {
            continue;
        }
    }
}
int main()
{
    int flag = 0;//socket状态标志符 1为开启监听
    vector<double> temp;
    vector<double> data;
    thread socket_thread(socket_listen, &temp, &flag);
    socket_thread.detach();
    sleep(3);//前三秒flag = 0 终端打印 wait for flag
    flag = 1;
    while(1)
    {
        get_socket_data(&temp,data);
        print_(&data);
    }
    return 0;
}

```

```CMakeLists.txt```

```cpp
cmake_minimum_required(VERSION 3.4.1)
project(socket)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(listen listen.cpp) 
```




