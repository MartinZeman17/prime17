#ifndef GeneratorFunctionLS_DHISTOGRAM_HPP
#define GeneratorFunctionLS_DHISTOGRAM_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


class GeneratorFunctionLS_DHistogram : public GeneratorFunction<GeneratorFunctionLS_DHistogram> {
    private:

    public:
    PrimeTest _t;
    GeneratorFunctionLS_DHistogram() noexcept;
    GeneratorFunctionLS_DHistogram(const GeneratorFunctionLS_DHistogram & O) noexcept;
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "LS D Histogram";}
};

#endif