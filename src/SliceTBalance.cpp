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
    myfile_.open("SliceTBalance.txt");
    myfile_ << "N" << " " << "j" << " " << "CentreOfMassP" << " " << "CentreOfMass" << " " <<  "AvgP" << " " << "Avg";
    myfile_ << " " << "Primes" << " " << "PrimesP"; 

    for (unsigned int j=0; j<C_HistItems; j++){
        myfile_ << " " << "HP" << (signed int) j -  (signed int) (C_HistItems/2);
    }
    myfile_ << endl; 
    myfile_.flush();
}

SliceTBalance::~SliceTBalance(){
    myfile_.close();
}

// N = 2 => interval = <4,5,6,7>, only two lower bits (0 and 1) are required to be checked  
void SliceTBalance::CompInterval(const unsigned int N){
    dAvg_.assign(N+1, 0.0);
    dAvgP_.assign(N+1, 0.0);

    dCentreOfMass_.assign(N+1, 0.0);
    dCentreOfMassP_.assign(N+1, 0.0);
    lCnt_.assign(N+1, 0.0);
    
    // initialize histogram matrix with 0

    // free old arrays
    HistCnt_ = vector<vector<unsigned long long>>();   
    HistP_ = vector<vector<long double>>();
    for(unsigned int i=0; i<=N; i++) {
        HistP_.push_back({});
        HistCnt_.push_back({});
        for(unsigned int j=0; j<C_HistItems; j++) {
            HistP_[i].push_back(0.0);
            HistCnt_[i].push_back(0);
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
        lCnt_[iSlice]++;
        
        signed long long diff = (signed long long) i - (signed long long) Half;
        dAvg_[iSlice] +=  (long double) i;
        dAvgP_[iSlice] +=  (long double) i / MaxLength;
        
        dCentreOfMass_[iSlice] += (long double) diff;
        dCentreOfMassP_[iSlice] += diff / MaxLength;

        unsigned long long iHist = i >> RShifts;
        HistCnt_[iSlice][iHist]++;        

        i++; // skip even numbers
    }

    // divide by counts
    for (unsigned int i = 1; i<=N; i++){
        dCentreOfMass_[i]/=lCnt_[i];
        dCentreOfMassP_[i]/=lCnt_[i];
        dAvg_[i]/=lCnt_[i];
        dAvgP_[i]/=lCnt_[i];

        for (unsigned int j=0; j<C_HistItems; j++){
            HistP_[i][j]= (long double) (100.0 * (long double)HistCnt_[i][j]) / (long double)lCnt_[i];
            std::cout << HistCnt_[i][j] << " " ;
        }
        std::cout << "              ";

        for (unsigned int j=0; j<C_HistItems; j++){
            std::cout << HistP_[i][j] << " " ;
        }
        std::cout << endl;

    }

    for (unsigned int i = 0; i<=N; i++){
        // std::cout << i << endl;
        std::cout << N << " " << i << " " <<dCentreOfMassP_[i] << " " << dCentreOfMass_[i] << " " <<  dAvgP_[i] << " " << dAvg_[i] ;
        myfile_ << N << " " << i << " " <<dCentreOfMassP_[i] << " " << dCentreOfMass_[i] << " " <<  dAvgP_[i] << " " << dAvg_[i] ;

        for (unsigned int j=0; j<C_HistItems; j++){
            myfile_ << " " << HistP_[i][j];
        }
        std::cout << endl;
        myfile_ << endl;

    }
    myfile_.flush();

    // VectorToFile(dCentreOfMassP_);
    // myfile_ << endl;
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

    dAvg_.assign(N+1, 0.0);
    dAvgP_.assign(N+1, 0.0);

    dCentreOfMass_.assign(N+1, 0.0);
    dCentreOfMassP_.assign(N+1, 0.0);
   
    lCntPrimes_.assign(N+1,0);
    lCnt_.assign(N+1, 0.0);
    
    // initialize histogram matrix with 0
    HistCnt_ = vector<vector<unsigned long long>>();   
    HistP_ = vector<vector<long double>>();
    for(unsigned int i=0; i<=N; i++) {
        HistP_.push_back({});
        HistCnt_.push_back({});
        for(unsigned int j=0; j<C_HistItems; j++) {
            HistP_[i].push_back(0.0);
            HistCnt_[i].push_back(0);
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
        lCnt_[iSlice]++;

        // TTN,i=TN,i + TN,N-i+1 pro i=1..N
        // T4,1={17}
        // T4,2={19,21,25}
        // T4,3={23,27,29} 
        // T4,4={31} 

        // primality check and if so increase count of primes in a slice        
        utils_mpz::mpz_set_ull(X, B+i);
        if (tests.IsPrimeBPSW(X)){
            lCntPrimes_[iSlice]++;
        };
        
        signed long long diff = (signed long long) i - (signed long long) Half;
        dAvg_[iSlice] +=  (long double) i;
        dAvgP_[iSlice] +=  (long double) i / MaxLength;
        
        dCentreOfMass_[iSlice] += (long double) diff;
        dCentreOfMassP_[iSlice] += (long double) diff / (long double) MaxLength;

        // compute histogram
        unsigned long long iHist = i >> RShifts;
        HistCnt_[iSlice][iHist]++;        

        i++; // skip even numbers
    }

    // total number of primes in interval as a sum from slices
    unsigned long long PrimesTotal = 0;
    for (auto i: lCntPrimes_) {
        PrimesTotal +=i;
    }


    // divide by counts
    for (unsigned int i = 0; i<=N; i++){
        dCentreOfMass_[i]/=lCnt_[i];
        dCentreOfMassP_[i]/=lCnt_[i];
        dAvg_[i]/=lCnt_[i];
        dAvgP_[i]/=lCnt_[i];
        
        for (unsigned int j=0; j<C_HistItems; j++){
            HistP_[i][j]= (long double) (100.0 * (long double) HistCnt_[i][j]) / (long double)lCnt_[i];
            std::cout << HistCnt_[i][j] << " " ;
        }
        std::cout << "              ";

        for (unsigned int j=0; j<C_HistItems; j++){
            std::cout << HistP_[i][j] << " " ;
        }
        std::cout << endl;

    }

    for (unsigned int i = 0; i<=N; i++){
        // std::cout << i << endl;
        std::cout << N << " " << i << " " <<dCentreOfMassP_[i] << " " << dCentreOfMass_[i] << " " <<  dAvgP_[i] << " " << dAvg_[i] ;
        std::cout << " " << lCntPrimes_[i] << " " << (100.0l) *  (long double) lCntPrimes_[i] / (long double) PrimesTotal; 

        myfile_ << N << " " << i << " " <<dCentreOfMassP_[i] << " " << dCentreOfMass_[i] << " " <<  dAvgP_[i] << " " << dAvg_[i] ;
        myfile_ << " " << lCntPrimes_[i] << " " << (100.0l) *  (long double) lCntPrimes_[i] / (long double) PrimesTotal; 
        
        for (unsigned int j=0; j<C_HistItems; j++){
            myfile_ << " " << HistP_[i][j];
        }
        std::cout << endl;
        myfile_ << endl;

    }
    myfile_.flush();

    mpz_clear(X);
    
    // VectorToFile(dCentreOfMassP_);
    // myfile_ << endl;
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
    std::chrono::time_point<std::chrono::high_resolution_clock> BeginTime_ = high_resolution_clock::now();
    
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

    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    Log::out() << "Duration sec: " << duration.count() / (1000.0l ) << "\n";;
}


