"""
Модуль для управления видеокамерой.
Камера — обязательный элемент (в отличие от serial).
"""

import cv2
import glob
import re


class CameraHandler:
    def __init__(self, dispW=320, dispH=240, fps=30):
        self.dispW = dispW
        self.dispH = dispH
        self.fps = fps
        self.cap = None
        self.device = None

    @staticmethod
    def _sortKey(path):
        match = re.search(r"(\d+)$", path)
        if match:
            return int(match.group(1))
        return 0

    def _scanCameras(self):
        devices = glob.glob("/dev/video*")
        devices.sort(key=self._sortKey)
        return devices

    def connect(self, device=None):
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
        assert self.cap is not None
        return self.cap.read()

    def getWidth(self):
        assert self.cap is not None
        return int(self.cap.get(3))

    def getHeight(self):
        assert self.cap is not None
        return int(self.cap.get(4))

    def close(self):
        if self.cap is not None:
            self.cap.release()
        self.cap = None
        self.device = None