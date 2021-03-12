#include "GeneratorFunctionBPSW2.hpp"

#include <gmp.h>

extern "C" {
#include "mpz_aprcl.h"
}

int GeneratorFunctionBPSW2::GenFunct(const uint128_t & X __attribute__((unused)), const mpz_t & mpz_X) {
    
    if (mpz_bpsw_prp(const_cast<mpz_t&>(mpz_X))) {
        _PrimesCnt++;
    }

    return 0;
}