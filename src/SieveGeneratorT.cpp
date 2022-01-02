#include "SieveGenerator.hpp"  //ToDo: This is a bit of an enigma here, if hpp is not included, VSCode reports strange issues, nevertheless the code compiles
#include "utils_mpz.hpp"
#include "Log.hpp"
#include "utils_str.hpp"

#include <iostream>
#include <math.h>
#include <mutex>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <gmp.h>

#include <string>  
#include <sstream>  
#include <chrono>


#ifdef __MINGW64__
# include <ncurses/ncurses.h>
#else
#include <ncurses.h> 
#endif


using std::endl;
using std::cout;
using std::string;
using std::to_string;
using std::vector;

// static variable must be defined outside of header, beware: declaration is not a definition
template <class T> unsigned int SieveGenerator<T>::MaxPrime_=0;
template <class T> unsigned long long SieveGenerator<T>::TestArrayCount_ = 0;
template <class T> std::vector<uint32_t> SieveGenerator<T>::TestArray_(0);
template <class T> uint32_t SieveGenerator<T>::Primorial_=1;
template <class T> std::mutex SieveGenerator<T>::TestArray_mutex_;


template <class T>
void SieveGenerator<T>::ResetClock() noexcept {
        BeginTime_ = std::chrono::high_resolution_clock::now(); 
 }

template <class T>
long double SieveGenerator<T>::DurationMinutes() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    return duration.count() / (1000.0l * 60.0l);
}

template <class T>
long double SieveGenerator<T>::DurationSeconds() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - BeginTime_);
    return duration.count() / (1000.0l);
}

template <class T> 
void SieveGenerator<T>::Constructor(unsigned int MaxPrime) {
    // ToDo: Test with more threads  ??? is it ok with ordinary object and no smart pointers
    // array may be in process of init from other thread???
    const std::lock_guard<std::mutex> lock(TestArray_mutex_); 
    ResetClock();

    if (MaxPrime_==0){
        mpz_t mpz_primor;
        mpz_t mpz_res;
        mpz_init2(mpz_primor,64);
        mpz_init2(mpz_res,64);
        mpz_primorial_ui(mpz_primor, MaxPrime);  //ok
        if (mpz_cmp_ui(mpz_primor, UINT32_MAX) > 0 ) { //ok
            const std::string C_Msg("SieveGenerator cannot initialize for primorial exceding 32 bits. Fatal, mission aborted.\n");
            Log::out() << C_Msg;
            throw(C_Msg); //ToDo Memory leak?? How to throw or exit??
        }
        Primorial_ = (uint32_t) utils_mpz::mpz_get<unsigned long long>(mpz_primor); // fits into 32 bits

        
        // load destination array size if known prom previous computations
        unsigned long long BufferSize=0;
        // for(unsigned int  i = 0; i < CPrimorial_Primes.size() && i < C_Effectivity.size(); i++){
        for(unsigned int  i = 0; i < CPrimorial_Primes.size(); i++){
            if (CPrimorial_Primes[i]<=MaxPrime) {
                BufferSize = CTestArray_Sizes[i];  
            }
        }
        if (BufferSize==0) BufferSize = (unsigned long long)  (1.0 + (long double) Primorial_ * .15); 
        TestArray_.reserve(BufferSize);
        
        // fill Test array with coprimes to primorial
        unsigned long long TACount=0;
        for (uint32_t i = 1; i < Primorial_;){
            mpz_gcd_ui(mpz_res, mpz_primor, i); //ok, mpz_primor throw an exception
            if(mpz_cmp_ui(mpz_res,1)==0){   //ok
                TestArray_.push_back(i);
                TACount++;
            }
            i+=2;
        }

        TestArray_.shrink_to_fit();
        mpz_clear(mpz_primor);
        mpz_clear(mpz_res);

        TestArrayCount_ = TACount;        
        // pTestArrayCount_=pTestArray_.size();
        MaxPrime_ = MaxPrime;
        long double MeasuredEffectivity = 100.0L * (long double) TestArrayCount_ / (long double) Primorial_;

        Log::out() << "Sieve Prime: " << MaxPrime << " ";
        Log::out() << "Primorial: " << utils_str::FormatUInt(Primorial_) << "\n";
        Log::out() << "Coprimes #:  " << utils_str::FormatUInt(TestArrayCount_) << " ";
        Log::out() << "Effectivity    : " << MeasuredEffectivity  << "[%]\n"; 
        // TODO sanitizer complains about % sign in printf

        Threads(100);        
    }
    else if (MaxPrime_!=MaxPrime){
        throw("SieveGenerator already initialized to a different primorial. Fatal, mission aborted.");
    }
}

