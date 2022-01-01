#ifndef __SLICETBALANCE_HPP
#define __SLICETBALANCE_HPP

#include <vector>
#include <fstream>
#include <array>

using std::vector;


class SliceTBalance {
    private:
     
    vector<long double> dCentreOfMass_;
    vector<long double> dCentreOfMassP_;
    
    vector<long double> dAvg_;
    vector<long double> dAvgP_;

    vector<unsigned long long> lCnt_;
    vector<unsigned long long> lCntPrimes_;

    const unsigned int C_HistBits = 6;
    const unsigned int C_HistItems= static_cast<uint32_t>(1) << C_HistBits;
  
    vector<vector<long double>> HistP_;
    vector<vector<unsigned long long>> HistCnt_;


    std::ofstream myfile_;

    // template <typename T> void VectorToFile(const vector<T> &Vec);
    // void VectorToFile(const std::vector<long double> &Vec);

    public: 
    SliceTBalance();
    ~SliceTBalance();
    void CompInterval(const unsigned int N);
    void CompIntervalJoin(const unsigned int N, const bool bJoin);

    void CompCorrectionLi(const unsigned int N);
    void CompCorrectionLiWorker(const unsigned int N, uint64_t Beg, uint64_t End, std::array<long double, 65> &res);


};

#endif 