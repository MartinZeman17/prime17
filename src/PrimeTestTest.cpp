#include "PrimeTest.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>


#include "SieveGenerator.hpp"
#include "GeneratorFunction.hpp"
#include "GeneratorFunctionMR.hpp"
#include "GeneratorFunctionLS.hpp"
#include "GeneratorFunctionLS_D.hpp"
#include "GeneratorFunctionLS_DHistogram.hpp"
#include "GeneratorFunctionBPSW.hpp"
#include "GeneratorFunctionBPSW2.hpp"
#include "GeneratorFunctionAPRCL.hpp"
#include "GeneratorFunctionPrimeCreator.hpp"
#include "VectorToFile.hpp"
#include "Li.hpp"
#include "utils_mpz.hpp"


extern "C" {
#include "mpz_aprcl.h"
}

using std::endl;
using std::cout;


void Histogram(const vector<unsigned long> &H, std::ofstream &myfile){
    unsigned long long Sum=0;
    for (auto i : H) {
        Sum+=i;
    }

    for (auto i : H) {
         long double P = (100.0L) * (long double) i / (long double) Sum;
         cout << std::setprecision(25) << P << " ";
         if (myfile) myfile << std::setprecision(15) << P << " ";
    }
    cout << endl;

    if (myfile) {
        myfile << endl;
        cout << endl;
        VectorToFile(H, myfile);
    }


}


