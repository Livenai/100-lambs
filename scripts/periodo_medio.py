#!/usr/bin/env python3

import json
from sys import argv
media_total = 0
for file_name in argv[1:]:
    with open(file_name) as file:
        data = json.load(file)
        lista = []
        for timestamp in data:
            lista.append(float(timestamp))

    media_log = 0
    for i in range(len(lista)-1):
        intervalo = lista[i+1] - lista[i]
        media_log += intervalo

    media_log /= (len(lista)-1)
    media_total += media_log
    print("{0}\n\t datos: {1}, periodo medio: {2}".format(file_name,len(lista), media_log))

media_total /= (len(argv) -1)
print("media total de los {0} logs: {1}".format(len(argv)-1, media_total))
# los tiempos medidos para los diferentes logs
