import os
import time
import urllib.request

import cv2
import numpy as np

try:
    import mediapipe as mp
    from mediapipe.tasks import python as mp_tasks_python
    from mediapipe.tasks.python import vision as mp_vision
except Exception as exc:
    raise RuntimeError(
        "MediaPipe Tasks API недоступен. Нужен mediapipe с модулем mediapipe.tasks."
    ) from exc


_MODELS = {
    0: (
        "pose_landmarker_lite.task",
        "https://storage.googleapis.com/mediapipe-models/pose_landmarker/"
        "pose_landmarker_lite/float16/1/pose_landmarker_lite.task",
    ),
    1: (
        "pose_landmarker_full.task",
        "https://storage.googleapis.com/mediapipe-models/pose_landmarker/"
        "pose_landmarker_full/float16/1/pose_landmarker_full.task",
    ),
    2: (
        "pose_landmarker_heavy.task",
        "https://storage.googleapis.com/mediapipe-models/pose_landmarker/"
        "pose_landmarker_heavy/float16/1/pose_landmarker_heavy.task",
    ),
}

_URL_BY_NAME = {os.path.basename(path): url for path, url in _MODELS.values()}


# fallback-соединения скелета MediaPipe Pose,
# если в новой версии mediapipe не будет готовой константы
_FALLBACK_POSE_CONNECTIONS = (
    (0, 1),
    (1, 2),
    (2, 3),
    (3, 7),
    (0, 4),
    (4, 5),
    (5, 6),
    (6, 8),
    (9, 10),
    (11, 12),
    (11, 13),
    (13, 15),
    (15, 17),
    (15, 19),
    (15, 21),
    (17, 19),
    (12, 14),
    (14, 16),
    (16, 18),
    (16, 20),
    (16, 22),
    (18, 20),
    (11, 23),
    (12, 24),
    (23, 24),
    (23, 25),
    (25, 27),
    (27, 29),
    (27, 31),
    (29, 31),
    (24, 26),
    (26, 28),
    (28, 30),
    (28, 32),
    (30, 32),
)


def _download_file(url: str, path: str) -> None:
    tmp_path = path + ".part"
    if os.path.exists(tmp_path):
        os.remove(tmp_path)

    urllib.request.urlretrieve(url, tmp_path)
    os.replace(tmp_path, path)


def _normalize_index(value) -> int:
    if hasattr(value, "value"):
        value = value.value
    return int(value)


def _get_pose_connections():
    """
    Пытаемся взять готовые соединения из нового mediapipe.
    Если их нет — используем fallback.
    """
    try:
        connections = getattr(mp_vision, "POSE_CONNECTIONS", None)
        if connections:
            return connections
    except Exception:
        pass

    try:
        pose_landmarks = getattr(mp_vision, "PoseLandmarks", None)
        connections = getattr(pose_landmarks, "POSE_CONNECTIONS", None)
        if connections:
            return connections
    except Exception:
        pass

    return _FALLBACK_POSE_CONNECTIONS


class _TaskLandmark:
    """
    Эмуляция landmark-объекта, похожего на старый
    mediapipe.solutions.pose.PoseLandmark.
    """
    __slots__ = ("x", "y", "z", "visibility")

    def __init__(self, lm):
        self.x = float(lm.x)
        self.y = float(lm.y)
        self.z = float(lm.z)

        visibility = getattr(lm, "visibility", None)
        if visibility is None:
            visibility = 1.0

        self.visibility = float(visibility)


class _TaskPoseLandmarks:
    __slots__ = ("landmark",)

    def __init__(self, landmarks):
        self.landmark = [_TaskLandmark(lm) for lm in landmarks]


class _TaskResult:
    """
    Эмуляция results, чтобы работало:
    detector.results.pose_landmarks.landmark
    """
    __slots__ = ("pose_landmarks",)

    def __init__(self, landmarks):
        if landmarks:
            self.pose_landmarks = _TaskPoseLandmarks(landmarks)
        else:
            self.pose_landmarks = None


