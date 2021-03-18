/*
    Constructor prepares Eratosthenes sieve with defined primorial
    Work method takes 128 bit range and repeatedly calls GeneratorFunction->GenFunct(uint128_t) 
    Sieve stops when generator function returns non zero
*/

#ifndef SIEVEGENERATOR_HPP
#define SIEVEGENERATOR_HPP

#include "int128_t.hpp"
#include "GeneratorFunction.hpp"
#include "Log.hpp"

using namespace primecount;

#include <vector>
#include <mutex>

// use Eratosthenes sieve with the following primorial
#ifdef NDEBUG
// const unsigned int C_SieveGeneratorDefaultMaxPrime=23;   //??? ToDo for final release
const unsigned int C_SieveGeneratorDefaultMaxPrime=19;
#else
const unsigned int C_SieveGeneratorDefaultMaxPrime=19;
#endif 

// 23# fits into uint_32 datatype (36 495 360 coprimes to primorial 223 092 870)
const std::vector <unsigned long long> C_PrimorialPrimes({2,3,5,7,11,13,17,19,23,29,31,37,41,43});
const std::vector<unsigned long long> C_Primorials({2,6,30, 210, 2310, 30030, 510510, 9699690, 223092870, 6469693230, 200560490130});
const std::vector<unsigned long long> C_TestArraySizes({1,2,8,48, 480, 5760, 92160, 1658880, 36495360, 1021870080, 30656102400});
// const std::vector <long double> C_Effectivity({0.5, 0.333333, 0.266667, 0.228571, 0.207792,  0.191808, 0.180525, 0.171024, 0.163588, 0.157947, 0.152852});

// ToDo: make class template for 64 or 128 bits?
template <class T> 
class SieveGenerator {
    private:
    static unsigned int p_MaxPrime;  // why can not be const??
    static std::vector<uint32_t> p_TestArray;
    static unsigned long long p_TestArrayCount;
    static uint32_t p_Primorial;  //
    static std::mutex p_TestArray_mutex;
    void Constructor(unsigned int MaxPrime);
    void PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const;
    T WorkMT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract> & GF, const std::string PId);

    T _kp;
    std::mutex _kp_mutex;
    
    long double _Percent;
    std::mutex _Percent_mutex;
    
    std::mutex _cout_mutex;

    std::chrono::time_point<std::chrono::high_resolution_clock> _StartTime;
    // std::mutex _StartTime_mutex;

    unsigned int _Threads;
    unsigned int _ncursesY;
    unsigned int _ncursesX;

    public:
    //ToDo can be static??? Class must be initialized first and work function must be ready for reentrancy. I will let it be for the time being
    // uint128_t Work(const uint128_t & Begin, const uint128_t & End, GeneratorFunction * GF) const; 
    // unsigned long long Work(const unsigned long long & Begin, const unsigned long long & End, GeneratorFunction * GF) const; 

    // T Work(const T & Begin, const T & End, GeneratorFunctionAbstract * GF) const;
    T Work(const T & Begin, const T & End, GeneratorFunctionAbstract & GF) const;
    // multithreading version
    T WorkMT(const T & Begin, const T & End, GeneratorFunctionAbstract & GF);


    SieveGenerator();
    // constructor, parameter may be set to C_SieveGeneratorDefaultMaxPrime
    // SieveGenerator(unsigned int MaxPrime = C_SieveGeneratorDefaultMaxPrime); 
    SieveGenerator(unsigned int MaxPrime ); 
    const unsigned int & MaxPrime = p_MaxPrime;
    // unsigned long long Primorial() const {return p_Primorial;};  
    const uint32_t & Primorial = p_Primorial;

    unsigned int Threads(const unsigned int Percent);

};

#include "SieveGeneratorT.cpp"


#endif