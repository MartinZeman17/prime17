#ifndef GeneratorFunctionAPRCL_HPP
#define GeneratorFunctionAPRCL_HPP

#include "GeneratorFunction.hpp"


class GeneratorFunctionAPRCL : public GeneratorFunction<GeneratorFunctionAPRCL> {


    public:
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "APRCL";}
};

#endif