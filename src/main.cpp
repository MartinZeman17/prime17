#include <iostream>
#include <gmp.h>

#include <stdio.h>
#include <curl/curl.h>

#include "PrimesRangeServiceTest.cpp"
#include "BitRangeTest.cpp"
#include "GeneratorFunctionBitStatistics.hpp"
#include "PrimeTestTest.cpp"
#include "SieveGeneratorTest.cpp"
#include "int128Test.cpp"
#include "SliceTBalance.hpp"
#include "SliceTBalanceTest.cpp"
#include "GeneratorFunctionTTTest.cpp"
#include "GeneratorFunctionBitStatisticsTest.cpp"

#include "WebWorkTest.cpp"

#include "Wizard.cpp"

// C code must be used from C++ code using extern in order to disable messing with functions names needed for overloading (allowed only in C++, not in C)
// Otherwise linking would fail
// Every decent header (and it is not the case of this one) should contain #ifdef __cplusplus extern "C" { } #endif or macros G_BEGIN_DECLS and G_END_DECLS
extern "C" {
#include "mpz_aprcl.h"
}


using namespace std::chrono;
using std::endl;
using std::cout;


void check(){
    #ifdef NDEBUG
    cout << "Version: Release ";
    #else
    cout << "Version: Debug ";
    #endif 
    cout << "__SIZEOF_LONG__: " << __SIZEOF_LONG__ << " ";

    cout << "GMP version: " << __gmp_version << " ";
    std::cout << "GMP_LIMB_BITS: " << GMP_LIMB_BITS << " ";
   
    #ifdef __MINGW64__
    cout << "Compiler: MinGW64 "  << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION << endl;
    #endif
    
    #ifdef __GNUC__
    cout << "Compiler: GNUC "  << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
    #endif


    mpz_t a, b;
    mpz_init(a);
    mpz_ui_pow_ui(a, 2, 64); //ok
    mpz_sub_ui(a, a, 1); // ok, so that a = UINT64_MAX

    mpz_init(b);
    utils_mpz::mpz_set_ull(b, UINT64_MAX);

    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT64_MAX to mpz using mpz_set_ull. Mission aborted!");
        cout << C_Msg << endl;
        throw(C_Msg);   
    }

    unsigned int iDiff = 987654321; 
    mpz_sub_ui(a, a, iDiff);  //ok
    utils_mpz::mpz_set_ull(b, UINT64_MAX - iDiff);

    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT64_MAX-x to mpz using mpz_set_ull. Mission aborted!");
        cout << C_Msg << endl;
        throw(C_Msg);   
    }

    unsigned long long res = utils_mpz::mpz_get<unsigned long long>(a);
    if (res!= UINT64_MAX - iDiff) {
        const std::string C_Msg("Check failed: Cannot get UINT64_MAX-x from mpz using mpz_get_ull. Mission aborted!");
        cout << C_Msg << endl;
        throw(C_Msg);   
    }

    mpz_ui_pow_ui(a, 2, 128); //ok
    mpz_sub_ui(a, a, 1); // ok, so that a = UINT128_MAX
    utils_mpz::mpz_set_ul128(b, prt::numeric_limits<uint128_t>::max());
    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT128_MAX to mpz using mpz_set_ull. Mission aborted!");
        cout << C_Msg << endl;
        throw(C_Msg);   
    }

    
    // a = UINT128_MAX - iDiff 
    mpz_sub_ui(a, a, iDiff); //ok
    utils_mpz::mpz_set<uint128_t>(b, prt::numeric_limits<uint128_t>::max() - iDiff);
    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT128_MAX -idiff to mpz using mpz_set<uitn128_t>. Mission aborted!");
        cout << C_Msg << endl;
        throw(C_Msg);   
    }

    
    uint128_t res3 = utils_mpz::mpz_get<uint128_t>(a);
    if (res3!= (uint128_t) (prt::numeric_limits<uint128_t>::max() - iDiff)) {
        const std::string C_Msg("Check failed: Cannot get UINT128_MAX-x from mpz using mpz_get_ull. Mission aborted!");
        cout << C_Msg << endl;
        throw(C_Msg);   
    }

    mpz_clear(a);
    mpz_clear(b);
}


int main(int argc, char* argv[])
{   
    check();

    bool bHelp = false;
    bool bWizard = false;
    bool bThread = false;
    bool bThreadTmp =false;
    // cout <<argc <<endl;
    if (argc>=2) {
        std::string Arg(argv[1]);
        // cout << Arg << endl;
        if (Arg=="-wizard" || Arg=="-w" ) bWizard = true;
        else if (Arg=="-thread" || Arg=="-threads" || Arg=="-t" || Arg=="-CPU" || Arg=="-cpu") bThread = true;
        else if (Arg=="-thread_tmp" || Arg=="-threads_tmp" || Arg=="-t_tmp" || Arg=="-CPU_tmp" || Arg=="-cpu_tmp") bThreadTmp = true;
        else bHelp = true;
    }

    // bHelp =true;
    if (bHelp) {
        cout <<  "Help:" << endl;
        cout << "\t-h \t\t\t\tHelp. You may also try to visit web pages at prime17.000webhostapp.com" << endl;
        cout << "\t-w -wizard \t\t\tRun wizard - first init or re-register." << endl;
        cout << "\t-t -thread -cpu \t\tSet number of threads in percent. Zero value will exit the program (after completion of the running task)." << endl;
        cout << "\t-t_tmp -thread_tmp -cpu_tmp \tTemporarily set number of threads in percent. Zero value will exit the program (after completion of the running task)." << endl;

    } else if (bThread || bThreadTmp) {
        WorkerStruct w;
        if (argc>=3) {
            std::string ArgThreads(argv[2]);
            // cout << ArgThreads << endl;
            if (bThread) {
                if (w.ThreadsPct(ArgThreads)) {
                    cout << "The number of threads is set, it will take effect when the work in progress ends." << endl;
                    w.SaveToConfigThreads();
                }
            } else {
                if (w.SaveToConfigThreadsTmp(ArgThreads)) {
                    cout << "The number of threads is temporarily set, it will take effect when the work in progress ends." << endl;
                }
            }
        } else {
            Wizard::ThreadSettings(w);
        }
    } else if (bWizard) {
        Wizard::NewWorker();
    } else {

        WorkerStruct w;
        if (w.LoadConfig()) {
            w.LoadThreads();
            if (w.ThreadsPct()<=0){
                Wizard::ThreadSettings(w);
            }
        } else {
            w = Wizard::NewWorker();
        }

        WebBitStatistics(w);
    }


    // downloadFile("https://drive.google.com/drive/u/0/folders/1zkuCWyipKIXkHrs62nKsnf834TxRhOFM", "aaa.txt");

    // PrimesRangeServiceTest();

    // BitRangeTest();

    // BitStatisticsTest();

    // PrimeTestTest();

    // int128Test();

    // SieveGeneratorTest();

    // EratosthenesEffectivity();

    // SliceTBalanceTest();

    // MethodTestsNearBegin();
    // MethodTestsNearEnd();
    // MethodTestsOnPrime();

    // SliceTBalanceTest(); 
    // GFTTest();
    // GFTTestMT();

    // GeneratorFunctionBitStatisticsTestOld();
    return 0; 
}



