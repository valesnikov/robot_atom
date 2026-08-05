class Vector(list):
    def __add__(self, other):
        return Vector(map(lambda x, y: x + y, self, other))

    def __sub__(self, other):
        return Vector(map(lambda x, y: x - y, self, other))

    def __neg__(self):
        return Vector(map(lambda x: -x, self))

    def __truediv__(self, other):
        return Vector(map(lambda x: x / other, self))

    def __mul__(self, other):
        return Vector(map(lambda x: x * other, self))

    def length(self):
        return sum(map(lambda x: x ** 2, self)) ** 0.5

    @staticmethod
    def emptyVector(directions_len):
        return Vector([0 for _ in range(directions_len)])

    @staticmethod
    def toVector(scalar, direction):
        return Vector([i * scalar / Vector(direction).length() for i in direction]) if scalar != 0 else Vector().emptyVector(len(direction))