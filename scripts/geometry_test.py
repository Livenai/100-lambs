#! /usr/bin/env python3
from geometry import *
from ui_test import Ui_MainWindow
from PyQt5 import QtWidgets
from PyQt5.QtGui import QPen, QBrush, QColor, QFont, QPainter, QImage
from PyQt5.Qt import Qt
from text_background import QGraphicsTextItemWithBackground

import sys
from random import Random


class mywindow(QtWidgets.QMainWindow):

    def __init__(self):
        super(mywindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        # TODO deberia crear self.ui.canvas aqui

        self.dotSize = 5
        self.borderSize = 3
        self.penWhite = QPen(Qt.white, self.borderSize)
        self.penRed = QPen(Qt.red, self.borderSize)
        self.penGreen = QPen(Qt.green, self.borderSize)
        self.penOrange = QPen(QColor( 243, 156, 18 ), self.borderSize)
        self.brushWhite = QBrush(Qt.white)
        self.r = Random()

        rec = self.ui.canvas.sceneRect()
        self.image = QImage(int(rec.width()),int(rec.height()),QImage.Format_RGB32)
        # self.image.fill(0)
        self.painter = QPainter(self.image);
        self.painter.setRenderHint(QPainter.Antialiasing);

    def generatePoints(self, num):
        abc = 'abcdefghijklmnopqrstuvwxyz'
        r1 = [-300, 300]
        r2 = [-100, 100]
        points = {n: Point(self.r.randint(*r1),self.r.randint(*r2)) for n in abc[:num] }

        return points

    def generateRectangles(self, num):
        abc = 'ABCDEFGHIJKLMNOPQRSTUWXYZ'
        r1 = [-200, 200]
        r2 = [100, 300]
        pos = lambda:Point(self.r.randint(*r1),self.r.randint(*r1))
        width  = lambda:self.r.randint(*r2)
        height = lambda:self.r.randint(*r2)
        rects = {n: Rectangle(pos(),width(),height()) for n in abc[:num] }
        return rects


    def paintPoints(self, points):
        for l,p in points.items():
            dot = self.ui.canvas.addEllipse(-self.dotSize/2, -self.dotSize/2 ,self.dotSize, self.dotSize , self.penWhite, self.brushWhite)
            label = self.ui.canvas.addText(l)
            x = p.x
            y = -p.y
            dot.setPos(x,y)
            label.setPos(x,y-20)
            label.setDefaultTextColor(Qt.white)
            print('{0} pos {1}'.format(l, p))


    def paintRectangles(self, rects):
        for l,r in rects.items():
            self.ui.canvas.addLine(r.x0, -r.y0, r.x1, -r.y0,self.penWhite)
            self.ui.canvas.addLine(r.x0, -r.y0, r.x0, -r.y1,self.penWhite)
            self.ui.canvas.addLine(r.x1, -r.y0, r.x1, -r.y1,self.penWhite)
            self.ui.canvas.addLine(r.x0, -r.y1, r.x1, -r.y1,self.penWhite)
            label = self.ui.canvas.addText(l)
            label.setPos(r.x0,-r.y0 -20)
            label.setDefaultTextColor(Qt.white)


    def test_minCircle(self, num = 3):
        points = self.generatePoints(num)
        # points = {}
        # points['a'] = Point(300, 0)
        # points['b'] = Point(100, 0)
        # points['c'] = Point(200, 100)

        c = Circle.min_circle(*[p for p in points.values()])

        circle = self.ui.canvas.addEllipse(-c.radius, -c.radius, c.radius*2, c.radius*2 , self.penGreen)
        circle.setPos(c.center.x, -c.center.y)

        self.paintPoints(points)
        print('circle pos: {0}, radius: {1}'.format(c.center, c.radius))


    #TODO Esta configuración concreta de puntos falla
    def test_minCircle2(self):
        points = {}

        points["1"] = Point(903, 5868)
        points["2"] = Point(928, 5858)
        points["3"] = Point(863, 5774)
        points["4"] = Point(863, 5774)
        points["5"] = Point(944, 5864)
        points["6"] = Point(963, 5844)
        points["7"] = Point(924, 5559)
        points["8"] = Point(924, 5559)
        points["9"] = Point(1561, 5591)

        c = Circle.min_circle(*[p for p in points.values()])
        for l, p in points.items():
            if not c.includePoint(p):
                print("{0}, queda a {1} de estar dentro".format(l,c.center.distance(p)-c.radius))

        for p in points.values():
            p.x-= 1000
            p.y-= 5700

        c.center.x -= 1000
        c.center.y -= 5700

        circle = self.ui.canvas.addEllipse(-c.radius, -c.radius, c.radius*2, c.radius*2 , self.penGreen)
        circle.setPos(c.center.x, -c.center.y)

        self.paintPoints(points)
        print('circle 2 pos: {0}, radius: {1}'.format(c.center, c.radius))

    def test_intersections(self, num = 4):
        points = self.generatePoints(num)
        # points['a'] = Point(404, 139)
        # points['b'] = Point(706, 654)
        # points['c'] = Point(379, 214)
        # points['d'] = Point(321, 173)

        p = list(points.values())

        for p0, p1 in zip(p[::2], p[1::2]):
            self.ui.canvas.addLine(p0.x, -p0.y, p1.x, -p1.y, self.penRed)

        bisectors = []
        for p0, p1 in zip(p[::2], p[1::2]):
            b = Point.bisector(p0, p1)
            bisectors.append(b)
            self.ui.canvas.addLine(b.p0.x, -b.p0.y, b.p1.x, -b.p1.y, self.penGreen)

        for l1, l2 in zip(bisectors[::2], bisectors[1::2]):
            dot = {'X': Line.intersection(l1, l2)}
            self.paintPoints(dot)

        self.paintPoints(points)

    def test_rectangles_distance(self, num):
        rects = self.generateRectangles(1)
        self.paintRectangles(rects)

        points = self.generatePoints(num)
        self.paintPoints(points)

        for r in rects.values():
            for p in points.values():
                radius = r.distance(p)
                c = Circle(p, radius)
                circle = self.ui.canvas.addEllipse(-c.radius, -c.radius, c.radius*2, c.radius*2 , self.penOrange)
                circle.setPos(c.center.x, -c.center.y)

    def snapshot(self, filename = "untitled"):
        self.ui.canvas.render(self.painter);
        self.image.save(filename+".png")

    def test_text(self):
        t = QGraphicsTextItemWithBackground("Hola")
        t.setPos(0,0)
        self.ui.canvas.addItem(t)

if __name__ == "__main__":

    app = QtWidgets.QApplication([])
    application = mywindow()
    application.show()
    # application.test_text()
    application.test_minCircle(5)
    # application.test_minCircle2()
    # application.snapshot("1")
    # application.test_intersections()
    # application.test_intersections()
    # application.test_rectangles_distance(5)
    sys.exit(app.exec())
