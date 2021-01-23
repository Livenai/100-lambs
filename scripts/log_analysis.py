#! /usr/bin/env python3
import json
from sys import argv
from os import makedirs , path
import numpy as np
from math import trunc
# from datetime import datetime
from geometry.geometry import *

class Entry:

    def __init__(self,time0, pos ,time1 = None, state = None):
        self.time_init = int(time0)
        self.pos = pos

        if time1 != None:
            self.time_end = int(time1)
            self.duration = time1 -time0
        else:
            self.time_end = time0
            self.duration = 0

        if state:
            self.state = state
        else:
            self.state = ''

    def __str__(self):
        return "{0}: {1}, {2} durante {3}'".format(self.time_init, self.pos, self.state, self.duration//60)


class Log():
    def __init__(self, file_path):
        self.entries = [Entry(float(t), Point(pos['x'], pos['y'])) for t, pos in json.load(open(file_path, "r")).items() ]
        self.file_path = file_path
        self.name = file_path[file_path.rfind("/")+1:]
        self.results = {}
        self.time_init = self.entries[0].time_init
        self.time_end = self.entries[-1].time_end

        self.num_days = (self.time_end - self.time_init ) / 86400

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name


    """
    Guarda el log para que pueda verse con viewer.py (programa externo)
    solo se guardan las atributos relevantes para el viewer.py
    """
    def save(self, file_path):
        dir_path = path.dirname(file_path)
        if not path.exists(dir_path):
            makedirs(dir_path)
        with open(file_path, 'w') as outfile:
            # data = {e.time_init:{'x':e.pos.x,'y':e.pos.y, 'state':e.state} for e in self.entries}
            data = {e.time_init:{'x':e.pos.x,'y':e.pos.y} for e in self.entries}
            for e in self.entries:
                # if e.state != '':
                #     data[e.time_init]['state'] = e.state
                duration = str(e.duration//60)
                data[e.time_init]['state'] = e.state +" "+ duration+"'"

            json.dump(data, outfile)
            print("Guardado en {0}".format(file_path))


# Esta funcion no la uso en el analisis
    def interpolate(self, increment = 60):
        print('Interpolando {0}'.format(log.name))
        min_time = self.entries[0].time
        max_time = self.entries[-1].time
        spaced_time_stamps = np.arange(min_time, max_time, increment)
        if spaced_time_stamps[-1] < max_time:
            spaced_time_stamps = np.append(spaced_time_stamps,max_time)

        time_stamps = [e.time_init for e in self.entries]
        x_list = [e.pos.x for e in self.entries]
        x_list = np.interp(spaced_time_stamps, time_stamps, x_list)
        y_list = [e.pos.y for e in self.entries]
        y_list = np.interp(spaced_time_stamps, time_stamps, y_list)
        self.entries = [Entry(t, Point(x, y))  for t, x, y in zip(spaced_time_stamps, x_list, y_list)]


    """
    Agrupa los puntos cercanos en un determinado radio si tienen el mismo estado.
    Sirve para siplicar visualmente el log y para estimar cuando está
    parado el cordero.
    """
    def _cluster(self, radius):
        new_entries = []
        group = []
        t0 = self.entries[0].time_init
        t1 = t0
        c_state = self.entries[0].state
        for e in self.entries:
            add_to_group = False
            if c_state == e.state:
                circle = Circle.min_circle(*group, e.pos)
                add_to_group = circle.radius <= radius

            if add_to_group:
                group.append(e.pos)
            else:
                duration = (t1 - t0)//60
                center = Point.average(*group)
                # new_entries.append(Entry(t0, circle.center, t1, state = c_state))
                new_entries.append(Entry(t0, center, t1, state = c_state))
                group = [e.pos]
                c_state = e.state
                t0 = e.time_init

            t1 = e.time_init

        circle = Circle.min_circle(*group)
        new_entries.append(Entry(t0, circle.center, t1, state = c_state))


        print("{0}  Simplificado: {1} puntos ->{2} puntos".format(self.name, len(self.entries), len(new_entries)) )
        self.entries = new_entries


    """
    Estimacion de en que momentos esta comiendo o bebiendo el cordero en base a
    la distancia a los comederos y bebederos
    """
    def _calc_forage(self, food, water, dis_threshold = 500):
        for e in self.entries:
            dis_food  = min([f.distance(e.pos)  for f in food] )
            dis_water = min([w.distance(e.pos)  for w in water])

            if dis_food <= dis_threshold:
                e.state = "comiendo"
            if dis_water <= dis_threshold and dis_water <= dis_food:
                e.state = "bebiendo"


    """
    Estimacion de en que momentos esta durmiendo el cordero
    Depende del resultado de _cluster()
    """
    def _calc_sleep(self, time_threshold = 10):
        time_threshold *= 60
        for e in self.entries:
            if e.duration >= time_threshold and e.state == '':
                e.state = "durmiendo"

    """
    Estadisticas referentes a la actividad de comer, beber y dormir
    Depende del resultado de _calc_forage()
    """
    def _states_stats(self):
        states = ["comiendo", "bebiendo", "durmiendo"]

        for state in states:
            # consideramos las paradas seguidas para comer/beber/dormir como una sola
            stop_durations = []
            sub_stop = []
            for e in self.entries:
                if e.state == state:
                    sub_stop.append(e)
                elif len(sub_stop) > 0:
                    # sumar e.duration daria un valor ligeramente menor
                    # stop_durations = sum([e.duration for e in sub_stop])
                    stop_duration = sub_stop[-1].time_end - sub_stop[0].time_init
                    stop_durations.append(stop_duration)
                    sub_stop = []

            self.results["media veces {0} por dia".format(state)] = len(stop_durations)/ self.num_days
            if(len(stop_durations) == 0):
                stop_durations = [0]

            self.results["media tiempo {0} cada vez (s)".format(state)] = int(np.average(stop_durations))
            self.results["varianza tiempo {0} cada vez".format(state)] = int(np.var(stop_durations))


    """
    Estadisticas sobre el desplazamiento: distancia, velocidad
    Depende de las funciones que estiman los estados
    """
    def _displacement_stats(self):
        c_pos = self.entries[0].pos
        c_time_end = self.entries[0].time_end
        vel = []
        for e in self.entries[1:]:
            if e.state == '':
                vel.append(e.pos.distance(c_pos)*60/((e.time_init - c_time_end)*1000))
            c_time_end = e.time_end
            c_pos = e.pos

        self.results["media velocidad (m/min)"] = np.average(vel)
        self.results["varianza velocidad"] = np.var(vel)


    # mostrar resultados
    def show_results(self):
        print('Analisis {0}:'.format(self.name))

        # print('   {0}  paradas : '.format(len(self.entries)))
        # for e in self.entries:
        #     print("{0}'{1}'', ".format(e.duration//60, e.duration%60), end='')
        # print()
        for k, v in self.results.items():
            print("  {0}: {1}".format(k, v))

        print()
        return self.results


    def individual_analisys(self):
        # comederos y bebederos representados por rectangulos
        # TODO revisar las posiciones y tamaño
        water_pos = [Point( 500, 6580), Point(7100, 6580)]
        food_pos= [Point(1640, 6580), Point(2780, 6580),
                        Point(4920, 6580), Point(5060, 6580)]

        water = [Rectangle(p, 1000, 500) for p in water_pos]
        food = [Rectangle(p, 1000, 500) for p in food_pos]

        # el orden en que se llaman estas funciones es relevante porque modifican
        # el log poco a poco, por eso son "privadas" y se llaman desde este metodo
        self._calc_forage(food, water, 1000)
        self._cluster(500)
        self._calc_sleep(10)
        self._states_stats()
        self._displacement_stats()

        self.save('logs_analysed/' + l.name)



if __name__ == '__main__':
    if len(argv) < 2:
        print("Uso:\n\t {0} file_logs".format( argv[0]))
    else:
        logs = [Log(file) for file in argv[1:]]
        for l in logs:
            # l.interpolate(300)
            # l.save('tracking_logs_interpolated/' + l.name)

            l.individual_analisys()
            l.show_results()
