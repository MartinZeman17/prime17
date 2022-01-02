/*
    Constructor prepares Eratosthenes sieve with defined primorial
    Work method takes 128 bit range and repeatedly calls GeneratorFunction->GenFunct(uint128_t) 
    Sieve stops when generator function returns non zero
*/

#ifndef SIEVE2GENERATOR_HPP
#define SIEVE2GENERATOR_HPP

#include "int128_t.hpp"
#include "GeneratorFunction.hpp"
#include "Log.hpp"
#include "utils_str.hpp"
#include "utils_mpz.hpp"

using namespace primecount;

#include <vector>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

// 23# fits into uint_32 datatype (36 495 360 coprimes to primorial 223 092 870)
// const std::vector<unsigned long long> CPrimorial_s({2,6,30, 210, 2310, 30030, 510510, 9699690, 223092870, 6469693230, 200560490130});
// const std::vector<unsigned long long> CTestArray_Sizes({1,2,8,48, 480, 5760, 92160, 1658880, 36495360, 1021870080, 30656102400});
// const std::vector <long double> C_Effectivity({0.5, 0.333333, 0.266667, 0.228571, 0.207792,  0.191808, 0.180525, 0.171024, 0.163588, 0.157947, 0.152852});

// use Eratosthenes sieve with the following primorial
// bigger primorial = better efficiency, however at the end some threads have nothing to do
const unsigned int C_Sieve2GeneratorPrime1=19;
const unsigned int C_Sieve2GeneratorPrime2=23;

constexpr unsigned int C_Prime1 = 19;
constexpr unsigned int C_Prime2 = 23;

constexpr size_t CTestArray_Coprimes19 = 1658880;  // primorial   9 699 690;
constexpr size_t CTestArray_Coprimes23 = 36495360; // primorial 223 092 870;


template <class T> 
class Sieve2Generator {
    private:
    unsigned int Threads_;

    public:
    class PrintProgressBar{
        private:
        Sieve2Generator & parent_;  
        enum class enPrintProgress : char{
            Untouched ='-',
            LeftPart = '<',
            RightPart = '>',
            InProgress = '*',
            Done = '#',
            ToBeProcessed = '_',
            NullTermination = '\0'
        };

        unsigned int iPId_;
        std::vector<enPrintProgress> PrintedProgress_;

        public:
        PrintProgressBar() = delete;
        PrintProgressBar( Sieve2Generator<T> & parent, const unsigned int iPId) noexcept : parent_{parent}, iPId_{iPId} {};
        long double inline ComputePercent(const T Begin, const T End, const T X) const noexcept;
    
        void Update(const unsigned int PId, const long double BPerc, const long double EPerc) noexcept;
        void UpdateOverallProgress(const long double Perc) noexcept;
    };

    enum class enSieveLength{
        SieveNotInitialized = 0,
        SieveShort,
        SieveLong 
    };

    private:
    unsigned int MaxPrime1_ = C_Prime1;
    unsigned int MaxPrime2_ = C_Prime2;  
    uint32_t Primorial1_;
    uint32_t Primorial2_;    
    uint32_t * TestArray1_ = nullptr;
    uint32_t * TestArray2_ = nullptr;
    size_t TestArrayCount1_ = CTestArray_Coprimes19;
    size_t TestArrayCount2_ = CTestArray_Coprimes23;

    // selected version for furter processing
    // change has not an immediate effect, it is rather a wish for further processing
    // all threads should store its own copies and not access this set as it si subject to change of other threads    
    // all changes by threads are protected by Untouched_mutex_. Changes by Constructor does not need locking.
    T Untouched_;
    std::mutex Untouched_mutex_;
    enSieveLength SieveLength_ = enSieveLength::SieveNotInitialized;
    uint32_t * TestArray_=nullptr;
    size_t TestArrayCount_=0;
    uint32_t Primorial_=0;
    uint32_t MaxPrime_=0;
    T SwitchPointOffset_ = 0; // 0 no switch point = start with shorter sieve since the beginning
    T SwitchPoint_ = 0;
    T SetSwitchPoint(const T & Begin, const T & End, unsigned int processor_count);
    
    long double Percent_;
    T FullyCompleted_ = 0;  // how many numbers to process => growing from 0 to End - Begin + 1
    std::mutex FullyCompleted_mutex_;


    uint32_t * SieveInit(const unsigned int SieveMaxPrime, uint32_t & MaxPrime, uint32_t & Primorial, size_t & CoprimesCount);    
    void PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const;
    // void PrintProgress2(const unsigned int PId, const long double BPerc, const long double EPerc, bool Finished) noexcept;
    // long double inline ComputePercent(const T Begin, const T End, const T X) const noexcept;

    uint32_t* BinarySearch(uint32_t BinarySearchValue, uint32_t * Array, size_t ArrayCount) const noexcept;

