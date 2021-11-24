#include <iostream>
#include <gmp.h>



// # include <algorithm>
# include <iostream>
# include <fstream>
# include <iterator>
# include <string>
# include <sstream>
# include <vector>

#include <stdio.h>
#include <curl/curl.h>
#include <locale.h>

#include <time.h>

#include <iomanip>
#include <ctime>

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
#include "Log.hpp"
#include "Binomials.hpp"
// #include "Correction.cpp"



// C code must be used from C++ code using extern in order to disable messing with functions names needed for overloading (allowed only in C++, not in C)
// Otherwise linking would fail
// Every decent header (and it is not the case of this one) should contain #ifdef __cplusplus extern "C" { } #endif or macros G_BEGIN_DECLS and G_END_DECLS
extern "C" {
#include "mpz_aprcl.h"
}



using namespace std::chrono;



// Convert compile time to system time 
time_t cvt_date(char const *date, char const *time)
{
    char s_month[5];
    int year;
    struct tm t;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    t.tm_yday = 0;
    t.tm_wday = 0;
    // Find where is s_month in month_names. Deduce month value.
    sscanf(date, "%s %d %d", s_month, &t.tm_mday, &year);
    sscanf(time, "%2d %*c %2d %*c %2d", &t.tm_hour, &t.tm_min, &t.tm_sec);
    t.tm_mon = (int) (((strstr(month_names, s_month) - month_names) / 3) + 1);  
    t.tm_mon--; // range for months is 0..11   
    t.tm_year = year - 1900;    
    return mktime(&t);
}

void check(){

    Log::out().logRight("Version: ");

    time_t  CompileTime =  cvt_date(__DATE__, __TIME__);    
    std::tm tm = *std::localtime(&CompileTime);

    Log::out().logRight (std::put_time(&tm, "%F"));
    Log::out().logRight("\n");

    #ifdef NDEBUG
    Log::out().logRight("Build: Release\n");
    #else
    Log::out().logRight("Build: Debug\n");
    #endif 

    Log::out().logRight("__SIZEOF_LONG__: ");
    Log::out().logRight( __SIZEOF_LONG__ );
    Log::out().logRight("\n");

    Log::out().logRight("GMP version: ");
    Log::out().logRight(__gmp_version);
    Log::out().logRight(" GMP_LIMB_BITS: ");
    Log::out().logRight(GMP_LIMB_BITS);
    Log::out().logRight("\n");
   
    #ifdef __MINGW64__
    Log::out().logRight("Compiler: MinGW64 ");
    Log::out().logRight(__MINGW32_MAJOR_VERSION);
    Log::out().logRight(".");
    Log::out().logRight(__MINGW32_MINOR_VERSION);
    Log::out().logRight("\n");
    #endif
    
    #ifdef __GNUC__
    Log::out().logRight("Compiler: GNUC ");
    Log::out().logRight( __GNUC__);
    Log::out().logRight(".");
    Log::out().logRight( __GNUC_MINOR__);
    Log::out().logRight(".");
    Log::out().logRight( __GNUC_PATCHLEVEL__);
    Log::out().logRight("\n");
    #endif

    Log::out().logRight("ncurses version: ");
    Log::out().logRight(curses_version());
    Log::out().logRight("\n");
    
    mpz_t a, b;
    mpz_init(a);
    mpz_ui_pow_ui(a, 2, 64); //ok
    mpz_sub_ui(a, a, 1); // ok, so that a = UINT64_MAX

    mpz_init(b);
    utils_mpz::mpz_set_ull(b, UINT64_MAX);

    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT64_MAX to mpz using mpz_set_ull. Mission aborted!");
        Log::out() << C_Msg << "\n";
        throw(C_Msg);   
    }

    unsigned int iDiff = 987654321; 
    mpz_sub_ui(a, a, iDiff);  //ok
    utils_mpz::mpz_set_ull(b, UINT64_MAX - iDiff);

    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT64_MAX-x to mpz using mpz_set_ull. Mission aborted!");
        Log::out() << C_Msg << "\n";
        throw(C_Msg);   
    }

    unsigned long long res = utils_mpz::mpz_get<unsigned long long>(a);
    if (res!= UINT64_MAX - iDiff) {
        const std::string C_Msg("Check failed: Cannot get UINT64_MAX-x from mpz using mpz_get_ull. Mission aborted!");
        Log::out() << C_Msg << "\n";
        throw(C_Msg);   
    }

    mpz_ui_pow_ui(a, 2, 128); //ok
    mpz_sub_ui(a, a, 1); // ok, so that a = UINT128_MAX
    utils_mpz::mpz_set_ul128(b, prt::numeric_limits<uint128_t>::max());
    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT128_MAX to mpz using mpz_set_ull. Mission aborted!");
        Log::out() << C_Msg << "\n";
        throw(C_Msg);   
    }

    
    // a = UINT128_MAX - iDiff 
    mpz_sub_ui(a, a, iDiff); //ok
    utils_mpz::mpz_set<uint128_t>(b, prt::numeric_limits<uint128_t>::max() - iDiff);
    if (mpz_cmp(a,b)!=0) {
        const std::string C_Msg("Check failed: Cannot set UINT128_MAX -idiff to mpz using mpz_set<uitn128_t>. Mission aborted!");
        Log::out() << C_Msg << "\n";
        throw(C_Msg);   
    }

    
    uint128_t res3 = utils_mpz::mpz_get<uint128_t>(a);
    if (res3!= (uint128_t) (prt::numeric_limits<uint128_t>::max() - iDiff)) {
        const std::string C_Msg("Check failed: Cannot get UINT128_MAX-x from mpz using mpz_get_ull. Mission aborted!");
        Log::out() << C_Msg << "\n";
        throw(C_Msg);   
    }

    mpz_clear(a);
    mpz_clear(b);
}