void MethodTestsOnPrime(){
    cout << "------------------ Testing on primes ---------------------" << endl;
    #ifdef NDEBUG
    const unsigned int C_ShiftsForPrimes =26;
    #else
    const unsigned int C_ShiftsForPrimes =16;
    #endif 
    
    const unsigned long long NumberOfPrimes = static_cast<uint64_t>(1) << C_ShiftsForPrimes;

    std::ofstream myfile;
    myfile.open("MeasurementOnPrime_" + std::to_string(C_ShiftsForPrimes) + ".txt");
    cout << "Power of 2 (shifts): " << C_ShiftsForPrimes << endl;
    myfile << "Power of 2 (shifts): " << C_ShiftsForPrimes << endl;
    cout << "Primes tested: " << NumberOfPrimes << endl;
    myfile << "Primes tested: " << NumberOfPrimes << endl;


    uint128_t Begin = UINT64_MAX;
    Begin++;

    const SieveGenerator<uint128_t> Sieve(C_SieveGeneratorDefaultMaxPrime);

    GeneratorFunctionPrimeCreator GF_Creator(NumberOfPrimes);
    Sieve.Work(Begin, prt::numeric_limits<uint128_t>::max(), GF_Creator);
    GeneratorFunctionAbstract &res= GF_Creator;
    cout << "BPSW Primes Created: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    myfile << "BPSW Primes Created: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    

    uint128_t UINT65_max = static_cast<uint128_t>(1) << 65;
    UINT65_max--;
    long double TotalPrimesInInt =  Li::NoPrimesInt(Begin, UINT65_max);
    cout << std::setprecision(25) << "TotalPrimes from 2^64 to 2^65: " << TotalPrimesInInt << endl; 
    myfile << std::setprecision(25) << "TotalPrimes from 2^64 to 2^65: " << TotalPrimesInInt << endl; 


    PrimeTest Tests(65);
    mpz_t mpz_X;
    mpz_init2(mpz_X, 65);
    auto _BeginTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);


    // // _BeginTime = high_resolution_clock::now();
    // // for (auto X : GF_Creator.CreatedPrimes) {
    // //     Tests.iMillerRabinBase2(X);
    // // }
    // // duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    // // cout << "MR 128 on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    // MR
    _BeginTime = high_resolution_clock::now();
    for (auto X : GF_Creator.CreatedPrimes) {
        // Utils::UInt128tompz(X, mpz_X);
        utils_mpz::mpz_set_ul128(mpz_X, X);
        Tests.iMillerRabinBase2(mpz_X);
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    cout << "MR mpz on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    myfile << "MR mpz on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    // LS
    _BeginTime = high_resolution_clock::now();
    for (auto X : GF_Creator.CreatedPrimes) {
        // Utils::UInt128tompz(X, mpz_X);
        utils_mpz::mpz_set_ul128(mpz_X, X);
        Tests.iLucasSelfridge(mpz_X);
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    cout << "LS on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    myfile << "LS on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    // LS D
    _BeginTime = high_resolution_clock::now();
    for (auto X : GF_Creator.CreatedPrimes) {
        // Utils::UInt128tompz(X, mpz_X);
        utils_mpz::mpz_set_ul128(mpz_X, X);
        Tests.iLucasSelfridge_D(mpz_X);
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    cout << "LS coef D on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    myfile << "LS coef D on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    // // LS Histogram
    // _BeginTime = high_resolution_clock::now();
    // for (auto X : GF_Creator.CreatedPrimes) {
    //     Utils::UInt128tompz(X, mpz_X);
    //     Tests.iLucasSelfridge_DHistogram(mpz_X);
    // }
    // duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    // cout << "LS coef D Histogram on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    // myfile << "LS coef D Histogram on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    // Histogram(Tests.DHist1, myfile);
    // Histogram(Tests.DHist10, myfile);
    // Histogram(Tests.DHist100, myfile);
    // Histogram(Tests.DHist1000, myfile);

    // cout << "MaxD:" << Tests.MaxD << endl;
    // myfile << "MaxD:" << Tests.MaxD << endl;
    // mpz_out_str(stdout, 10, Tests.NumberWithMaxD);
    // // mpz_out_str(myfile, 10, GF_LS_DHistogram._t.NumberWithMaxD);
    // // ToDo How to write mpz_t to file?
    // cout << endl;
    // myfile << endl;


    // // // _BeginTime = high_resolution_clock::now();
    // // // for (auto X : GF_Creator.CreatedPrimes) {
    // // //     Tests.IsPrimeBPSW(X);
    // // // }
    // // // duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    // // // cout << "BPSW 128 mpz on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    // BPSW
    _BeginTime = high_resolution_clock::now();
    for (auto X : GF_Creator.CreatedPrimes) {
        // Utils::UInt128tompz(X, mpz_X);
        utils_mpz::mpz_set_ul128(mpz_X, X);
        Tests.IsPrimeBPSW(mpz_X);
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    cout << "BPSW on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    myfile << "BPSW on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    _BeginTime = high_resolution_clock::now();
    for (auto X : GF_Creator.CreatedPrimes) {
        // Utils::UInt128tompz(X, mpz_X);
        utils_mpz::mpz_set_ul128(mpz_X, X);
        mpz_aprcl(const_cast<mpz_t&>(mpz_X));
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    cout << "APRLCL on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;   
    myfile << "APRLCL on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;   
  
    // BPSW 2
    _BeginTime = high_resolution_clock::now();
    for (auto X : GF_Creator.CreatedPrimes) {
        // Utils::UInt128tompz(X, mpz_X);
        utils_mpz::mpz_set_ul128(mpz_X, X);
        mpz_bpsw_prp(const_cast<mpz_t&>(mpz_X));
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    cout << "BPSW2 on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;
    myfile << "BPSW2 on Primes [min]:" << duration.count() / (1000.0l * 60.0l)  << endl;

    // APRLCL
    mpz_clear(mpz_X);
    myfile.close();
}

void MethodTests(const uint128_t Begin, const uint128_t End){
    std::ofstream myfile;
    ostringstream sFileName;
    sFileName << "Measurement_";
    sFileName << Begin << "_" << End << "_" << ".txt";
    myfile.open(sFileName.str());
    cout << "Begin: " << Begin << endl;    
    myfile << "Begin: " << Begin << endl;
    cout << "End:   " << End << endl;    
    myfile << "End:   " << End << endl;
    cout << "End-Begin: " << End - Begin << endl;
    myfile << "End-Begin: " << End - Begin << endl;
    
    long double NoOfPrimesEst = Li::NoPrimesInt(Begin, End);
    cout << "Number of expected primes between Begin and End: " << NoOfPrimesEst << endl;
    myfile << "Number of expected primes between Begin and End: " << NoOfPrimesEst << endl;
    
    myfile << endl;

    const SieveGenerator<uint128_t> Sieve(C_SieveGeneratorDefaultMaxPrime);

    GeneratorFunctionMR GF_MR;
    Sieve.Work(Begin, End, GF_MR);
    GeneratorFunctionAbstract & res=GF_MR;
    cout << "MR PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    myfile << "MR PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;


    // // // GeneratorFunctionMR128 GF_MR128;
    // // // GeneratorFunction * pGF_MR128 = (GeneratorFunction *) &GF_MR128;
    // // // Sieve.Work(Begin, End, pGF_MR128);
    // // // res=GF_MR128;
    // // // cout << "MR 128 PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;


    // GeneratorFunctionLS GF_LS;
    // GeneratorFunction * pGF_LS = (GeneratorFunction *) &GF_LS;
    // Sieve.Work(Begin, End, pGF_LS);
    // res= GF_LS;
    // cout << "LS PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    // myfile << "LS PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;

    // GeneratorFunctionLS_D GF_LS_D;
    // GeneratorFunction * pGF_LS_D = (GeneratorFunction *) &GF_LS_D;
    // Sieve.Work(Begin, End, pGF_LS_D);
    // res= GF_LS_D;
    // cout << "LS_D PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    // myfile << "LS_D PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;


    GeneratorFunctionLS_DHistogram GF_LS_DHistogram;
    Sieve.Work(Begin, End, GF_LS_DHistogram);
    res= GF_LS_DHistogram;
    cout << "LS_DHistogram: " << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    myfile << "LS_DHistogram: " << " Duration minutes: " << res.DurationMinutes() << endl << endl;

    Histogram(GF_LS_DHistogram._t.DHist1, myfile);
    Histogram(GF_LS_DHistogram._t.DHist10, myfile);
    Histogram(GF_LS_DHistogram._t.DHist100, myfile);
    Histogram(GF_LS_DHistogram._t.DHist1000, myfile);
    cout << endl;
    myfile << endl;

    cout << "MaxD:" << GF_LS_DHistogram._t.MaxD << endl;
    myfile << "MaxD:" << GF_LS_DHistogram._t.MaxD << endl;
    mpz_out_str(stdout, 10, GF_LS_DHistogram._t.NumberWithMaxD);
    // mpz_out_str(myfile, 10, GF_LS_DHistogram._t.NumberWithMaxD);
    // ToDo How to write mpz_t to file?
    cout << endl;
    myfile << endl;
   

    // GeneratorFunctionBPSW GF_BPSW;
    // GeneratorFunction * pGF_BPSW = (GeneratorFunction *) &GF_BPSW;
    // Sieve.Work(Begin, End, pGF_BPSW);
    // res= GF_BPSW;
    // cout << "BPSW PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    // myfile << "BPSW PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;

    // // // GeneratorFunctionBPSW128 GF_BPSW128;
    // // // GeneratorFunction * pGF_BPSW128 = (GeneratorFunction *) &GF_BPSW128;
    // // // Sieve.Work(Begin, End, pGF_BPSW128);
    // // // res= GF_BPSW128;
    // // // cout << "BPSW 128 PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;

    // GeneratorFunctionBPSW2 GF_BPSW2;
    // GeneratorFunction * pGF_BPSW2 = (GeneratorFunction *) &GF_BPSW2;
    // Sieve.Work(Begin, End, pGF_BPSW2);
    // res= GF_BPSW2;
    // cout << "BPSW2 PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    // myfile << "BPSW2 PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;

    // GeneratorFunctionAPRCL GF_APRCL;
    // GeneratorFunction * pGF_APRCL = (GeneratorFunction *) &GF_APRCL;
    // Sieve.Work(Begin, End, pGF_APRCL);
    // res= GF_APRCL;
    // cout << "APRL PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;
    // myfile << "APRL PrimesCount: " << res.PrimesCnt() << " ProbablePrimesCnt: " << res.ProbablePrimesCnt() << " Duration minutes: " << res.DurationMinutes() << endl << endl;


    cout << endl << endl;
    myfile.close();
}


#ifdef NDEBUG
const unsigned int C_ShiftsStadard = 31;
#else
const unsigned int C_ShiftsStadard = 26;
#endif 

void MethodTestsNearBegin(){
    cout << "------------------ Testing from 2^64 ---------------------" << endl;
    uint128_t Begin = UINT64_MAX;
    Begin++;
    uint128_t End = Begin;
    End += static_cast<uint64_t>(1) << C_ShiftsStadard;

    MethodTests(Begin, End);    
}

void MethodTestsNearEnd(){
    cout << "------------------ Testing near 2^65 ---------------------" << endl;
    uint128_t End = static_cast<uint128_t>(1) << 65;
    End--;
    uint128_t Begin = End;
    Begin -= (static_cast<uint64_t>(1) << C_ShiftsStadard);

    MethodTests(Begin, End);    
}

void PrimeTestTest(){

    int Res=-1;
    int ResBool=false;
    unsigned long long NumberToTest=0;
    PrimeTest t(64);
    mpz_t Tst;
    mpz_init(Tst);

    // cout << gmp_version << endl;

    ResBool=false;
    // NumberToTest = 106528657;
    NumberToTest = 1024+1+8;
    ResBool = t.iMillerRabinBase2(NumberToTest);
    std::cout << NumberToTest << " " << ResBool << endl;

    Res=-1;
    // NumberToTest = 106528657;
    NumberToTest = 1024+1+8;
    utils_mpz::mpz_set_ull(Tst, NumberToTest);
    
    Res = t.iMillerRabinBase2(Tst);
    std::cout << NumberToTest << " " << Res << endl;
/*
    Res=-1;
    unsigned long long NumberToTest = 106528657;
    Utils::mpz_set_ull(Tst, NumberToTest);
    Res = t.iLucasSelfridge(Tst);
    std::cout << NumberToTest << " " << Res << endl;
    
    Res=-1;
    NumberToTest = 106528699;
    Utils::mpz_set_ull(Tst, NumberToTest);
    Res = t.iLucasSelfridge(Tst);
    std::cout << NumberToTest << " " << Res << endl;
    
    Res=-1;
    NumberToTest = 123456789;
    Utils::mpz_set_ull(Tst, NumberToTest);
    Res = t.iLucasSelfridge(Tst);
	std::cout << NumberToTest << " " << Res << endl;
*/
}

  /*
    mpz_t X;
    mpz_init(X);
    Utils::mpz_set_ull(X, 6074001001);
    unsigned long long j = 0;
    for(unsigned long long i=0; i<100000000000; i++)
    {
        if (iMillerRabinBase2(X)= =0) j++;
        //if (iLucasSelfridge(X)==0) j++;
    }
    mpz_clear(X);
    return j;
*/