    T Work2MT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract<T>> & GF, const std::string PId);
    std::chrono::time_point<std::chrono::high_resolution_clock> BeginTime_; //  = std::high_resolution_clock::now(); 

    
    
    std::mutex cout_mutex_;

    std::chrono::time_point<std::chrono::high_resolution_clock> StartTime_;

    // unsigned int Threads_;

    public:
    // enSieveLength GetSieveLength() const noexcept { return SieveLength_; }
    enSieveLength SetSieveLength(const enSieveLength DesiredSieveLength) noexcept;
    // uint32_t Primorial() const noexcept { return Primorial_; }

    Sieve2Generator();
    ~Sieve2Generator();

    // T Work(const T & Begin, const T & End, GeneratorFunctionAbstract & GF) const;
    // multithreading version
    T Work2MT(const T & Begin, const T & End, GeneratorFunctionAbstract<T> & GF);


    // const unsigned int & MaxPrime = pMaxPrime_;
    // // unsigned long long Primorial() const {return pPrimorial_;};  
    // const uint32_t & Primorial = pPrimorial_;

    unsigned int Threads(const unsigned int Percent);

    void ResetClock() noexcept;
    long double DurationMinutes() const noexcept;
    long double DurationSeconds() const noexcept;

};

// #include "SieveGeneratorT.cpp"


// called from constructor and threads where Untouched_mutex_ locks other threads and prevents changes
// for more complex scenarios consider an independent lock or better  recursivemutex_
template <class T>
typename Sieve2Generator<T>::enSieveLength Sieve2Generator<T>::SetSieveLength(const Sieve2Generator<T>::enSieveLength DesiredSieveLength) noexcept {
    assert(DesiredSieveLength != Sieve2Generator<T>::enSieveLength::SieveNotInitialized); 
    
    if (DesiredSieveLength != SieveLength_) {
        if (DesiredSieveLength == Sieve2Generator<T>::enSieveLength::SieveShort) {
            if (TestArray1_ != nullptr) {   
                TestArray_ = TestArray1_;
                TestArrayCount_ = TestArrayCount1_;
                Primorial_ = Primorial1_;
                MaxPrime_ = MaxPrime1_;
                SieveLength_ = Sieve2Generator<T>::enSieveLength::SieveShort;
                // Log::out() << "Switch to short sieve.\n";
            }
        } 
        else if (DesiredSieveLength == Sieve2Generator<T>::enSieveLength::SieveLong){
            if (TestArray2_ != nullptr) {   
                TestArray_ = TestArray2_;
                TestArrayCount_ = TestArrayCount2_;
                Primorial_ = Primorial2_;
                MaxPrime_ = MaxPrime2_; 
                SieveLength_ = Sieve2Generator<T>::enSieveLength::SieveLong;
                // Log::out() << "Switch to long sieve.\n";            
            }
        }
    }
    return SieveLength_;
};

template <class T>
Sieve2Generator<T>::Sieve2Generator(){
    ResetClock();

    SieveLength_ = enSieveLength::SieveNotInitialized;

    TestArray1_ = SieveInit(C_Prime1, MaxPrime1_, Primorial1_, TestArrayCount1_);
    SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveShort);

    TestArray2_ = SieveInit(C_Prime2, MaxPrime2_, Primorial2_, TestArrayCount2_);
    SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveLong);

    assert(SieveLength_ != enSieveLength::SieveNotInitialized);
}

template <class T>
Sieve2Generator<T>::~Sieve2Generator(){
    free(TestArray1_);
    TestArray1_ = nullptr;
    free(TestArray2_);
    TestArray2_ = nullptr;
}

template <class T> 
uint32_t * Sieve2Generator<T>::SieveInit(const unsigned int SieveMaxPrime, uint32_t & MaxPrime, uint32_t & Primorial, size_t & CoprimesCount) {

    assert(MaxPrime == C_Prime1 || MaxPrime == C_Prime2);
    
    MaxPrime = SieveMaxPrime;
    if (MaxPrime == 19) {
        CoprimesCount = CTestArray_Coprimes19;
    } else if ( MaxPrime == 23 ) {
        CoprimesCount = CTestArray_Coprimes23;
    }

    // Allocate one ore position and put 0 just after the end of coprimes array.
    // This memory is not expected to be accessed by standard algorithm (only pointer may point to this memmory).
    // However in a special corner-case this memory is accessed when computing progress percentage.
    // Debuger may also read this memory
    uint32_t * DestArr = (uint32_t *) malloc((CoprimesCount+1) * sizeof(uint32_t)); 


    mpz_t mpz_primor;
    mpz_t mpz_res;
    mpz_init2(mpz_primor,64);
    mpz_init2(mpz_res,64);
    mpz_primorial_ui(mpz_primor, MaxPrime); 

    assert(mpz_cmp_ui(mpz_primor, UINT32_MAX) <= 0); //SieveGenerator cannot initialize for primorial exceding 32 bits.
    Primorial = (uint32_t) utils_mpz::mpz_get<unsigned long long>(mpz_primor); // fits into 32 bits

    // fill Test array with coprimes to primorial
    unsigned long long TACount = 0;
    uint32_t * iter = DestArr; 
    for (uint32_t i = 1; i<Primorial;){
        mpz_gcd_ui(mpz_res, mpz_primor, i);
        if(mpz_cmp_ui(mpz_res,1) == 0) {
            (*iter) = i;
            ++iter;
            TACount++;
        }
        i+=2;
    }
    *(iter) = 0;  // put 0 after the coprimes array as a last item
    assert(TACount==TestArrayCount1_ || TACount==TestArrayCount2_);

    mpz_clear(mpz_primor);
    mpz_clear(mpz_res);

    long double MeasuredEffectivity = 100.0L * (long double) TACount / (long double) Primorial;

    Log::out() << "Sieve Prime:    " << MaxPrime << "\n";
    Log::out() << "Primorial:      " << utils_str::FormatUInt(Primorial) << "\n";
    Log::out() << "Coprimes #:     " << utils_str::FormatUInt(TACount) << "\n";
    Log::out() << "Effectivity:    " << MeasuredEffectivity  << "%\n"; 

    return DestArr;
}