void checkWeb(){
    try{
        CURL *curl;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        curl_version_info_data * curlinfo = curl_version_info(CURLVERSION_NOW);
        Log::out().logRight("curl version: ");
        Log::out().logRight(curlinfo->version);
        Log::out().logRight("\n"); //Ubuntu 20 7.68.0
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (curl == nullptr) {
            Log::out() << "curl is null\n";
        }
        Log::out().logRight("\n");
    } catch(...) {
        Log::out() << "curl exception\n\n";
    }

}

void ShowHelp(){
    std::cout  <<  "Help:" << "\n";
    std::cout  << "\t-h -help\t\t\tHelp. You may also try to visit web pages at prime17.000webhostapp.com" << "\n";
    std::cout  << "\t-v -ver\t\t\tDisplay Prime17 version, libraries and so on..." << "\n";
    std::cout  << "\t-w -wizard \t\t\tRun wizard - first init or re-register." << "\n";
    std::cout  << "\t-t -thread -cpu \t\tSet number of threads in percent. Zero value will exit the program (after completion of the running task)." << "\n";
    std::cout  << "\t-t_tmp -thread_tmp -cpu_tmp \tTemporarily set number of threads in percent. Zero value will exit the program (after completion of the running task)." << "\n";
    std::cout  << "\t-c -check -ch \tRun testing computation and check results to ensure everything works just fine." << "\n";
    std::cout  << "\n";
    std::cout  << "\tExamples:\n";
    std::cout  << "\tprime17 -t 100 Utilize all available computational power.\n";
    std::cout  << "\tprime17 -t_tmp 0 Finish computation and shut down. Previously set cpu untilization remains unchanged for further executions.\n\n"; 
}

void ConfigThread(int argc, char* argv[], bool bThread){
    WorkerStruct w;
    if (argc>=3) {
        std::string ArgThreads(argv[2]);
        if (bThread) {
            if (w.ThreadsPct(ArgThreads)) {
                Log::out() << "The number of threads is set, it will take effect when the work in progress ends." << "\n";
                w.SaveToConfigThreads();
            }
        } else {
            if (w.SaveToConfigThreadsTmp(ArgThreads)) {
                Log::out() << "The number of threads is temporarily set, it will take effect when the work in progress ends." << "\n";
            }
        }
    } else {
        Wizard::ThreadSettings(w);
    }
}


