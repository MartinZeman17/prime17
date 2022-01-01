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
#include <chrono>

// use Eratosthenes sieve with the following primorial
// bigger primorial = better efficiency, however at the end some threads have nothing to do
#ifdef NDEBUG
// const unsigned int C_SieveGeneratorDefaultMaxPrime=23; 
const unsigned int C_SieveGeneratorDefaultMaxPrime=19;
#else
const unsigned int C_SieveGeneratorDefaultMaxPrime=19;
// const unsigned int C_SieveGeneratorDefaultMaxPrime=23;
#endif 

// 23# fits into uint_32 datatype (36 495 360 coprimes to primorial 223 092 870)
const std::vector <unsigned long long> CPrimorial_Primes({2,3,5,7,11,13,17,19,23,29,31,37,41,43});
const std::vector<unsigned long long> CPrimorial_s({2,6,30, 210, 2310, 30030, 510510, 9699690, 223092870, 6469693230, 200560490130});
const std::vector<unsigned long long> CTestArray_Sizes({1,2,8,48, 480, 5760, 92160, 1658880, 36495360, 1021870080, 30656102400});
// const std::vector <long double> C_Effectivity({0.5, 0.333333, 0.266667, 0.228571, 0.207792,  0.191808, 0.180525, 0.171024, 0.163588, 0.157947, 0.152852});

template <class T> 
class SieveGenerator {
    private:
    static unsigned int pMaxPrime_;  // why can not be const??
    static std::vector<uint32_t> pTestArray_;
    static unsigned long long pTestArrayCount_;
    static uint32_t pPrimorial_;  
    static std::mutex pTestArray_mutex_;

    void Constructor(unsigned int MaxPrime);
    void PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const;
    T WorkMT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract<T>> & GF, const std::string PId);
    std::chrono::time_point<std::chrono::high_resolution_clock> BeginTime_; //  = std::high_resolution_clock::now(); 

    T kp_;
    std::mutex kp_mutex_;
    
    long double Percent_;
    std::mutex Percent_mutex_;
    
    std::mutex cout_mutex_;

    std::chrono::time_point<std::chrono::high_resolution_clock> StartTime_;
    // std::mutex StartTime_mutex_;

    unsigned int Threads_;
    unsigned int ncursesY_;
    unsigned int ncursesX_;

    public:
    // ToDo can be static??? Class must be initialized first and work function must be ready for reentrancy. I will let it be for the time being

    T Work(const T & Begin, const T & End, GeneratorFunctionAbstract<T> & GF) const;
    // multithreading version
    T WorkMT(const T & Begin, const T & End, GeneratorFunctionAbstract<T> & GF);


    SieveGenerator();
    // constructor, parameter may be set to C_SieveGeneratorDefaultMaxPrime
    // SieveGenerator(unsigned int MaxPrime = C_SieveGeneratorDefaultMaxPrime); 
    SieveGenerator(unsigned int MaxPrime ); 
    const unsigned int & MaxPrime = pMaxPrime_;
    // unsigned long long Primorial() const {return pPrimorial_;};  
    const uint32_t & Primorial = pPrimorial_;

    unsigned int Threads(const unsigned int Percent);

    void ResetClock() noexcept;
    long double DurationMinutes() const noexcept;
    long double DurationSeconds() const noexcept;
};

#include "SieveGeneratorT.cpp"


#endif