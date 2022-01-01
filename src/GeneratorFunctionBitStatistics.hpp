#ifndef GENERATORFUNCTIONBITSTATISTICS_HPP
#define GENERATORFUNCTIONBITSTATISTICS_HPP

#include "GeneratorFunction.hpp"
#include "PrimeTest.hpp"


#include "int128_t.hpp"
using namespace primecount;

#include <gmp.h>
#include <vector>
#include <cassert>

#include <iostream>
#include <filesystem>
#include <fstream>

#include <gmp.h>
extern "C" {
#include "mpz_aprcl.h"
}


using std::endl;
using std::cout;


template <class T>
class GeneratorFunctionBitStatistics : public GeneratorFunction<GeneratorFunctionBitStatistics<T>, T> {
    private:
    mpz_t mpz_X_;
    PrimeTest t_;
    void SaveFileContent(std::ofstream &myfile, bool WithHeader=true) const;
    void Constructor() noexcept;

    
    protected:
    const unsigned int iSliceBitIndex_;
    unsigned long long Begin_;
    unsigned long long End_;
    static constexpr unsigned int C_Bits_ = 64; // 0..64 = 65 array items //ToDo 128
    // Number of primes which have X inner bits set to 1
    // PrimesWithOneCnt[0 inner bits]=1 only if 2 is being tested. It is the only prime which has only one bit set to 1
    // All other intervals must have PrimesWithOneCnt[0]=0 
    unsigned long long CntPrimesWithNumOfSetBits_[C_Bits_ + 1];
    
    // CntPrimesWithOneAtPosition[4] = number of primes in a range which has bit number 4 (counted from 0, bit 0 determines whether number is odd) set to 1
    // CntPrimesWithOneAtPosition[0] is the highest number as all primes are odd except of prime 2
    unsigned long long CntPrimesWithOneAtPosition_[C_Bits_ + 1];  

    public:
    GeneratorFunctionBitStatistics(const unsigned int iSliceBitIndex=0) noexcept; //  Slice is than defined as a range from 2^iSliceBitIndex to 2^(iSliceBitIndex+1)-1
    GeneratorFunctionBitStatistics(const GeneratorFunctionBitStatistics<T> & O) noexcept;

    virtual ~GeneratorFunctionBitStatistics();

    unsigned long long Begin() const noexcept {return Begin_;};
    unsigned long long End() const noexcept {return End_;};
    // const unsigned long long * CntPrimesWithNumOfSetBits = &CntPrimesWithOneAtPosition_;
    unsigned long long CntPrimesWithOneAtPosition[C_Bits_ + 1]; 
    void SaveFile() const;
    std::vector<std::string> WebPost_SetFields() const;
    std::string WebPost_SerializeCntPrimesWithOneAtPosition_() const;
    
    int GenFunct(const T & N, const mpz_t & mpz_X) override;
    virtual std::string toName() const noexcept override  {return "Bit Statistics";}
    
    virtual GeneratorFunctionBitStatistics<T>& operator+=(const GeneratorFunctionAbstract<T>& rhs) noexcept override;

    unsigned long long CntPrimesWithNumOfSetBits(unsigned int index) const {
        assert(index <= C_Bits_ + 1);
        return CntPrimesWithNumOfSetBits_[index];
    }
    
};




template <class T>
void GeneratorFunctionBitStatistics<T>::Constructor() noexcept {
    mpz_init2(mpz_X_, 64); // ToDo 128
    Begin_ = static_cast<unsigned long long>(1) << iSliceBitIndex_;
    End_ = Begin_ + (Begin_ - 1);

    for(unsigned int n=0; n<65; n++)
    {
        CntPrimesWithNumOfSetBits_[n]=0;
        CntPrimesWithOneAtPosition_[n]=0;
    }
}

template <class T>
GeneratorFunctionBitStatistics<T>::GeneratorFunctionBitStatistics(const unsigned int iSliceBitIndex) noexcept : t_(64), iSliceBitIndex_(iSliceBitIndex)  {
    Constructor();
}

template <class T>
GeneratorFunctionBitStatistics<T>::GeneratorFunctionBitStatistics(const GeneratorFunctionBitStatistics<T> & O) noexcept : t_(64), iSliceBitIndex_(O.iSliceBitIndex_) {
    Constructor();
}

template <class T>
GeneratorFunctionBitStatistics<T>::~GeneratorFunctionBitStatistics() {
    mpz_clear(mpz_X_);
}

template <class T>
GeneratorFunctionBitStatistics<T>& GeneratorFunctionBitStatistics<T>::operator+=(const GeneratorFunctionAbstract<T>& rhs) noexcept {
    // if (iSliceBitIndex_ != rhs.iSliceBitIndex_) throw("Operator + assumes summation of equally initialized objects. Fatal error. mission aborted!");
    
    const GeneratorFunctionBitStatistics<T> * rhsDerived = dynamic_cast<const GeneratorFunctionBitStatistics<T>*>(&rhs);

    GeneratorFunctionAbstract<T>::PrimesCnt_ += rhsDerived->PrimesCnt_;
    GeneratorFunctionAbstract<T>::ProbablePrimesCnt_ += rhsDerived->ProbablePrimesCnt_;   

    for(unsigned int n=0; n<65; n++)
    {
        CntPrimesWithNumOfSetBits_[n]+=rhsDerived->CntPrimesWithNumOfSetBits_[n];
        CntPrimesWithOneAtPosition_[n]+=rhsDerived->CntPrimesWithOneAtPosition_[n];
    }
    return *this;
}

