import time
import serial
import re
import glob

class my_serial:
    def __init__(self, speed=115200, timeout=1, ):
        self.speed = speed
        self.timeout = timeout

        self.ser = None
        self.connected = False
        self.device = None

    @staticmethod
    def _sort_key(path):
        match = re.search(r"(\d+)$", path)
        if match:
            return int(match.group(1))
        return 0

    def _scan_ports(self):
        """
        Ищет /dev/ttyUSB* и /dev/ttyACM*.
        """
        ports = []

        for pattern in ("/dev/ttyUSB*", "/dev/ttyACM*"):
            found = glob.glob(pattern)
            found.sort(key=self._sort_key)
            ports.extend(found)

        return ports

    def connect(self, device=None, test=False):
        if self.connected and self.ser is not None:
            return True

        if device is not None:
            candidates = [device]
        else:
            candidates = self._scan_ports()

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
        Проверка связи. Ардуино должна отвечать строкой heartbeat.
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
        Читает строку. Если порт не подключён — возвращает пустую строку.
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
        Отправка строки. Если порт не подключён — команда игнорируется.
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
        Отправка одного байта. Если порт не подключён — команда игнорируется.
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
        Количество байт в буфере приёма.
        Если порт не подключён — возвращает 0.
        """
        if not self.connected or self.ser is None:
            return 0

        try:
            return self.ser.in_waiting

        except (serial.SerialException, OSError):
            self.close()
            return 0
