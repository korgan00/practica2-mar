#include "Processor.h"

Processor::Processor(Processor &p) {
	for (Work &w : p._workQueue) {
		_workQueue.push_back(w);
	}
	_totalTime = p.totalTime();
}

double Processor::totalTime() {
	return _totalTime;
}

void Processor::AddWork(Work w) {
	_workQueue.push_back(w);
	_totalTime += w;
}
void Processor::RemoveWork() {
	_totalTime -= _workQueue.back();
	_workQueue.pop_back();
}
std::string Processor::toString() {
	std::stringstream ss;
	ss << "time: " << _totalTime << " - Works: " << _workQueue.size();
	return ss.str();
}