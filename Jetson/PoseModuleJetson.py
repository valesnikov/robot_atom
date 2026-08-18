import cv2
import mediapipe as mp


class PoseModule:
    def __init__(
        self,
        static_image_mode=False,
        model_complexity=1,
        smooth_landmarks=True,
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5,
    ):
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
