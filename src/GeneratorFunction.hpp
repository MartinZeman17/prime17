#ifndef __GENERATORFUNCTION_HPP
#define __GENERATORFUNCTION_HPP

#include <gmp.h>
#include <chrono>
#include <memory>

#include "int128_t.hpp"
using namespace primecount;

#include <iostream>
#include <chrono>

using namespace std::chrono;
using std::endl;
using std::cout;


template <typename T>
class GeneratorFunctionAbstract{
    protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> BeginTime_; //  = std::high_resolution_clock::now(); 
    unsigned long long PrimesCnt_ = 0;
    unsigned long long ProbablePrimesCnt_ = 0;



    public:
    void ResetClock() noexcept;
    GeneratorFunctionAbstract () noexcept;
    // polymorphic class (declares or inherits at least one virtual function), and its destructor is not virtual, deleting it is undefined behavior
    virtual ~GeneratorFunctionAbstract()= default;
    virtual std::string toName() const noexcept = 0;

    unsigned long long PrimesCnt() const noexcept;
    unsigned long long ProbablePrimesCnt() const noexcept;
    long double DurationMinutes() const noexcept;
    long double DurationSeconds() const noexcept;
    void PrintResult() const noexcept;

    virtual GeneratorFunctionAbstract& operator+=(const GeneratorFunctionAbstract& rhs) noexcept;
    virtual std::unique_ptr<GeneratorFunctionAbstract> clone() const = 0;

    // virtual int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) = 0; //pure virtual function leads to an abstract class
    virtual int GenFunct(const T & N, const mpz_t & mpz_X) = 0; //pure virtual function leads to an abstract class

};


// This CRTP class implements clone() for Derived
template <typename Derived, typename T>
class GeneratorFunction : public GeneratorFunctionAbstract<T> {
public:
    std::unique_ptr<GeneratorFunctionAbstract<T>> clone() const override {
        return std::make_unique<Derived>(static_cast<Derived const&>(*this));
    }

protected:
   // We make clear GeneratorFunction class needs to be inherited
   GeneratorFunction() = default;
   GeneratorFunction(const GeneratorFunction&) = default;
   GeneratorFunction(GeneratorFunction&&) = default;
};



template <class T>
void GeneratorFunctionAbstract<T>::ResetClock() noexcept {
    BeginTime_ = high_resolution_clock::now();
}

template <class T>
GeneratorFunctionAbstract<T>::GeneratorFunctionAbstract() noexcept:  PrimesCnt_{0}, ProbablePrimesCnt_{0}  {
    ResetClock();
}

template <class T>
GeneratorFunctionAbstract<T>& GeneratorFunctionAbstract<T>::operator+=(const GeneratorFunctionAbstract& rhs) noexcept {
    PrimesCnt_ += rhs.PrimesCnt_;
    ProbablePrimesCnt_ += rhs.ProbablePrimesCnt_;   
    return *this;
}

// GeneratorFunction GeneratorFunction::operator+(const GeneratorFunction& rhs) {
//     GeneratorFunction result;
//     result.PrimesCnt_ = PrimesCnt_ + rhs.PrimesCnt_;
//     result.ProbablePrimesCnt_ = ProbablePrimesCnt_ + rhs.ProbablePrimesCnt_;
//     return result;
// }

template <class T>
long double GeneratorFunctionAbstract<T>::DurationMinutes() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    return duration.count() / (1000.0l * 60.0l);
}

template <class T>
long double GeneratorFunctionAbstract<T>::DurationSeconds() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    return duration.count() / (1000.0l);
}


template <class T>
unsigned long long GeneratorFunctionAbstract<T>::ProbablePrimesCnt() const noexcept {
    return ProbablePrimesCnt_;
}

template <class T>
unsigned long long GeneratorFunctionAbstract<T>::PrimesCnt() const noexcept {
    return PrimesCnt_;
}

template <class T>
void GeneratorFunctionAbstract<T>::PrintResult() const noexcept {
    cout << toName() << " Primes Cnt: " << PrimesCnt_ << " Probable Primes Cnt: " << ProbablePrimesCnt_ << " Duration minutes: " << DurationMinutes() << endl;
}


#endif
