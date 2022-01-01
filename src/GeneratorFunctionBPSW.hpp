#ifndef GeneratorFunctionBPSW_HPP
#define GeneratorFunctionBPSW_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


template <class T>
class GeneratorFunctionBPSW : public GeneratorFunction<GeneratorFunctionBPSW<T>, T> {
    private:
    PrimeTest t_;

    public:
    GeneratorFunctionBPSW() noexcept;
    GeneratorFunctionBPSW(const GeneratorFunctionBPSW<T> & O) noexcept;
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;

    virtual std::string toName() const noexcept override  {return "BPSW";}
};



template <class T>
GeneratorFunctionBPSW<T>::GeneratorFunctionBPSW() noexcept: t_(65) {}

template <class T>
GeneratorFunctionBPSW<T>::GeneratorFunctionBPSW(const GeneratorFunctionBPSW<T> & O __attribute__((unused))) noexcept: t_(65) {}
    

template <class T>
int GeneratorFunctionBPSW<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    // if (mpz_bpsw_prp(const_cast<mpz_t&>(mpz_X))) {
    //     PrimesCnt_++;
    // }
    
    if( t_.IsPrimeBPSW(mpz_X)) {
        // it seems that in first phase of template compiling PrimeCnt_ is a dependant name and so qualifier is required
        GeneratorFunctionAbstract<T>::PrimesCnt_++;
    }                  
    return 0;
} 

#endif