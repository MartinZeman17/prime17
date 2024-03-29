#ifndef GeneratorFunctionMR_HPP
#define GeneratorFunctionMR_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



template <class T>
class GeneratorFunctionMR : public GeneratorFunction<GeneratorFunctionMR<T>, T> {
    private:
    PrimeTest t_;

    public:
    GeneratorFunctionMR() noexcept;
    GeneratorFunctionMR(const GeneratorFunctionMR<T> & O) noexcept;
    
    // no need for this code as += operator works fine automatically on a base class
    // GeneratorFunctionMR operator+(const GeneratorFunctionMR& rhs) {
    //     GeneratorFunctionMR result;
    //     result.operator+=((GeneratorFunction) rhs);
    //     result.operator+=((GeneratorFunction) *this);
    //     return result;
    // };


    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "MR";}
};





template <class T>
GeneratorFunctionMR<T>::GeneratorFunctionMR() noexcept : t_(65) {}

template <class T>
GeneratorFunctionMR<T>::GeneratorFunctionMR(const GeneratorFunctionMR<T> & O __attribute__((unused))) noexcept: t_(65) {}


template <class T>
int GeneratorFunctionMR<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    
    if( t_.iMillerRabinBase2(mpz_X)) {
        GeneratorFunctionAbstract<T>::PrimesCnt_++;
    }                  
    return 0;
}

#endif