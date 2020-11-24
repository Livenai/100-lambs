Compilacion del proyecto:
0) Es necesario haber instalado previamente argos3
		https://www.argos-sim.info/core.php

1) Creamos la carpeta build

		mkdir build

2) Nos movemos a esta

		cd build/

3) Preparamos el CMake (asegurarse de borrar CMakeCache si se encontrara alguno por ahi)

		cmake ..

4) Compilamos (usar -jn donde n es un numero para activar el multihilo y compilar mas rapido :p)

		make -j7

5) Instalamos las entidades trough (comedero) y lambot(footbot personalizado)
		sudo make install


Ejecucion del proyecto desde la carpeta 100_lambs:

	argos3 -c experiments/50_lambs.argos

ó

	argos3 -c 1_lamb.argos


en el caso de querer una ejecucion de prueba con una sola entidad.