template <class T>
unsigned int Sieve2Generator<T>::Threads(const unsigned int Percent){
    const auto processor_count = std::thread::hardware_concurrency();
    Threads_ = (unsigned int) round(( Percent * processor_count) / 100.0);
    if (Threads_>processor_count) Threads_ = processor_count;
    if (Threads_<=0) Threads_ = 1;
    return Threads_;
}

template <class T>
void Sieve2Generator<T>::ResetClock() noexcept {
        BeginTime_ = std::chrono::high_resolution_clock::now(); 
 }

template <class T>
long double Sieve2Generator<T>::DurationMinutes() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    return duration.count() / (1000.0l * 60.0l);
}

template <class T>
long double Sieve2Generator<T>::DurationSeconds() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    return duration.count() / (1000.0l);
}




template <class T>
void Sieve2Generator<T>::PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const{
    assert(Percent>=0.0);
    assert(Percent<=100.0);
    if (Log::out().win_right == nullptr) return;
    
    unsigned int rows, cols;				/* to store the number of rows and the number of colums of the screen */
    getmaxyx(Log::out().win_right, rows, cols);	    	/* get the number of rows and columns */
    unsigned int row = rows - (Threads_ - PId);
    row = 0 + PId;

    constexpr unsigned int C_MinRightWindowLength = 20; 
    if (cols >= C_MinRightWindowLength) {
        unsigned int ProgressBarCnt = cols - C_MinRightWindowLength;

        std::stringstream RowMsg;
        RowMsg << utils_str::FormatNumber(PId, 2, 0) << ":" ;
        unsigned int Chars = (unsigned int) ((Percent * ProgressBarCnt) / 100.0);
        for(unsigned int i = 0; i< Chars; i++){
            RowMsg << "#";
        }
        for(unsigned int i = 0; i < ProgressBarCnt - Chars; i++){
            RowMsg << "_";
        }

        RowMsg << utils_str::FormatNumber(Percent, 4, 1);
        RowMsg << "% ";
        if (MinTillEnd>0) {
            RowMsg << utils_str::FormatNumber(MinTillEnd, 4, 1);
            RowMsg << "m ";
        }

        {
            // const std::lock_guard<std::mutex> lock(cout_mutex_); 
            mvwaddstr(Log::out().win_right, row, 0, RowMsg.str().c_str());
            wrefresh(Log::out().win_right);
        }
    }
}

// returns pointer to an item which is equal to or greater than the value beeing searched
template <class T>
uint32_t* Sieve2Generator<T>::BinarySearch(uint32_t BinarySearchValue, uint32_t * Array, size_t ArrayCount) const noexcept {

    // auto TTArrayBegin2 = high_resolution_clock::now();
    uint32_t *pCoprime;
    size_t B=0;
    size_t E=ArrayCount-1;
    size_t M; 

    assert(BinarySearchValue >= Array[B]);
    assert(BinarySearchValue <= Array[E]);

    do {
        //M = B + (E - B) / 2; // safer code with respect to possible integer overflow in binary search, not needed here
        M = (E + B) / 2;  // no overflow can happen for the coprimes number is ~1.7 mio for 19# and ~36.5 mio for 23#
        if (Array[M] < BinarySearchValue){
            B=M;
        } else {
            E=M;
        }
    } while (B + 1 < E);
    // cycle was optimized as much as possible with cost of an extra step outside of a loop
    // for correct inputs == operator should be ok, but if value beeing searched is smaller than the first item, the first item is returned
    if (Array[B] >= BinarySearchValue) { 
        pCoprime = &Array[B];            
    } else {
        pCoprime = &Array[E];
    }

    // auto TTArrayDuration2 = duration_cast<milliseconds>(high_resolution_clock::now() - TTArrayBegin2);
    // if (TTArrayDuration2.count() > 0)
    // {
    //     // method cannot be const
    //     const std::lock_guard<std::mutex> lock4(cout_mutex_);
    //     Log::out() << "> Coprimes array binary seek: " << TTArrayDuration2.count() << " ms\n";
    // }
    return pCoprime;
}

