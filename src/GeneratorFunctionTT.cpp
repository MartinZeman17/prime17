#include "GeneratorFunctionTT.hpp"

#include <iostream>
#include <chrono>

using namespace std::chrono;


using std::endl;
using std::cout;

GeneratorFunctionTT::GeneratorFunctionTT() noexcept:_t(65, false, false) {}

// copy constructor which does not copy mutex nor _t, 
// new mutex will be created using default constructor, prime test does not have default constructor
GeneratorFunctionTT::GeneratorFunctionTT(const GeneratorFunctionTT & O __attribute__((unused))) noexcept:_t(65, false, false) {
}

int GeneratorFunctionTT::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    // change constructor and copy constructor to load precomputed primes
    // if (_t.IsPrimeDivByPrime(X)) {
    //     {
    //         const std::lock_guard<std::mutex> lock(_Cnt_mutex);
    //         _PrimesCnt++;
    //     }
    // }

//     if(_t.iMillerRabinBase2(mpz_X)) {
//         {
//   //          const std::lock_guard<std::mutex> lock(_Cnt_mutex);
//             _PrimesCnt++;
//         }
//     }


    if(_t.IsPrimeBPSW(mpz_X)) {
        _PrimesCnt++;
    }                  
    return 0;
}