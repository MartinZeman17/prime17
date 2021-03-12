#ifndef GeneratorFunctionBPSW2_HPP
#define GeneratorFunctionBPSW2_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



class GeneratorFunctionBPSW2 : public GeneratorFunction<GeneratorFunctionBPSW2> {
    private:

    public:
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "BPSW 2";}
};

#endif