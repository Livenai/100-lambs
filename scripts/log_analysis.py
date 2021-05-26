#! /usr/bin/env python3
import json
from sys import argv
from os import makedirs , path
import numpy as np
from math import trunc
from matplotlib import pyplot as plt
# from datetime import datetime

from geometry import *

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
        self.entries = [Entry(float(t), Point(data['x'], data['y'])) for t, data in json.load(open(file_path, "r")).items() ]
        # self.entries = [Entry(float(t), Point(0, 0), state=data['state']) for t, data in json.load(open(file_path, "r")).items() ]

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
    def save_log(self, file_path):
        dir_path = path.dirname(file_path)
        if not path.exists(dir_path):
            makedirs(dir_path)
        with open(file_path, 'w') as outfile:
            # data = {e.time_init:{'x':e.pos.x,'y':e.pos.y, 'state':e.state} for e in self.entries}
            data = {e.time_init:{'x':e.pos.x,'y':e.pos.y} for e in self.entries}
            for e in self.entries:
                # if e.state != '':
                data[e.time_init]['state'] = e.state
                data[e.time_init]['duration'] = e.duration


            json.dump(data, outfile)
            print("Log guardado en {0}".format(file_path))


    @staticmethod
    def save_matrix(matrix):
        file_path = './transition_matrix.json'
        with open(file_path, 'w') as outfile:
            json.dump(matrix, outfile)
        print("Matriz de transicion guardada en {0}".format(file_path))


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

            # al tardar menos tiempo en beber hay mas posibilidades de que
            # no se registre la posicion. para compensarlo beber tiene un margen
            # un poco mas amplio
            if dis_water <= dis_threshold*1.75:
                e.state = "bebiendo"
            if dis_food <= dis_threshold:
                e.state = "comiendo"


    """
    Estimacion de en que momentos esta descansando el cordero
    Depende del resultado de _cluster()
    """
    def _calc_sleep(self, time_threshold = 10):
        time_threshold *= 60
        for e in self.entries:
            if e.duration >= time_threshold and e.state == '':
                e.state = "descansando"

    """
    Estadisticas referentes a la actividad de comer, beber y dormir
    Depende del resultado de _calc_forage()
    """
    def _states_stats(self):
        states = ["comiendo", "bebiendo", "descansando"]

        for state in states:
            # si hay varios puntos seguidos con el mismo estado, se contaran como uno solo
            durations = []
            consecutive_states = []
            for e in self.entries:
                if e.state == state:
                    consecutive_states.append(e)
                elif len(consecutive_states) > 0:
                    dur = consecutive_states[-1].time_end - consecutive_states[0].time_init
                    durations.append(dur)
                    consecutive_states = []

            if len(consecutive_states) > 0:
                dur = consecutive_states[-1].time_end - consecutive_states[0].time_init
                durations.append(dur)

            self.results["media veces {0} por dia".format(state)] = len(durations)/ self.num_days
            if(len(durations) == 0):
                durations = [0]

            self.results["media tiempo {0} cada vez (s)".format(state)] = int(np.average(durations))
            self.results["varianza tiempo {0} cada vez".format(state)] = int(np.var(durations))
            self.results["tiempo total {0}(h)".format(state)] = sum(durations)/3600


    """
    Estadisticas sobre el desplazamiento: distancia, velocidad
    Depende de las funciones que estiman los estados
    """
    def _displacement_stats(self):
        vel = []
        total_time = 0
        for e in self.entries:
            if e.state == '':
                e.state = 'caminando'

        # se cuenta la distancia y el tiempo transcurrido entre cada par de puntos
        # siempre que alguno de los dos tenga de estado 'caminando'
        for e_prev, e_current in zip(self.entries[:-1], self.entries[1:]):
            if e_current.state == 'caminando' or e_prev.state == 'caminando':
                if e_current.state == 'caminando':
                    t = e_current.time_end - e_prev.time_end
                    e_current.duration = t
                else:
                    t = e_current.time_init - e_prev.time_end

                dis = e_current.pos.distance(e_prev.pos)
                total_time += t
                vel.append((dis*6)/(t*100))


        self.results["media velocidad (m/min)"] = np.average(vel)
        self.results["varianza velocidad"] = np.var(vel)
        self.results["tiempo total {0}(h)".format('caminando')] = total_time/3600


    def _transition_matrix(self):
        states = ['descansando', 'comiendo', 'bebiendo', 'caminando']
        self.matrix = [ [0]*len(states) for s in states]
        for prev, cur in zip(self.entries[:-1],self.entries[1:]):
            self.matrix[states.index(prev.state)][states.index(cur.state)] += 1

        for row in self.matrix:
            n = sum(row)
            if n != 0:
                for i in range(len(row)):
                    row[i] /= n


    @staticmethod
    def average_transition_matrix(logs):
        matrix = [ [0]*4 for i in range(4)]

        for i in range(4):
            for j in range(4):
                matrix[i][j] = sum([l.matrix[i][j] for l in logs])/len(logs)

        return matrix

    @staticmethod
    def print_transition_matrix(matrix):
        print('Matriz de frequencias de transicion entre estados')

        states = ['descansando', 'comiendo', 'bebiendo', 'caminando']
        for s in ['']+states:
            print('{0:>11}'.format(s), end='|')
        print('\n'+'-'*12*5)

        for i in range(len(matrix)):
            print('{0:>11}'.format(states[i]), end='|')
            for j in range(len(matrix[0])):
                print('{0:>11.2f}'.format(matrix[j][i]), end='|')
            # print('\n'+'-'*12*5)
            print()


    # mostrar resultados
    def show_results(self):
        print('Analisis {0}:'.format(self.name))
        hours = (self.time_end - self.time_init )/3600
        print('  {0:.2f} horas'.format(hours))
        states = ['descansando', 'comiendo', 'bebiendo', 'caminando']

        # Debug
        # print('  {0:.2f} horas perdidas[DEBUG]'.format(hours-sum([ self.results["tiempo total {0}(h)".format(s)] for s in states])))

        for k, v in self.results.items():
            print("  {0}: {1}".format(k, v))

        print()
        Log.print_transition_matrix(self.matrix)


    def _plot_state_time(self):
        x = ['comiendo', 'bebiendo', 'descansando', 'caminando']
        y = [self.results["tiempo total {0}(h)".format(s)] / self.num_days for s in x]

        # Debug
        # print('  {0} horas de actividad al dia[DEBUG]'.format(sum(y)))

        plt.figure(self.name)
        plt.bar(x, y, align = 'center')
        plt.title('{0} - Tiempo medio en cada estado por día'.format(self.name))
        plt.ylabel('Tiempo total (horas)')
        plt.xlabel('Estados')
        plt.ylim(0,24)
        plt.yticks(np.arange(0,24,2))
        plt.grid(True, which='major', axis='y', zorder=0)
        plt.gca().set_axisbelow(True)

        dir_path = './figures'
        if not path.exists(dir_path):
            makedirs(dir_path)
        file_name = '{0}/{1}.png'.format(dir_path, self.name[:-5])
        plt.savefig(file_name, format='png')
        # savefig(file_name, format='png',transparent=False, frameon=None, metadata=None)

        # plt.show()


    @staticmethod
    def plot_state_time_averaged(logs):
        x = ['comiendo', 'bebiendo', 'descansando', 'caminando']
        y = [0]*4
        for l in logs:
            y_l = [l.results["tiempo total {0}(h)".format(s)] / l.num_days for s in x]
            y = [i+j for i,j in zip(y, y_l)]
        y = [i/len(logs) for i in y]

        plt.figure('{0} corderos'.format(len(logs)))
        plt.bar(x, y, align = 'center')
        plt.title('Media de {0} corderos - Tiempo medio en cada estado por día'.format(len(logs)))
        plt.ylabel('Tiempo total (horas)')
        plt.xlabel('Estados')
        plt.ylim(0,24)
        plt.yticks(np.arange(0,24,2))
        plt.grid(True, which='major', axis='y', zorder=0)
        plt.gca().set_axisbelow(True)

        dir_path = './figures'
        if not path.exists(dir_path):
            makedirs(dir_path)
        file_name = '{0}/{1}_corderos.png'.format(dir_path, len(logs))
        plt.savefig(file_name, format='png')

        # plt.show()

    def individual_analisys(self):
        # comederos y bebederos representados por rectangulos
        water_pos = [Point( 500, 6580), Point(6100, 6580)]
        food_pos= [Point(3300, 6580)]

        water = [Rectangle(p, 1000, 500) for p in water_pos]
        food = [Rectangle(p, 4500, 500) for p in food_pos]

        # el orden en que se llaman estas funciones es relevante porque modifican
        # el log poco a poco, por eso son "privadas" y se llaman desde este metodo
        self._calc_forage(food, water, 1000)
        self._cluster(500)
        self._calc_sleep(10)
        # self._calc_sleep(0.3)
        self._states_stats()
        self._displacement_stats()
        self._transition_matrix()

        self.save_log('logs_analysed/' + l.name)


if __name__ == '__main__':
    if len(argv) < 2:
        print("Uso:\n\t {0} file_logs".format( argv[0]))
    else:
        logs = [Log(file) for file in argv[1:]]
        for l in logs:
            # l.interpolate(300)
            # l.save_log('tracking_logs_interpolated/' + l.name)

            l.individual_analisys()
            l.show_results()
            l._plot_state_time()
        Log.plot_state_time_averaged(logs)
        m = Log.average_transition_matrix(logs)
        print('Promedio de {0} corderos'.format(len(logs)))
        Log.print_transition_matrix(m)
        Log.save_matrix(m)
