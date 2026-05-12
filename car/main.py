RED_THRESHOLD = (40, 24, -1, 32, 2, 24)
import sensor, image, time
from pyb import UART,LED

uart = UART(3, 115200, timeout_char=1000)
led1 = LED(1)

sensor.reset()
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQQVGA)
sensor.skip_frames(time=500)
#sensor.set_auto_whitebal(True)
#sensor.set_auto_gain(False)
#sensor.set_auto_whitebal(False)

ROI_BOTTOM = (8, 42, 64, 18)
ROI_LEFT = (14, 34, 12, 12)
ROI_RIGHT = (53, 34, 12, 12)

flag0 = 0
flag1 = 0
flag2 = 0
state = 0
offset = 0


def largest_blob(img, roi, min_pixels):
    blobs = img.find_blobs([RED_THRESHOLD], roi=roi, merge=True,
                           pixels_threshold=min_pixels, area_threshold=min_pixels)
    return max(blobs, key=lambda b: b.pixels()) if blobs else None


while True:
    img = sensor.snapshot()

    bottom = largest_blob(img, ROI_BOTTOM, 8)      #中
    left = largest_blob(img, ROI_LEFT, 3)          #左
    right = largest_blob(img, ROI_RIGHT, 3)        #右

    if bottom:
        if left and right:                         #十字路口为state2
            flag1 = 0
            flag0 = 0
            img.draw_rectangle(left.rect(), color=(255,0,0))
            img.draw_rectangle(right.rect(), color=(255,0,0))
            flag2 = 0
            state = 2
            offset = bottom.cx() - (img.width() / 2)

        else:                                       #直线为state1
            flag2 = 0
            flag0 = 0
            img.draw_rectangle(bottom.rect(), color=(255,0,0))            #中间识别到的blob画框
            img.draw_cross(bottom.cx(), bottom.cy(), color=(0,255,0))     #中心十字

            flag1 = 0
            state = 1
            offset = bottom.cx() - (img.width() / 2)

    else:                                           #空白为state0
        flag0 = flag0 + 1
        flag1 = 0
        flag2 = 0
        if flag0 >= 9:
            flag0 = 0
            state = 0
            offset = 0

    img.draw_rectangle(ROI_BOTTOM, color=(0,255,0))                   #底部检测区域
    img.draw_rectangle(ROI_LEFT, color=(0,255,0))                     #左
    img.draw_rectangle(ROI_RIGHT, color=(0,255,0))                    #右

    if state == 1:
        led1.on()
    else:
        led1.off()
    uart.write("{},{}\r\n".format(state, offset))             #状态和偏移量
    print(state, offset)
