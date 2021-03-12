#ifndef GENERATORFUNCTIONTT_HPP
#define GENERATORFUNCTIONTT_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"

#include "int128_t.hpp"
using namespace primecount;

#include <chrono>
#include <gmp.h>

using namespace std::chrono;


class GeneratorFunctionTT : public GeneratorFunction<GeneratorFunctionTT> {
    private:
    PrimeTest _t;

    protected:    

    public:
    GeneratorFunctionTT() noexcept;
    GeneratorFunctionTT(const GeneratorFunctionTT & O) noexcept;
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "TT (multithread tester)";}

};



#endif