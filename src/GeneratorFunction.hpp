#ifndef __GENERATORFUNCTION_HPP
#define __GENERATORFUNCTION_HPP

#include <gmp.h>
#include <chrono>
#include <memory>

#include "int128_t.hpp"
using namespace primecount;


using namespace std::chrono;

class GeneratorFunctionAbstract{
    protected:
    unsigned long long _PrimesCnt = 0;
    unsigned long long _ProbablePrimesCnt = 0;   

    std::chrono::time_point<std::chrono::high_resolution_clock> _BeginTime; //  = std::high_resolution_clock::now(); 
    void ResetClock() noexcept;


    public:
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

    virtual int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) = 0; //pure virtual function leads to an abstract class

};


// This CRTP class implements clone() for Derived
template <typename Derived>
class GeneratorFunction : public GeneratorFunctionAbstract {
public:
    std::unique_ptr<GeneratorFunctionAbstract> clone() const override {
        return std::make_unique<Derived>(static_cast<Derived const&>(*this));
    }

protected:
   // We make clear GeneratorFunction class needs to be inherited
   GeneratorFunction() = default;
   GeneratorFunction(const GeneratorFunction&) = default;
   GeneratorFunction(GeneratorFunction&&) = default;
};


#endif
