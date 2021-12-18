#ifndef GeneratorFunctionAPRCL_HPP
#define GeneratorFunctionAPRCL_HPP

#include "GeneratorFunction.hpp"

extern "C" {
#include "mpz_aprcl.h"
}

template <class T>
class GeneratorFunctionAPRCL : public GeneratorFunction<GeneratorFunctionAPRCL<T>,T> {


    public:
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "APRCL";}
};

template <class T>
int GeneratorFunctionAPRCL<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {

    if (mpz_aprcl(const_cast<mpz_t&>(mpz_X))) {
        GeneratorFunctionAbstract<T>::_PrimesCnt++; 
    }  
    return 0;
}

#endif