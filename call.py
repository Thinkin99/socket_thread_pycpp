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