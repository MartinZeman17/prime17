#include "GeneratorFunctionBitStatistics.hpp"
#include "utils_mpz.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>

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
    // Utils::mpz_set_ul128(_mpz_X, X);   
    if(_t.IsPrimeBPSW(mpz_X)) {
        _PrimesCnt++;
        // std::cout << X << endl;
        // _pBS->ComputePrimeStats(X); //???

        // only 64 bit numbers supported so far, Be careful with the input.
        unsigned long long Prime = (unsigned long long) X;
        int CntOfOnes=-1;  // correction in order to count only inner bits, the highest bit defining interval being tested will increase it to 0
        for(unsigned long long j=0; j<=C_Bits_; j++) {
            unsigned long long Bit=Prime & 1;
            if(Bit==1)
            {
                _CntPrimesWithOneAtPosition[j]++;
                CntOfOnes++;
            }
            //std::cout << " Prime: " << Prime << "  " << " Bit: "<< Bit << endl;
            Prime >>= 1;
        }
        _CntPrimesWithNumOfSetBits[CntOfOnes]++; // 
    }                     

    return 0;
}
