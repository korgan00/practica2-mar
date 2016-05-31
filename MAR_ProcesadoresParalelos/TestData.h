#include <vector>
#include "Processor.h"

#ifndef TEST_DATA
#define TEST_DATA

// Estructura para alimentar la ejecucion.
typedef struct {
	int processorCount;
	std::vector<Work> workList;
} TestData;

// Genera un array con los amount + 2 numeros de fibonacci
std::vector<Work> fibonacciGenerate(int amount) {
	std::vector<Work> fibo;
	if (amount == 0) return fibo;
	fibo.push_back(1);
	if (amount == 1) return fibo;
	fibo.push_back(1);

	for (int i = 0; i < amount; i++) {
		fibo.push_back(fibo.back() + fibo[fibo.size() - 2]);
	}

	return fibo;
}
// Genera un array con los "amount + 2" numeros de fibonacci "times" veces
std::vector<Work> multiFibonacciGenerate(int times, int amount) {
	std::vector<Work> fibo;

	for (int i = 0; i < times; i++) {
		std::vector<Work> addedFibo = fibonacciGenerate(amount);
		for (Work w : addedFibo) { fibo.push_back(w); }
	}

	return fibo;
}
// Genera un array juntando series de fibonacci de una longitud progresiva
// empezando en from +2 y terminando en to +2
std::vector<Work> progressiveFibonacciGenerate(int from, int to) {
	std::vector<Work> fibo;

	for (int i = from; i < to; i++) {
		std::vector<Work> addedFibo = fibonacciGenerate(i);
		for (Work w : addedFibo) { fibo.push_back(w); }
	}

	return fibo;
}

TestData trivial = {
	2,
	std::vector<Work>(2, 1.0f)
};

TestData trivial2 = {
	2,
	std::vector<Work>(11, 1.0f)
};

TestData manyProcessors = {
	6,
	std::vector<Work>(123, 1.0f)
};

TestData many = {
	15,
	std::vector<Work>(450, 1.0f)
};

TestData manyMuch = {
	30,
	std::vector<Work>(1500, 1.0f)
};

TestData fibonacci = {
	20,
	fibonacciGenerate(50)
};
TestData multiFibonacci = {
	10,
	multiFibonacciGenerate(3, 11)
};



TestData PracticeData1 = {
	6,
	std::vector<Work>(11, 1.0f)
};
TestData PracticeData2 = {
	4,
	progressiveFibonacciGenerate(3, 5)
};
TestData PracticeData3 = {
	3,
	multiFibonacciGenerate(3, 2)
};


#endif // !TEST_DATA