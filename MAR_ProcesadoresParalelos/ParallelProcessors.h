#include <vector>
#include <cfloat>
#include <iostream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "Processor.h"

#ifndef PARALLEL_PROCESSORS
#define PARALLEL_PROCESSORS

class ParallelProcessors {
private:
	std::vector<Processor> _processorList;
	std::vector<Processor> _bestDistribution;
	double _bestDistributionTime;
	double _bestPesimist;
	long long unsigned int _visitedNodes;
	std::vector<Work> _workList;

	void _FindBestDistribution(std::vector<Work> currentWorkList, int processorsUsed);
	double _CurrentTime();
	void _EvaluateCurrentSolution();
	bool _IsBadBranch(std::vector<Work> currentWorkList);

	double _OptimistBound(std::vector<Work> currentWorkList);
	double _PesimistBound(std::vector<Work> currentWorkList);
public:

	ParallelProcessors(int processorCount, std::vector<Work> workList);

	void FindBestDistribution();

	std::string toString();
};

#endif // !PARALLEL_PROCESSORS
