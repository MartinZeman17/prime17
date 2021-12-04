#include "GeneratorFunction.hpp"

#include <iostream>
#include <chrono>

using namespace std::chrono;
using std::endl;
using std::cout;


void GeneratorFunctionAbstract::ResetClock() noexcept {
    _BeginTime = high_resolution_clock::now();
}

GeneratorFunctionAbstract::GeneratorFunctionAbstract() noexcept:  _PrimesCnt{0}, _ProbablePrimesCnt{0}  {
    ResetClock();
}

GeneratorFunctionAbstract& GeneratorFunctionAbstract::operator+=(const GeneratorFunctionAbstract& rhs) noexcept {
    _PrimesCnt += rhs._PrimesCnt;
    _ProbablePrimesCnt += rhs._ProbablePrimesCnt;   
    return *this;
}

// GeneratorFunction GeneratorFunction::operator+(const GeneratorFunction& rhs) {
//     GeneratorFunction result;
//     result._PrimesCnt = _PrimesCnt + rhs._PrimesCnt;
//     result._ProbablePrimesCnt = _ProbablePrimesCnt + rhs._ProbablePrimesCnt;
//     return result;
// }

long double GeneratorFunctionAbstract::DurationMinutes() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    return duration.count() / (1000.0l * 60.0l);
}

long double GeneratorFunctionAbstract::DurationSeconds() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    return duration.count() / (1000.0l);
}


unsigned long long GeneratorFunctionAbstract::ProbablePrimesCnt() const noexcept {
    return _ProbablePrimesCnt;
}

unsigned long long GeneratorFunctionAbstract::PrimesCnt() const noexcept {
    return _PrimesCnt;
}

void GeneratorFunctionAbstract::PrintResult() const noexcept {
    cout << toName() << " Primes Cnt: " << _PrimesCnt << " Probable Primes Cnt: " << _ProbablePrimesCnt << " Duration minutes: " << DurationMinutes() << endl;
}
