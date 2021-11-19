#include "WebWork.hpp"
#include "WorkerStruct.hpp"


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


bool RunCheckComputation(){
    #ifdef NDEBUG
    constexpr long double C_StandardCheckDuration = 321.0;
    // constexpr long double C_StandardCheckDurationDebug = 560.0;
    // minimum in order to make 8 cores sweaty
    constexpr uint64_t C_TestLength = static_cast<unsigned long long>(1) << 30;  
    #else
    constexpr long double C_StandardCheckDuration = 321.0;
    // constexpr long double C_StandardCheckDurationDebug = 560.0;
    constexpr uint64_t C_TestLength = static_cast<unsigned long long>(1) << 28;  

    #endif

    Log::out() << "Running check computation (better safe than sorry).\n";
        
    clsNewWork NewWork;
    NewWork.c_power2 = 63;
    NewWork.new_begin = static_cast<unsigned long long>(1) << (NewWork.c_power2-1) ;
    // NewWork.new_end =   NewWork.new_begin ;
    NewWork.new_end =   NewWork.new_begin + C_TestLength;
    NewWork.new_begin = NewWork.new_begin -  C_TestLength;
    
    NewWork.LogHeader();

    GeneratorFunctionBitStatistics BSMT(NewWork.c_power2);
    SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);
    Sieve.Threads(100);

    // auto BeginTime = high_resolution_clock::now();
    Sieve.ResetClock();
    Sieve.WorkMT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    // auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime);

    Log::out() << "Primes:   " << BSMT.PrimesCnt() << "\n";
    


    
    long double duration = Sieve.DurationSeconds();
    Log::out() << "Duration [s]: " <<  utils_str::FormatNumber(duration, 10, 1) << " \t~" << utils_str::FormatNumber( (100.0L * duration)/C_StandardCheckDuration, 4,1)  << "% \n";

    // for(unsigned int i=0; i<=64; i++){
    //     Log::out() << "BSMT.CntPrimesWithNumOfSetBits(" << i << ")==    " << BSMT.CntPrimesWithNumOfSetBits(i) << " &&\n";
    // } 

    #if NDEBUG

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
    #else
    return true;
    #endif
}
