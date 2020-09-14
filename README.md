Compilacion del proyecto:

1) Creamos la carpeta build

		mkdir build

2) Nos movemos a esta

		cd build/

3) Preparamos el CMake (asegurarse de borrar CMakeCache si se encontrara alguno por ahi)

		cmake ..

4) Compilamos (usar -jn donde n es un numero para activar el multihilo y compilar mas rapido :p)

		make -j7



Ejecucion del proyecto:

	argos3 -c 100_lambs.argos

ó

	argos3 -c 1_lamb.argos


en el caso de querer una ejecucion de prueba con una sola entidad.
