"""Модуль детекции позы через классический mediapipe.solutions.pose (старый API)."""

import cv2
import mediapipe as mp


class PoseModule:
    """
    Детектор позы человека через mediapipe.solutions.pose (старый API).

    Предоставляет единый метод process(img, draw=True), возвращающий
    (img, landmarks). Используется на Jetson с mediapipe 0.8.5.
    """

    def __init__(
        self,
        static_image_mode=False,
        model_complexity=1,
        smooth_landmarks=True,
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5,
    ):
        """
        Параметры соответствуют mp.solutions.pose.Pose.
        """
        self.mp_draw = mp.solutions.drawing_utils
        self.mp_pose = mp.solutions.pose
        self.pose = self.mp_pose.Pose(
            static_image_mode,
            model_complexity,
            smooth_landmarks,
            min_detection_confidence,
            min_tracking_confidence,
        )

    def process(self, img, draw=True):
        """
        Детектирует позу на кадре.

        Parameters
        ----------
        img : ndarray
            Кадр BGR.
        draw : bool
            Рисовать ли скелет на изображении.

        Returns
        -------
        tuple (img, lms): кадр с отрисованным скелетом и список landmarks.
        """
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        results = self.pose.process(imgRGB)
        lms = []
        if results.pose_landmarks:
            lms = results.pose_landmarks.landmark
            if draw:
                self.mp_draw.draw_landmarks(
                    img, results.pose_landmarks, self.mp_pose.POSE_CONNECTIONS
                )

        return img, lms
