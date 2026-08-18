from SerialHandler import SerialHandler
from CameraHandler import CameraHandler
import time
import cv2
import math
import numpy as np

# На Jetson (mediapipe 0.8.5-cuda102) нет модуля mediapipe.tasks,
# поэтому пробуем новый модуль, а при неудаче — старый Jetson-модуль.
# try:
#     from PoseModule import PoseModule
# except (ImportError, RuntimeError):
#     from PoseModuleJetson import PoseModule
from PoseModuleJetson import PoseModule


print("******************************")
print("Библиотеки подключены")


def worldLandmarks(marks, res):
    w = float(res[0])
    h = float(res[1])
    aspect = h / w if w != 0.0 else 1.0

    dots = [np.array([m.x, m.y * aspect, m.z / 2.0], dtype=float) for m in marks]
    centre = (dots[11] + dots[12] + dots[23] + dots[24]) / 4.0

    base = np.linalg.norm(dots[11] - dots[12])
    k = 0.26 / base if base != 0.0 else 0.0

    for c, dot in enumerate(dots):
        dots[c] = (centre - dot) * k

    return dots, k, float(centre[0])


def detectAngle(points):
    a = np.linalg.norm(points[0] - points[2])
    b = np.linalg.norm(points[0] - points[1])
    c = np.linalg.norm(points[2] - points[1])

    if b == 0.0 or c == 0.0:
        return 0.0

    cos = (b * b + c * c - a * a) / (2.0 * b * c)
    cos = max(-1.0, min(1.0, float(cos)))

    return math.degrees(math.acos(cos))


def toByte(val, min_, max_):
    val = float(val)
    min_ = float(min_)
    max_ = float(max_)

    span = max_ - min_
    if span == 0.0:
        return (2).to_bytes(1, "big")

    limited = min(max(val - min_, 0.0), span)
    byte_val = int(round(limited * (253.0 / span))) + 2

    return byte_val.to_bytes(1, "big")


class RobotApp:
    def __init__(self, arduino, camera, detector):
        self.arduino = arduino
        self.camera = camera
        self.detector = detector
        self.switch = 0

    def _serialCommand(self):
        if self.arduino.available():
            return self.arduino.read()
        return None

    def _handleCommand(self, inp, success, img, curTime):
        if inp == "mirror":
            self.switch = 1
            print("send mirror")
            self.arduino.write("mirror\n")
            time.sleep(0.5)

        elif inp == "reset":
            self.switch = 0

        elif inp == "screenshot":
            if success:
                cv2.imwrite("screenshots/{0}.jpg".format(curTime), img)
                self.arduino.write("flash\n")
                time.sleep(0.5)

    def _sendPose(self, wlms, k, centre):
        angleR = detectAngle([wlms[13], wlms[11], wlms[23]])
        angleL = detectAngle([wlms[14], wlms[12], wlms[24]])

        self.arduino.bytewrite((1).to_bytes(1, "big"))
        self.arduino.bytewrite(toByte(wlms[15][2] - wlms[11][2], 0, 1))
        self.arduino.bytewrite(toByte(wlms[16][2] - wlms[12][2], 0, 1))
        self.arduino.bytewrite(toByte(angleR, 20, 180))
        self.arduino.bytewrite(toByte(angleL, 20, 180))
        self.arduino.bytewrite(toByte(centre, 0, 1))
        self.arduino.bytewrite(toByte(k / 5.0, 0, 1))

        if angleR > 160 and angleL > 160:
            self.arduino.bytewrite((0).to_bytes(1, "big"))
            self.switch = 0

    def run(self):
        prevTime = time.time()

        while True:
            curTime = time.time()
            success, img = self.camera.read()

            inp = self._serialCommand()
            self._handleCommand(inp, success, img, curTime)

            if not success:
                continue

            img, lms = self.detector.process(img)

            if lms:
                wlms, k, centre = worldLandmarks(
                    lms, (self.camera.getWidth(), self.camera.getHeight())
                )

                if self.switch == 1:
                    self._sendPose(wlms, k, centre)

            dt = curTime - prevTime
            frameTime = round(1.0 / dt, 1) if dt > 0.0 else 0.0

            cv2.putText(
                img,
                str(frameTime) + "fps",
                (40, 50),
                cv2.FONT_HERSHEY_PLAIN,
                1.5,
                (0, 0, 255),
                2,
            )

            cv2.imshow("Image", img)
            cv2.waitKey(1)

            prevTime = curTime


def main():
    arduino = SerialHandler()
    print("Наличие Ардуино:")
    print(arduino.connect(test=True))

    camera = CameraHandler()
    if not camera.connect():
        return

    time.sleep(1)

    detector = PoseModule()
    app = RobotApp(arduino, camera, detector)
    app.run()


if __name__ == "__main__":
    main()