class pose_module:
    def __init__(
        self,
        static_image_mode=False,
        model_complexity=1,
        smooth_landmarks=True,
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5,
        model_path=None,
        auto_download=True,
    ):
        self.results = None
        self.lms = []

        self.static_image_mode = static_image_mode
        self.model_complexity = model_complexity
        self.smooth_landmarks = smooth_landmarks
        self.min_detection_confidence = min_detection_confidence
        self.min_tracking_confidence = min_tracking_confidence

        self.model_path = self._prepare_model(model_path, auto_download)
        self._last_timestamp_ms = 0
        self._connections = _get_pose_connections()

        if self.static_image_mode:
            running_mode = mp_vision.RunningMode.IMAGE
        else:
            running_mode = mp_vision.RunningMode.VIDEO

        options_kwargs = {
            "base_options": mp_tasks_python.BaseOptions(
                model_asset_path=self.model_path
            ),
            "running_mode": running_mode,
            "num_poses": 1,
            "min_pose_detection_confidence": float(self.min_detection_confidence),
        }

        # min_tracking_confidence имеет смысл для видео-режима
        if not self.static_image_mode:
            options_kwargs["min_tracking_confidence"] = float(
                self.min_tracking_confidence
            )

        options = mp_vision.PoseLandmarkerOptions(**options_kwargs)
        self.landmarker = mp_vision.PoseLandmarker.create_from_options(options)

    def _prepare_model(self, model_path, auto_download):
        """
        Возвращает путь к модели .task.
        Если model_path=None, выбирает модель по model_complexity:
        0 -> lite
        1 -> full
        2 -> heavy
        """
        if model_path is None:
            try:
                complexity = int(self.model_complexity)
            except Exception:
                complexity = 1

            if complexity < 0:
                complexity = 0
            if complexity > 2:
                complexity = 2

            model_path, model_url = _MODELS[complexity]
        else:
            model_url = _URL_BY_NAME.get(os.path.basename(model_path))

        if os.path.isfile(model_path):
            return model_path

        if auto_download and model_url:
            directory = os.path.dirname(os.path.abspath(model_path))
            if directory:
                os.makedirs(directory, exist_ok=True)

            try:
                _download_file(model_url, model_path)
            except Exception as exc:
                raise FileNotFoundError(
                    f"Не удалось скачать модель MediaPipe: {model_url}\n"
                    f"Скачай файл вручную и положи его как: {model_path}"
                ) from exc

            return model_path

        raise FileNotFoundError(
            f"Файл модели MediaPipe не найден: {model_path}\n"
            "Скачай модель pose_landmarker .task и укажи путь в model_path."
        )

    def process(self, img, draw=True):
        if img is None:
            return img

        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        img_rgb = np.ascontiguousarray(img_rgb)

        mp_image = mp.Image(
            image_format=mp.ImageFormat.SRGB,
            data=img_rgb,
        )

        if self.static_image_mode:
            result = self.landmarker.detect(mp_image)
        else:
            timestamp_ms = int(time.time() * 1000)

            if timestamp_ms <= self._last_timestamp_ms:
                timestamp_ms = self._last_timestamp_ms + 1

            self._last_timestamp_ms = timestamp_ms
            result = self.landmarker.detect_for_video(mp_image, timestamp_ms)

        raw_landmarks = []

        if result is not None:
            pose_landmarks = getattr(result, "pose_landmarks", None)
            if pose_landmarks:
                raw_landmarks = pose_landmarks[0] or []

        self.results = _TaskResult(raw_landmarks)
        self.lms = self.get_landmarks() or []

        if draw and self.results.pose_landmarks:
            self._draw_landmarks(img, self.results.pose_landmarks.landmark)

        return img

    def get_landmarks(self):
        if (
            self.results is not None
            and self.results.pose_landmarks is not None
        ):
            return self.results.pose_landmarks.landmark

        return None

    def _draw_landmarks(self, img, landmarks):
        h, w = img.shape[:2]
        points = []

        for lm in landmarks:
            x = int(lm.x * w)
            y = int(lm.y * h)
            points.append((x, y))
            cv2.circle(img, (x, y), 2, (0, 255, 0), -1)

        for connection in self._connections:
            if hasattr(connection, "start") and hasattr(connection, "end"):
                a = _normalize_index(connection.start)
                b = _normalize_index(connection.end)
            else:
                try:
                    a, b = connection
                    a = _normalize_index(a)
                    b = _normalize_index(b)
                except Exception:
                    continue

            if not (0 <= a < len(points) and 0 <= b < len(points)):
                continue

            if landmarks[a].visibility <= 0.1 or landmarks[b].visibility <= 0.1:
                continue

            cv2.line(img, points[a], points[b], (0, 255, 0), 2)

    def close(self):
        if getattr(self, "landmarker", None) is not None:
            try:
                self.landmarker.close()
            except Exception:
                pass
            self.landmarker = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def __del__(self):
        try:
            self.close()
        except Exception:
            pass