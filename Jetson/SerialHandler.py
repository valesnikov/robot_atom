"""
Модуль для управления последовательным портом (Serial).

Предоставляет функциональность для сканирования портов, подключения
к устройствам (например Arduino), чтения/записи данных и проверки
связи через heartbeat.
"""

import time
import serial
import re
import glob


class SerialHandler:
    """
    Менеджер последовательного порта.

    Сканирует /dev/ttyUSB* и /dev/ttyACM*, подключается и предоставляет
    методы для чтения/записи строк и байтов, а также для heartbeat-проверки.
    """

    def __init__(self, speed=115200, timeout=1):
        """
        speed   — скорость порта (бод).
        timeout — таймаут чтения (сек).
        """
        self.speed = speed
        self.timeout = timeout

        self.ser = None
        self.connected = False
        self.device = None

    @staticmethod
    def _sort_key(path):
        """Извлекает числовой суффикс из пути порта для сортировки."""
        match = re.search(r"(\d+)$", path)
        if match:
            return int(match.group(1))
        return 0

    def _scanPorts(self):
        """
        Ищет устройства /dev/ttyUSB* и /dev/ttyACM*.
        Возвращает отсортированный список.
        """
        ports = []

        for pattern in ("/dev/ttyUSB*", "/dev/ttyACM*"):
            found = glob.glob(pattern)
            found.sort(key=self._sort_key)
            ports.extend(found)

        return ports

    def connect(self, device=None, test=False):
        """
        Подключается к порту. Если device=None — сканирует автоматически.

        Если test=True — после подключения выполняет heartbeat-проверку
        и при неудаче закрывает порт.
        """
        if self.connected and self.ser is not None:
            return True

        if device is not None:
            candidates = [device]
        else:
            candidates = self._scanPorts()

        for port in candidates:
            print(f"Try {port}")
            try:
                ser = serial.Serial(port, self.speed, timeout=self.timeout)
            except (serial.SerialException, OSError):
                continue

            try:
                ser.flush()
                ser.reset_input_buffer()
                ser.reset_output_buffer()
            except (serial.SerialException, OSError):
                try:
                    ser.close()
                except Exception:
                    pass
                continue

            self.ser = ser
            self.device = port
            self.connected = True

            if test:
                if self.heartbeat(10):
                    return True
                self.close()
                continue

            return True

        # Ничего не нашли
        self.ser = None
        self.device = None
        self.connected = False
        return False

    def close(self):
        """Закрывает порт и сбрасывает состояние."""
        if self.ser is not None:
            try:
                self.ser.close()
            except Exception:
                pass

        self.ser = None
        self.connected = False
        self.device = None

    def heartbeat(self, Wtime=1):
        """
        Проверка связи с устройством.

        Отправляет "heartbeat" и ждёт ответ "heartbeat" в течение Wtime секунд.
        Возвращает True при успехе.
        """
        if not self.connected or self.ser is None:
            return False

        start = time.monotonic()

        while time.monotonic() - start < Wtime:
            if not self.write("heartbeat\n"):
                return False

            if self.read() == "heartbeat":
                return True

            time.sleep(0.02)

        return False

    def read(self):
        """
        Читает строку из порта.
        Возвращает пустую строку, если порт не подключён или данных нет.
        """
        if not self.connected or self.ser is None:
            return ""

        try:
            raw = self.ser.readline()

            if not raw:
                return ""

            return raw.decode(errors="ignore").rstrip()

        except (serial.SerialException, OSError):
            self.close()
            return ""

    def write(self, data):
        """
        Отправляет строку в порт.
        Игнорирует команду, если порт не подключён.
        """
        if not self.connected or self.ser is None:
            return False

        try:
            self.ser.write(str(data).encode("utf-8"))
            return True

        except (serial.SerialException, OSError):
            self.close()
            return False

    def bytewrite(self, byte):
        """
        Отправляет один байт в порт.
        Игнорирует команду, если порт не подключён.
        """
        if not self.connected or self.ser is None:
            return False

        try:
            self.ser.write(byte)
            return True

        except (serial.SerialException, OSError):
            self.close()
            return False

    def available(self):
        """
        Возвращает количество байт в буфере приёма.
        Если порт не подключён — возвращает 0.
        """
        if not self.connected or self.ser is None:
            return 0

        try:
            return self.ser.in_waiting

        except (serial.SerialException, OSError):
            self.close()
            return 0
