#include "WebWork.hpp"
#include "WorkerStruct.hpp"
// #include "Sieve2Generator.hpp"


#include <chrono>

using namespace std::chrono;


void WebBitStatistics(WorkerStruct &w){

    WebWork WW;
    // todo break ??
    
    while(w.ThreadsPct()>0) {
        Log::out().init();
        WW.WebBitStatisticsWork(w);   
    } 
}


bool RunCheckComputationOk(){
    constexpr unsigned int C_TestPower2 = 30;
    constexpr long double C_StandardCheckDuration = 297.0;  //power 50;
    // constexpr long double C_StandardCheckDurationDebug = 560.0;

    #ifdef NDEBUG
    // minimum in order to make 8 cores sweaty
    constexpr uint64_t C_TestLength = static_cast<unsigned long long>(1) << C_TestPower2;  
    #else
    constexpr uint64_t C_TestLength = static_cast<unsigned long long>(1) << C_TestPower2;  
    #endif

    Log::out() << "Running check computation (better safe than sorry).\n";
        
    clsNewWork NewWork;
    
    NewWork.c_power2 = 63;
    NewWork.c_power2 = 50;
    
    NewWork.new_begin = static_cast<unsigned long long>(1) << (NewWork.c_power2-1) ;
    // NewWork.new_end =   NewWork.new_begin ;
    NewWork.new_end =   NewWork.new_begin + C_TestLength;
    NewWork.new_begin = NewWork.new_begin - C_TestLength;
    
    NewWork.LogHeader();

    GeneratorFunctionBitStatistics BSMT(NewWork.c_power2);
    SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);
    // Sieve2Generator<unsigned long long> Sieve;
    Sieve.Threads(100);
    Sieve.Threads(12);


    // auto BeginTime = high_resolution_clock::now();
    Sieve.ResetClock();
    Sieve.WorkMT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    // Sieve.Work2MT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    // auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime);

    long double duration = Sieve.DurationSeconds();
    Log::out() << "Duration [s]: " <<  utils_str::FormatNumber(duration, 10, 1) << " \t~" << utils_str::FormatNumber( (100.0L * duration)/C_StandardCheckDuration, 4,1)  << "% \n";

    for(unsigned int i=0; i<=64; i++){
        Log::out() << "BSMT.CntPrimesWithNumOfSetBits(" << i << ")==    " << BSMT.CntPrimesWithNumOfSetBits(i) << " &&\n";
    } 

    // #if NDEBUG

    if (NewWork.c_power2==50 && C_TestPower2 == 30) {        
        return(    
            BSMT.CntPrimesWithNumOfSetBits(0)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(1)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(2)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(3)==    4 &&
            BSMT.CntPrimesWithNumOfSetBits(4)==    31 &&
            BSMT.CntPrimesWithNumOfSetBits(5)==    141 &&
            BSMT.CntPrimesWithNumOfSetBits(6)==    1502 &&
            BSMT.CntPrimesWithNumOfSetBits(7)==    6695 &&
            BSMT.CntPrimesWithNumOfSetBits(8)==    27581 &&
            BSMT.CntPrimesWithNumOfSetBits(9)==    86078 &&
            BSMT.CntPrimesWithNumOfSetBits(10)==    252848 &&
            BSMT.CntPrimesWithNumOfSetBits(11)==    554115 &&
            BSMT.CntPrimesWithNumOfSetBits(12)==    1162138 &&
            BSMT.CntPrimesWithNumOfSetBits(13)==    1954870 &&
            BSMT.CntPrimesWithNumOfSetBits(14)==    2982594 &&
            BSMT.CntPrimesWithNumOfSetBits(15)==    3831950 &&
            BSMT.CntPrimesWithNumOfSetBits(16)==    4485384 &&
            BSMT.CntPrimesWithNumOfSetBits(17)==    4347557 &&
            BSMT.CntPrimesWithNumOfSetBits(18)==    3933386 &&
            BSMT.CntPrimesWithNumOfSetBits(19)==    2926661 &&
            BSMT.CntPrimesWithNumOfSetBits(20)==    1989375 &&
            BSMT.CntPrimesWithNumOfSetBits(21)==    1126119 &&
            BSMT.CntPrimesWithNumOfSetBits(22)==    585725 &&
            BSMT.CntPrimesWithNumOfSetBits(23)==    236166 &&
            BSMT.CntPrimesWithNumOfSetBits(24)==    94161 &&
            BSMT.CntPrimesWithNumOfSetBits(25)==    33486 &&
            BSMT.CntPrimesWithNumOfSetBits(26)==    33307 &&
            BSMT.CntPrimesWithNumOfSetBits(27)==    93162 &&
            BSMT.CntPrimesWithNumOfSetBits(28)==    238295 &&
            BSMT.CntPrimesWithNumOfSetBits(29)==    583086 &&
            BSMT.CntPrimesWithNumOfSetBits(30)==    1122450 &&
            BSMT.CntPrimesWithNumOfSetBits(31)==    2009003 &&
            BSMT.CntPrimesWithNumOfSetBits(32)==    2918509 &&
            BSMT.CntPrimesWithNumOfSetBits(33)==    3904770 &&
            BSMT.CntPrimesWithNumOfSetBits(34)==    4371146 &&
            BSMT.CntPrimesWithNumOfSetBits(35)==    4480805 &&
            BSMT.CntPrimesWithNumOfSetBits(36)==    3829453 &&
            BSMT.CntPrimesWithNumOfSetBits(37)==    2996298 &&
            BSMT.CntPrimesWithNumOfSetBits(38)==    1940393 &&
            BSMT.CntPrimesWithNumOfSetBits(39)==    1168039 &&
            BSMT.CntPrimesWithNumOfSetBits(40)==    559460 &&
            BSMT.CntPrimesWithNumOfSetBits(41)==    247257 &&
            BSMT.CntPrimesWithNumOfSetBits(42)==    88005 &&
            BSMT.CntPrimesWithNumOfSetBits(43)==    28345 &&
            BSMT.CntPrimesWithNumOfSetBits(44)==    6541 &&
            BSMT.CntPrimesWithNumOfSetBits(45)==    1475 &&
            BSMT.CntPrimesWithNumOfSetBits(46)==    196 &&
            BSMT.CntPrimesWithNumOfSetBits(47)==    29 &&
            BSMT.CntPrimesWithNumOfSetBits(48)==    3 &&
            BSMT.CntPrimesWithNumOfSetBits(49)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(50)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(51)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(52)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(53)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(54)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(55)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(56)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(57)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(58)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(59)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(60)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(61)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(62)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(63)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(64)==    0
            );
    }
    else if (NewWork.c_power2==63 && C_TestPower2 == 30) {
        return (    
            BSMT.CntPrimesWithNumOfSetBits(0)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(1)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(2)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(3)==    1 &&
            BSMT.CntPrimesWithNumOfSetBits(4)==    18 &&
            BSMT.CntPrimesWithNumOfSetBits(5)==    134 &&
            BSMT.CntPrimesWithNumOfSetBits(6)==    1109 &&
            BSMT.CntPrimesWithNumOfSetBits(7)==    5335 &&
            BSMT.CntPrimesWithNumOfSetBits(8)==    22970 &&
            BSMT.CntPrimesWithNumOfSetBits(9)==    67333 &&
            BSMT.CntPrimesWithNumOfSetBits(10)==    199150 &&
            BSMT.CntPrimesWithNumOfSetBits(11)==    451095 &&
            BSMT.CntPrimesWithNumOfSetBits(12)==    928002 &&
            BSMT.CntPrimesWithNumOfSetBits(13)==    1542546 &&
            BSMT.CntPrimesWithNumOfSetBits(14)==    2376629 &&
            BSMT.CntPrimesWithNumOfSetBits(15)==    3054966 &&
            BSMT.CntPrimesWithNumOfSetBits(16)==    3561385 &&
            BSMT.CntPrimesWithNumOfSetBits(17)==    3456987 &&
            BSMT.CntPrimesWithNumOfSetBits(18)==    3130015 &&
            BSMT.CntPrimesWithNumOfSetBits(19)==    2334608 &&
            BSMT.CntPrimesWithNumOfSetBits(20)==    1589229 &&
            BSMT.CntPrimesWithNumOfSetBits(21)==    883567 &&
            BSMT.CntPrimesWithNumOfSetBits(22)==    466751 &&
            BSMT.CntPrimesWithNumOfSetBits(23)==    191008 &&
            BSMT.CntPrimesWithNumOfSetBits(24)==    73061 &&
            BSMT.CntPrimesWithNumOfSetBits(25)==    20256 &&
            BSMT.CntPrimesWithNumOfSetBits(26)==    6032 &&
            BSMT.CntPrimesWithNumOfSetBits(27)==    1041 &&
            BSMT.CntPrimesWithNumOfSetBits(28)==    207 &&
            BSMT.CntPrimesWithNumOfSetBits(29)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(30)==    3 &&
            BSMT.CntPrimesWithNumOfSetBits(31)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(32)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(33)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(34)==    2 &&
            BSMT.CntPrimesWithNumOfSetBits(35)==    13 &&
            BSMT.CntPrimesWithNumOfSetBits(36)==    197 &&
            BSMT.CntPrimesWithNumOfSetBits(37)==    892 &&
            BSMT.CntPrimesWithNumOfSetBits(38)==    5728 &&
            BSMT.CntPrimesWithNumOfSetBits(39)==    21223 &&
            BSMT.CntPrimesWithNumOfSetBits(40)==    72298 &&
            BSMT.CntPrimesWithNumOfSetBits(41)==    188754 &&
            BSMT.CntPrimesWithNumOfSetBits(42)==    464598 &&
            BSMT.CntPrimesWithNumOfSetBits(43)==    890312 &&
            BSMT.CntPrimesWithNumOfSetBits(44)==    1598963 &&
            BSMT.CntPrimesWithNumOfSetBits(45)==    2324334 &&
            BSMT.CntPrimesWithNumOfSetBits(46)==    3104145 &&
            BSMT.CntPrimesWithNumOfSetBits(47)==    3492699 &&
            BSMT.CntPrimesWithNumOfSetBits(48)==    3557207 &&
            BSMT.CntPrimesWithNumOfSetBits(49)==    3022403 &&
            BSMT.CntPrimesWithNumOfSetBits(50)==    2397442 &&
            BSMT.CntPrimesWithNumOfSetBits(51)==    1558881 &&
            BSMT.CntPrimesWithNumOfSetBits(52)==    917178 &&
            BSMT.CntPrimesWithNumOfSetBits(53)==    444272 &&
            BSMT.CntPrimesWithNumOfSetBits(54)==    199890 &&
            BSMT.CntPrimesWithNumOfSetBits(55)==    68111 &&
            BSMT.CntPrimesWithNumOfSetBits(56)==    22616 &&
            BSMT.CntPrimesWithNumOfSetBits(57)==    5188 &&
            BSMT.CntPrimesWithNumOfSetBits(58)==    1168 &&
            BSMT.CntPrimesWithNumOfSetBits(59)==    140 &&
            BSMT.CntPrimesWithNumOfSetBits(60)==    24 &&
            BSMT.CntPrimesWithNumOfSetBits(61)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(62)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(63)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(64)==    0 
        );
    }
    else {
        Log::out() << "Missing reference results for comparison. I am sorry. Something is being tested? \n";
        return false;
    }
    // #else
    // Log::out() << "Skipping check compute results evaluation.\n";
    // return true;
    // #endif
}


