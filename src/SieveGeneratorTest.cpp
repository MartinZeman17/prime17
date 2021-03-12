#include <iostream>
#include <memory>
#include <fstream>

#include <cassert>
#define assertm(exp, msg) assert(((void)msg, exp));


#include "SieveGenerator.hpp"
#include "GeneratorFunctionBitStatistics.hpp"
#include "PrimesRange.hpp"
#include "PrimesRangeService.hpp"


using std::cout;

void SieveGeneratorTest(){
    const SieveGenerator<uint128_t> A(C_SieveGeneratorDefaultMaxPrime);
    // GeneratorFunctionBitStatistics GF(nullptr);
    GeneratorFunctionBitStatistics GF;

    uint128_t Begin = UINT64_MAX; // UINT64_MAX + 1 is evaluated at compile time as 0 !!!  
    Begin++;
    uint128_t End = Begin + 1000000000UL;

    A.Work(Begin, End, GF);

}

/* Works fine up to 31 (computation time is several hours). For higher primorials some form of optimization is needed.*/
void EratosthenesEffectivity(){

    cout <<"EratosthenesEffectivity"<<endl;

    uint32_t *Primes=nullptr;
    std::shared_ptr<PrimesRange> P = PrimesRangeService::GetPrimesRange(2,101);
    unsigned long long PrimesCount = P->PrimesCount;
    Primes = P->PrimesArray32();
    
    mpz_t mpz_primor;
    mpz_t mpz_res;
    mpz_init2(mpz_primor,64);
    mpz_init2(mpz_res,64);

    std::ofstream myfile;
    myfile.open("EratosthenesEffectivity.txt");
    myfile  << "Prime" << " " << "Primorial" << " "<< "Coprimes" << " "<< "Effectivity" <<endl;
  
    mpz_t mpz_Max;
    mpz_init(mpz_Max);
    utils_mpz::mpz_set_ull(mpz_Max, UINT64_MAX);

    for (unsigned long long idx=0; idx<PrimesCount; idx++) {
        mpz_primorial_ui(mpz_primor, Primes[idx]);  //ok
        if (mpz_cmp(mpz_primor, mpz_Max) > 0 ){
            cout << "Cannot proceed for primorial exceding 64 bits. Fatal, mission aborted." << endl;
            break;
        }        
        unsigned long long Primorial = utils_mpz::mpz_get<unsigned long long>(mpz_primor);
        assertm(Primorial<=UINT32_MAX, "Porting limitation to 32 bits");
        //count coprimes to primorial
        unsigned long long TACount=0;
        for (uint32_t i = 1; i<Primorial;){
            mpz_gcd_ui(mpz_res, mpz_primor, i);  //ok, i fits into 32 bits
            if(mpz_cmp_ui(mpz_res,1)==0){   //ok
                TACount++;
            }
            i+=2;
        }
        long double effectivity = 100.0L * (long double) TACount / (long double) Primorial;
        cout << Primes[idx] << " " << Primorial  << " "<< TACount << " " << effectivity <<endl;
        myfile << Primes[idx] << " " << Primorial  << " "<< TACount << " " << effectivity <<endl;
        myfile.flush();
    }
    myfile.close();
    mpz_clear(mpz_Max);
    mpz_clear(mpz_primor);
    mpz_clear(mpz_res);
}