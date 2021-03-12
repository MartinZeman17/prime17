#ifndef GeneratorFunctionMR_HPP
#define GeneratorFunctionMR_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



class GeneratorFunctionMR : public GeneratorFunction<GeneratorFunctionMR> {
    private:
    PrimeTest _t;

    public:
    GeneratorFunctionMR() noexcept;
    GeneratorFunctionMR(const GeneratorFunctionMR & O) noexcept;
    
    // no need for this code as += operator works fine automatically on a base class
    // GeneratorFunctionMR operator+(const GeneratorFunctionMR& rhs) {
    //     GeneratorFunctionMR result;
    //     result.operator+=((GeneratorFunction) rhs);
    //     result.operator+=((GeneratorFunction) *this);
    //     return result;
    // };


    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "MR";}
};

#endif