#ifndef GeneratorFunctionLS_D_HPP
#define GeneratorFunctionLS_D_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



class GeneratorFunctionLS_D : public GeneratorFunction<GeneratorFunctionLS_D> {
    private:
    PrimeTest _t;

    public:
    GeneratorFunctionLS_D() noexcept;
    GeneratorFunctionLS_D(const GeneratorFunctionLS_D & O) noexcept;
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "LS D";}
};

#endif