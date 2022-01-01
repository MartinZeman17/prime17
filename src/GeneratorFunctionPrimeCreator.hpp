#ifndef GeneratorFunctionPRIMECREATOR_HPP
#define GeneratorFunctionPRIMECREATOR_HPP

#include <vector>

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


template <class T>
class GeneratorFunctionPrimeCreator : public GeneratorFunction<GeneratorFunctionPrimeCreator<T>, T> {
    private:
    PrimeTest t_;
    unsigned long long NumberOfPrimesToCreate_;

    public:
    GeneratorFunctionPrimeCreator(size_t NumberOfPrimes);
    // copy consructor is missing which ensures this class is not usable in multithreading sieve. 
    // It would be a chalenge to join results in the right order

    std::vector<T> CreatedPrimes;

    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "Prime Creator";}
};



template <class T>
GeneratorFunctionPrimeCreator<T>::GeneratorFunctionPrimeCreator(size_t NumberOfPrimes) : t_(65), NumberOfPrimesToCreate_{NumberOfPrimes} 
{   
    CreatedPrimes.reserve(NumberOfPrimesToCreate_);
}


template <class T>
int GeneratorFunctionPrimeCreator<T>::GenFunct(const T & X, const mpz_t & mpz_X) {
    
    if( t_.IsPrimeBPSW(mpz_X)) {
        GeneratorFunctionAbstract<T>::PrimesCnt_++;
        CreatedPrimes.push_back(X);
        if (GeneratorFunctionAbstract<T>::PrimesCnt_ == NumberOfPrimesToCreate_) return 1;
    }                  
    return 0;
}


#endif