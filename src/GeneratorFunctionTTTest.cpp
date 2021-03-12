#include <iostream>
#include <memory>
#include <fstream>

#include "GeneratorFunctionTT.hpp"

#include "SieveGenerator.hpp"

#include "PrimesRange.hpp"
#include "PrimesRangeService.hpp"
#include "GeneratorFunctionBPSW.hpp"
#include "GeneratorFunctionMR.hpp"


using std::cout;

const unsigned int C_Shifts = 25;

// Generator Function Template Test
void GFTTest(){
    const SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);
    
    unsigned long long Begin = static_cast<uint64_t>(1) << C_Shifts;
    unsigned long long End = Begin -1;
    End +=Begin;
    Begin++;
    
    PrimeTest _t(65, false, false);

    GeneratorFunctionTT GF;
    Sieve.Work(Begin, End, GF);
    GF.PrintResult();

    GeneratorFunctionBPSW GF_BPSW;
    Sieve.Work(Begin, End, GF_BPSW);
    GF_BPSW.PrintResult();
}



// Generator Function Template Test MultiThreading
void GFTTestMT(){
    SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);
    
    unsigned long long Begin = static_cast<uint64_t>(1) << C_Shifts;
    unsigned long long End = Begin -1;
    End +=Begin;
    Begin++;
    
    PrimeTest _t(65, false, false);
      
    GeneratorFunctionTT GFTT;
    Sieve.Work(Begin, End, GFTT);
    GFTT.PrintResult();

    GeneratorFunctionTT GFTT_;
    Sieve.WorkMT(Begin, End, GFTT_);
    GFTT_.PrintResult();

}
