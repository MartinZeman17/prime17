#include "WebWork.hpp"
#include "WorkerStruct.hpp"
// #include "Sieve2Generator.hpp"


#include <chrono>

using namespace std::chrono;


void WebBitStatistics(WorkerStruct &w){

    WebWork WW;
    
    while(w.ThreadsPct()>0) {
        Log::out().init();
        WW.WebBitStatisticsWork(w);   
    } 
}

template <class T>
class TestCase{
    public:
    int Power2 = -1;
    T BegOffset = 0;
    T EndOffset = 0;
    T Primes = 0;
};

template <class T>
bool CheckCornesCases(std::vector<TestCase<T>> TC) {

    Sieve2Generator<T> Sieve;
    Sieve.Threads(100);
    // Sieve.Threads(12);

    for (auto tc :TC ) {
        clsNewWork<T> NewWork;
        T Offset;
        NewWork.power2 = tc.Power2;
        Offset = NewWork.Offset(); 
        NewWork.new_begin = tc.BegOffset;
        NewWork.new_end = tc.EndOffset;

        NewWork.LogHeader();

        GeneratorFunctionBitStatistics<T> BSMT(NewWork.power2);
        Sieve.ResetClock();
        BSMT.ResetClock();
        Sieve.Work2MT(Offset + NewWork.new_begin, Offset + NewWork.new_end, BSMT);

        if (BSMT.PrimesCnt() != tc.Primes ) {
            Log::out() << "Check of the sieve failed! There is something wrong in the world today, something deeply troubling. Mission aborted." << BSMT.PrimesCnt() << "\t" << tc.Primes << "\n";
            abort();
        };
        

    }

    return true;
}

