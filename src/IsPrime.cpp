#include <iostream>
#include <chrono>
#include <gmp.h>
#include <math.h>
#include <stdint.h>
#include <fstream>

#include "PrimeTest.hpp"
#include "utils_mpz.hpp"

using namespace std::chrono;
using std::endl;
using std::cout;


// ToDo: use a service and remove these variables, create a class?
uint32_t *Primes32=nullptr;
unsigned long long PrimesCount32 = 0;
unsigned long long Limit32 = 0;

uint64_t *Primes64=nullptr;
unsigned long long PrimesCount64 = 0;
unsigned long long Limit64 = 0;


/*
PrimeBitStorageLimit 106528683
Number of primes to store 6115364
Biggest prime 106528657

PrimeBitStorageLimit 106528702
Number of primes to store 6115365
Biggest prime 106528699
*/



bool IsPrimeDivByPrime(mpz_t x)
{
    mpz_t tmp;
    mpz_init(tmp);
    mpz_t mpz_Prime;
    mpz_init(mpz_Prime);
    unsigned long long MaxToTest;
    mpz_sqrt(tmp,x);
    MaxToTest = utils_mpz::mpz_get<unsigned long long>(tmp);    // should fit into 32 bit
    
    uint32_t Prime=0;
    if(mpz_cmp_ui(x,1)<=0) return false; //ok
    for(std::size_t i = 0; i<PrimesCount32; i++)
    {
        Prime = Primes32 [i];
        if (Prime > MaxToTest) return true;
        mpz_mod_ui(tmp, x, Prime); //ok,32 bits only
        if(mpz_cmp_ui(tmp, 0) == 0) return false; //ok
    }
    for(std::size_t i = 0; i<PrimesCount64; i++)
    {
        unsigned long long Prime64 = Primes64[i];
        utils_mpz::mpz_set_ull(mpz_Prime, Prime64);
        if (Prime64 > MaxToTest) return true;
        mpz_mod(tmp, x, mpz_Prime);
        if(mpz_cmp_ui(tmp, 0) == 0) return false;  //ok
    }
    if(MaxToTest>Limit64) std::cout << "IsPrimeDivByPrime: Limit exceeded" << endl;    
    return true;
}

bool IsPrimeDivByPrime(const unsigned long long x)
{
    unsigned long long MaxToTest = (unsigned long long) sqrtl(x);
    uint32_t Prime=0;
    if (x<=1) return false;
    for (unsigned long long i = 0; i<PrimesCount32; i++)
    {
        Prime = Primes32 [i];
        if (Prime > MaxToTest) return true;
        if (x % Prime == 0) return false;
    }
    return true;
}

bool IsPrimeDiv(const unsigned long long x)
{
    unsigned long long MaxToTest = (unsigned long long) sqrtl(x);
    //cout << x << endl;
    if (x % 2 == 0)
        return false;
    for (unsigned long long i = 3; i <= MaxToTest; i += 2)
    {
        if (x % i == 0)
            return false;
    }
    return true;
}

unsigned long long TestArray[8] = {1, 7, 11, 13, 17, 19, 23, 29};
bool IsPrimePrimorial(const unsigned long long x)
{
    uint32_t MaxToTest = (uint32_t) sqrtl(x);
    //cout << x << endl;
    if (x <= 30)
    {
        if (x == 2)
            return true;
        if (x == 3)
            return true;
        if (x == 5)
            return true;
        if (x == 7)
            return true;
        if (x == 11)
            return true;
        if (x == 13)
            return true;
        if (x == 17)
            return true;
        if (x == 19)
            return true;
        if (x == 23)
            return true;
        if (x == 29)
            return true;
        return false;
    }
    if (x % 2 == 0)
        return false;
    if (x % 3 == 0)
        return false;
    if (x % 5 == 0)
        return false;
    if (x % 7 == 0)
        return false;
    if (x % 11 == 0)
        return false;
    if (x % 13 == 0)
        return false;
    if (x % 17 == 0)
        return false;
    if (x % 19 == 0)
        return false;
    if (x % 23 == 0)
        return false;
    if (x % 29 == 0)
        return false;

    for (unsigned long i = 30; i <= MaxToTest; i += 30)
    {
        for (unsigned long j = 0; j <= 7; j++)
        {
            if (x % (i + TestArray[j]) == 0)
            return false;
        }
    }
    return true;
}

