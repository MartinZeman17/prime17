#ifndef GeneratorFunctionBPSW_HPP
#define GeneratorFunctionBPSW_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



class GeneratorFunctionBPSW : public GeneratorFunction<GeneratorFunctionBPSW> {
    private:
    PrimeTest _t;

    public:
    GeneratorFunctionBPSW() noexcept;
    GeneratorFunctionBPSW(const GeneratorFunctionBPSW & O) noexcept;
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "BPSW";}
};

#endif