#ifndef PRIMETEST_HPP
#define PRIMETEST_HPP

#include <gmp.h>
#include <vector>
#include <mutex>
#include <math.h>

#include "int128_t.hpp"
#include "PrimesRangeService.hpp"


using namespace std;
using namespace primecount;


class PrimeTest{
    private:
        mpz_t mpzB, mpzNm1, mpzd, mpzRem, mpzOne; 
        unsigned long long ulDmax;

        mpz_t mpzN;
        mpz_t mpz_X;

        //DivByPrime
        mpz_t tmp;
        mpz_t mpz_Prime64;


        std::shared_ptr<PrimesRange> P;

        uint32_t *Primes32=nullptr;
        unsigned long long PrimesCount32 = 0;

        uint64_t *Primes64=nullptr;
        unsigned long long PrimesCount64 = 0;
        unsigned long long LimitForDivByPrime = 0;

        void Constructor(unsigned int iBits, bool bLoadPrimes32, bool bLoadPrimes64);
        
    
    public:
    //??? defaultni parametry construktoru
        PrimeTest(unsigned int iBits);
        PrimeTest(unsigned int iBits, bool bLoadPrimes32, bool bLoadPrimes64);

        ~PrimeTest();

        void LoadPrimes32();
        void LoadPrimes64();

        bool iMillerRabinBase2(const unsigned long long & X);
        bool iMillerRabinBase2(const uint128_t & X);
        bool iMillerRabinBase2(const mpz_t & mpzN);
        // bool iMillerRabinBase2Optimized(const mpz_t & mpzN);


        bool iLucasSelfridge(const mpz_t & mpzN);
        long iLucasSelfridge_D(const mpz_t & mpzN); //returns only D coef
        void iLucasSelfridge_DHistogram(const mpz_t & mpzN); //histogram of D coeficient
        
        static const unsigned int C_HistItems=10; //10 columns and one forall bigger values
        vector<unsigned long> DHist1;
        vector<unsigned long> DHist10;
        vector<unsigned long> DHist100;
        vector<unsigned long> DHist1000;
        long long MaxD;
        mpz_t NumberWithMaxD;

        bool IsPrimeBPSW(const unsigned long long X);
        bool IsPrimeBPSW(const uint128_t & X);
        bool IsPrimeBPSW(const mpz_t & N);

        bool IsPrimeDivByPrime(mpz_t x);
        bool IsPrimeDivByPrime(const unsigned long long x);

        
};
#endif  