template <class T>
bool CheckReferenceResults(clsNewWork<T> & NewWork, GeneratorFunctionBitStatistics<T> & BSMT){
    bool res;
    // #if NDEBUG

    if ( NewWork.power2==50 && NewWork.new_begin == 562948879679488 && NewWork.new_end == 562951027163136 ) {        
        res = (    
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
    else if ( NewWork.power2==63 && NewWork.new_begin == 4611686017353646080 && NewWork.new_end == 4611686019501129728 ) {
        res = (    
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
        return true;
    }


    if (res) {
        Log::out() << "Checks passed ok. Long and heavy labor ahead :-)\n";
    } else{        
        constexpr std::string_view C_ChecksFailed = "Checks failed, mission aborted.\n"
                        "It is either a nasty bug or a naughty hardware ghost is wreaking havoc among digital ones and zeros.\n"
                        "The cause may be related to a compilation for a specific platform (ARM, 32 bit, ...?) - portability is damn difficult!\n"
                        "Please contact Martin.Zeman17@gmail.com. Thank you!\n\n"
                        "Press any key to abort (Prime17 is deeply sorry).\n";
                            
        Log::out() << C_ChecksFailed;
        Log::out().getlineLeft();
        abort();
    }
    return res;
}

template <class T>
GeneratorFunctionBitStatistics<T> RunOld(clsNewWork<T> NewWork){
    GeneratorFunctionBitStatistics<T> BSMT(NewWork.power2);
    SieveGenerator<uint64_t> Sieve;
    Sieve.Threads(100);
    // Sieve.Threads(12);

    Sieve.ResetClock();
    Sieve.WorkMT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    // Sieve.WorkMT( NewWork.newBegin_,  NewWork.newEnd_, BSMT);
    
    Log::out() << "Duration old [s]: " <<  utils_str::FormatNumber(Sieve.DurationSeconds(), 10, 1) << "\n";
    return BSMT;
}

template <class T>
GeneratorFunctionBitStatistics<T> RunNew(clsNewWork<T> NewWork){
    NewWork.LogHeader();
    Sieve2Generator<T> Sieve;
    Sieve.Threads(100);
    // Sieve.Threads(12);

    GeneratorFunctionBitStatistics<T> BSMT(NewWork.power2);
    Sieve.ResetClock();
    BSMT.ResetClock(); //???
    Sieve.Work2MT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
    // Sieve.Work2MT( NewWork.newBegin_,  NewWork.newEnd_, BSMT);
    
    Log::out() << "Duration new [s]: " <<  utils_str::FormatNumber(Sieve.DurationSeconds(), 1, 1) << "\n";
    return BSMT;
}

template <class T>
bool RunCheckComputationNew(){
    constexpr unsigned int C_TestPower2 = 30;    
    constexpr long double C_StandardCheckDuration = 273.6;
    constexpr long double C_StandardCheckDurationDebug = 283.3;  
    // clang generates smaller code size
    // clang    10:     283.3   debug
    // gcc      11.1:   287.4   debug
    // gcc      11.1:   276.2   278.8
    // clang    10      273.6
    
    #ifdef NDEBUG
    // minimum in order to make 8 cores sweaty
    constexpr uint64_t C_CheckLength = static_cast<unsigned long long>(1) << C_TestPower2;  
    #else
    constexpr uint64_t C_CheckLength = static_cast<unsigned long long>(1) << C_TestPower2;  
    #endif

    Log::out() << "Running check computation (better safe than sorry).\n";
        
    clsNewWork<T> NewWork;    
    NewWork.power2 = 63;
    // NewWork.power2 = 50;    
    NewWork.new_begin = static_cast<unsigned long long>(1) << (NewWork.power2-1) ;
    NewWork.new_end   = NewWork.new_begin + C_CheckLength;
    NewWork.new_begin = NewWork.new_begin - C_CheckLength;
    
    GeneratorFunctionBitStatistics BSMT = RunNew(NewWork);

    // Sieve.WorkMT(NewWork.Offset() + NewWork.newBegin_, NewWork.Offset() + NewWork.newEnd_, BSMT);
    // Sieve.Work2MT(NewWork.Offset() + NewWork.newBegin_, NewWork.Offset() + NewWork.newEnd_, BSMT);

    long double duration = BSMT.DurationSeconds();
    Log::out() << "Duration [s]: " <<  utils_str::FormatNumber(duration, 10, 1);
    Log::out() << " \t~" << utils_str::FormatNumber( (100.0L * duration)/C_StandardCheckDuration, 4,1)  << " % (release)";
    Log::out() << " \t~" << utils_str::FormatNumber( (100.0L * duration)/C_StandardCheckDurationDebug, 4,1)  << " % (debug)" << "\n";

    // log correct expectation to the log file
    // for(unsigned int i=0; i<=64; i++){
    //     Log::out() << "BSMT.CntPrimesWithNumOfSetBits(" << i << ")==    " << BSMT.CntPrimesWithNumOfSetBits(i) << " &&\n";
    // } 

    return CheckReferenceResults(NewWork, BSMT);
}

template <class T>
void CompareOldNew(clsNewWork<T> NewWork){
    NewWork.LogHeader();
    auto O = RunOld(NewWork);
    Log::out() <<"\n";
    auto N = RunNew(NewWork);

    for(unsigned int i=0; i<=64; i++){
        if (O.CntPrimesWithNumOfSetBits(i) != N.CntPrimesWithNumOfSetBits(i)) {
            Log::out() << "Difference old new: " << i << " " << O.CntPrimesWithNumOfSetBits(i) << "!=    " << N.CntPrimesWithNumOfSetBits(i) << "\n";
        }
    } 

}

bool RunCheckComputation(){
    return RunCheckComputationNew<uint64_t>();
}

bool RunCornerCases(){
    
    std::vector<TestCase<uint64_t>> TC64 = {
        {-1,0,0,0}, {-1,1,1,0}, 
        {-1,2,2,1}, {-1,3,3,1}, {-1,4,4,0},
        {-1,1,29,9}, {-1,2,100,12}, 
        {-1,9699689,9699689,0}, {-1,9699690,9699690,0}, {-1,9699689,9699690,0},
        {31, (static_cast<uint64_t>(1) << 31) -1, (static_cast<uint64_t>(1) << 31) + 15, 1 }, 
        {32, 0, 15, 1} 
    };

    std::vector<TestCase<uint128_t>> TC128 = {
        {-1,0,0,0}, {-1,1,1,0}, 
        {-1,2,2,1}, {-1,3,3,1}, {-1,4,4,0},
        {-1,1,29,9}, {-1,2,100,12}, 
        {-1,9699689,9699689,0}, {-1,9699690,9699690,0}, {-1,9699689,9699690,0},
        {31, (static_cast<uint64_t>(1) << 31) -1, (static_cast<uint64_t>(1) << 31) + 15, 1 }, 
        {32, 0, 15, 1},
        {63, (static_cast<uint64_t>(1) << 63) -1, (static_cast<uint64_t>(1) << 63) + 0, 0},
        {63, (static_cast<uint64_t>(1) << 63) -1, (static_cast<uint64_t>(1) << 63) + 10, 0}, 
        {64, 0, 0, 0},
        {64, 0 , 1, 0},
        {64, 0 , 13, 1},
        {64, 13 , 13, 1},
        {64, 0 , 100, 5},
        {127, 0 , 100, 3}
    };

    CheckCornesCases<uint64_t>(TC64);
    CheckCornesCases<uint128_t>(TC128);

    return true;
}