#ifndef __SLICETBALANCE_HPP
#define __SLICETBALANCE_HPP

#include <vector>
#include <fstream>

using std::vector;


class SliceTBalance {
    private:
     
    vector<long double> _dCentreOfMass;
    vector<long double> _dCentreOfMassP;
    
    vector<long double> _dAvg;
    vector<long double> _dAvgP;

    vector<unsigned long long> _lCnt;
    vector<unsigned long long> _lCntPrimes;

    const unsigned int C_HistBits = 6;
    const unsigned int C_HistItems= static_cast<uint64_t>(1) << C_HistBits;
  
    vector<vector<long double>> _HistP;
    vector<vector<unsigned long long>> _HistCnt;


    std::ofstream _myfile;

    // template <typename T> void VectorToFile(const vector<T> &Vec);
    // void VectorToFile(const std::vector<long double> &Vec);

    public: 
    SliceTBalance();
    ~SliceTBalance();
    void CompInterval(const unsigned int N);
    void CompIntervalJoin(const unsigned int N, const bool bJoin);

};

#endif 