template <class T>
long double inline Sieve2Generator<T>::PrintProgressBar::
ComputePercent(const T Begin, const T End, const T X) const noexcept {
    assert(Begin<=End); // Begin = End is a valid input 
    assert(Begin<=X);
    // if (!(Begin<=X)){
    //     return 0;
    // }
    assert(X<=End);
    // if (Begin == End )  return 0.0L;
    long double NewPerc = (100.0L*(long double)(X-Begin + 1)/(long double)(End-Begin + 1));

    if (NewPerc > 100.0) NewPerc = 100.0; // break on exceeding End leads to greater value than 100%
    NewPerc = (std::floor(NewPerc*10.0L)) / 10.0L;  // round to 1 decimal place and return
    return NewPerc;
}


template <class T>
void inline Sieve2Generator<T>::PrintProgressBar::
UpdateOverallProgress(const long double Perc) noexcept {

    long double MinTillEnd;
    if (Perc < 1.5) {
        MinTillEnd = 0;
    } else {
        auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - parent_.BeginTime_);
        MinTillEnd = ceill(((duration.count() / Perc) * (100.0L-Perc))/6000.0L) / 10.0L;
    }

    std::stringstream Msg;
    Msg << utils_str::FormatNumber(Perc, 4, 1);
    Msg << "% ";
    if (MinTillEnd>0) {
        Msg << utils_str::FormatNumber(MinTillEnd, 4, 1);
        Msg << "m ";
    }

    {
        const std::lock_guard<std::mutex> lock(parent_.cout_mutex_); 
        mvwaddstr(Log::out().win_right, parent_.Threads_, 0, Msg.str().c_str());
        wrefresh(Log::out().win_right);
    }

}


template <class T>
void inline Sieve2Generator<T>::PrintProgressBar::
Update(const unsigned int PId, const long double BPerc, const long double EPerc) noexcept
{
    assert(BPerc>=0.0L && BPerc<=100.0L);
    assert(EPerc>=0.0L && EPerc<=100.0L);
    assert(BPerc<=EPerc);
   
    if (Log::out().win_right == nullptr) return;
    
    unsigned int rows, cols;				/* to store the number of rows and the number of colums of the screen */
    getmaxyx(Log::out().win_right, rows, cols);	    	/* get the number of rows and columns */
    // if (rows==0) rows=0; // to supress an annoing compiler warning
    unsigned int row;
    //  = rows - ( Threads_ - PId);
    row = 0 + PId;
    if (row>=rows) return;

    constexpr unsigned int C_MinRightWindowLength = 20; 
    if ( cols >= C_MinRightWindowLength ) {
        unsigned int ProgressBarCnt = cols - C_MinRightWindowLength;

        if ( PrintedProgress_.size() != (size_t) ProgressBarCnt + 1) {
            // window resized or first call
            PrintedProgress_.resize(ProgressBarCnt + 1);
            fill(PrintedProgress_.begin(), PrintedProgress_.end()-1, enPrintProgress::Untouched);
            PrintedProgress_[ProgressBarCnt] = enPrintProgress::NullTermination;
        }

        unsigned int CharsB = (unsigned int) ((BPerc * ProgressBarCnt) / 100.0);
        unsigned int CharsE = (unsigned int) ((EPerc * ProgressBarCnt) / 100.0);
        float fCharsB = (float) ((BPerc * ProgressBarCnt) / 100.0);
        float fCharsE = (float) ((EPerc * ProgressBarCnt) / 100.0);

        unsigned int iPrintPos = 1;
        for(; iPrintPos < CharsB; iPrintPos++){
            // RowMsg << "-";
            if ( PrintedProgress_[iPrintPos-1] == enPrintProgress::InProgress 
              || PrintedProgress_[iPrintPos-1] == enPrintProgress::LeftPart
              || PrintedProgress_[iPrintPos-1] == enPrintProgress::RightPart
            ) {
                PrintedProgress_[iPrintPos-1] = enPrintProgress::Done;
            } 
            // PrintedProgress_[iPrintPos-1] = PrintedProgress_[iPrintPos-1] == enPrintProgress::InProgress ? enPrintProgress::Done : enPrintProgress::Untouched;
        }

        if ( fCharsB - (float) CharsB > 0.05 ) {
            // RowMsg << "<";
            PrintedProgress_[iPrintPos-1]=enPrintProgress::LeftPart;
            iPrintPos++;
        }

        for ( ; iPrintPos < CharsE; ) {
            // RowMsg << "*";
            PrintedProgress_[iPrintPos-1]=enPrintProgress::InProgress;
            iPrintPos++; 
        }

        {
            float fEndDiff = fCharsE - (float) CharsE;
            if ( fEndDiff > 0.05 ) {
                if ( fEndDiff < 0.95 ) {
                    // RowMsg << ">";
                    PrintedProgress_[iPrintPos-1]=enPrintProgress::RightPart;
                } else {
                    // RowMsg << "*";
                    PrintedProgress_[iPrintPos-1]=enPrintProgress::InProgress;
                }
                iPrintPos++;
            }
        }

        for ( ; iPrintPos <= (unsigned) ProgressBarCnt; ) {
            // RowMsg << "_";
            PrintedProgress_[iPrintPos-1]=enPrintProgress::ToBeProcessed;
            iPrintPos++;
        }
    }

    std::stringstream PidMsg;
    PidMsg << utils_str::FormatNumber(PId, 2, 0) << ":" ;

    std::stringstream PercMsg;
    PercMsg << utils_str::FormatNumber(EPerc, 4, 1);
    PercMsg << "% ";
    // if (MinTillEnd>0) {
    //     RowMsg << utils_str::FormatNumber(MinTillEnd, 4, 1);
    //     RowMsg << "m ";
    // }

    {
        const std::lock_guard<std::mutex> lock(parent_.cout_mutex_);
        mvwaddstr(Log::out().win_right, row, 0, PidMsg.str().c_str());
        waddstr(Log::out().win_right, (const char *) static_cast<const void*>(PrintedProgress_.data()));
        waddstr(Log::out().win_right, PercMsg.str().c_str());
        wrefresh(Log::out().win_right);
    }

}



