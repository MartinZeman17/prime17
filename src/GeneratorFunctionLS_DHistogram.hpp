#ifndef GeneratorFunctionLS_DHISTOGRAM_HPP
#define GeneratorFunctionLS_DHISTOGRAM_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


template <class T>
class GeneratorFunctionLS_DHistogram : public GeneratorFunction<GeneratorFunctionLS_DHistogram<T>, T> {
    private:

    public:
    PrimeTest _t;
    GeneratorFunctionLS_DHistogram() noexcept;
    GeneratorFunctionLS_DHistogram(const GeneratorFunctionLS_DHistogram<T> & O) noexcept;
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "LS D Histogram";}
};



template <class T>
GeneratorFunctionLS_DHistogram<T>::GeneratorFunctionLS_DHistogram() noexcept: _t(65) {}

template <class T>
GeneratorFunctionLS_DHistogram<T>::GeneratorFunctionLS_DHistogram(const GeneratorFunctionLS_DHistogram<T> & O __attribute__((unused))) noexcept: _t(65) {}

template <class T>
int GeneratorFunctionLS_DHistogram<T>::GenFunct(const T & X __attribute__((unused)), const mpz_t & mpz_X) {
    

    _t.iLucasSelfridge_DHistogram(mpz_X);
    return 0;
}

#endif