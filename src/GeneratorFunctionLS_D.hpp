#ifndef GeneratorFunctionLS_D_HPP
#define GeneratorFunctionLS_D_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


template <class T>
class GeneratorFunctionLS_D : public GeneratorFunction<GeneratorFunctionLS_D<T>, T> {
    private:
    PrimeTest t_;

    public:
    GeneratorFunctionLS_D() noexcept;
    GeneratorFunctionLS_D(const GeneratorFunctionLS_D<T> & O) noexcept;
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "LS D";}
};


template <class T>
GeneratorFunctionLS_D<T>::GeneratorFunctionLS_D() noexcept: t_(65) {}

template <class T>
GeneratorFunctionLS_D<T>::GeneratorFunctionLS_D(const GeneratorFunctionLS_D<T> & O __attribute__((unused))) noexcept: t_(65) {}

template <class T>
int GeneratorFunctionLS_D<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    

    if( t_.iLucasSelfridge_D(mpz_X)) {
        GeneratorFunctionAbstract<T>::PrimesCnt_++;
    }                  
    return 0;
}

#endif