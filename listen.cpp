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
