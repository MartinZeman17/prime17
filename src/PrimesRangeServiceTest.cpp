#include <iostream>
#include <fstream>
#include <math.h>

#include <gmp.h>
#include <stdint.h>

#include <filesystem>

#include "Li.hpp"
#include "PrimesRangeService.hpp"

using namespace std::chrono;
using std::endl;
using std::cout;



void PrimesRangeServiceTest() {

    uint32_t *Primes32=nullptr;
    unsigned long long PrimesCount32 = 0;

    uint64_t *Primes64=nullptr;
    unsigned long long PrimesCount64 = 0;


    cout <<"PrimesRangeServiceTest"<<endl;
    {
        std::shared_ptr<PrimesRange> P = PrimesRangeService::GetPrimesRange(0,UINT32_MAX);
        PrimesCount32 = P->PrimesCount;
        if (PrimesCount32 != 203280221) {
            const std::string C_Msg("Mission aborted. Number of 32 bit primes must be 203 280 221, but computation yielded: " + std::to_string(PrimesCount32));
            std::cout << C_Msg << std::endl;
            throw(C_Msg);
        }
        Primes32 = P->PrimesArray32();
        unsigned long long Sum=0;
        auto Begin = high_resolution_clock::now();
        for (unsigned long long i = 0 ; i < PrimesCount32; i++){
            Sum += Primes32[i];
        }
        auto End = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(End - Begin);
        cout << duration.count() << endl;       

        cout << Sum << endl;

        Sum = 0;
        std::shared_ptr<PrimesRange> Q = PrimesRangeService::GetPrimesRange(0,UINT32_MAX);
        Begin = high_resolution_clock::now();
        for (unsigned long long i = 0 ; i<Q->PrimesCount; i++){
            Sum +=  Q->PrimesArray32()[i];
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        cout << duration.count() << endl;
        cout << Sum << endl;
        cout << endl;

        Sum = 0;
        std::shared_ptr<PrimesRange> W = PrimesRangeService::GetPrimesRange(UINT32_MAX,(uint64_t) UINT32_MAX + 1000000);
        Primes64 = W->PrimesArray64();
        PrimesCount64 = W->PrimesCount;
        if (PrimesCount64 != 45038) {
            const std::string C_Msg("Mission aborted. Number of smallest 64 bit primes in first 1 mio of 64 bit numbers 45 038, but computation yielded: " + std::to_string(PrimesCount64));
            std::cout << C_Msg << std::endl;
            throw(C_Msg);
        }

        Begin = high_resolution_clock::now();
        for (unsigned long long i = 0 ; i<PrimesCount64; i++){
            Sum +=  Primes64[i];
        }
        End = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(End - Begin);
        cout << duration.count() << endl;
        cout << Sum << endl;
        cout << endl;
    }
    
    cout <<"end"<<endl; 
}