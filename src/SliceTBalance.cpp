#include "SliceTBalance.hpp"

#include <stdint.h>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cmath>    
#include <chrono>

#include "PrimeTest.hpp"
#include "utils_mpz.hpp"
#include "Li.hpp"
#include "Log.hpp"
#include "utils_str.hpp"
#include "VectorToFile.hpp"

using namespace std::chrono;
using std::endl;
#define assertm(exp, msg) assert(((void)msg, exp));



namespace {
    // N=0..64
    // N=0 => 0 is returned
    // N=2, perform two bit shifts
    unsigned int Bits(unsigned long long X, const unsigned int N){
        unsigned int ret = 0;
        const unsigned long long C_IsOdd = 1;
        for (unsigned int i = 0 ; i<N; i++){
            if (C_IsOdd & X) ret++ ;
            X >>=1;
        }
        return ret;
    } 
}


SliceTBalance::SliceTBalance(){
    _myfile.open("SliceTBalance.txt");
    _myfile << "N" << " " << "j" << " " << "CentreOfMassP" << " " << "CentreOfMass" << " " <<  "AvgP" << " " << "Avg";
    _myfile << " " << "Primes" << " " << "PrimesP"; 

    for (unsigned int j=0; j<C_HistItems; j++){
        _myfile << " " << "HP" << (signed int) j -  (signed int) (C_HistItems/2);
    }
    _myfile << endl; 
    _myfile.flush();
}

SliceTBalance::~SliceTBalance(){
    _myfile.close();
}

// N = 2 => interval = <4,5,6,7>, only two lower bits (0 and 1) are required to be checked  
void SliceTBalance::CompInterval(const unsigned int N){
    _dAvg.assign(N+1, 0.0);
    _dAvgP.assign(N+1, 0.0);

    _dCentreOfMass.assign(N+1, 0.0);
    _dCentreOfMassP.assign(N+1, 0.0);
    _lCnt.assign(N+1, 0.0);
    
    // initialize histogram matrix with 0

    // free old arrays
    _HistCnt = vector<vector<unsigned long long>>();   
    _HistP = vector<vector<long double>>();
    for(unsigned int i=0; i<=N; i++) {
        _HistP.push_back({});
        _HistCnt.push_back({});
        for(unsigned int j=0; j<C_HistItems; j++) {
            _HistP[i].push_back(0.0);
            _HistCnt[i].push_back(0);
        }
    }

    assertm(N>=1, "N must be bigger than 1");
    assertm(N<=63, "N must be smaller or equal than 63");
    unsigned long long Half = static_cast<uint64_t>(1) << (N-1);
    unsigned long long B = static_cast<uint64_t>(1) << N;

    // abs value of max odd number in an interval when counted from the middle of the ionterval
    // odd numbers are 5,7 its center is 6
    // when shifted it is 1, 3 and its center is 2
    long double MaxLength = Half-1; 

    int RShifts = N-C_HistBits;
    if (RShifts<0) RShifts = 0; 

    for(unsigned long long i=0; i<B; ){
        i++; //move to odd number
        unsigned int iSlice = Bits(i, N); //0..N
        _lCnt[iSlice]++;
        
        signed long long diff = (signed long long) i - (signed long long) Half;
        _dAvg[iSlice] +=  (long double) i;
        _dAvgP[iSlice] +=  (long double) i / MaxLength;
        
        _dCentreOfMass[iSlice] += (long double) diff;
        _dCentreOfMassP[iSlice] += diff / MaxLength;

        unsigned long long iHist = i >> RShifts;
        _HistCnt[iSlice][iHist]++;        

        i++; // skip even numbers
    }

    // divide by counts
    for (unsigned int i = 1; i<=N; i++){
        _dCentreOfMass[i]/=_lCnt[i];
        _dCentreOfMassP[i]/=_lCnt[i];
        _dAvg[i]/=_lCnt[i];
        _dAvgP[i]/=_lCnt[i];

        for (unsigned int j=0; j<C_HistItems; j++){
            _HistP[i][j]= (long double) (100.0 * (long double)_HistCnt[i][j]) / (long double)_lCnt[i];
            std::cout << _HistCnt[i][j] << " " ;
        }
        std::cout << "              ";

        for (unsigned int j=0; j<C_HistItems; j++){
            std::cout << _HistP[i][j] << " " ;
        }
        std::cout << endl;

    }

    for (unsigned int i = 0; i<=N; i++){
        // std::cout << i << endl;
        std::cout << N << " " << i << " " <<_dCentreOfMassP[i] << " " << _dCentreOfMass[i] << " " <<  _dAvgP[i] << " " << _dAvg[i] ;
        _myfile << N << " " << i << " " <<_dCentreOfMassP[i] << " " << _dCentreOfMass[i] << " " <<  _dAvgP[i] << " " << _dAvg[i] ;

        for (unsigned int j=0; j<C_HistItems; j++){
            _myfile << " " << _HistP[i][j];
        }
        std::cout << endl;
        _myfile << endl;

    }
    _myfile.flush();

    // VectorToFile(_dCentreOfMassP);
    // _myfile << endl;
    // std::cout << endl;

}