void TimeToTest(unsigned long long ulBegin, unsigned long long ulBigStep, unsigned long long ulSmallStep, unsigned long long ulEnd)
{
    PrimeTest t(64);
    mpz_t Tst;
    mpz_init(Tst);

    const unsigned int ArrSize = 7;
    unsigned long long Dur[ArrSize];
    unsigned long long Cnt[ArrSize];
    unsigned long long PrimesCount;
    bool Result;
    std::ofstream myfile;
    myfile.open("TimeToTest.txt");

    for (unsigned long long T = ulBegin; T <= ulEnd; T += ulBigStep)
    {
        for (unsigned int j=0; j<ArrSize; j++){
            Dur[j]=0;
            Cnt[j]=0;
        }

        PrimesCount=0;
        auto Begin = high_resolution_clock::now();
        for (unsigned long long K = 1; K <= ulSmallStep; K++)
        {
            //std::cout << T << "   " << K << "   " << T+K << endl;
            Result = IsPrimeDiv(T + K);
            if (Result) PrimesCount++;
            //std::cout << T+K << "   " << Result << endl;
        }
        auto End = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(End - Begin);
        Dur[0] = (unsigned) duration.count();
        Cnt[0] = PrimesCount;

        PrimesCount=0;
        Begin = high_resolution_clock::now();
        for (unsigned long long K = 1; K <= ulSmallStep; K++)
        {
            //std::cout << T << "   " << K << "   " << T+K << endl;
            Result = IsPrimePrimorial(T + K);
            if (Result) PrimesCount++;
            //std::cout << T+K << "   " << Result << endl;
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        Dur[1] = (unsigned) duration.count();
        Cnt[1] = PrimesCount;
        

        PrimesCount=0;
        Begin = high_resolution_clock::now();
        for (unsigned long long K = 1; K <= ulSmallStep; K++)
        {
            //std::cout << T << "   " << K << "   " << T+K << endl;
            Result = IsPrimeDivByPrime(T + K);
            if (Result) PrimesCount++;
            //std::cout << T+K << "   " << Result << endl;
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        Dur[2] = (unsigned) duration.count();
        Cnt[2] = PrimesCount;

        
        PrimesCount=0;
        Begin = high_resolution_clock::now();
        for (unsigned long long K = 1; K <= ulSmallStep; K++)
        {
            utils_mpz::mpz_set_ull(Tst, T + K);
            Result = t.iLucasSelfridge(Tst);
            if (Result) PrimesCount++;
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        Dur[3] =(unsigned)  duration.count();
        Cnt[3] = PrimesCount;

        PrimesCount=0;
        Begin = high_resolution_clock::now();
        for (unsigned long long K = 1; K <= ulSmallStep; K++)
        {
            utils_mpz::mpz_set_ull(Tst, T + K);
            Result = t.iMillerRabinBase2(Tst);
            if (Result) PrimesCount++;
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        Dur[4] = (unsigned) duration.count();
        Cnt[4] = PrimesCount;

        Dur[5] = 0;
        Cnt[5] = 0;

        PrimesCount=0;
        Begin = high_resolution_clock::now();
        for (unsigned long long K = 1; K <= ulSmallStep; K++)
        {
            utils_mpz::mpz_set_ull(Tst, T + K);
            Result = t.IsPrimeBPSW(Tst);
            if (Result) PrimesCount++;
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        Dur[6] = (unsigned) duration.count();
        Cnt[6] = PrimesCount;

        // update primes count:
        // count in 0 position is should be correct
        // further positions will contain only changes
        
        for (unsigned int j=1; j<ArrSize; j++){
            Cnt[j]=Cnt[j]-Cnt[0];
        }

        std::cout << T << " " ;
        myfile << T << " " ;
        for (unsigned int j=0; j<ArrSize; j++){
            std::cout << Dur[j]<<" ";
            myfile << Dur[j]<<" ";
        }

        std::cout << "  ";
        myfile << "  ";

        for (unsigned int j=0; j<ArrSize; j++)
        {
            std::cout << Cnt[j]<<" ";
            myfile << Cnt[j]<<" ";
        }
        std::cout << endl;
        myfile << endl;
    }
    myfile.close();
    mpz_clear(Tst);
}

void SmallStep(unsigned long long Cycles){
    auto Begin = high_resolution_clock::now();
    // auto BeginPr = high_resolution_clock::now();
    // auto EndPr = high_resolution_clock::now();
    // auto durationPr = duration_cast<milliseconds>(EndPr - BeginPr);

    unsigned long long PrimeCount = 0;
    
    mpz_t x;
    mpz_init(x);
    utils_mpz::mpz_set_ull(x, UINT64_MAX);
    bool Res;
    PrimeTest t(64);

    for(unsigned long long i=0;i<Cycles;i++){
        Res = t.IsPrimeBPSW(x);
        if(Res){
            PrimeCount++;
            //BeginPr = high_resolution_clock::now();
            //Res = IsPrimeDivByPrime(x);
            //EndPr = high_resolution_clock::now();
            //durationPr = duration_cast<milliseconds>(EndPr - BeginPr);
            //std::cout << PrimeCount << " " << durationPr.count() << endl;
            //if(!Res) std::cout << "BPSW Pseudoprime detected    !!!!!!!!!!!!!!!" << endl;
        }
        mpz_add_ui(x, x, 2); //ok
    }
    
    auto End = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(End - Begin);
    std::cout << endl;
    std::cout << duration.count() << endl << endl << PrimeCount << endl;
}


unsigned long long** TestPrimeArray64;
unsigned long long** CreateTestPrimes (unsigned long long Begin, unsigned long long Step, unsigned long long StepCount, unsigned long long PrimesCount)
{    
    unsigned long long **TestPrimeArray, *ptr;
    unsigned long long len = sizeof(unsigned long long *) * StepCount + sizeof(unsigned long long) * PrimesCount * StepCount; 
    TestPrimeArray = (unsigned long long **)malloc(len);
    ptr = (unsigned long long *)(TestPrimeArray + StepCount);  

    // for loop to point rows pointer to appropriate location in 2D array 
    for(unsigned long long i = 0; i < StepCount; i++) 
        TestPrimeArray[i] = (ptr + PrimesCount * i); 

    for(unsigned long long s=0;s<StepCount;s++)
    {
        unsigned long long x =Begin + s*Step;
        for(unsigned long long i=0;i<PrimesCount;)
        {
            if (IsPrimeDivByPrime(x)) {
                TestPrimeArray [s] [i] = x;
                std::cout << x << " " << s << " " << i <<endl;
                i++;
            }
            std::cout << x << endl; 
            x++;
        }
    }
    return TestPrimeArray;
}



  //TestPrimeArray64=CreateTestPrimes(0,1000000,5,2);
    //cout << TestPrimeArray64[0][1]<<endl;

    //SmallStep(1000000);

    //Primes32;
    //TimeToTest(0,100000000000000000,1000,UINT64_MAX/2UL);
    //TimeToTest(0,100000000000000000UL,1,10000000000000000000UL);
    //TimeToTest(0UL,1000000000000000UL,100000,100000000000000000UL);

    //TimeToTest(0UL,1000000000000UL,1000,100000000000000UL);

    //10000000000000000000UL

    //bool result;
    //result = IsPrimeDiv(999994273);
    //std::cout << result << endl;
    /*
    result = IsPrimePrimorial(7);
    std::cout << result << endl; 
*/

    //mpz_clear(Tst);
 //   std::free(TestPrimeArray64);    
 //   std::free(Primes32);
 //   std::free(Primes64);
    //std::cout << "Done..." << endl; 
    //char str[1];
    //cin.getline(str, 1);