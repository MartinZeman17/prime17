#include "GeneratorFunctionBitStatistics.hpp"
#include "SieveGenerator.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>

#ifdef __MINGW64__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif


using std::endl;
using std::cout;

void GeneratorFunctionBitStatisticsTestOld(){

    for (unsigned int i=1; i<=64; i++)
    {
        {
            cout << "Bit statistics for power: " << i << endl;
            GeneratorFunctionBitStatistics BS(i);
            // GeneratorFunctionBPSW BS;
            // GeneratorFunctionAPRCL BS;
            // GeneratorFunctionTT BS;

            SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);

            #ifdef NDEBUG
            const unsigned int MTChunks = 200; //at least number of cores
            #else
            const unsigned int MTChunks = 50; //at least number of cores
            #endif             

            // od beg chunks/
            unsigned long long X = BS.Begin();
            unsigned long long Y = ((BS.Begin() / Sieve.Primorial) + MTChunks) * Sieve.Primorial -1;            
            if (Y > BS.End()) Y = BS.End();

            unsigned long long NoOfChunks = (unsigned long long) ceil ((long double) (BS.End() - Y) / (Sieve.Primorial * MTChunks));
            unsigned long long CurrentChunk = 0; 

            while (true) {
                cout << "Chunk: " << CurrentChunk << " of " << NoOfChunks << " : " << X << " - " << Y << endl;
                GeneratorFunctionBitStatistics BSMT(BS);                 
                Sieve.WorkMT(X, Y, BSMT);
                BS += BSMT;
                //Save work
                if (Y == BS.End()) break;
                CurrentChunk++;
                X = Y+1;
                Y = X + MTChunks * Sieve.Primorial -1;
                if (Y > BS.End()) Y = BS.End();
            }
    
            // Sieve.Work(BS.Begin, BS.End, BS);            
            Sieve.WorkMT(BS.Begin(), BS.End(), BS);
            

            BS.SaveFile();


        }

    }
    
}