template <class T>
T Sieve2Generator<T>::Work2MT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract<T>> & GF, const std::string PId) {
    auto TBegin = high_resolution_clock::now();

    mpz_t mpz_kp, mpz_X;

    #undef RETURN
    #define RETURN(n)         \
    {                         \
        mpz_clear(mpz_kp);    \
        mpz_clear(mpz_X);     \
        return(n);            \
    }

    unsigned int iPId = (unsigned int) std::stoul(PId);

    mpz_init2(mpz_kp, 65);
    mpz_init2(mpz_X, 65);
    T res = 0;

    PrintProgressBar ppb(*this, iPId); 

    // perform an overflow test 
    {
        T OverflowTest1 = std::max(Primorial1_, Primorial2_); 
        T OverflowTest2 = OverflowTest1 + End;
        // prevent overflow in End + 1, End + Primorial_     
        if (!(OverflowTest2 > End && OverflowTest2 > OverflowTest1 && End + 1 > End)) {
            {
                const std::lock_guard<std::mutex> lock(cout_mutex_);
                Log::out() << "Input parameters would lead to an overflow in a sieve algorithm. Use template with bigger data type if possible (e.g. uint128_t). Mission aborted.\n";
                abort();
            }
        }
    }

    if (Begin> End) {
        {
            const std::lock_guard<std::mutex> lock(cout_mutex_);
            Log::out() << "Begin greater than End. Mission aborted.\n";
            abort();
        }
    }

    while(true) {
        // local thread copies. Untouched_ is locked, so it is used directly (main purpose is to update Untouched_).  
        T Untouched=0;
        enSieveLength SieveLength;
        uint32_t * TestArray;
        size_t TestArrayCount;
        uint32_t Primorial;
        uint32_t MaxPrime;

        // ordinary local variables
        T kp;
        T ToProcess;

        // Lock variables, update Untouched and fill in local variables
        {
            const std::lock_guard<std::mutex> lock(Untouched_mutex_);

            // evaluate break condition first
            if (Untouched_ > End) break;  // ??? is this needed ???
 
            if (Untouched_ == Begin) StartTime_ = high_resolution_clock::now();

            // change of sieve length?
            SieveLength = SieveLength_;
            T SwitchPoint = SwitchPointOffset_ + Begin;  // overflow test is not needed (SwitchPointOffset_ + Begin <= End + Primorial)
            if (Untouched_ + Primorial_ > SwitchPoint) {  // going to exceed switch point in this run
                if (SieveLength != enSieveLength::SieveShort) {
                    SieveLength = SetSieveLength(enSieveLength::SieveShort);
                    {
                        const std::lock_guard<std::mutex> lock(cout_mutex_);
                        long double Perc = 100.0L* ( (long double)(Untouched_-Begin) / (long double)(End-Begin) );                        
                        Perc = (std::floor(Perc*10.0L))/10.0L;
                        Log::out() << PId << "> " <<"Switching sieve to: " << (unsigned int) SieveLength << " at: " <<  utils_str::FormatUInt(Untouched_)  << " = ~" << utils_str::FormatNumber(Perc, 4, 1) <<"%\n";
                        // << " SwitchPoint: " << utils_str::FormatUInt(SwitchPoint) << "\n";
                    }
                }
            }
            //fill in local thread copies of Sieve params
            TestArray = TestArray_;
            TestArrayCount = TestArrayCount_;
            Primorial = Primorial_;
            MaxPrime = MaxPrime_;

            ToProcess = Primorial;  // assume whole primorial range should be processed
            
            T k = Untouched_ / Primorial;
            kp = k * Primorial;

            T tmp = kp + TestArray[TestArrayCount-1];
            if (tmp < Untouched_) {
                // Untouched are is very close to the next multiple of Primorial 
                // so whole TestArray would be exhausted and pCoprime would out be of bounds
                // all numbers between kp + last item of TestArray and the next multiple of primorial must have a common divisor (=>not primes) 
                // -> go directly to the next primorial and start with the first Coprime 
                // The probability is very low: Primorial-1 and Primorial are typically coprimes and so Primorial-1 should be the last TestArray number.
                // However it happens when Begin is Multiple of Primorial decreased by one. ToDo Test it.
                kp += Primorial;
                ToProcess += (kp - Untouched_);
                Untouched_ = kp;
                {
                    const std::lock_guard<std::mutex> lock(cout_mutex_);
                    Log::out() << PId << "> " << "Begin after last TestArray item. Wow !!! Is Begin multiple of Primorial decreased by 1?" << "\n";
                }
            }
            // load local copy of Untouched
            Untouched = Untouched_;
            // Update Untouched_ for another thread to process 
            Untouched_ = kp + Primorial; // by the way it is not a prime
            // overflow test has been performed at the beginning
            assert(Untouched_ > Untouched);
        } 
        // End of lock.
        // Untouched_ is updated, Untouched, kp and Sieve are set (having local copies to work on), Sieve could have switched
        // Do not use underscored variables like Untouched_ in the following code 

        // {
        //     const std::lock_guard<std::mutex> lock(cout_mutex_);
        //     Log::out() << PId << "> " << "Sieve: " << (unsigned int) SieveLength << " k: " << utils_str::FormatUInt(k) << " kp: " << utils_str::FormatUInt(kp) << " Untouched: " << utils_str::FormatUInt(Untouched) << "\n";
        // }

        uint32_t *pCoprime;

        // set the beginning for the main cycle
        //  binary search searching for value equal to or greater than BinarySearchValue, store output in pCoprime
        {
            if (kp == Untouched) {
                pCoprime = TestArray;
            } else { 
                // kp < Untouched, i.e. starting with smaller primorial, TestArray do not have to be processes in its entirety. 
                // Let's skip beginning part of TestArray and move to the first TestArray item greater than or equal to Untouched
                // Test whether Untouched is greater than the last TestArray item has been done
                auto TTArrayBegin2 = high_resolution_clock::now();
                uint32_t BinarySearchValue = (uint32_t) (Untouched - kp);
                ToProcess -= BinarySearchValue;  // whole primorial range was supposed to be processed so decrease it by the length of the skipped range
                pCoprime = BinarySearch(BinarySearchValue, TestArray, TestArrayCount);

                auto TTArrayDuration2 = duration_cast<milliseconds>(high_resolution_clock::now() - TTArrayBegin2);
                if (TTArrayDuration2.count() > 0) {
                    const std::lock_guard<std::mutex> lock(cout_mutex_);
                    Log::out() << PId << "> Coprimes array binary seek: " << TTArrayDuration2.count() << " ms\n";
                }
            }
        }


        // set the end condition for the main cycle
        uint32_t * pCoprimeDoNotTest;  // pointer signaling end of loop
        T EndForPrint;     // should be updated to the last number to be tested
        EndForPrint =  kp + TestArray[TestArrayCount-1];
        // set an end condition for while loop
        if (EndForPrint <= End) {
            EndForPrint = kp + (Primorial - 1);
            pCoprimeDoNotTest = &TestArray[TestArrayCount];  // Pointer to an invalid memory beyond last array item!!! This memory must not be read.
        } else { // last coprime + kp sharply exceeds End
            EndForPrint = End;
            uint32_t BinarySearchValue = (uint32_t) (End - kp);    // <0, Primorial - 1>        
            ToProcess -= (Primorial - BinarySearchValue - 1);
            if (BinarySearchValue < *TestArray) { // End may be before kp + 1 (first coprime)
                // in such a case while loop must not test any coprime (so set pCoprimeDoNotTest to the beginning of TestArray)
                pCoprimeDoNotTest = TestArray;
            } else {
                // find first greater (or equal to) coprime
                pCoprimeDoNotTest = BinarySearch(BinarySearchValue, TestArray, TestArrayCount);
                // if End matches some particular coprime exactlyn(and so it was found), move pointer in order to ensure such coprime will be tested
                if (*pCoprimeDoNotTest == BinarySearchValue) {
                    pCoprimeDoNotTest++;
                }
            }
        }

        {
            long double PercB = ppb.ComputePercent(Begin, End, Untouched);
            long double PercE = ppb.ComputePercent(Begin, End, EndForPrint);
            ppb.Update(iPId, PercB, PercE);
        }

        // set mpz_kp from kp
        // The branch that fails the constexpr condition will not be compiled for the given template instantiation. Or I hope so...
        // But it looks like constexpr is not needed, both binaries are identical.
        if constexpr(std::is_same<T, uint64_t>::value) { 
            utils_mpz::mpz_set_ull(mpz_kp, kp);
        } else if constexpr(std::is_same<T, uint128_t>::value){
            utils_mpz::mpz_set_ul128(mpz_kp, kp);
        } else {
            assert(false);
            abort();
        }

        // test primorial primes
        // MaxPrime = 2, Primorial = 2, TestArray = [1] -> ??? ToDo test
        // Beg = 0, k=0 - should test 0+1, k=1, should test 1*2+1, k=3  
        assert(Primorial>MaxPrime);
        // ToDo c++20 constexpr vector
        assert(MaxPrime<=43);
        static const std::vector <unsigned long long> CPrimorial2_Primes({2,3,5,7,11,13,17,19,23,29,31,37,41,43});
        if (Untouched <= MaxPrime) {
            // test all primes from primorial construction between Begin and End as the algorithm does not test them
            for (auto & Prime : CPrimorial2_Primes) {
                if (Untouched<=Prime) {
                    if (Prime <= End && Prime <= MaxPrime) {
                        utils_mpz::mpz_set_ull(mpz_X, Prime);
                        if (GF->GenFunct((T) Prime, mpz_X)!=0) {
                            res = Prime;
                            RETURN(res);
                        }
                    } else {
                        break;
                    }                
                }
            }
            {
                const std::lock_guard<std::mutex> lock(cout_mutex_);
                Log::out() << PId << "> " << "Primorial primes tested" << "\n";
            }
        } 

        // this is the core functionality consuming almost all of the resources, be carefull
        while (pCoprime != pCoprimeDoNotTest){
            uint32_t Coprime = *pCoprime; //TestArray[i];
            pCoprime++;
            T X = kp + Coprime;
            mpz_add_ui(mpz_X, mpz_kp, Coprime);
            GF->GenFunct(X, mpz_X);
            // if (GF->GenFunct(X, mpz_X)!=0) {
            //     res = X;
            //     RETURN(res);
            // };
        }

        // update progress
        {
            const std::lock_guard<std::mutex> lock(FullyCompleted_mutex_);
            FullyCompleted_ += ToProcess;
        }

        long double NewPerc = ppb.ComputePercent(Begin, End, Begin + FullyCompleted_ - 1);
        ppb.UpdateOverallProgress(NewPerc);
    }

    auto TEnd = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(TEnd - TBegin);

    //if (duration.count()!=0)
    {
        const std::lock_guard<std::mutex> lock(cout_mutex_);
        Log::out() << "Progress   :    " << FullyCompleted_ << "\n"; 
        Log::out() << PId << "> Sieve (";
        if constexpr(std::is_same<T, uint64_t>::value) {  
            Log::out() << "64";
        } else if constexpr(std::is_same<T, uint128_t>::value){
            Log::out() << "128";
        } else {
            assert(false);
            abort();
        }
        Log::out() << "-bit) duration: " << duration.count() << " s = " << utils_str::FormatNumber( (float) duration.count() / 60.0f, 1,1) << " m" << "\n";
    }

    RETURN(0);

}



