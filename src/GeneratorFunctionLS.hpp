#ifndef GeneratorFunctionLS_HPP
#define GeneratorFunctionLS_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



class GeneratorFunctionLS : public GeneratorFunction<GeneratorFunctionLS> {
    private:
    PrimeTest _t;

    public:
    GeneratorFunctionLS() noexcept;
    GeneratorFunctionLS(const GeneratorFunctionLS & O) noexcept;
    
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "LS";}
};

#endif