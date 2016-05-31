#include "ProcessorDistributor.h"

ProcessorDistributor::ProcessorDistributor(int processorCount, std::vector<Work> workList)
	: _workList(workList) {
	for (int i = 0; i < processorCount; i++) {
		_processorList.push_back(Processor());
	}
}


void ProcessorDistributor::FindBestDistribution() {
	_visitedNodes = 0;

	// nodo inicial vacio
	_openNodes.push_back({
		0,
		{},
		_processorList,
		FLT_MAX,
		FLT_MAX,
		false
	});

	_bestNode = _openNodes.back();

	clock_t t1, t2;
	t1 = clock();
	while(!_openNodes.empty()) {
		_FindBestDistribution();
	}
	t2 = clock();
	_executionTime = t2 - t1;
}


void ProcessorDistributor::_FindBestDistribution() {
	// LIFO
	Node currentNode = _openNodes.back(); 
	_openNodes.pop_back();

	// Comprobamos si esta rama sigue siendo buena
	if (!_IsGoodBranch(currentNode)) return;


	// Contador de nodos visitados
	_visitedNodes++;
	if ((_visitedNodes % 1000) == 0) {
		std::cout << "\rVisitedNodes: " << _visitedNodes / 1000.0f << "K || Opt: " << _bestNode.optimist << " || Pes: " << _bestNode.pesimist << " || isSol: " << (_bestNode.isSolution? 'T' : 'F') << "                ";
	}


	Work w = _workList[currentNode.solution.size()];
	int processorsToUse = std::min(currentNode.processorsUsed + 1, (int)_processorList.size());
	Node n;

	// Generación de los nodos hijo
	for (int i = 0; i < processorsToUse; i++) { 
		n = Node({
			std::max(currentNode.processorsUsed, i + 1), // processorsUsed
			currentNode.solution,
			currentNode.processorList,
			currentNode.optimist,
			currentNode.pesimist,
			false
		});

		n.solution.push_back(i);
		n.processorList[i].AddWork(w);
		n.optimist = _OptimistBound(n);
		n.pesimist = _PesimistBound(n);
		n.isSolution = _IsSolution(n);

		// Comprobar si la rama merece la pena ser explorada
		if (_IsGoodBranch(n)) { // pesimista y optimista
			if (n.isSolution) {
				_bestNode = n;
			} else {
				// Comprobaciones de las cotas pesimista y optimista
				if (_bestNode.isSolution) {
					_bestNode = _bestNode.optimist > n.pesimist ? n : _bestNode;
				} else {
					_bestNode.pesimist = fmin(_bestNode.pesimist, n.pesimist);
				}
				_openNodes.push_back(n);
			}
		}

	}
}

bool ProcessorDistributor::_IsSolution(Node currentNode) {
	return currentNode.solution.size() == _workList.size();
}

bool ProcessorDistributor::_IsGoodBranch(Node currentNode) {

#if defined(BOUNDS_NAIVE) || defined(BOUNDS_HEAVY)
	if (_bestNode.isSolution) { // Nodo solucion
		return _bestNode.optimist > currentNode.optimist;
}
	else { // Cota pesimista Óptima
		return _bestNode.pesimist >= currentNode.optimist;
	}
#else
	return true;
#endif


}


#ifdef BOUNDS_NAIVE

double ProcessorDistributor::_OptimistBound(Node n) {
	return _MaxProcessorTime(n);
}

double ProcessorDistributor::_PesimistBound(Node n) {
	// Menor tiempo actual
	double time = _MinProcessorTime(n);
	// Añadimos el resto de trabajos al mínimo
	time += std::accumulate(_workList.begin() + n.solution.size(), _workList.end(), 0.0);
	return fmax(_MaxProcessorTime(n), time);
}

#else
#ifdef BOUNDS_HEAVY

double ProcessorDistributor::_OptimistBound(Node n) {
	double currentTime = _MaxProcessorTime(n);
	// diferencia de los procesadores con el procesador mas cargado
	double processorUnusedTime = 0.0f;
	for (Processor &p : n.processorList) {
		processorUnusedTime += currentTime - p.totalTime();
	}
	// tiempo total de trabajo restante
	double workAccumulatedTime = std::accumulate(_workList.begin() + n.solution.size(), _workList.end(), 0.0);
	double overflowTime = workAccumulatedTime - processorUnusedTime;

	// si no sobra tiempo devolvemos el tiempo mayor
	if (overflowTime < 0) {
		return currentTime;
	} else { // sino, repartimos entre todos los procesadores
		return currentTime + (overflowTime / _processorList.size());
	}
}