// N = 2 => interval = <4,5,6,7>, only two lower bits (0 and 1) needed to be checked  
void SliceTBalance::CompIntervalJoin(const unsigned int N, const bool  bJoin) {
    //  there is division by 0 for N=1
    assertm(N>=1, "N must be bigger than 1");
    assertm(N<=63, "N must be smaller or equal than 63");


    PrimeTest tests(64);
    mpz_t X;
    mpz_init2(X, 64);

    _dAvg.assign(N+1, 0.0);
    _dAvgP.assign(N+1, 0.0);

    _dCentreOfMass.assign(N+1, 0.0);
    _dCentreOfMassP.assign(N+1, 0.0);
   
    _lCntPrimes.assign(N+1,0);
    _lCnt.assign(N+1, 0.0);
    
    // initialize histogram matrix with 0
    _HistCnt = vector<vector<unsigned long long>>();   
    _HistP = vector<vector<long double>>();
    for(unsigned int i=0; i<=N; i++) {
        _HistP.push_back({});
        _HistCnt.push_back({});
        for(unsigned int j=0; j<C_HistItems; j++) {
            _HistP[i].push_back(0.0);
            _HistCnt[i].push_back(0);
        }
    }

    unsigned long long Half = static_cast<uint64_t>(1) << (N-1);
    unsigned long long B = static_cast<uint64_t>(1) << N;

    // abs value of max odd number in an interval when counted from the middle of the interval
    // odd numbers are 5,7 its center is 6 = 4 + 2 = 2^2 + 2^1 = 2^N + 2^(N-1)
    // when shifted it is 1, 3 and its center is 2. It is ok to substract 2^N
    long double MaxLength = Half-1; 

    int RShifts = N-C_HistBits;
    if (RShifts<0) RShifts = 0; 

    for(unsigned long long i=0; i<B; ){
        i++; //move to odd number

        // compute slice index
        unsigned int iSlice = Bits(i, N); //0..N

        // i belongs to T_Nj, 
        // is =  i symetrical around centre
        // is belongs to T_Nk
        // j + k = N+1
        // if (bJoin) {
        //     unsigned int InnerBits=0;
        //     // unsigned long long is=0;
        //     if (i>Half){
        //         // is = Half - (i-Half);
        //         // iSlice = InnerBits + 2
        //         InnerBits = iSlice -2; // one bit for odd nmber, another bit for for exceeding Half
        //     } else
        //     {
        //         // is = Half + (Half-i);
        //         // iSlice=N-1-InnerBits
        //         InnerBits = N-1-iSlice; 
        //     }
        //     // cout << i << " " << iSlice << " " << Bits(is, N) << " " << iSlice + Bits(is, N) <<endl;
        // }


        if (bJoin) {
            if (iSlice > N/2 ) {
                iSlice = N-iSlice+1;
            }
        }
        _lCnt[iSlice]++;

        // TTN,i=TN,i + TN,N-i+1 pro i=1..N
        // T4,1={17}
        // T4,2={19,21,25}
        // T4,3={23,27,29} 
        // T4,4={31} 

        // primality check and if so increase count of primes in a slice        
        utils_mpz::mpz_set_ull(X, B+i);
        if (tests.IsPrimeBPSW(X)){
            _lCntPrimes[iSlice]++;
        };
        
        signed long long diff = (signed long long) i - (signed long long) Half;
        _dAvg[iSlice] +=  (long double) i;
        _dAvgP[iSlice] +=  (long double) i / MaxLength;
        
        _dCentreOfMass[iSlice] += (long double) diff;
        _dCentreOfMassP[iSlice] += (long double) diff / (long double) MaxLength;

        // compute histogram
        unsigned long long iHist = i >> RShifts;
        _HistCnt[iSlice][iHist]++;        

        i++; // skip even numbers
    }

    // total number of primes in interval as a sum from slices
    unsigned long long PrimesTotal = 0;
    for (auto i: _lCntPrimes) {
        PrimesTotal +=i;
    }


    // divide by counts
    for (unsigned int i = 0; i<=N; i++){
        _dCentreOfMass[i]/=_lCnt[i];
        _dCentreOfMassP[i]/=_lCnt[i];
        _dAvg[i]/=_lCnt[i];
        _dAvgP[i]/=_lCnt[i];
        
        for (unsigned int j=0; j<C_HistItems; j++){
            _HistP[i][j]= (long double) (100.0 * (long double) _HistCnt[i][j]) / (long double)_lCnt[i];
            std::cout << _HistCnt[i][j] << " " ;
        }
        std::cout << "              ";

        for (unsigned int j=0; j<C_HistItems; j++){
            std::cout << _HistP[i][j] << " " ;
        }
        std::cout << endl;

    }

    for (unsigned int i = 0; i<=N; i++){
        // std::cout << i << endl;
        std::cout << N << " " << i << " " <<_dCentreOfMassP[i] << " " << _dCentreOfMass[i] << " " <<  _dAvgP[i] << " " << _dAvg[i] ;
        std::cout << " " << _lCntPrimes[i] << " " << (100.0l) *  (long double) _lCntPrimes[i] / (long double) PrimesTotal; 

        _myfile << N << " " << i << " " <<_dCentreOfMassP[i] << " " << _dCentreOfMass[i] << " " <<  _dAvgP[i] << " " << _dAvg[i] ;
        _myfile << " " << _lCntPrimes[i] << " " << (100.0l) *  (long double) _lCntPrimes[i] / (long double) PrimesTotal; 
        
        for (unsigned int j=0; j<C_HistItems; j++){
            _myfile << " " << _HistP[i][j];
        }
        std::cout << endl;
        _myfile << endl;

    }
    _myfile.flush();

    mpz_clear(X);
    
    // VectorToFile(_dCentreOfMassP);
    // _myfile << endl;
    // std::cout << endl;

}



