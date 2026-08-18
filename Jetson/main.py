"""
Главный модуль управления роботом через компьютерное зрение.

Запускает цикл захвата видео с камеры, детекции позы человека через MediaPipe
и отправку управляющих команд на Arduino по Serial.
"""

from SerialHandler import SerialHandler
from CameraHandler import CameraHandler
import time
import cv2
import math
import numpy as np

try:
    from PoseModule import PoseModule
except (ImportError, RuntimeError):
    from PoseModuleJetson import PoseModule


print("******************************")
print("Библиотеки подключены")


def worldLandmarks(marks, res):
    """
    Преобразует мировые координаты landmarks в нормализованный массив
    с центром в середине корпуса и фиксированным масштабом.

    Возвращает (dots, k, centre):
        dots   — массив (33, 3) с координатами, центрированными относительно торса
        k      — коэффициент масштаба (0.26 / расстояние между плечами)
        centre — x-координата центра торса
    """
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
    """
    Вычисляет угол между тремя точками через теорему косинусов.
    points: [p0, p1, p2], где p1 — вершина угла.
    Возвращает угол в градусах (0–180).
    """
    a = np.linalg.norm(points[0] - points[2])
    b = np.linalg.norm(points[0] - points[1])
    c = np.linalg.norm(points[2] - points[1])

    if b == 0.0 or c == 0.0:
        return 0.0

    cos = (b * b + c * c - a * a) / (2.0 * b * c)
    cos = max(-1.0, min(1.0, float(cos)))

    return math.degrees(math.acos(cos))


def toByte(val, min_, max_):
    """
    Преобразует float из диапазона [min_, max_] в byte [2..255].
    2 = минимум, 255 = максимум.
    Используется для упаковки данных перед отправкой на Arduino.
    """
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
    """
    Главный класс приложения: связывает камеру, детектор позы и Arduino.

    В цикле получает кадры, детектирует позу, слушает команды с Arduino
    (mirror, reset, screenshot) и при необходимости отправляет данные позы.
    """

    def __init__(self, arduino, camera, detector):
        """
        arduino  : SerialHandler
        camera   : CameraHandler
        detector : PoseModule
        """
        self.arduino = arduino
        self.camera = camera
        self.detector = detector
        self.switch = 0

    def _serialCommand(self):
        """Читает команду с Arduino, если есть данные в буфере."""
        if self.arduino.available():
            return self.arduino.read()
        return None

    def _handleCommand(self, inp, success, img, curTime):
        """
        Обрабатывает команды с Arduino:
          mirror     — активирует режим отправки позы
          reset      — деактивирует режим отправки позы
          screenshot — сохраняет кадр в screenshots/
        """
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
        """
        Упаковывает параметры позы и отправляет на Arduino (7 байт):
          [0] = 1 (маркер)
          [1] = высота правой руки относительно плеча
          [2] = высота левой руки относительно плеча
          [3] = угол правого плеча
          [4] = угол левого плеча
          [5] = центр торса (x)
          [6] = масштаб / 5

        Если оба угла > 160° — автоматический выход из mirror.
        """
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
        """
        Основной цикл: захват кадра → детекция позы → обработка команд →
        отправка данных на Arduino (если mirror) → отображение с FPS.
        """
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
    """Точка входа: инициализация SerialHandler, CameraHandler, PoseModule и запуск RobotApp."""
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
