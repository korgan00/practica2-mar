#include <vector>
#include <sstream>
#include <iostream>

#ifndef PROCESSOR
#define PROCESSOR

typedef double Work;

class Processor {
private:
	std::vector<Work> _workQueue;
	double _totalTime;

public:

	Processor() : _totalTime(0.0f) {}
	Processor(Processor &p); //clone

	double totalTime();

	void AddWork(Work w);
	void RemoveWork();

	std::string toString();
};

#endif // !PROCESSOR
