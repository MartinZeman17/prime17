#ifndef GeneratorFunctionPRIMECREATOR_HPP
#define GeneratorFunctionPRIMECREATOR_HPP

#include <vector>

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"



class GeneratorFunctionPrimeCreator : public GeneratorFunction<GeneratorFunctionPrimeCreator> {
    private:
    PrimeTest _t;
    unsigned long long _NumberOfPrimesToCreate;

    public:
    GeneratorFunctionPrimeCreator(unsigned long long NumberOfPrimes);
    // copy consructor is missing which ensures this class is not usable in multithreading sieve. 
    // It would be a chalenge to join results in right order

    std::vector<uint128_t> CreatedPrimes;

    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "Prime Creator";}
};

#endif