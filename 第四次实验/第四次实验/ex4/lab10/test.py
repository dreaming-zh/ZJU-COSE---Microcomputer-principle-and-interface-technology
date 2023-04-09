'''
Date: 2022-06-05 12:33:13
Author: Simon QM
Version: 
Description: 
'''


import serial # 导入串口包
import time # 导入时间包
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as ani
 
ser = serial.Serial("COM6", 38400, timeout = 5)
ser.flushInput() # 清空缓冲区
flag = 0
Volta = []
Temper = []


def main():
    global flag
    plt.figure()
    while True:
        count = ser.inWaiting() # 获取串口缓冲区数据
        if count !=0 :
            recv = float(ser.read(ser.in_waiting).decode('UTF-8'))
            if flag == 0:
                flag = 1
                Volta.append(recv)
            else:
                flag = 0
                Temper.append(recv)

        print(Temper)
        print(Volta)
        
        
        
        plt.plot(Temper[-35:-1], color = 'r',linewidth = '1', label = "Temperature", )
        # plt.plot(Volta[-35:-1], color = 'k', linewidth = '1', label = "Voltage",)
        
        # plt.ylim(0,50)
        plt.xlim(0,20)
        

        plt.legend()
        plt.pause(0.05)
        plt.cla()

if __name__ == '__main__':
    main()