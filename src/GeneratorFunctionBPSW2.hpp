#ifndef GeneratorFunctionBPSW2_HPP
#define GeneratorFunctionBPSW2_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


#include <gmp.h>

extern "C" {
#include "mpz_aprcl.h"
}

template <class T>
class GeneratorFunctionBPSW2 : public GeneratorFunction<GeneratorFunctionBPSW2<T>, T> {
    private:

    public:
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "BPSW 2";}
};


template <class T>
int GeneratorFunctionBPSW2<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    
    if (mpz_bpsw_prp(const_cast<mpz_t&>(mpz_X))) {
        GeneratorFunctionAbstract<T>::PrimesCnt_++;
    }

    return 0;
}

#endif