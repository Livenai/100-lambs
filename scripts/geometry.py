from math import sqrt

class Point:
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

    def __str__(self):
        return "(x: {0}, y :{1})".format(self.x, self.y)

    def __repr__(self):
        return "(x: {0}, y :{1})".format(self.x, self.y)

    def __add__(self, other):
        return Point(self.x + other.x,  self.y + other.y)

    def __sub__(self, other):
        return Point(self.x - other.x,  self.y - other.y)

    def __truediv__(self, number):
        return Point(self.x/number, self.y/number)

    def distance(self, p):
        return sqrt((self.x - p.x)**2 + (self.y - p.y)**2)

    @staticmethod
    def average(*points):
        x = 0
        y = 0
        for p in points:
            x += p.x
            y += p.y
        n = len(points)
        return Point(x/n, y/n)


    @staticmethod
    def mid_point(p0, p1):
        return (p0+p1)/2


    @staticmethod
    def bisector(p0, p1):
        p = Point.mid_point(p0, p1)
        aux = p1 - p0
        dir = Point(-aux.y, aux.x)
        return Line(p, p+dir)



# Rectangulo alineado con los ejes x, y
class Rectangle:
    def __init__(self, pos, width, height):
        self.x0 = pos.x - (width / 2)
        self.x1 = self.x0 + width
        self.y0 = pos.y - (height / 2)
        self.y1 = self.y0 + height

    # distancia desde un punto hasta el rectangulo alineado con los ejes x e y
    def distance(self, point):
        if point.x < self.x0:
            h = self.x0 - point.x
        elif point.x > self.x1:
            h = self.x1 - point.x
        else:
            h = 0
        if point.y < self.y0:
            v = self.y0 - point.y
        elif point.y > self.y1:
            v = self.y1 - point.y
        else:
            v = 0

        return sqrt(h**2 + v**2)


class Circle:
    def __init__(self, center = Point(0,0) , radius = 1):
        self.center = center
        self.radius = radius

    def __str__(self):
        return 'center: {0}, radius: {1}'.format(self.center, self.radius)


    def includePoint(self, point):
        return point.distance(self.center) <= self.radius #+ error TODO


    # Funcion recursiva para calcular el circulo minimo
    # Se llama desde min_cicle para iniciarla correctamente
    # p inicialmente contiene todos los puntos que hay que incluir en el circulo
    # r contiene puntos que podrian estar en la circunferencia exterior
    @staticmethod
    def _welzl(p, r):
        if len(p) == 1 and len(r) == 0:
            return Circle(center = p[0], radius = 0)
        elif len(p) == 0 and len(r) <3:
            if len(r) == 1:
                return Circle(center = r[0], radius = 0)
            elif len(r) == 2:
                center = Point.mid_point(r[0], r[1])
                radius = center.distance(r[0])
                return Circle(center, radius)
        elif len(r) >= 3:
            a = Point.bisector(r[0],r[1])
            b = Point.bisector(r[1],r[2])
            center = Line.intersection(a, b)
            if center != None:
                radius = center.distance(r[0])
                return Circle(center, radius)
            else:
            # en este caso los puntos de r estan alineados se excluye el que
            # está en el medio y se hace el circulo minimo con los otros
                p2 = [(v, sum([v.distance(z) for z in r[:3]])) for v in r[:3]]
                p2.remove(min(p2, key=lambda x: x[1])) #estos elimina ese punto
                center = Point.mid_point(p2[0][0], p2[1][0])
                radius = center.distance(p2[0][0])

                # return Circle(center, 0)
                return Circle(center, radius)
        else:
            circle = Circle._welzl(p[1:], r)
            if circle.includePoint(p[0]):
                return circle
            else:
                return Circle._welzl(p[1:], r + [p[0]])
    # @staticmethod
    # def _welzl(p, r):
    #     # soluciones trivial
    #     print("p:{}".format(len(p)),end=', ')
    #     print("r:{}".format(len(r)))
    #     if len(p) == 1 and len(r) == 0:
    #         return Circle(center = p[0], radius = 0)
    #     elif len(p) == 0 or len(r) >= 3:
    #         if len(r) == 1:
    #             return Circle(center = r[0], radius = 0)
    #         elif len(r) == 2:
    #             center = Point.mid_point(r[0], r[1])
    #             radius = center.distance(r[0])
    #             return Circle(center, radius)
    #         else:
    #             a = Point.bisector(r[0],r[1])
    #             b = Point.bisector(r[1],r[2])
    #             center = Line.intersection(a, b)
    #             radius = center.distance(r[0])
    #             return Circle(center, radius)
    #     else:
    #         circle = Circle._welzl(p[1:], r)
    #         if circle.includePoint(p[0]):
    #             print("NO")
    #             return circle
    #         else:
    #
    #
    #             return Circle._welzl(p[1:], r + [p[0]])

    #Devuelve el circulo más pequeño que incluye todos los puntos dados
    @staticmethod
    def min_circle(* points):
        if len(points) == 0:
            raise AttributeError("min_circle tiene que ser llamados con uno o mas puntos")
        r = []
        # randomize(points) #TODO en cierto modo se pueden considerar ya aleatorios
        return Circle._welzl(points, r)

class Line:
    def __init__(self, p0, p1):
        dy = (p1.y -p0.y)
        dx = (p1.x - p0.x)
        if(dx != 0):
            self.slope = dy/ dx
        else:
            self.slope = "inf"
        self.p0 = p0
        self.p1 = p1

    # TODO getpointatx


    @staticmethod
    def intersection(l0, l1):
        if l0.slope == l1.slope:
            return None # son paralelas

        if "inf" not in (l0.slope, l1.slope):
            x = (l0.slope*l0.p0.x) -l0.p0.y -(l1.slope* l1.p0.x)+l1.p0.y
            x /= (l0.slope - l1.slope)
            y = l0.slope*(x - l0.p0.x) + l0.p0.y
        else:
            if l0.slope == "inf":
                aux = l0
                l0 = l1
                l1 = aux

            x = l1.p0.x
            y = l0.slope *(x -l0.p0.x) + l0.p0.y

        return Point(x, y)
