#include "GeneratorFunctionPrimeCreator.hpp"


GeneratorFunctionPrimeCreator::GeneratorFunctionPrimeCreator(unsigned long long NumberOfPrimes) :_t(65), _NumberOfPrimesToCreate{NumberOfPrimes} 
{   
    CreatedPrimes.reserve(_NumberOfPrimesToCreate);
}


int GeneratorFunctionPrimeCreator::GenFunct(const uint128_t & X, const mpz_t & mpz_X) {
    
    if(_t.IsPrimeBPSW(mpz_X)) {
        _PrimesCnt++;
        CreatedPrimes.push_back(X);
        if (_PrimesCnt == _NumberOfPrimesToCreate) return 1;
    }                  
    return 0;
}