#ifndef GeneratorFunctionPRIMECREATOR_HPP
#define GeneratorFunctionPRIMECREATOR_HPP

#include <vector>

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


template <class T>
class GeneratorFunctionPrimeCreator : public GeneratorFunction<GeneratorFunctionPrimeCreator<T>, T> {
    private:
    PrimeTest _t;
    unsigned long long _NumberOfPrimesToCreate;

    public:
    GeneratorFunctionPrimeCreator(size_t NumberOfPrimes);
    // copy consructor is missing which ensures this class is not usable in multithreading sieve. 
    // It would be a chalenge to join results in the right order

    std::vector<T> CreatedPrimes;

    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "Prime Creator";}
};



template <class T>
GeneratorFunctionPrimeCreator<T>::GeneratorFunctionPrimeCreator(size_t NumberOfPrimes) :_t(65), _NumberOfPrimesToCreate{NumberOfPrimes} 
{   
    CreatedPrimes.reserve(_NumberOfPrimesToCreate);
}


template <class T>
int GeneratorFunctionPrimeCreator<T>::GenFunct(const T & X, const mpz_t & mpz_X) {
    
    if(_t.IsPrimeBPSW(mpz_X)) {
        GeneratorFunctionAbstract<T>::_PrimesCnt++;
        CreatedPrimes.push_back(X);
        if (GeneratorFunctionAbstract<T>::_PrimesCnt == _NumberOfPrimesToCreate) return 1;
    }                  
    return 0;
}


#endif