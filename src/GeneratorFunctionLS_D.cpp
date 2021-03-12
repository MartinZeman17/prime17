#include "GeneratorFunctionLS_D.hpp"


GeneratorFunctionLS_D::GeneratorFunctionLS_D() noexcept: _t(65) {}
GeneratorFunctionLS_D::GeneratorFunctionLS_D(const GeneratorFunctionLS_D & O __attribute__((unused))) noexcept: _t(65) {}

int GeneratorFunctionLS_D::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    

    if(_t.iLucasSelfridge_D(mpz_X)) {
        _PrimesCnt++;
    }                  
    return 0;
}