void RunDefaultWork(){
    WorkerStruct w;
    if (w.LoadConfig()) {
        w.LoadThreads();
        if (w.ThreadsPct()<=0){
            Wizard::ThreadSettings(w);
        }
    } else {
        w = Wizard::NewWorker();
    }
    
    //check worker and in case of suspicious troubles run wizard
    //TODO: change of user in config file to nonsensical value goes unnoticed by this check    
    while (!Wizard::CheckWorker(w)) {
        w = Wizard::NewWorker(); 
    }

    WebBitStatistics(w);
}

bool RunCheckComputationEx() {

    if (RunCheckComputation()) {
        Log::out() << "Checks passed ok. Long and heavy labor ahead :-)\n";
        return true;
    } else{
        
        constexpr std::string_view C_ChecksFailed = "Checks failed, mission aborted.\n"
                        "It is either a nasty bug or a naughty hardware ghost is wreaking havoc among digital ones and zeros.\n"
                        "The cause may be related to a compilation for a specific platform (ARM, 32 bit, ...?) - portability is damn difficult!\n"
                        "Please contact Martin.Zeman17@gmail.com. Thank you!\n\n"
                        "Press any key to abort (Prime17 is deeply sorry).\n";
                         
        Log::out() << C_ChecksFailed;
        Log::out().getlineLeft();
        abort();
        return false;
    }        
}


// bool gPush;
// std::vector<uint64_t> gT1;
// std::vector<uint64_t> gT2;


int main(int argc, char* argv[])
{   
    Log::out().SetLogFileName(utils::ExtractFilename(std::string(argv[0])));

    bool bHelp = false;
    bool bWizard = false;
    bool bThread = false;
    bool bThreadTmp =false;
    bool bVersion = false;
    bool bCheck = false;
    bool bCheckPassed = false;
    if (argc>=2) {
        std::string Arg(argv[1]);
        utils_str::string_replace(Arg, "--", "-");
        if (Arg=="-wizard" || Arg=="-w" ) bWizard = true;
        else if (Arg=="-thread" || Arg=="-threads" || Arg=="-Threads" || Arg=="-t" || Arg=="-CPU" || Arg=="-cpu") bThread = true;
        else if (Arg=="-thread_tmp" || Arg=="-threads_tmp" || Arg=="-Threads_tmp" || Arg=="-t_tmp" || Arg=="-CPU_tmp" || Arg=="-cpu_tmp") bThreadTmp = true;
        else if (Arg=="-v" || Arg=="-ver" || Arg=="-version" || Arg=="-V" || Arg=="-Ver" || Arg=="-Version" ) bVersion = true;
        else if (Arg=="-c" || Arg=="-ch" || Arg=="-check" || Arg=="-Check" || Arg=="-C" || Arg=="-Ch" || Arg=="-Chk" || Arg=="-chk") bCheck = true;
        else bHelp = true;
    }


    if (bHelp){
        ShowHelp();
    } else {

        Log::out().init(!(bHelp || bWizard || bThread || bThreadTmp));
        check();
        checkWeb();

        // when debugging  pass standard computation as a safety measure
        #ifdef NDEBUG
        bool bRunAsUsusal = true;
        #else
        bool bRunAsUsusal = false;
        bCheck = true;
        #endif

        // *********************************************************************************************************************
        //override default settings      
        // bRunAsUsusal = false;
        // bRunAsUsusal = true;
        bCheck = true;

        if (bThread || bThreadTmp) {
            ConfigThread(argc, argv, bThread);
        } else if (bWizard) {
            Wizard::NewWorker();
        } else if (bCheck) {
            bCheckPassed = RunCheckComputationEx();
            Log::out().getlineLeft();

        } else if (bRunAsUsusal) {
            bCheckPassed = RunCheckComputationEx();
            // bCheckPassed = true;
            if (bCheckPassed) {
                RunDefaultWork();
            };
        } else if (bVersion) {    
            Log::out() << "See the version info in the right panel and press Enter to exit. \n\nKind regards from Prime17\n" ;
            Log::out().getlineLeft();
        } else {
            Log::out() << "Usual run has been passed. Apparently something is being tested.\n";

            SliceTBalanceCorrection();
            
        // *********************************************************************************************************************
            // SieveGeneratorTestMT();
            // SieveGeneratorTest();

            // Binomials::ComputeBinomials();

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
        }

        Log::out().Free();

    }
    return 0; 
}



