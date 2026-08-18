"""
Модуль для управления видеокамерой через OpenCV.

Автоматически сканирует устройства /dev/video*, подключается к первой
рабочей камере и предоставляет методы для чтения кадров и получения
параметров разрешения.
"""

import cv2
import glob
import re


class CameraHandler:
    """
    Менеджер видеокамеры. Сканирует /dev/video*, подключается и
    предоставляет кадры через read().

    Использование:
        cam = CameraHandler()
        if cam.connect():
            success, frame = cam.read()
    """

    def __init__(self, dispW=320, dispH=240, fps=30):
        """
        dispW, dispH — желаемое разрешение (может быть скорректировано камерой).
        fps — желаемый FPS.
        """
        self.dispW = dispW
        self.dispH = dispH
        self.fps = fps
        self.cap = None
        self.device = None

    @staticmethod
    def _sortKey(path):
        """Извлекает числовой суффикс из пути /dev/videoN для сортировки."""
        match = re.search(r"(\d+)$", path)
        if match:
            return int(match.group(1))
        return 0

    def _scanCameras(self):
        """Возвращает отсортированный список устройств /dev/video*."""
        devices = glob.glob("/dev/video*")
        devices.sort(key=self._sortKey)
        return devices

    def connect(self, device=None):
        """
        Подключается к указанному устройству или сканирует все доступные.
        Возвращает True при успешном подключении.
        """
        if device is not None:
            candidates = [device]
        else:
            candidates = self._scanCameras()

        for dev in candidates:
            print("Попытка подключиться к камере", dev)
            cap = cv2.VideoCapture(dev)

            cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.dispW)
            cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.dispH)
            cap.set(5, self.fps)
            cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

            width = int(cap.get(3))
            height = int(cap.get(4))
            fps = int(cap.get(5))

            if width == 0 or height == 0 or fps == 0:
                cap.release()
                continue

            self.cap = cap
            self.device = dev
            print("Camera resolution: {0}x{1}, FPS: {2}".format(width, height, fps))
            return True

        print("Камера не найдена")
        return False

    def read(self):
        """Возвращает (success, frame) — очередной кадр с камеры."""
        assert self.cap is not None
        return self.cap.read()

    def getWidth(self):
        """Возвращает актуальную ширину кадра в пикселях."""
        assert self.cap is not None
        return int(self.cap.get(3))

    def getHeight(self):
        """Возвращает актуальную высоту кадра в пикселях."""
        assert self.cap is not None
        return int(self.cap.get(4))

    def close(self):
        """Освобождает камеру."""
        if self.cap is not None:
            self.cap.release()
        self.cap = None
        self.device = None
