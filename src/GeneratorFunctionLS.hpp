#ifndef GeneratorFunctionLS_HPP
#define GeneratorFunctionLS_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



template <class T>
class GeneratorFunctionLS : public GeneratorFunction<GeneratorFunctionLS<T>, T> {
    private:
    PrimeTest _t;

    public:
    GeneratorFunctionLS() noexcept;
    GeneratorFunctionLS(const GeneratorFunctionLS<T> & O) noexcept;
    
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "LS";}
};



template <class T>
GeneratorFunctionLS<T>::GeneratorFunctionLS() noexcept : _t(65) {}

template <class T>
GeneratorFunctionLS<T>::GeneratorFunctionLS(const GeneratorFunctionLS<T> & O __attribute__((unused))) noexcept: _t(65) {}

template <class T>
int GeneratorFunctionLS<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    
    if(_t.iLucasSelfridge(mpz_X)) {
        GeneratorFunctionAbstract<T>::_PrimesCnt++;
    }                  
    return 0;
}

#endif