template <class T> 
SieveGenerator<T>::SieveGenerator() {
    Constructor(C_SieveGeneratorDefaultMaxPrime);
}

template <class T> 
SieveGenerator<T>::SieveGenerator(unsigned int MaxPrime){
    Constructor(MaxPrime);
}

/************************************************************* single thread version *************************************************************/
template <class T>
T SieveGenerator<T>::Work(const T & Begin, const T & End, GeneratorFunctionAbstract<T> & GF) const {
    mpz_t mpz_kp, mpz_X;

    #undef RETURN
    #define RETURN(n)         \
    {                         \
        mpz_clear(mpz_kp);    \
        mpz_clear(mpz_X);     \
        return(n);            \
    }

    mpz_init2(mpz_kp, 65);
    mpz_init2(mpz_X, 65);

    T k = Begin/Primorial_;
    T kp = k * Primorial_;  // primorial p multiplied by some integer k

    Log::out()  << "Sieve Begin: " << Begin << " ";        
    Log::out()  << "Sieve End:   " << End <<  " ";
    Log::out()  << "End-Begin:   " << End - Begin << "\n";        
    // Log::out()  << "End - UINT64_MAX: " << (uint128_t) End - (uint128_t) UINT64_MAX << endl;
    // Log::out()  << "End - 2 * UINT64_MAX: " << (uint128_t) End - (uint128_t) UINT64_MAX - (uint128_t) UINT64_MAX << endl;        
    // Log::out()  << "UINT64_MAX: " <<  UINT64_MAX << endl;        
    if ((uint128_t) End / ( (uint128_t) UINT64_MAX+  (uint128_t)1) >1) {
        Log::out() << "A serious warning!!! End looks too big.\n";
    }
    
    unsigned long long i=0;
    double OldPerc = -1;
    double NewPerc = 0;

    T tmp;
    tmp = TestArray_[TestArrayCount_-1] + kp; 
    if (tmp < Begin){
        // Begin is very close to the next multiple of Primorial 
        // so whole TestArray would be exhausted and variable i would out be of bounds
        // all numbers between kp + last item of TestArrayand the next multiple of primorial must have common divisor (not primes) 
        // -> go directly to the next primorial and leave i index at 0
        kp += Primorial_;
        k++;
        Log::out() << "Begin after last TestArray item. Wow !!!\n";
    } else {
        // search where to start with testing and set index i
        // for(; pTestArray_[i]+kp < Begin; i++);  // this for cycle works only for 64bit variables
        tmp = TestArray_[i]+kp;
        while (tmp < Begin){
            ++i;
            tmp = TestArray_[i]+kp;
        } 
    }

    Log::out() << "Primorials skipped: " << k << "\n";
    Log::out() << "Multiple of primorial: " << kp << "\n";
    Log::out() << "... till Begin: " << Begin - kp << "\n";

    if (std::is_same_v<T, uint64_t>) {
        utils_mpz::mpz_set_ull(mpz_kp, (uint64_t) kp);
    } else {
        utils_mpz::mpz_set_ul128(mpz_kp, kp);
    }
    
    // // check correct computation of kp
    // Log::out() << "kp:     " ;
    // Log::out() << kp << "\n;
    // Log::out() << "mpz_kp: " ;
    // mpz_out_str(stdout, 10, mpz_kp);
    // Log::out() << endl;

    if (Begin <= MaxPrime_) {
        // test all primes from primorial construction between Begin and End as the algorithm does not test them
        for (auto & Prime : CPrimorial_Primes) {
            if (Begin<=Prime) {
                if (Prime <= End && Prime <= MaxPrime_) {
                    utils_mpz::mpz_set_ull(mpz_X, Prime);
                    if (GF.GenFunct((T) Prime, mpz_X)!=0) {
                        RETURN(Prime);
                    }
                } else {
                    break;
                }                
            }
        }
        Log::out() << "Primorial primes tested" << "\n";
    }


    // auto TBegin = high_resolution_clock::now();
    // ResetClock();  //method is const 
    // i is now a valid index within the array bounds
    while(true)
    {
        T X = kp + TestArray_[i];
        mpz_add_ui(mpz_X, mpz_kp, TestArray_[i]);  //ok

        if(X > End) break;            

        // mpz_t Comp;
        // mpz_init2(Comp, 512);
        // UInt128tompz(X, Comp);
        // if (mpz_cmp(mpz_X, Comp)!=0){
        //     Log::out() << "not equal ..........." << endl;
        // }

        if (GF.GenFunct(X, mpz_X)!=0) {
            RETURN(X);
        };
                  
        i++;
        if(i == TestArrayCount_) 
        {
            i=0;
            kp += Primorial_;
            mpz_add_ui(mpz_kp, mpz_kp, Primorial_);  //ok, fits in 32 bits
            NewPerc = (100.0*(double)(kp-Begin)/(double)(End-Begin));
            NewPerc = (std::floor(NewPerc*10.0))/10.0;
            if(NewPerc !=OldPerc){
                OldPerc = NewPerc;
                Log::out()  << OldPerc << " "; // << std::flush;
            }
        }
    }
    Log::out()  << "\n";
    // auto TEnd = high_resolution_clock::now();
    // auto duration = duration_cast<milliseconds>(TEnd - TBegin);
    // Log::out() << "Sieve duration: " << duration.count() << endl;
    Log::out() << "Sieve Duration [m]: " << GF.DurationMinutes() << "\n";
    Log::out() << "\n";

    RETURN(0);
}


