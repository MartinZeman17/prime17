#ifndef GENERATORFUNCTIONTT_HPP
#define GENERATORFUNCTIONTT_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"

#include "int128_t.hpp"
using namespace primecount;

#include <chrono>
#include <gmp.h>
#include <iostream>

using namespace std::chrono;
using std::endl;
using std::cout;



template <class T>
class GeneratorFunctionTT : public GeneratorFunction<GeneratorFunctionTT<T>, T> {
    private:
    PrimeTest t_;

    protected:    

    public:
    GeneratorFunctionTT() noexcept;
    GeneratorFunctionTT(const GeneratorFunctionTT<T> & O) noexcept;
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "TT (multithread old tester)";}

};


template <class T>
GeneratorFunctionTT<T>::GeneratorFunctionTT() noexcept: t_(65, false, false) {}

// copy constructor which does not copy mutex nor t_, 
// new mutex will be created using default constructor, prime test does not have default constructor
template <class T>
GeneratorFunctionTT<T>::GeneratorFunctionTT(const GeneratorFunctionTT<T> & O __attribute__((unused))) noexcept: t_(65, false, false) {
}

template <class T>
int GeneratorFunctionTT<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    // change constructor and copy constructor to load precomputed primes
    // if (_t.IsPrimeDivByPrime(X)) {
    //     {
    //         const std::lock_guard<std::mutex> lock(Cntmutex__);
    //         PrimesCnt_++;
    //     }
    // }

//     if(_t.iMillerRabinBase2(mpz_X)) {
//         {
//   //          const std::lock_guard<std::mutex> lock(Cntmutex__);
//             PrimesCnt_++;
//         }
//     }


    if( t_.IsPrimeBPSW(mpz_X)) {
        GeneratorFunctionAbstract<T>::PrimesCnt_++;
    }                  
    return 0;
}

#endif