bool RunCheckComputationNotOk(){
    constexpr unsigned int C_TestPower2 = 30;
    constexpr long double C_StandardCheckDuration = 297.0;  //power 50;
    // constexpr long double C_StandardCheckDurationDebug = 560.0;

    #ifdef NDEBUG
    // minimum in order to make 8 cores sweaty
    constexpr uint64_t C_TestLength = static_cast<unsigned long long>(1) << C_TestPower2;  
    #else
    constexpr uint64_t C_TestLength = static_cast<unsigned long long>(1) << C_TestPower2;  
    #endif

    Log::out() << "Running check computation (better safe than sorry).\n";
        
    clsNewWork NewWork;
    
    NewWork.c_power2 = 63;
    NewWork.c_power2 = 50;
    
    NewWork.new_begin = static_cast<unsigned long long>(1) << (NewWork.c_power2-1) ;
    // NewWork.new_end =   NewWork.new_begin ;
    NewWork.new_end =   NewWork.new_begin + C_TestLength;
    NewWork.new_begin = NewWork.new_begin - C_TestLength;
    
    NewWork.LogHeader();

    GeneratorFunctionBitStatistics BSMT(NewWork.c_power2);
    // SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);
    Sieve2Generator<unsigned long long> Sieve;
    Sieve.Threads(100);
    Sieve.Threads(12);


    // auto BeginTime = high_resolution_clock::now();
    Sieve.ResetClock();
    // Sieve.WorkMT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    Sieve.Work2MT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    // auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime);

    long double duration = Sieve.DurationSeconds();
    Log::out() << "Duration [s]: " <<  utils_str::FormatNumber(duration, 10, 1) << " \t~" << utils_str::FormatNumber( (100.0L * duration)/C_StandardCheckDuration, 4,1)  << "% \n";

    for(unsigned int i=0; i<=64; i++){
        Log::out() << "BSMT.CntPrimesWithNumOfSetBits(" << i << ")==    " << BSMT.CntPrimesWithNumOfSetBits(i) << " &&\n";
    } 

    // #if NDEBUG

    if (NewWork.c_power2==50 && C_TestPower2 == 30) {        
        return(    
            BSMT.CntPrimesWithNumOfSetBits(0)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(1)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(2)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(3)==    4 &&
            BSMT.CntPrimesWithNumOfSetBits(4)==    31 &&
            BSMT.CntPrimesWithNumOfSetBits(5)==    141 &&
            BSMT.CntPrimesWithNumOfSetBits(6)==    1502 &&
            BSMT.CntPrimesWithNumOfSetBits(7)==    6695 &&
            BSMT.CntPrimesWithNumOfSetBits(8)==    27581 &&
            BSMT.CntPrimesWithNumOfSetBits(9)==    86078 &&
            BSMT.CntPrimesWithNumOfSetBits(10)==    252848 &&
            BSMT.CntPrimesWithNumOfSetBits(11)==    554115 &&
            BSMT.CntPrimesWithNumOfSetBits(12)==    1162138 &&
            BSMT.CntPrimesWithNumOfSetBits(13)==    1954870 &&
            BSMT.CntPrimesWithNumOfSetBits(14)==    2982594 &&
            BSMT.CntPrimesWithNumOfSetBits(15)==    3831950 &&
            BSMT.CntPrimesWithNumOfSetBits(16)==    4485384 &&
            BSMT.CntPrimesWithNumOfSetBits(17)==    4347557 &&
            BSMT.CntPrimesWithNumOfSetBits(18)==    3933386 &&
            BSMT.CntPrimesWithNumOfSetBits(19)==    2926661 &&
            BSMT.CntPrimesWithNumOfSetBits(20)==    1989375 &&
            BSMT.CntPrimesWithNumOfSetBits(21)==    1126119 &&
            BSMT.CntPrimesWithNumOfSetBits(22)==    585725 &&
            BSMT.CntPrimesWithNumOfSetBits(23)==    236166 &&
            BSMT.CntPrimesWithNumOfSetBits(24)==    94161 &&
            BSMT.CntPrimesWithNumOfSetBits(25)==    33486 &&
            BSMT.CntPrimesWithNumOfSetBits(26)==    33307 &&
            BSMT.CntPrimesWithNumOfSetBits(27)==    93162 &&
            BSMT.CntPrimesWithNumOfSetBits(28)==    238295 &&
            BSMT.CntPrimesWithNumOfSetBits(29)==    583086 &&
            BSMT.CntPrimesWithNumOfSetBits(30)==    1122450 &&
            BSMT.CntPrimesWithNumOfSetBits(31)==    2009003 &&
            BSMT.CntPrimesWithNumOfSetBits(32)==    2918509 &&
            BSMT.CntPrimesWithNumOfSetBits(33)==    3904770 &&
            BSMT.CntPrimesWithNumOfSetBits(34)==    4371146 &&
            BSMT.CntPrimesWithNumOfSetBits(35)==    4480805 &&
            BSMT.CntPrimesWithNumOfSetBits(36)==    3829453 &&
            BSMT.CntPrimesWithNumOfSetBits(37)==    2996298 &&
            BSMT.CntPrimesWithNumOfSetBits(38)==    1940393 &&
            BSMT.CntPrimesWithNumOfSetBits(39)==    1168039 &&
            BSMT.CntPrimesWithNumOfSetBits(40)==    559460 &&
            BSMT.CntPrimesWithNumOfSetBits(41)==    247257 &&
            BSMT.CntPrimesWithNumOfSetBits(42)==    88005 &&
            BSMT.CntPrimesWithNumOfSetBits(43)==    28345 &&
            BSMT.CntPrimesWithNumOfSetBits(44)==    6541 &&
            BSMT.CntPrimesWithNumOfSetBits(45)==    1475 &&
            BSMT.CntPrimesWithNumOfSetBits(46)==    196 &&
            BSMT.CntPrimesWithNumOfSetBits(47)==    29 &&
            BSMT.CntPrimesWithNumOfSetBits(48)==    3 &&
            BSMT.CntPrimesWithNumOfSetBits(49)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(50)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(51)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(52)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(53)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(54)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(55)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(56)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(57)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(58)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(59)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(60)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(61)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(62)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(63)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(64)==    0
            );
    }
    else if (NewWork.c_power2==63 && C_TestPower2 == 30) {
        return (    
            BSMT.CntPrimesWithNumOfSetBits(0)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(1)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(2)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(3)==    1 &&
            BSMT.CntPrimesWithNumOfSetBits(4)==    18 &&
            BSMT.CntPrimesWithNumOfSetBits(5)==    134 &&
            BSMT.CntPrimesWithNumOfSetBits(6)==    1109 &&
            BSMT.CntPrimesWithNumOfSetBits(7)==    5335 &&
            BSMT.CntPrimesWithNumOfSetBits(8)==    22970 &&
            BSMT.CntPrimesWithNumOfSetBits(9)==    67333 &&
            BSMT.CntPrimesWithNumOfSetBits(10)==    199150 &&
            BSMT.CntPrimesWithNumOfSetBits(11)==    451095 &&
            BSMT.CntPrimesWithNumOfSetBits(12)==    928002 &&
            BSMT.CntPrimesWithNumOfSetBits(13)==    1542546 &&
            BSMT.CntPrimesWithNumOfSetBits(14)==    2376629 &&
            BSMT.CntPrimesWithNumOfSetBits(15)==    3054966 &&
            BSMT.CntPrimesWithNumOfSetBits(16)==    3561385 &&
            BSMT.CntPrimesWithNumOfSetBits(17)==    3456987 &&
            BSMT.CntPrimesWithNumOfSetBits(18)==    3130015 &&
            BSMT.CntPrimesWithNumOfSetBits(19)==    2334608 &&
            BSMT.CntPrimesWithNumOfSetBits(20)==    1589229 &&
            BSMT.CntPrimesWithNumOfSetBits(21)==    883567 &&
            BSMT.CntPrimesWithNumOfSetBits(22)==    466751 &&
            BSMT.CntPrimesWithNumOfSetBits(23)==    191008 &&
            BSMT.CntPrimesWithNumOfSetBits(24)==    73061 &&
            BSMT.CntPrimesWithNumOfSetBits(25)==    20256 &&
            BSMT.CntPrimesWithNumOfSetBits(26)==    6032 &&
            BSMT.CntPrimesWithNumOfSetBits(27)==    1041 &&
            BSMT.CntPrimesWithNumOfSetBits(28)==    207 &&
            BSMT.CntPrimesWithNumOfSetBits(29)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(30)==    3 &&
            BSMT.CntPrimesWithNumOfSetBits(31)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(32)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(33)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(34)==    2 &&
            BSMT.CntPrimesWithNumOfSetBits(35)==    13 &&
            BSMT.CntPrimesWithNumOfSetBits(36)==    197 &&
            BSMT.CntPrimesWithNumOfSetBits(37)==    892 &&
            BSMT.CntPrimesWithNumOfSetBits(38)==    5728 &&
            BSMT.CntPrimesWithNumOfSetBits(39)==    21223 &&
            BSMT.CntPrimesWithNumOfSetBits(40)==    72298 &&
            BSMT.CntPrimesWithNumOfSetBits(41)==    188754 &&
            BSMT.CntPrimesWithNumOfSetBits(42)==    464598 &&
            BSMT.CntPrimesWithNumOfSetBits(43)==    890312 &&
            BSMT.CntPrimesWithNumOfSetBits(44)==    1598963 &&
            BSMT.CntPrimesWithNumOfSetBits(45)==    2324334 &&
            BSMT.CntPrimesWithNumOfSetBits(46)==    3104145 &&
            BSMT.CntPrimesWithNumOfSetBits(47)==    3492699 &&
            BSMT.CntPrimesWithNumOfSetBits(48)==    3557207 &&
            BSMT.CntPrimesWithNumOfSetBits(49)==    3022403 &&
            BSMT.CntPrimesWithNumOfSetBits(50)==    2397442 &&
            BSMT.CntPrimesWithNumOfSetBits(51)==    1558881 &&
            BSMT.CntPrimesWithNumOfSetBits(52)==    917178 &&
            BSMT.CntPrimesWithNumOfSetBits(53)==    444272 &&
            BSMT.CntPrimesWithNumOfSetBits(54)==    199890 &&
            BSMT.CntPrimesWithNumOfSetBits(55)==    68111 &&
            BSMT.CntPrimesWithNumOfSetBits(56)==    22616 &&
            BSMT.CntPrimesWithNumOfSetBits(57)==    5188 &&
            BSMT.CntPrimesWithNumOfSetBits(58)==    1168 &&
            BSMT.CntPrimesWithNumOfSetBits(59)==    140 &&
            BSMT.CntPrimesWithNumOfSetBits(60)==    24 &&
            BSMT.CntPrimesWithNumOfSetBits(61)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(62)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(63)==    0 &&
            BSMT.CntPrimesWithNumOfSetBits(64)==    0 
        );
    }
    else {
        Log::out() << "Missing reference results for comparison. I am sorry. Something is being tested? \n";
        return false;
    }
    // #else
    // Log::out() << "Skipping check compute results evaluation.\n";
    // return true;
    // #endif
}

bool RunCheckComputation(){
    // gPush =true;
    // // RunCheckComputationOk();
    // gPush = false;
    RunCheckComputationNotOk();

    // // gT1.sort
    // sort(gT1.begin(), gT1.end(), less<uint64_t>());
    // sort(gT2.begin(), gT2.end(), less<uint64_t>());
    // for (size_t i = 0; i<10000000;i++){
    //     if (gT1[i]!=gT2[i]){
    //         Log::out() << i << "  " << gT1[i] << "  "<< gT2[i]<< "\n";
    //     }
    // }
    // 0  1688848786522141  1688848778957401
    // 1688848786522141 is skipped
}
