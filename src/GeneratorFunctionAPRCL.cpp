#include "GeneratorFunctionAPRCL.hpp"

extern "C" {
#include "mpz_aprcl.h"
}


int GeneratorFunctionAPRCL::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {

    if (mpz_aprcl(const_cast<mpz_t&>(mpz_X))) {
        _PrimesCnt++; 
    }  
    return 0;
}