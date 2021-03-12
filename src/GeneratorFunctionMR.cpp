#include "GeneratorFunctionMR.hpp"


GeneratorFunctionMR::GeneratorFunctionMR() noexcept :_t(65) {}
GeneratorFunctionMR::GeneratorFunctionMR(const GeneratorFunctionMR & O __attribute__((unused))) noexcept: _t(65) {}


int GeneratorFunctionMR::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    
    if(_t.iMillerRabinBase2(mpz_X)) {
        _PrimesCnt++;
    }                  
    return 0;
}