/**
 * Selecciona el primer procesador que no sea el más cargado y 
 * le añade trabajos hasta que se convierta en el más cargado, 
 * después selecciona el siguiente procesador hasta que se convierta 
 * en el más cargado. Uno a uno va repartiendo los trabajos de tal 
 * manera que todos los procesadores queden con una carga similar.
 */
double ProcessorDistributor::_PesimistBound(Node n) {
	double currentTime = _MaxProcessorTime(n);
	std::vector<Processor> processorListTemp = n.processorList;

	bool asigned = false;
	int targetProcessor = 0;
	int startLoopProcessor = 0;
	std::vector<Work>::iterator currentWorkList = _workList.begin() + n.solution.size() - 1;
	Work w;
	while (++currentWorkList != _workList.end()) {
		w = *currentWorkList;
		asigned = false;
		startLoopProcessor = targetProcessor;
		// Añadimos el trabajo si procede
		if (processorListTemp[targetProcessor].totalTime() < currentTime) {
			processorListTemp[targetProcessor].AddWork(w);
			if (processorListTemp[targetProcessor].totalTime() > currentTime) {
				currentTime = processorListTemp[targetProcessor].totalTime();
			}
			asigned = true;
		} else {
			// sino, buscamos el primer procesador con menos carga
			targetProcessor = (targetProcessor + 1) % _processorList.size();
			while (!asigned && targetProcessor != startLoopProcessor) {
				if (processorListTemp[targetProcessor].totalTime() < currentTime) {
					processorListTemp[targetProcessor].AddWork(w);
					if (processorListTemp[targetProcessor].totalTime() > currentTime) {
						currentTime = processorListTemp[targetProcessor].totalTime();
					}
					asigned = true;
				} else {
					targetProcessor = (targetProcessor + 1) % _processorList.size();
				}
			}
		}

		// si todos los procesadores tienen la misma carga, lo añadimos al primero que encontremos
		if (!asigned) {
			processorListTemp[targetProcessor].AddWork(w);
			if (processorListTemp[targetProcessor].totalTime() > currentTime) {
				currentTime = processorListTemp[targetProcessor].totalTime();
			}
		}
	}
	
	return _MaxProcessorTime({ 0,{}, processorListTemp, FLT_MAX, FLT_MAX, false });
}
#else

double ProcessorDistributor::_OptimistBound(Node n) {
	return FLT_MIN;
}

double ProcessorDistributor::_PesimistBound(Node n) {
	return FLT_MAX;
}

#endif
#endif


double ProcessorDistributor::_MaxProcessorTime(Node n) {
	double totalTime = FLT_MIN;
	for (Processor &p : n.processorList) {
		totalTime = std::fmax(totalTime, p.totalTime());
	}
	return totalTime;
}

double ProcessorDistributor::_MinProcessorTime(Node n) {
	double lessTime = FLT_MAX;
	for (Processor &p : n.processorList) {
		lessTime = std::fmin(lessTime, p.totalTime());
	}
	return lessTime;
}

std::string ProcessorDistributor::PrintTime(clock_t milis) {
	std::stringstream ss;
	ss << "Calculation Time: ";
	if (milis < 1000) {
		ss << milis << "ms" << std::endl;
	}
	else {
		float segf = milis / 1000.0;
		if (segf < 60) {
			ss << segf << "s" << std::endl;
		}
		else {
			int min = (int)segf / 60;
			int seg = (int)segf % 60;
			ss << min << "m " << seg << "s" << std::endl;
		}
	}
	return ss.str();
}



std::string ProcessorDistributor::ToString() {
	int i = 0;
	std::stringstream ss;

#if defined(BOUNDS_NAIVE) || defined(BOUNDS_HEAVY)
	ss << std::endl << std::endl << "\tMaxTime: " << _bestNode.optimist << " || Visited Nodes: " << _visitedNodes << std::endl;
#else
	ss << std::endl << std::endl << "\tMaxTime: " << _MaxProcessorTime(_bestNode) << " || Visited Nodes: " << _visitedNodes << std::endl;
#endif

	for (Processor &p : _bestNode.processorList) {
		ss << "\t- Processor " << i++ << ": " << p.toString() << std::endl;
	}
	ss << std::endl << "\t" << PrintTime(_executionTime);
	ss << "\tUsing Bounds: ";

#ifdef BOUNDS_NAIVE
	ss << "Naive";
#else
	ss << "Heavy";
#endif
	ss << std::endl;
	return ss.str();
}