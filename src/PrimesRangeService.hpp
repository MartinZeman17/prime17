#ifndef __PRIMESRANGESERVICE_HPP
#define __PRIMESRANGESERVICE_HPP

#include "PrimesRange.hpp"

#include <memory>
#include <vector>
#include <mutex>

 class PrimesRangeService {
    private:
    static std::vector<std::weak_ptr<PrimesRange>> p_Obj;
    static std::mutex p_Obj_mutex;

    static std::shared_ptr<PrimesRange> FindExisting(const unsigned long long Begin, 
                                                     const unsigned long long End);


    public: 
    static std::shared_ptr<PrimesRange> GetPrimesRange(const unsigned long long Begin, 
                                                       const unsigned long long End);
};


#endif

// Performance
// Time ms:
// Debug, C type array in extra pointer Primes32[i]: 405
// Debug, Access to array via class method Q->PrimesArray32()[i]: 2808
// No debug, Optimized -O3, both approaches: 57 


        // std::shared_ptr<PrimesRange> P = PrimesRangeService::GetPrimesRange(0,UINT32_MAX);
        // unsigned long long PrC = P->PrimesCount;
        // uint32_t * Primes;
        // Primes32 = P->PrimesArray32();
        // unsigned long long Sum=0;
        // auto Begin = high_resolution_clock::now();
        // for (unsigned long long i = 0 ; i < PrC; i++){
        //     Sum += Primes32[i];
        // }
        // auto End = high_resolution_clock::now();
        // auto duration = duration_cast<milliseconds>(End - Begin);
        // cout << duration.count() << endl;

        // Primes32 = nullptr;
        // cout << Sum << endl;

        // Sum = 0;
        // std::shared_ptr<PrimesRange> Q = PrimesRangeService::GetPrimesRange(0,UINT32_MAX);
        // Begin = high_resolution_clock::now();
        // for (unsigned long long i = 0 ; i<Q->PrimesCount; i++){
        //     // cout << Q->PrimesArray32()[i] << " " ;
        //     Sum +=  Q->PrimesArray32()[i];
        // }
        // End = high_resolution_clock::now();
        // duration = duration_cast<milliseconds>(End - Begin);
        // cout << duration.count() << endl;
        // cout << Sum << endl;