template <class T>
T Sieve2Generator<T>::SetSwitchPoint(const T & Begin, const T & End, unsigned int threads){
    
    SwitchPointOffset_=0;   
    SwitchPoint_=0; 
    
    // if (threads <= 1 || Primorial1_ == 0 || Primorial2_ == 0) {
    // no competition, use large sieve as it is more effective
    // however for old and slow CPUs short sieve might be preferable due to more frequent updates

    if (Primorial1_ != 0 && Primorial2_ != 0) {
        T SPOffset;
        T Cnt2perCPU;
        T Primorials2 = Begin / Primorial2_;
        if (Primorials2 * Primorial2_ < Begin )  Primorials2++;
        T BeginOffset2 = Primorials2 * Primorial2_;  // overflow ???

        // overflow, Begin is near TMax barrier
        if (BeginOffset2 >= Begin) {
            T Len = 0;
            if (BeginOffset2 < End) {
                Len = ((End - BeginOffset2) + 1);
            }
            BeginOffset2 -= Begin; // make it an offset from the Beginning 

            T Cnt2 =  Len / Primorial2_;
            assert(threads>0);
            Cnt2perCPU = Cnt2 / threads;

            SPOffset =  Cnt2perCPU * Primorial2_ * threads;
            
            
            // if BeginOffset2 is too close to Primorial, may be harmful
            // ToDo: fine-tuning for very small BeginOffset2 Compare with ending part
            // if (SPOffset > 0) SPOffset += BeginOffset2; 
            if (SPOffset>0) {
                SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveLong);
            } else {
                SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveShort);
            }

            // long double SmallPrimorials = ((((End - (SPOffset + Begin) + 1)) / (long double) Primorial1_) ) /  (long double) threads; 
            long double SmallPrimorials = ((  (long double) ((End - (SPOffset + Begin) + 1)) / (long double) Primorial1_) ) /  (long double) threads; 
            // Log::out() << "Each core is supposed to process " << Cnt2perCPU << " large primorial(s) and ~" << utils_str::trim_copy(utils_str::FormatNumber(SmallPrimorials, 3,1)) << " small primorials.\n";
            Log::out() << "Each core:      " << Cnt2perCPU << " large + ~" << utils_str::trim_copy(utils_str::FormatNumber(SmallPrimorials, 3,1)) << " small primorial(s)\n";


            SwitchPointOffset_ = SPOffset; 
            SwitchPoint_ =  SPOffset + Begin;
        }
       

    }
    Log::out() << "Sieve switch:   " << utils_str::FormatUInt(SwitchPoint_) << " offset: " << utils_str::FormatUInt(SwitchPointOffset_) << "\n";
    return SwitchPointOffset_;
}


