#! /usr/bin/env python3
import json
from sys import argv
from os import makedirs , path
from math import sqrt
import numpy as np

# from datetime import datetime

class Entry:

    def __init__(self,time_beginning, pos ,time_end = None):
        self.time = int(time_beginning)
        if time_end != None:
            self.time_end = int(time_end)
        self.pos = pos
        self.state = 'unknown'
        self.duration = -1

    def __str__(self):
        return "{0}: {1}, {2} durante {3}'".format(self.time, self.pos, self.state, self.duration//60)


class Point:
    def __init__(self, x, y):
        self.x = int(x)
        self.y = int(y)

    def __str__(self):
        return "(x: {0}, y :{1})".format(self.x, self.y)

    def __add__(self, other):
        return Point(self.x + other.x,  self.y + other.y)

    def __sub__(self, other):
        return Point(self.x - other.x,  self.y - other.y)


    def distance(self, p):
        return sqrt((self.x - p.x)**2 + (self.y - p.y)**2)


    def average(*points):
        x = 0
        y = 0
        for p in points:
            x += p.x
            y += p.y
        n = len(points)
        return Point(x/n, y/n)


# Rectangulo alineado con los ejes x, y
# para representar los comederos
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


    def includePoint(self, point):
        return point.distance(self.center) <= self.radius


    @staticmethod
    def _welzl(p, r):
        # solucion trivial
        if len(p) == 0 or len(r) >= 3:
            if len(r) == 1:
                return Circle(center = r[0], radius = 0)
            elif len(r) == 2:
                center = Point.mid_point(r[0], r[1])
                radius = center.distance(r[0])
                return Circle(center, radius)
            else:
                # TODO
                # hallar el circulo que inscribe al triangulo formado por los r[:3]

        else:
            circle = Circle._welzl(p[1:], r)
            if circle.includePoint(p[0]):
                return circle
            else:
                return welzl(p[1:], r + [p[0]])


    @staticmethod
    def min_circle(* points):
        if(len(points) == 1):
            return Circle(center = points[0], radius = 0)
        else:
            r = []
            # randomize(points) #TODO en cierto modo se pueden considerar ya aleatorios
            return Circle._welzl(points, r)


class Log():
    def __init__(self, file_path):
        self.entries = [Entry(int(t), Point(pos['x'], pos['y'])) for t, pos in json.load(open(file_path, "r")).items() ]
        self.file_path = file_path
        self.name = file_path[file_path.rfind("/")+1:]
        self.results = {}

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name


    def cluster(self, radius):
        new_entries = []
        group = []
        t0 = self.entries[0].time
        t1 = t0
        for e in self.entries:
            circle = Circle.min_circle(*group, e.pos)
            if circle.radius <= radius:
                group.append(e.pos)
            else:
                new_entries.append(Entry(t0, circle.center, t1))
                group = [e.pos]
                t0 = e.time

            t1 = e.time

        circle = Circle.min_circle(*group)
        new_entries.append(Entry(t0, circle.center, t1))


        print("{0}  Simplificado: {1}->{2}".format(self.name, len(self.entries), len(new_entries)) )
        self.entries = new_entries


    # def cluster(self, radius):
    #     new_entries = []
    #     group = []
    #     t0 = self.entries[0].time
    #     t1 = t0
    #     for e in self.entries:
    #         center = Point.average(*group, e.pos)
    #         if False in [center.distance(p) < radius for p in [e.pos] + group]:
    #             new_entries.append(Entry(t0, center, t1))
    #             group = [e.pos]
    #             t0 = e.time
    #         else:
    #             group.append(e.pos)
    #             t1 = e.time
    #
    #     center = Point.average(*group)
    #     new_entries.append(Entry(t0,center, self.entries[-1].time))
    #
    #
    #     print("{0}  Simplificado: {1}->{2}".format(self.name, len(self.entries), len(new_entries)) )
    #     self.entries = new_entries


    def save(self, file_path):
        dir_path = path.dirname(file_path)
        if not path.exists(dir_path):
            makedirs(dir_path)
        with open(file_path, 'w') as outfile:
            data = {e.time:{'x':e.pos.x,'y':e.pos.y, 'state':e.state} for e in self.entries}
            # data = {e.time:{'x':e.pos.x,'y':e.pos.y} for e in self.entries}
            # for e in self.entries:
            #     if e.state != '':
            #         data[e.time]['state'] = e.state

            json.dump(data, outfile)
            print("Guardado en {0}".format(file_path))


    # velocidad media
    def calc_vel(self):
        c_time = self.entries[0].time
        c_pos = self.entries[0].pos
        vel = 0
        for e in self.entries[1:]:
            vel += e.pos.distance(c_pos)/(e.time - c_time)
            c_time = e.time
            c_pos = e.pos

        n = len(self.entries) -1
        vel /= n

        self.results["velocidad media"] = vel


    # media de paradas por dia
    def calc_stops_per_day(self):
        days = (self.entries[-1].time - self.entries[0].time) / 86400
        stops = len(self.entries) / days

        self.results['paradas por dia'] = stops


    def calc_stops_duration(self):
        # tiempo de cada parada
        for e_i, e_j in zip(self.entries[:-1], self.entries[1:]):
            e_i.duration = e_j.time - e_i.time

        self.entries[-1].duration = -1

        # media de tiempo en cada parada
        n = len(self.entries) -1
        stop_duration = (self.entries[-1].time - self.entries[0].time) / n

        self.results['tiempo medio de paradas'] = stop_duration


    def analysis_state(self, food, water, threshold = 500):
        for e in self.entries[:-1]:
            if   True in [f.distance(e.pos) < threshold for f in food]:
                e.state = "comiendo"
            elif True in [w.distance(e.pos) < threshold for w in water]:
                e.state = "bebiendo"


    def analysis(self, food, water):
        self.calc_vel()
        self.calc_stops_duration()
        self.calc_stops_per_day()
        self.analysis_state(food, water)

        self.show_results()


    # mostrar resultados
    def show_results(self):
        print('Analisis {0}:'.format(log.name))

        print('  Paradas : ', end='')
        for e in self.entries[:-1]:
            print("{0}'{1}'', ".format(e.duration//60, e.duration%60), end='')
        print()

        for k, v in self.results.items():
            print("  {0}: {1}".format(k, v))
        print()

        return self.results


    def interpolate(self, increment = 60):
        min_time = self.entries[0].time
        max_time = self.entries[-1].time
        spaced_time_stamps = np.arange(min_time, max_time, increment)
        if spaced_time_stamps[-1] < max_time:
            spaced_time_stamps = np.append(spaced_time_stamps,max_time)

        time_stamps = [e.time for e in self.entries]
        x_list = [e.pos.x for e in self.entries]
        x_list = np.interp(spaced_time_stamps, time_stamps, x_list)
        y_list = [e.pos.y for e in self.entries]
        y_list = np.interp(spaced_time_stamps, time_stamps, y_list)
        self.entries = [Entry(t, Point(x, y))  for t, x, y in zip(spaced_time_stamps, x_list, y_list)]


if __name__ == '__main__':
    if len(argv) < 2:
        print("Uso:\n\t {0} file_logs".format( argv[0]))

    else:
        # TODO revisar las posiciones y tamaño de los comederos
        bebederos = [Rectangle(pos = Point( 500, 6580), width = 1000, height = 500 ),\
                     Rectangle(pos = Point(7100, 6580), width = 1000, height = 500 )]

        comederos = [Rectangle(pos = Point(1640, 6580), width = 1000, height = 500 ),\
                     Rectangle(pos = Point(2780, 6580), width = 1000, height = 500 ),\
                     Rectangle(pos = Point(4920, 6580), width = 1000, height = 500 ),\
                     Rectangle(pos = Point(5060, 6580), width = 1000, height = 500 )]

        logs = [Log(file) for file in argv[1:]]
        for log in logs:
            # print('Interpolando {0}'.format(log.name))
            # log.interpolate(300)
            # log.save('tracking_logs_interpolated/' + log.name)

            log.cluster(2000)
            # log.save('tracking_logs_clustered/' + log.name)

            # log.analysis(food = comederos, water = bebederos)
            # log.save('tracking_logs_labeled/' + log.name)