/************************************************************* multi-thread version *************************************************************/

template <class T>
void SieveGenerator<T>::PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const{
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
            // mvwprintw(Log::out().win_right, row, 0, RowMsg.str().c_str());
            mvwaddstr(Log::out().win_right, row, 0, RowMsg.str().c_str());
            wrefresh(Log::out().win_right);
        }
    }
}


template <class T>
T SieveGenerator<T>::WorkMT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract<T>> & GF, const std::string PId) {
    auto TBegin = high_resolution_clock::now();

    mpz_t mpz_kp, mpz_X;

    #undef RETURN
    #define RETURN(n)         \
    {                         \
        mpz_clear(mpz_kp);    \
        mpz_clear(mpz_X);     \
        return(n);            \
    }

    unsigned int iPId = (unsigned int) std::stol(PId);

    if (iPId == 0) {
        // const std::lock_guard<std::mutex> lock(StartTime_mutex_);
        StartTime_ = high_resolution_clock::now();
        // Log::out() << precision(1);
        // Log::out() << std::fixed;
    }

    mpz_init2(mpz_kp, 65);
    mpz_init2(mpz_X, 65);
    T res = 0;
    T kp;
    {
        const std::lock_guard<std::mutex> lock(kp_mutex_);
        T tmp;
        tmp = TestArray_[TestArrayCount_-1] + kp_; 
        if (tmp < Begin){
            // Begin is very close to the next multiple of Primorial 
            // so whole TestArray would be exhausted and variable i would out be of bounds
            // all numbers between kp + last item of TestArray and the next multiple of primorial must have common divisor (not primes) 
            // -> go directly to the next primorial and leave i index at 0
           kp_ += Primorial_;
            {
                const std::lock_guard<std::mutex> lock(cout_mutex_);
                Log::out() << PId << "> " << "Begin after last TestArray item. Wow !!!" << "\n";
            }
        }
        // local copy
        kp =kp_;
        // preparekp_ for another thread to process 
       kp_ += Primorial_;
    }

    // {
        // if (kp < End) {
        //     const std::lock_guard<std::mutex> lock(_Log::out()mutex_);
        //     Log::out() << PId << "> " << " k: " <<  kp / pPrimorial_ << " kp: " << kp  << endl;
        // }
    // }

    unsigned long long i=0;
    if (kp < Begin) {
        T tmp;
        
        auto TTArrayBegin = high_resolution_clock::now();

        // search where to start with testing and set index i
        // for(; pTestArray_[i]+kp < Begin; i++);  // this for cycle works only for 64bit variables

        tmp = TestArray_[i]+kp;
        while (tmp < Begin){
            ++i;
            tmp = TestArray_[i]+kp;
        }

        auto TTArrayDuration = duration_cast<milliseconds>(high_resolution_clock::now() - TTArrayBegin);
        if (TTArrayDuration.count()> 0)
        {
            const std::lock_guard<std::mutex> lock(cout_mutex_);
            Log::out() << "Coprimes array seek: " << TTArrayDuration.count() << " ms\n";
        }
    }
 



    // The branch that fails the constexpr condition will not be compiled for the given template instantiation. Or I hope so...
    // But it looks like constexpr is not needed, both binaries are identical.
    // if (std::is_same_v<T, unsigned long long>) {

    if constexpr(std::is_same_v<T, uint64_t>) {
        utils_mpz::mpz_set_ull(mpz_kp, kp);
    } else if constexpr(std::is_same_v<T, uint128_t>){
        utils_mpz::mpz_set_ul128(mpz_kp, kp);
    } else {
        assert(false);
        abort();
    }
    
    if (Begin <= MaxPrime_ && iPId == 0) {
        // test all primes from primorial construction between Begin and End as the algorithm does not test them
        for (auto & Prime : CPrimorial_Primes) {
            if (Begin<=Prime) {
                if (Prime <= End && Prime <= MaxPrime_) {
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


    // i is now a valid index within the array bounds
    while(true)
    {
        T X = kp + TestArray_[i];
        mpz_add_ui(mpz_X, mpz_kp, TestArray_[i]); //ok

        if(X > End) break;            
        
        if (GF->GenFunct(X, mpz_X)!=0) {
            res = X;
            RETURN(res);
        };
  
        i++;
        if(i == TestArrayCount_) 
        {
            // first print progress 
            long double NewPerc = (100.0L*(long double)((kp + TestArray_[i-1])-Begin)/(long double)(End-Begin));
            NewPerc = (std::floor(NewPerc*10.0L))/10.0L;
            {
                const std::lock_guard<std::mutex> lock(Percent_mutex_);
                if(NewPerc !=Percent_){
                    Percent_ = NewPerc;
                    long double MinTillEnd;
                    if (NewPerc<1) {
                        MinTillEnd = 0;
                    } else {

                        auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - StartTime_);
                        MinTillEnd = ceill(((duration.count() / NewPerc) * (100.0L-NewPerc))/6000.0L) / 10.0L;
                    }
                    {
                    // const std::lock_guard<std::mutex> lock(cout_mutex_);
                    PrintProgress(iPId,Percent_, MinTillEnd);
                    // Log::out() << MinTillEnd << " ";
                    // std::cout << flush;
                    }
                }
            }
            
            //update kp
            i=0;
            {
                const std::lock_guard<std::mutex> lock( kp_mutex_);
                kp = kp_;
               kp_ += Primorial_;
            }

            if (std::is_same_v<T, unsigned long long>) {
                utils_mpz::mpz_set_ull(mpz_kp, kp);
            } else {
                utils_mpz::mpz_set_ul128(mpz_kp, kp);
            }

        }
    }

    auto TEnd = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(TEnd - TBegin);

    if (duration.count()!=0)
    {
        const std::lock_guard<std::mutex> lock(cout_mutex_);
        Log::out() << PId << "> Sieve duration: " << duration.count() << " s = " << utils_str::FormatNumber((float) duration.count() / 60.0f, 1 ,1) << " m" << "\n";
    }

    RETURN(0);

}



template <class T>
unsigned int SieveGenerator<T>::Threads(const unsigned int Percent){
    const auto processor_count = std::thread::hardware_concurrency();
    Threads_ = (unsigned int) round(( Percent * processor_count) / 100.0);
    if (Threads_>processor_count) Threads_ = processor_count;
    if (Threads_<=0) Threads_ = 1;
    return Threads_;
}

template <class T>
T SieveGenerator<T>::WorkMT(const T & Begin, const T & End, GeneratorFunctionAbstract<T> & GF) {

    ResetClock();
    T res = 0;
    T k = Begin / Primorial_;
    kp_ = k * Primorial_;  // primorial p multiplied by some integer k
    Percent_=-1.0;

    Log::out()  << "Sieve Begin: " << utils_str::FormatUInt(Begin) << "\n";        
    Log::out()  << "Sieve End:   " << utils_str::FormatUInt(End) <<  "\n";
    Log::out()  << "End-Begin:   " << utils_str::FormatUInt(End - Begin) << "\n";        
    // Log::out()  << "End - UINT64_MAX: " << (uint128_t) End - (uint128_t) UINT64_MAX << endl;
    // Log::out()  << "End - 2 * UINT64_MAX: " << (uint128_t) End - (uint128_t) UINT64_MAX - (uint128_t) UINT64_MAX << endl;        
    // Log::out()  << "UINT64_MAX: " <<  UINT64_MAX << endl;        
    if ((uint128_t) End / ( (uint128_t) UINT64_MAX+  (uint128_t)1) >1) {
        Log::out() << "A serious warning!!! End looks too big." << "\n";
    }    
   
    Log::out() << "Primorials skipped: " << utils_str::FormatUInt(k) <<  "\n";
    Log::out() << "Multiple of primorial: " << utils_str::FormatUInt(kp_) <<  " ";
    Log::out() << "...till Begin: " << utils_str::FormatUInt(Begin - kp_) << "\n";
   
    const auto processor_count = std::thread::hardware_concurrency();
    // const size_t CThreads_ = processor_count;
    // Log::out() << "Processor count detected: " << processor_count << " Threads to spawn: " << Threads_  << "\n";
    Log::out() << "Using " << (unsigned int) (100 * Threads_)/processor_count << "% CPU = spawning " << Threads_ << " thread";
    if (Threads_ > 1) Log::out() << "s";
    Log::out() << " out of " << processor_count << " available logical CPUs.\n";
    Log::out().ClearRight();

    // No two std::thread objects may represent the same thread of execution;
    // std::thread is not CopyConstructible or CopyAssignable, 
    // although it is MoveConstructible and MoveAssignable.
    // T (the container's element type) must meet the requirements of MoveInsertable and EmplaceConstructible.
    std::vector<std::thread> Threads;
    Threads.reserve(Threads_);

    // prepare all clones of GeneratorFunction before starting any thread
    std::vector<std::unique_ptr<GeneratorFunctionAbstract<T>>> GFClones;
    GFClones.reserve(Threads_);
    for (unsigned int i = 0; i<Threads_; i++){
        GFClones.emplace_back(GF.clone());
    }

    // let's start all threads
    for (unsigned int i = 0; i<Threads_; i++){
        Threads.emplace_back(std::thread(&SieveGenerator::WorkMT_Thread, this, Begin, End, std::ref(GFClones[i]), std::to_string(i)));
    }

    for (auto &TS : Threads) {
        TS.join();
    }

    // consolidate all results into input GF object
    for (unsigned int i = 0; i < Threads_; i++){
        Log::out() << i << "> " << utils_str::FormatUInt(GF.PrimesCnt()) << " + " << utils_str::FormatUInt(GFClones[i]->PrimesCnt());
        GF+=(*GFClones[i]);
        Log::out() << " = " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    }
    long double PrimesRatioAfterSieve =  100.L * GF.PrimesCnt() / (( TestArrayCount_ / (long double) Primorial_) * (End - Begin + 1));
    Log::out() << "Primes:       " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    Log::out() << "Primes ratio (from interval): " << utils_str::FormatNumber(GF.PrimesCnt() * 100.0L / (End - Begin + 1), 6,3) << "%\n";
    Log::out() << "Primes ratio (after sieve):   " << utils_str::FormatNumber(PrimesRatioAfterSieve, 6,3) << "%\n";

    Log::out() << "Multithreading Sieve Duration [m]: " << GF.DurationMinutes() << "\n";
 
    if (res!=0) return res; 
    return 0;
}

// DivByPrime no MT
//  PrimesCnt: 1894120 Duration minutes: 0.4002

// DivByPrime MT
//  PrimesCnt: 1894120 Duration minutes: 0.194333
//  PrimesCnt: 1894120 Duration minutes: 0.174517


// MR T1
// PrimesCnt: 1894224 Duration minutes: 0.0460333, 0.0380667 0.04615 no mutex 0.0358333 0.0358
// MR T4  0.0125833   no mutex 0.0129667 0.0125833 0.01245

// BPSW T1 1894120  0.272033 0.266367
// T4: 0.0872333 0.0947333 0.0943667