template <class T>
T Sieve2Generator<T>::Work2MT(const T & Begin, const T & End, GeneratorFunctionAbstract<T> & GF) {
    ResetClock();
    T res = 0;

    const auto processor_count = std::thread::hardware_concurrency();
    // Log::out() << "Using " << (unsigned int) (100 * Threads_)/processor_count << "% CPU = spawning " << Threads_ << " thread";    
    // if (Threads_ > 1) Log::out() << "s";
    // Log::out() << " out of " << processor_count << " available logical CPUs.\n";
    
    Log::out() << "Cores usage:    " << Threads_ << " / " << processor_count << " = " << (unsigned int) (100 * Threads_)/processor_count << "% CPU\n";


    SetSwitchPoint(Begin, End, Threads_);

    Untouched_ = Begin;


    // T k = Begin/Primorial1_;
    // Tkp_ = k * Primorial1_;  // primorial p multiplied by some integer k
    Percent_=-1.0;

    Log::out()  << "Sieve Begin:    " << utils_str::FormatUInt(Begin) << "\n";        
    Log::out()  << "Sieve End  :    " << utils_str::FormatUInt(End) <<  "\n";
    Log::out()  << "End-Begin  :    " << utils_str::FormatUInt(End - Begin) << "\n";        
    if ((uint128_t) End / ( (uint128_t) UINT64_MAX+  (uint128_t)1) >1) {
        Log::out() << "A serious warning!!! End looks too big." << "\n";
    }    
   
    // Log::out() << "Primorials skipped: " << utils_str::FormatUInt(k) <<  "\n";
    // Log::out() << "Multiple of primorial: " << utils_str::FormatUInt(_kp) <<  " ";
    // Log::out() << "...till Begin: " << utils_str::FormatUInt(Begin -kp_) << "\n";
    
    Log::out().ClearRight();
   


    // prepare all clones of GeneratorFunction before starting any thread
    std::vector<std::unique_ptr<GeneratorFunctionAbstract<T>>> GFClones;
    GFClones.reserve(Threads_);
    for (unsigned int i = 0; i<Threads_; i++){
        GFClones.emplace_back(GF.clone());
    }
    // GF.ResetClock();
    // auto aaa = std::ref(GFClones[0]);
    
    if (Threads_ == 1) {
        Work2MT_Thread(Begin, End, std::ref(GFClones[0]), "0");
    } else {    

        // No two std::thread objects may represent the same thread of execution;
        // std::thread is not CopyConstructible or CopyAssignable, 
        // although it is MoveConstructible and MoveAssignable.
        // T (the container's element type) must meet the requirements of MoveInsertable and EmplaceConstructible.
        std::vector<std::thread> Threads;
        Threads.reserve(Threads_);

        // let's start all threads
        for (unsigned int i = 0; i<Threads_; i++){
            Threads.emplace_back(std::thread(&Sieve2Generator::Work2MT_Thread, this, Begin, End, std::ref(GFClones[i]), std::to_string(i)));
        }

        for (auto &TS : Threads) {
            TS.join();
        }
    }

    // consolidate all results into input GF object
    for (unsigned int i = 0; i < Threads_; i++){
        Log::out() << i << "> " << utils_str::FormatUInt(GF.PrimesCnt()) << " + " << utils_str::FormatUInt(GFClones[i]->PrimesCnt());
        GF+=(*GFClones[i]);
        Log::out() << " = " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    }

    // long double PrimesRatioAfterSieve =  100.L * GF.PrimesCnt() / (( TestArrayCount_ / (long double) Primorial1_) * (End - Begin + 1));
    long double PrimesRatioAfterSieve =  100.L * GF.PrimesCnt() / (( (long double) TestArrayCount_ / (long double) Primorial1_) * (long double)(End - Begin + 1));
    Log::out() << "Primes: " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    // Log::out() << "Primes ratio (from interval): " << utils_str::FormatNumber(GF.PrimesCnt() * 100.0L / (End - Begin + 1), 6,3) << "%\n";
    Log::out() << "Primes ratio (from interval): " << utils_str::FormatNumber(GF.PrimesCnt() * 100.0L / (long double)(End - Begin + 1), 6,3) << "%\n";
    Log::out() << "Primes ratio (after sieve)  : " << utils_str::FormatNumber(PrimesRatioAfterSieve, 6,3) << "%\n";


    Log::out() << "Multithreading Sieve Duration [m]: " << GF.DurationMinutes() <<  " [s]: " << GF.DurationSeconds() <<"\n";
 
    if (res!=0) return res; 
    return 0;
}



#endif