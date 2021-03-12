#include "GeneratorFunctionLS_DHistogram.hpp"


GeneratorFunctionLS_DHistogram::GeneratorFunctionLS_DHistogram() noexcept: _t(65) {}
GeneratorFunctionLS_DHistogram::GeneratorFunctionLS_DHistogram(const GeneratorFunctionLS_DHistogram & O __attribute__((unused))) noexcept: _t(65) {}


int GeneratorFunctionLS_DHistogram::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    

    _t.iLucasSelfridge_DHistogram(mpz_X);
    return 0;
}