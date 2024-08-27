# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Single Color Code Tracking Example
#
# This example shows off single color code tracking using the OpenMV Cam.
#
# A color code is a blob composed of two or more colors. The example below will
# only track colored objects which have both the colors below in them.

import sensor
import time
import math
import display

import pyb
from pyb import UART,LED,Pin

LEDB = LED(3)                  # 蓝色LED

uart = UART(3, 115200)         # 初始化串口3，波特率115200 --》母板上 UART接口

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green things. You may wish to tune them...
thresholds = [
    (
        2,
        43,
        17,
        -18,
        32,
        -4,
    ),  # generic_red_thresholds -> index is 0 so code == (1 << 0)
    (29, 87, -106, -17, 19, 52),
    (0, 38, 26, 127, -26, 30)
]  # generic_green_thresholds -> index is 1 so code == (1 << 1)
# Codes are or'ed together when "merge=True" for "find_blobs".

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA2)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
lcd = display.SPIDisplay()

clock = time.clock()

def find_low(blobs):
    max_cy=0
    for blob in blobs:
        if blob.cy() > max_cy:
            max_blob=blob
            max_cy = blob.cy()
    return max_blob

def find_max(blobs):
    max_size = 0
    for blob in blobs:
        if blob[2] * blob[3] > max_size:
            max_blob = blob
            max_size = blob[2] * blob[3]
    return max_blob

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" must be set to merge overlapping color blobs for color codes.

while True:
    clock.tick()
    img = sensor.snapshot()
    x_error = 10000
    x_error_red = 10000
    #找黑色
    blobs1 = img.find_blobs([thresholds[0]], roi = [0, 50, 160, 140], pixels_threshold=100, area_threshold=100, merge=True)
    if blobs1:                  # 找到追踪目标
        blob = find_low(blobs1)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
            img.draw_rectangle((0, 50, 160, 140),color=(255, 0, 0))       # 根据色块blob位置画红色框
            img.draw_cross(80, 120,color=(0, 0, 255))  # 根据色块位置在中心画蓝色十字
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        x_error = 2 * (blob.cx()/img.width()) - 1 - 0.2
        x_error_red = x_error
        print(x_error)
        y_up = blob.y()
        if blob.cy() < 90:
            x_error = 100


    #找绿色
        blobs2 = img.find_blobs([thresholds[1]], roi = [0, 50, 160, 140], pixels_threshold=100, area_threshold=100, merge=True)
        if blobs2:                  # 找到追踪目标
            blob = find_max(blobs2)  # 提取blobs中面积最大的一个色块blob
            if blob.elongation() > 0.5:
                img.draw_edges(blob.min_corners(), color=(255, 0, 0))
                img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
                img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
                img.draw_rectangle((0, 50, 160, 140),color=(255, 0, 0))       # 根据色块blob位置画红色框
                img.draw_cross(80, 120,color=(0, 0, 255))  # 根据色块位置在中心画蓝色十字
            # These values are stable all the time.
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(), blob.cy())
            # Note - the blob rotation is unique to 0-180 only.
            img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
            x_error = 2 * (blob.cx()/img.width()) - 1 - 0.2
            x_error_red = x_error
            #print(blob.area())
            y_up = blob.y()
            if blob.area() > 8200:
                x_error = 500
                #print("OKKKKKKKKKKKLKKK")

        #找绿色
    blobs3 = img.find_blobs([thresholds[2]], roi = [0, 50, 160, 140], pixels_threshold=100, area_threshold=100, merge=True)
    if blobs3:                  # 找到追踪目标
        blob = find_max(blobs3)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
            img.draw_rectangle((0, 50, 160, 140),color=(255, 0, 0))       # 根据色块blob位置画红色框
            img.draw_cross(80, 120,color=(0, 0, 255))  # 根据色块位置在中心画蓝色十字
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        x_error = 2 * (blob.cx()/img.width()) - 1 - 0.2
        x_error_red = x_error
        print(blob.area())
        y_up = blob.y()
        if blob.area() > 6000:
            x_error = 800







        #print(blob.cx())
        #print("xy")
        #print(blob.cy())
                     # 计算中心偏差x_error


    uart.write(str(x_error) + '\n')                       # 将变量通过串口发送出

    time.sleep_ms(100)                          # 延时0.5秒
    #LEDB.toggle()
    if uart.any():                              # 如果接收到任何消息
        receive = uart.read().decode().strip()  # 将接收到的消息提取出来
        #print(receive)  #在串行终端 打印接收到的信息
    #lcd.write(img)
    #print(clock.fps())


