#ifndef GENERATORFUNCTIONBITSTATISTICS_HPP
#define GENERATORFUNCTIONBITSTATISTICS_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


#include "int128_t.hpp"
using namespace primecount;

#include <gmp.h>
#include <vector>



class GeneratorFunctionBitStatistics : public GeneratorFunction<GeneratorFunctionBitStatistics> {
    private:
    mpz_t _mpz_X;
    PrimeTest _t;
    void SaveFileContent(std::ofstream &myfile, bool WithHeader=true) const;
    void Constructor() noexcept;

    
    protected:
    const unsigned int _iSliceBitIndex;
    unsigned long long _Begin;
    unsigned long long _End;
    static constexpr unsigned int C_Bits_ = 64; // 0..64 = 65 array items
    // Number of primes which have X inner bits set to 1
    // PrimesWithOneCnt[0 inner bits]=1 only if 2 is being tested. It is the only prime which has only one bit set to 1
    // All other intervals must have PrimesWithOneCnt[0]=0 
    unsigned long long _CntPrimesWithNumOfSetBits[C_Bits_ + 1];
    
    // CntPrimesWithOneAtPosition[4] = number of primes in a range which has bit number 4 (counted from 0, bit 0 determines whether number is odd) set to 1
    // CntPrimesWithOneAtPosition[0] is the highest number as all primes are odd except of prime 2
    unsigned long long _CntPrimesWithOneAtPosition[C_Bits_ + 1];  

    public:
    // GeneratorFunctionBitStatistics(BitStatistics *pBS);
    GeneratorFunctionBitStatistics(const unsigned int iSliceBitIndex=0) noexcept; //  Slice is than defined as a range from 2^iSliceBitIndex to 2^(iSliceBitIndex+1)-1
    GeneratorFunctionBitStatistics(const GeneratorFunctionBitStatistics & O) noexcept;

    virtual ~GeneratorFunctionBitStatistics();

    // unsigned long long Begin() const noexcept {return _Begin;};
    // unsigned long long End() const noexcept {return _End;};
    const unsigned long long & Begin = _Begin;
    const unsigned long long & End = _End;
    void SaveFile() const;
    std::vector<std::string> WebPost_SetFields() const;
    std::string WebPost_Serialize_CntPrimesWithOneAtPosition() const;
    
    int GenFunct(const __uint128_t & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "Bit Statistics";}
    
    virtual GeneratorFunctionBitStatistics& operator+=(const GeneratorFunctionAbstract& rhs) noexcept override;
    
};

#endif