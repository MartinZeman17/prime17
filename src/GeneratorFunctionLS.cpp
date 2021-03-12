#include "GeneratorFunctionLS.hpp"


GeneratorFunctionLS::GeneratorFunctionLS() noexcept : _t(65) {}
GeneratorFunctionLS::GeneratorFunctionLS(const GeneratorFunctionLS & O __attribute__((unused))) noexcept: _t(65) {}


int GeneratorFunctionLS::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    
    if(_t.iLucasSelfridge(mpz_X)) {
        _PrimesCnt++;
    }                  
    return 0;
}