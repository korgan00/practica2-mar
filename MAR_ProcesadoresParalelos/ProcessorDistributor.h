#include <vector>
#include <cfloat>
#include <iostream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <numeric>
#include <time.h>
#include "Processor.h"

#ifndef PROCESSOR_DISTRIBUTOR
#define PROCESSOR_DISTRIBUTOR

/**
* Uncomment the desired bound.
*/
//#define BOUNDS_NAIVE
#define BOUNDS_HEAVY

/**
* Realiza los cálculos para obtener la mejor distribución
* en los procesadores.
*/
class ProcessorDistributor {
private:
	typedef struct {
		int processorsUsed;

		std::vector<int> solution;
		std::vector<Processor> processorList;
		
		double optimist;
		double pesimist;

		bool isSolution;
	} Node;

	std::vector<Processor> _processorList;
	std::vector<Work> _workList;
	std::vector<Node> _openNodes;
	Node _bestNode;
	long long unsigned int _visitedNodes;

	clock_t _executionTime;

	void   _FindBestDistribution();
	double _OptimistBound(Node n);
	double _PesimistBound(Node n);
	bool   _IsGoodBranch(Node n);
	bool   _IsSolution(Node n);

	double _MaxProcessorTime(Node n);
	double _MinProcessorTime(Node n);

public:

	ProcessorDistributor(int processorCount, std::vector<Work> workList);

	void FindBestDistribution();
	clock_t ExecutionTime() const {	return _executionTime; }

	std::string ToString();



	std::string PrintTime(clock_t milis);
};



#endif