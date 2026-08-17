from pose_module import pose_module
from SerialHandler import SerialHandler
from Vector import Vector
import time
import cv2
import math

print("******************************")
print("Библиотеки подключены")
arduino = SerialHandler()
print("Наличие Ардуино:")
print(arduino.connect(test=True))

def world_landmarks(marks, res):
    dots = [Vector([i.x, i.y * (res[1] / res[0]), i.z / 2]) for i in marks]
    visibility = [i.visibility for i in marks]
    centre = (dots[11] + dots[12] + dots[23] + dots[24]) / 4
    k = (dots[11] - dots[12]).length()
    k = 0.26 / (k)
    for c, dot in enumerate(dots):
        dots[c] = (centre - dot) * k
    return dots, visibility, k, centre[0]


def detect_angle(dots):
    a = (dots[0] - dots[2]).length()
    b = (dots[0] - dots[1]).length()
    c = (dots[2] - dots[1]).length()
    cos = ((b**2) + (c**2) - (a**2)) / (2 * b * c)
    angle = math.degrees(math.acos(cos))
    return angle


def serial_monitor():
    if arduino.available():
        return arduino.read()
    else:
        return None


def toByte(val, min_, max_):
    return (
        round(min(max(val - min_, 0), max_ - min_) * (253.0 / (max_ - min_))) + 2
    ).to_bytes(1, "big")


def main():
    global cap
    global switch
    global final_sign
    prev_time = time.time()
    while True:
        cur_time = time.time()
        success, img = cap.read()
        inp = serial_monitor()
        if inp == "mirror":
            switch = 1
            print("send mirror")
            arduino.write("mirror\n")
            time.sleep(0.5)
        if inp == "reset":
            switch = 0
        if inp == "screenshot":
            cv2.imwrite(f"screenshots/{cur_time}.jpg", img)
            arduino.write("flash\n")
            time.sleep(0.5)

        success, img = cap.read()
        if not success:
            continue

        print(switch)

        img = detector.process(img)
        lms = detector.get_landmarks()
        if lms:
            wlms, visible, k, centre = world_landmarks(
                lms, (int(cap.get(3)), int(cap.get(4)))
            )
            if switch == 1:
                angle_r = detect_angle([wlms[13], wlms[11], wlms[23]])
                angle_l = detect_angle([wlms[14], wlms[12], wlms[24]])
                min_ = 20
                max_ = 180
                # print((round(min(max(angle_r-min_, 0), max_-min_)*(253.0/(max_-min_)))+2))
                # print(wlms[16][2]-wlms[12][2], wlms[15][2]-wlms[11][2])
                arduino.bytewrite((1).to_bytes(1, "big"))
                arduino.bytewrite(toByte(wlms[15][2] - wlms[11][2], 0, 1))
                arduino.bytewrite(toByte(wlms[16][2] - wlms[12][2], 0, 1))
                arduino.bytewrite(toByte(angle_r, 20, 180))
                arduino.bytewrite(toByte(angle_l, 20, 180))
                arduino.bytewrite(toByte(centre, 0, 1))
                arduino.bytewrite(toByte(k / 5, 0, 1))
                if angle_r > 160 and angle_l > 160 and 0:
                    arduino.bytewrite((0).to_bytes(1, "big"))
                    switch = 0
                # detect(wlms)

        frame_time = round(1 / (cur_time - prev_time), 1)
        cv2.putText(
            img,
            str(frame_time) + "fps",
            (40, 50),
            cv2.FONT_HERSHEY_PLAIN,
            1.5,
            (0, 0, 255),
            2,
        )
        cv2.imshow("Image", img)
        cv2.waitKey(1)
        prev_time = cur_time


dispW = 320
dispH = 240
i = 0

while True:
    cap = cv2.VideoCapture(i)
    font = cv2.FONT_HERSHEY_SIMPLEX
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, dispW)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, dispH)
    cap.set(5, 30)
    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    width, height = int(cap.get(3)), int(cap.get(4))
    fps = int(cap.get(5))

    if width == 0 or height == 0 or fps == 0:
        print("Попытка подключиться к камере ", i)
        i = i + 1
        if i > 10:
            print("Камера не найдена")
            exit()
    else:
        print(f"Camera resolution: {width}x{height}, FPS: {fps}")
        break

time.sleep(1)
detector = pose_module()
switch = 0
final_sign = ""

global_visibility = 0.5

main()
