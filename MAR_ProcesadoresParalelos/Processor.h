#include <vector>
#include <sstream>
#include <iostream>

#ifndef PROCESSOR
#define PROCESSOR

typedef double Work;


/**
 * Gestiona las principales funciones y datos que se van
 * a utilizar para modificar la carga de un procesador.
 */
class Processor {
private:
	double _totalTime;
	int _totalWorks;

public:

	Processor() : _totalTime(0.0f), _totalWorks(0) {}
	Processor(const Processor &p); //clone

	double totalTime() const;

	void AddWork(Work w);
	void RemoveWork(Work w);

	std::string toString();
};

#endif // !PROCESSOR
