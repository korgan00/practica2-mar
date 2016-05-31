#include "Processor.h"

Processor::Processor(const Processor &p) {
	_totalTime = p._totalTime;
	_totalWorks = p._totalWorks;
}

double Processor::totalTime() const {
	return _totalTime;
}

void Processor::AddWork(Work w) {
	_totalWorks++;
	_totalTime += w;
}
void Processor::RemoveWork(Work w) {
	_totalTime -= w;
	_totalWorks--;
}
std::string Processor::toString() {
	std::stringstream ss;
	ss << "time: " << _totalTime << " - Works: " << _totalWorks;
	return ss.str();
}