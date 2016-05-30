#include <iostream>
#include <vector>
#include <time.h>
#include "ParallelProcessors.h"
#include "Processor.h"
#include "TestData.h"
using namespace std;


/*
Tenemos n trabajos que ejecutar y m procesadores iguales que trabajan en paralelo (m < n).
El tiempo necesario para ejecutar el trabajo i en cualquiera de los procesadores es ti.
Teniendo en cuenta que el orden en el que se ejecuten los trabajos dentro de un mismo procesador
no es significativo, escribe un algoritmo utilizando la tecnica de ramificacion y poda que determine
que trabajos se deben ejecutar en que procesadores de tal forma que se minimice el tiempo de finalizacion
del ultimo trabajo que se ejecuta. Hay que evitar encontrar soluciones equivalentes: dos soluciones
se consideran equivalentes si una se consigue a partir de la otra simplemente mediante la permutacion
de los procesadores.
*/


/*
** Normas para la presentacion de la practica: **

	 1. Se podra hacer la practica indistintamente en C++ o Java.
	 2. El desarrollo sera completamente individual.
	 3. Solo se podra escoger una de las propuestas.
	 4. Debe comunicarse al profesor la intencion de hacerla en el plazo de 5 dıas (a lo sumo el 11 de mayo).
	 5. Se ejecutara el programa con al menos tres juegos de datos distintos, con datos lo mas voluminosos posibles.
	 6. Para podar, se utilizaran al menos dos estimaciones optimistas y dos pesimistas (si son aplicables):
	    una ingenua y poco costosa, y otra mas ajustada y posiblemente mas costosa.
	 7. Se imprimiran el numero de nodos explorados cuando no se utiliza mas poda que la de
	    factibilidad y los explorados cuando se utiliza cada una de las dos podas.
	 8. Tambien se dara el tiempo total y el tiempo medio por nodo explorado en cada uno de los tres casos.
	 9. Se entregaran las fuentes del programa con suficientes comentarios, los ficheros de prueba, los
	    resultados obtenidos, y una breve memoria (1 a 3 paginas) con las explicaciones adicionales
	    que se consideren necesarias y las conclusiones personales obtenidas.
	10. La entrega se hara subiendo un fichero comprimido al campus virtual a lo sumo el martes 31
		de mayo. Las practicas no subidas en esa fecha se daran por no entregadas.
	11. Se podran fijar fechas durante mayo o junio para hacer una demostracion ante el profesor y
		responder a sus preguntas.
	12. Cada practica cuenta como salir dos veces en clase.
*/

void printTime(float);

int main(int argc, char* argv[]) {
	clock_t t1, t2;
	TestData data = multiFibonacci;

	ParallelProcessors program(data.processorCount, data.workList);

	t1 = clock(); 
	program.FindBestDistribution();
	t2 = clock();

	cout << program.toString() << endl << endl;
	printTime(t2 - t1);

	cin.get();
	return 0;
}


void printTime(float milis) {
	cout << "Calculation Time: ";
	if (milis < 1000) {
		cout << milis << "ms" << endl;
	} else {
		float segf = milis / 1000;
		if (segf < 60) {
			cout << segf << "s" << endl;
		} else {
			int min = (int)segf / 60;
			int seg = (int)segf % 60;
			cout << min << "m " << seg << "s" << endl;
		}
	}
}