#include "GeneratorFunctionBitStatistics.hpp"
#include "utils_mpz.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>

#include <gmp.h>
extern "C" {
#include "mpz_aprcl.h"
}


using std::endl;
using std::cout;


void GeneratorFunctionBitStatistics::Constructor() noexcept {
    mpz_init2(_mpz_X, 64);
    _Begin = static_cast<unsigned long long>(1) << _iSliceBitIndex;
    _End = _Begin + (_Begin - 1);

    for(unsigned int n=0; n<65; n++)
    {
        _CntPrimesWithNumOfSetBits[n]=0;
        _CntPrimesWithOneAtPosition[n]=0;
    }
}

GeneratorFunctionBitStatistics::GeneratorFunctionBitStatistics(const unsigned int iSliceBitIndex) noexcept : _t(64), _iSliceBitIndex(iSliceBitIndex)  {
    Constructor();
}

GeneratorFunctionBitStatistics::GeneratorFunctionBitStatistics(const GeneratorFunctionBitStatistics & O) noexcept :_t(64), _iSliceBitIndex(O._iSliceBitIndex) {
    Constructor();
}

GeneratorFunctionBitStatistics::~GeneratorFunctionBitStatistics() {
    mpz_clear(_mpz_X);
}


GeneratorFunctionBitStatistics& GeneratorFunctionBitStatistics::operator+=(const GeneratorFunctionAbstract& rhs) noexcept {
    // if (_iSliceBitIndex != rhs._iSliceBitIndex) throw("Operator + assumes summation of equally initialized objects. Fatal error. mission aborted!");
    
    const GeneratorFunctionBitStatistics* rhsDerived = dynamic_cast<const GeneratorFunctionBitStatistics*>(&rhs);

    _PrimesCnt += rhsDerived->_PrimesCnt;
    _ProbablePrimesCnt += rhsDerived->_ProbablePrimesCnt;   

    for(unsigned int n=0; n<65; n++)
    {
        _CntPrimesWithNumOfSetBits[n]+=rhsDerived->_CntPrimesWithNumOfSetBits[n];
        _CntPrimesWithOneAtPosition[n]+=rhsDerived->_CntPrimesWithOneAtPosition[n];
    }
    return *this;
}

void GeneratorFunctionBitStatistics::SaveFileContent(std::ofstream &myfile, bool WithHeader) const {
    if (WithHeader) myfile  << "Power" << " " << "BitIndex" << " "<< "CntPrimesWithNumOfSetBits" << " "<< "CntPrimesWithOneAtPosition" <<endl;
    for(unsigned long long o=0; o<=C_Bits_; o++)
        {
            myfile << _iSliceBitIndex << " " << o <<" "<< _CntPrimesWithNumOfSetBits[o] << " " << _CntPrimesWithOneAtPosition[o] <<" "<< endl;
        }
}

void GeneratorFunctionBitStatistics::SaveFile() const {
    // RAII not neccessary - close is called when an object goes out of scope automatically
    std::ofstream myfile;
    myfile.open("PrimeBitStatistics_" + std::to_string(_iSliceBitIndex) + ".txt");
    SaveFileContent(myfile);
    myfile.close();

    std::ofstream myfileAll;
    myfileAll.open("PrimeBitStatistics.txt", std::ios_base::app);
    SaveFileContent(myfileAll, false);
    myfileAll.close();
}


std::string GeneratorFunctionBitStatistics::WebPost_Serialize_CntPrimesWithOneAtPosition() const{
    std::string res;
    for (size_t i = 0; i<=C_Bits_; i++){
        res.append("," + std::to_string(_CntPrimesWithNumOfSetBits[i]));
    }
    return res;
}


std::vector<std::string> GeneratorFunctionBitStatistics::WebPost_SetFields() const{
    std::vector<std::string> res;
    res.reserve(C_Bits_+1);
    for (size_t i = 0; i<=C_Bits_; i++){
        res.emplace_back(std::string("cnt_") + std::to_string(i) + "=" + std::to_string(_CntPrimesWithNumOfSetBits[i]));
    }
    return res;
}



int GeneratorFunctionBitStatistics::GenFunct(const uint128_t & X, const mpz_t & mpz_X) {
    // TODO only 64 bit numbers supported so far, be careful with the input.
    assert(X <= UINT64_MAX);  
    // Utils::mpz_set_ul128(_mpz_X, X); 

    uint64_t Prime = (uint64_t) X;
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
    if(_t.IsPrimeBPSW(mpz_X)) {
        _PrimesCnt++;

        // uint64_t Prime = (uint64_t) X;
        // Prime = (uint64_t) X;
        int CntOfOnes=-1;  // correction in order to count only inner bits, the highest bit defining interval being tested will increase it to 0
        for(uint64_t j=0; j<=C_Bits_; j++) {
            uint64_t Bit=Prime & 1;
            if(Bit==1)
            {
                // it is not needed for Prime17
                // _CntPrimesWithOneAtPosition[j]++;
                CntOfOnes++;
            }
            //std::cout << " Prime: " << Prime << "  " << " Bit: "<< Bit << endl;
            Prime >>= 1;
        }
        _CntPrimesWithNumOfSetBits[CntOfOnes]++; 
    }                     

    return 0;
}