template <class T>
void GeneratorFunctionBitStatistics<T>::SaveFileContent(std::ofstream &myfile, bool WithHeader) const {
    if (WithHeader) myfile  << "Power" << " " << "BitIndex" << " "<< "CntPrimesWithNumOfSetBits" << " "<< "CntPrimesWithOneAtPosition" <<endl;
    for(unsigned long long o=0; o<=C_Bits_; o++)
        {
            myfile << iSliceBitIndex_ << " " << o <<" "<< CntPrimesWithNumOfSetBits_[o] << " " << CntPrimesWithOneAtPosition_[o] <<" "<< endl;
        }
}

template <class T>
void GeneratorFunctionBitStatistics<T>::SaveFile() const {
    // RAII not neccessary - close is called when an object goes out of scope automatically
    std::ofstream myfile;
    myfile.open("PrimeBitStatistics_" + std::to_string(iSliceBitIndex_) + ".txt");
    SaveFileContent(myfile);
    myfile.close();

    std::ofstream myfileAll;
    myfileAll.open("PrimeBitStatistics.txt", std::ios_base::app);
    SaveFileContent(myfileAll, false);
    myfileAll.close();
}

template <class T>
std::string GeneratorFunctionBitStatistics<T>::WebPost_SerializeCntPrimesWithOneAtPosition_() const{
    std::string res;
    for (size_t i = 0; i<=C_Bits_; i++){
        res.append("," + std::to_string(CntPrimesWithNumOfSetBits_[i]));
    }
    return res;
}

template <class T>
std::vector<std::string> GeneratorFunctionBitStatistics<T>::WebPost_SetFields() const{
    std::vector<std::string> res;
    res.reserve(C_Bits_+1);
    for (size_t i = 0; i<=C_Bits_; i++){
        res.emplace_back(std::string("cnt_") + std::to_string(i) + "=" + std::to_string(CntPrimesWithNumOfSetBits_[i]));
    }
    return res;
}


// #include "Log.hpp"
template <class T>
int GeneratorFunctionBitStatistics<T>::GenFunct(const T & X, const mpz_t & mpz_X) {

    T Prime = (T) X;
    if (X % 29 == 0 || X % 31 == 0 || X % 37 == 0 || X % 41 == 0 || X % 43 == 0 || X % 47 == 0 || X % 53 == 0 || X % 59 == 0 ) return 0;


    if (X % 61 == 0 || X % 67 == 0 || X % 71 == 0 || X % 73 == 0 || X % 79 == 0  ) return 0;
    // 296.8s
    
    // if (X % 61 == 0 || X % 67 == 0 || X % 71 == 0 || X % 73 == 0 || X % 79 == 0 || X % 83 == 0 || X % 89 == 0 || X % 97 == 0 ) return 0;
    //297s

    // if (X % 101 == 0 || X % 103 == 0 || X % 107 == 0 || X % 109 == 0 || X % 113 == 0 || X % 127 == 0 || X % 131 == 0 || X % 137 == 0 ) return 0;
    // if (X % 139 == 0 || X % 149 == 0 || X % 151 == 0 || X % 157 == 0 || X % 163 == 0 || X % 167 == 0 || X % 173 == 0 || X % 179 == 0 ) return 0;
    // if (X % 181 == 0 || X % 191 == 0 || X % 193 == 0 || X % 197 == 0 || X % 199 == 0 || X % 211 == 0 || X % 223 == 0 || X % 227 == 0 ) return 0;
    // if (X % 229 == 0 || X % 233 == 0 || X % 239 == 0 || X % 241 == 0 || X % 251 == 0 || X % 257 == 0 || X % 263 == 0 || X % 269 == 0 ) return 0;


    // todo freebsd prime
    // https://math.stackexchange.com/questions/2481148/primality-testing-for-64-bit-numbers

    // power 50: 343s, 107 %
    // if (mpz_bpsw_prp(const_cast<mpz_t&>(mpz_X))>0){
    
    // power 50 : 309s, 96.4%s
    if(t_.IsPrimeBPSW(mpz_X)) {
        // if (gPush) {
        //     gT1.push_back(X);
        // } else {
        //     gT2.push_back(X);
        // }


        GeneratorFunctionAbstract<T>::PrimesCnt_++;

        // T Prime = (T) X;
        // Prime = (T) X;
        int CntOfOnes=-1;  // correction in order to count only inner bits, the highest bit defining interval being tested will increase it to 0
        for(T j=0; j<=C_Bits_; j++) {
            T Bit=Prime & 1;
            if(Bit==1)
            {
                // it is not needed for Prime17
                // CntPrimesWithOneAtPosition_[j]++;
                CntOfOnes++;
            }
            //std::cout << " Prime: " << Prime << "  " << " Bit: "<< Bit << endl;
            Prime >>= 1;
        }
        CntPrimesWithNumOfSetBits_[CntOfOnes]++; 
    }                     

    return 0;
}


#endif