#include "ParallelProcessors.h"



ParallelProcessors::ParallelProcessors(int processorCount, std::vector<Work> workList) 
		: _workList(workList), _bestDistributionTime(FLT_MAX), _bestPesimist(FLT_MAX) {
	for (int i = 0; i < processorCount; i++) {
		_processorList.push_back(Processor());
	}
}

void ParallelProcessors::FindBestDistribution() {
	//std::cout << "Preparando..." << std::endl;
	std::vector<Work> currentWorkList;
	for (Work &w : _workList) {
		currentWorkList.push_back(w);
	}

	//std::cout << "Preparado" << std::endl;
	_FindBestDistribution(currentWorkList, 0);
}

void ParallelProcessors::_FindBestDistribution(std::vector<Work> currentWorkList, int processorsUsed) {
	if (currentWorkList.size() == 0) {
		_EvaluateCurrentSolution();
		return;
	}
	if (_IsBadBranch(currentWorkList)) {
		//std::cout << "WRONG BRANCH" << std::endl;
		return; // EvaluarCotas
	}


	// La cota inferior comprueba 
	Work w = currentWorkList.back();
	currentWorkList.pop_back();
	int i = 0;
	for (Processor &p : _processorList) {
		if (processorsUsed + 1 < i) {
			//std::cout << "Too much Children: " << i << std::endl;
			break;
		}
		p.AddWork(w);
		//std::cout << "Processor: " << i << " - Add: " << w << " || Total: " << p.totalTime() << std::endl;
		_FindBestDistribution(currentWorkList, std::max(processorsUsed, i));
		// Restore node
		p.RemoveWork();
		//std::cout << "Processor: " << i << " - Rem: " << w << " || Total: " << p.totalTime() << std::endl;
		i++;
	}
	currentWorkList.push_back(w);
}
double ParallelProcessors::_CurrentTime() {
	double totalTime = 0.f;
	for (Processor &p : _processorList) {
		totalTime = std::fmax(totalTime, p.totalTime());
	}
	return totalTime;
}

void ParallelProcessors::_EvaluateCurrentSolution() {
	//std::cout << "Evaluando una solucion." << std::endl;
	_CurrentTime();
	double totalTime = _CurrentTime();

	if (_bestDistributionTime > totalTime) {
		_bestDistributionTime = totalTime;
		_bestDistribution.clear();
		for (Processor &p : _processorList) {
			_bestDistribution.push_back(Processor(p));
		}
		/*std::cout << "======= Encontrada mejor solucion. =======" << std::endl;
		std::cout << toString();
		std::cout << "==========================================" << std::endl;*/
	}
	// if _processorList > _bestDistribution : _bestDistribution = _processorList
}

bool ParallelProcessors::_IsBadBranch(std::vector<Work> currentWorkList) {
	double optimistBound = _OptimistBound(currentWorkList);
	if (_bestDistributionTime <= optimistBound) {
		//std::cout << "Optimist: " << optimistBound << " BOUND!! || ";
		return true;
	}
	//std::cout << "Optimist: " << optimistBound << " not bound || ";

	double pesimistBound = _PesimistBound(currentWorkList);
	if (_bestPesimist > pesimistBound) {
		_bestPesimist = pesimistBound;
		//std::cout << "NEW BEST PESIMIST TIME: " << _bestPesimist << std::endl;
	}

	if (_bestPesimist < optimistBound) {
		//std::cout << " PESIMIST BOUND!! || ";
		return true;
	}
	return _bestPesimist < optimistBound;
}
#ifdef BOUNDS_A 
double ParallelProcessors::_OptimistBound(std::vector<Work> currentWorkList) {
	return FLT_MAX;
}
double ParallelProcessors::_PesimistBound(std::vector<Work> currentWorkList) {
	return FLT_MAX;
}
#else
double ParallelProcessors::_OptimistBound(std::vector<Work> currentWorkList) {
	double currentTime = _CurrentTime();
	double processorUnusedTime = 0.0f;
	for (Processor &p : _processorList) {
		processorUnusedTime += currentTime - p.totalTime();
	}

	double workAccumulatedTime = 0.0f;
	for (Work &w : currentWorkList) {
		workAccumulatedTime += w;
	}
	
	double overflowTime = workAccumulatedTime - processorUnusedTime;

	if (overflowTime < 0) {
		return currentTime;
	} else {
		return currentTime + (overflowTime / _processorList.size());
	}
}
double ParallelProcessors::_PesimistBound(std::vector<Work> currentWorkList) {
	double currentTime = _CurrentTime();
	std::vector<double> times;
	for (Processor &p : _processorList) {
		times.push_back(p.totalTime());
	}

	bool asigned = false;
	int targetProcessor = 0;
	int startLoopProcessor = 0;
	for (Work &w : currentWorkList) {
		asigned = false;
		startLoopProcessor = targetProcessor;
		if (times[targetProcessor] < currentTime) {
			times[targetProcessor] += w;
			if (times[targetProcessor] > currentTime) {
				currentTime = times[targetProcessor];
			}
			asigned = true;
		} else {
			targetProcessor = (targetProcessor + 1) % _processorList.size();
			while (!asigned && targetProcessor != startLoopProcessor) {
				if (times[targetProcessor] < currentTime) {
					times[targetProcessor] += w;
					if (times[targetProcessor] > currentTime) {
						currentTime = times[targetProcessor];
					}
					asigned = true;
				} else {
					targetProcessor = (targetProcessor + 1) % _processorList.size();
				}
			}
		}

		if (!asigned) {
			times[targetProcessor] += w;
			if (times[targetProcessor] > currentTime) {
				currentTime = times[targetProcessor];
			}
		}
	}

	double maxTime = 0;
	for (double &t : times) {
		maxTime = std::fmax(maxTime, t);
	}

	return maxTime;
}
#endif

std::string ParallelProcessors::toString() {
	int i = 0;
	std::stringstream ss;
	ss << "\tMaxTime: " << _bestDistributionTime << std::endl;
	for (Processor &p : _bestDistribution) {
		ss << "\t- Processor " << i++ << ": " << p.toString() << std::endl;
	}

	return ss.str();
}