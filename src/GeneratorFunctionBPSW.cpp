#include "GeneratorFunctionBPSW.hpp"


GeneratorFunctionBPSW::GeneratorFunctionBPSW() noexcept:_t(65) {}
GeneratorFunctionBPSW::GeneratorFunctionBPSW(const GeneratorFunctionBPSW & O __attribute__((unused))) noexcept: _t(65) {}
    


int GeneratorFunctionBPSW::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    // if (mpz_bpsw_prp(const_cast<mpz_t&>(mpz_X))) {
    //     _PrimesCnt++;
    // }
    
    if(_t.IsPrimeBPSW(mpz_X)) {
        // _PrimesCnt++
        _PrimesCnt++;
    }                  
    return 0;
}