/*
I3 1 000 .. 1 111
Beg = 9
End = 8+7 = 15 
9,11,13,15
9:   8 - 10
11: 10 - 12
13: 12 - 14
*/
        // N=0..64
        // N=0 => 0 is returned
        // N=2, perform two bit shifts
void  SliceTBalance::CompCorrectionLi(const unsigned int N){
    uint64_t Beg = static_cast<uint64_t>(1) << N;
    uint64_t End = Beg + (Beg-1);

    std::array<long double, 65> res {{
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0   }};

    CompCorrectionLiWorker(N, Beg, End, res);

    std::ofstream myfile;
    myfile.open("LiCorrectionNew_" + std::to_string(N) + ".txt");

    long double Sum=0;
    for (auto i : res){
        Sum+=i;
    }

    for (auto i : res){
        // Log::out() << i << " " << (100.0L* i/Sum) << "\n";
        myfile << utils_str::FormatNumber(i, 60, 20) << " " << (100.0L* i/Sum) << endl;

    }
    Log::out() << "\n";
    myfile << endl;
    myfile.close();
}

void  SliceTBalance::CompCorrectionLiWorker(const unsigned int N, uint64_t Beg, uint64_t End, std::array<long double, 65> &res){
    std::chrono::time_point<std::chrono::high_resolution_clock> _BeginTime = high_resolution_clock::now();
    
    //Beg must be even
    if (Beg&1) Beg--;

    //End must be odd    
    if (!(End & 1)) End++;

    long double X=Beg;
    // long double iB = X / std::log(X);    
    long double iB = Li::li(X);

    Beg++;  //move to the first odd number

    for (uint64_t i = Beg; i <= End; ){
        unsigned int iBits = Bits(i, N);
        X = (long double) i + 1.0L;
        // long double iE = X / std::log(X);
        long double iE = Li::li(X);
        
        res[iBits] += iE - iB;
        iB = iE;

        i+=2;
    }

    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    Log::out() << "Duration sec: " << duration.count() / (1000.0l ) << "\n";;
}


