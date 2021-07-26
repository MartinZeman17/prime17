// #include "SieveGenerator.hpp"
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
using namespace std::chrono;


// static variable must be defined outside of header, beware: declaration is not a definition
template <class T> unsigned int SieveGenerator<T>::p_MaxPrime=0;
template <class T> unsigned long long SieveGenerator<T>::p_TestArrayCount = 0;
template <class T> std::vector<uint32_t> SieveGenerator<T>::p_TestArray(0);
template <class T> uint32_t SieveGenerator<T>::p_Primorial=1;
template <class T> std::mutex SieveGenerator<T>::p_TestArray_mutex;


template <class T> 
void SieveGenerator<T>::Constructor(unsigned int MaxPrime) {
    // ToDo: Test with more threads  ??? is it ok with ordinary object and no smart pointers
    // array may be in process of init from other thread???
    const std::lock_guard<std::mutex> lock(p_TestArray_mutex); 

    if (p_MaxPrime==0){
        mpz_t mpz_primor;
        mpz_t mpz_res;
        mpz_init2(mpz_primor,64);
        mpz_init2(mpz_res,64);
        mpz_primorial_ui(mpz_primor, MaxPrime);  //ok
        if (mpz_cmp_ui(mpz_primor, UINT32_MAX) > 0 ){ //ok
            const std::string C_Msg("SieveGenerator cannot initialize for primorial exceding 32 bits. Fatal, mission aborted.\n");
            Log::out() << C_Msg;
            throw(C_Msg); //ToDo Memory leak?? How to throw or exit??
        }
        p_Primorial = (uint32_t) utils_mpz::mpz_get<unsigned long long>(mpz_primor); // fits into 32 bits

        
        // load destination array size if known prom previous computations
        unsigned long long BufferSize=0;
        // for(unsigned int  i = 0; i < C_PrimorialPrimes.size() && i < C_Effectivity.size(); i++){
        for(unsigned int  i = 0; i < C_PrimorialPrimes.size(); i++){
            if (C_PrimorialPrimes[i]<=MaxPrime) {
                BufferSize = C_TestArraySizes[i];  
            }
        }
        if (BufferSize==0) BufferSize = (unsigned long long)  (1.0 + (long double) p_Primorial * .15); 
        p_TestArray.reserve(BufferSize);
        
        // fill Test array with coprimes to primorial
        unsigned long long TACount=0;
        for (uint32_t i = 1; i<p_Primorial;){
            mpz_gcd_ui(mpz_res, mpz_primor, i); //ok, mpz_primor throw an exception
            if(mpz_cmp_ui(mpz_res,1)==0){   //ok
                p_TestArray.push_back(i);
                TACount++;
            }
            i+=2;
        }

        p_TestArray.shrink_to_fit();
        mpz_clear(mpz_primor);
        mpz_clear(mpz_res);

        p_TestArrayCount = TACount;        
        // p_TestArrayCount=p_TestArray.size();
        p_MaxPrime = MaxPrime;
        long double MeasuredEffectivity = 100.0L * (long double) p_TestArrayCount / (long double) p_Primorial;

        Log::out() << "Sieve Prime: " << MaxPrime << " ";
        Log::out() << "Primorial: " << utils_str::FormatUInt(p_Primorial) << "\n";
        Log::out() << "Coprimes #: " << utils_str::FormatUInt(p_TestArrayCount) << " ";
        Log::out() << "Effectivity %: " << MeasuredEffectivity  << "\n"; 
        // TODO sanitizer complains about % sign in printf

        Threads(100);        
    }
    else if (p_MaxPrime!=MaxPrime){
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
T SieveGenerator<T>::Work(const T & Begin, const T & End, GeneratorFunctionAbstract & GF) const {
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

    T k = Begin/p_Primorial;
    T kp = k * p_Primorial;  // primorial p multiplied by some integer k

    Log::out()  << "Sieve Begin: " << Begin << " ";        
    Log::out()  << "Sieve End  : " << End <<  " ";
    Log::out()  << "Sieve End-Begin: " << End - Begin << "\n";        
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
    tmp = p_TestArray[p_TestArrayCount-1] + kp; 
    if (tmp < Begin){
        // Begin is very close to the next multiple of Primorial 
        // so whole TestArray would be exhausted and variable i would out be of bounds
        // all numbers between kp + last item of TestArrayand the next multiple of primorial must have common divisor (not primes) 
        // -> go directly to the next primorial and leave i index at 0
        kp += p_Primorial;
        k++;
        Log::out() << "Begin after last TestArray item. Wow !!!\n";
    } else {
        // search where to start with testing and set index i
        // for(; p_TestArray[i]+kp < Begin; i++);  // this for cycle works only for 64bit variables
        tmp = p_TestArray[i]+kp;
        while (tmp < Begin){
            ++i;
            tmp = p_TestArray[i]+kp;
        } 
    }

    Log::out() << "Primorials skipped: " << k << "\n";
    Log::out() << "Nearest left multiple of primorial: " << kp << "\n";
    Log::out() << "... till Begin: " << Begin - kp << "\n";

    if (std::is_same_v<T, unsigned long long>) {
        utils_mpz::mpz_set_ull(mpz_kp, kp);
    } else {
        utils_mpz::mpz_set_ul128(mpz_kp, kp);
    }
    
    // // check correct computation of kp
    // Log::out() << "kp:     " ;
    // Log::out() << kp << "\n;
    // Log::out() << "mpz_kp: " ;
    // mpz_out_str(stdout, 10, mpz_kp);
    // Log::out() << endl;

    if (Begin <=p_MaxPrime) {
        // test all primes from primorial construction between Begin and End as the algorithm does not test them
        for (auto & Prime : C_PrimorialPrimes) {
            if (Begin<=Prime) {
                if (Prime <= End && Prime <=p_MaxPrime) {
                    utils_mpz::mpz_set_ull(mpz_X, Prime);
                    if (GF.GenFunct((uint128_t) Prime, mpz_X)!=0) {
                        RETURN(Prime);
                    }
                } else {
                    break;
                }                
            }
        }
        Log::out() << "Primorial primes tested" << "\n";
    }


    auto TBegin = high_resolution_clock::now();
    // i is now a valid index within the array bounds
    while(true)
    {
        T X = kp+p_TestArray[i];
        mpz_add_ui(mpz_X, mpz_kp, p_TestArray[i]);  //ok

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
        if(i==p_TestArrayCount) 
        {
            i=0;
            kp += p_Primorial;
            mpz_add_ui(mpz_kp, mpz_kp, p_Primorial);  //ok, fits in 32 bits
            NewPerc = (100.0*(double)(kp-Begin)/(double)(End-Begin));
            NewPerc = (std::floor(NewPerc*10.0))/10.0;
            if(NewPerc !=OldPerc){
                OldPerc = NewPerc;
                Log::out()  << OldPerc << " "; // << std::flush;
            }
        }
    }
    Log::out()  << "\n";
    auto TEnd = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(TEnd - TBegin);
    // Log::out() << "Sieve duration: " << duration.count() << endl;
    Log::out() << "Sieve Duration minutes: " << GF.DurationMinutes() << "\n";
    Log::out() << "\n";

    RETURN(0);
}


/************************************************************* multi-thread version *************************************************************/

template <class T>
void SieveGenerator<T>::PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const{
    unsigned int rows, cols;				/* to store the number of rows and the number of colums of the screen */
    getmaxyx(Log::out().win_right, rows, cols);	    	/* get the number of rows and columns */
    unsigned int row = rows - (_Threads - PId);
    row = 0 + PId;

    std::stringstream RowMsg;
    RowMsg << utils_str::FormatNumber(PId, 2, 0) << ":" ;
    int C_ProgressBarCnt = cols - 20;
    unsigned int Chars = (unsigned int) ((Percent * C_ProgressBarCnt) / 100.0);
    for(unsigned int i = 0; i< Chars; i++){
        RowMsg << "#";
    }
    for(unsigned int i = 0; i< C_ProgressBarCnt - Chars; i++){
        RowMsg << "_";
    }

    RowMsg << utils_str::FormatNumber(Percent, 4, 1);
    RowMsg << "%% ";
    RowMsg << utils_str::FormatNumber(MinTillEnd, 4, 1);
    RowMsg << "m ";

    {
        // const std::lock_guard<std::mutex> lock(_cout_mutex);
        mvwprintw(Log::out().win_right, row, 0, RowMsg.str().c_str());
        wrefresh(Log::out().win_right);
    }
    // endwin();
}


template <class T>
T SieveGenerator<T>::WorkMT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract> & GF, const std::string PId) {
    auto TBegin = high_resolution_clock::now();

    mpz_t mpz_kp, mpz_X;

    #undef RETURN
    #define RETURN(n)         \
    {                         \
        mpz_clear(mpz_kp);    \
        mpz_clear(mpz_X);     \
        return(n);            \
    }

    int iPId = std::stoi(PId);

    if (iPId == 0) {
        // const std::lock_guard<std::mutex> lock(_StartTime_mutex);
        _StartTime = high_resolution_clock::now();
        // Log::out() << precision(1);
        // Log::out() << std::fixed;
    }

    mpz_init2(mpz_kp, 65);
    mpz_init2(mpz_X, 65);
    T res = 0;
    T kp;
    {
        const std::lock_guard<std::mutex> lock(_kp_mutex);
        T tmp;
        tmp = p_TestArray[p_TestArrayCount-1] + _kp; 
        if (tmp < Begin){
            // Begin is very close to the next multiple of Primorial 
            // so whole TestArray would be exhausted and variable i would out be of bounds
            // all numbers between kp + last item of TestArrayand the next multiple of primorial must have common divisor (not primes) 
            // -> go directly to the next primorial and leave i index at 0
            _kp += p_Primorial;
            {
                const std::lock_guard<std::mutex> lock(_cout_mutex);
                Log::out() << PId << "> " << "Begin after last TestArray item. Wow !!!" << "\n";
            }
        }
        kp = _kp;
        // prepare _kp for another thread to process 
        _kp += p_Primorial;
    }

    {
        // if (kp < End) {
        //     const std::lock_guard<std::mutex> lock(_Log::out()_mutex);
        //     Log::out() << PId << "> " << " k: " <<  kp / p_Primorial << " kp: " << kp  << endl;
        // }
    }


    unsigned long long i=0;
    if (kp < Begin) {
        T tmp;
        // search where to start with testing and set index i
        // for(; p_TestArray[i]+kp < Begin; i++);  // this for cycle works only for 64bit variables

        tmp = p_TestArray[i]+kp;
        while (tmp < Begin){
            ++i;
            tmp = p_TestArray[i]+kp;
        }

        // for(tmp = p_TestArray[i]+kp; tmp < Begin; i++ ){
        //     tmp = p_TestArray[i]+kp;
        // };

    }


    if (std::is_same_v<T, unsigned long long>) {
        utils_mpz::mpz_set_ull(mpz_kp, kp);
    } else {
        utils_mpz::mpz_set_ul128(mpz_kp, kp);
    }
    
    if (Begin <=p_MaxPrime && iPId == 0) {
        // test all primes from primorial construction between Begin and End as the algorithm does not test them
        for (auto & Prime : C_PrimorialPrimes) {
            if (Begin<=Prime) {
                if (Prime <= End && Prime <=p_MaxPrime) {
                    utils_mpz::mpz_set_ull(mpz_X, Prime);
                    if (GF->GenFunct((uint128_t) Prime, mpz_X)!=0) {
                        res = Prime;
                        RETURN(res);
                    }
                } else {
                    break;
                }                
            }
        }
        {
            const std::lock_guard<std::mutex> lock(_cout_mutex);
            Log::out() << PId << "> " << "Primorial primes tested" << "\n";
        }
    }


    // i is now a valid index within the array bounds
    while(true)
    {
        T X = kp+p_TestArray[i];
        mpz_add_ui(mpz_X, mpz_kp, p_TestArray[i]); //ok

        if(X > End) break;            

        if (GF->GenFunct(X, mpz_X)!=0) {
            res = X;
            RETURN(res);
        };
                  
        i++;
        if(i==p_TestArrayCount) 
        {
            // first print progress 
            long double NewPerc = (100.0L*(long double)((kp+p_Primorial)-Begin)/(long double)(End-Begin));
            NewPerc = (std::floor(NewPerc*10.0L))/10.0L;
            {
                const std::lock_guard<std::mutex> lock(_Percent_mutex);
                if(NewPerc !=_Percent){
                    _Percent = NewPerc;
                    long double MinTillEnd;
                    if (NewPerc<1) {
                        MinTillEnd = 0;
                    } else {

                        auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _StartTime);
                        MinTillEnd = ceill(((duration.count() / NewPerc) * (100.0L-NewPerc))/6000.0L) / 10.0L;
                    }
                    {
                    // const std::lock_guard<std::mutex> lock(_cout_mutex);
                    PrintProgress(iPId,_Percent, MinTillEnd);
                    // Log::out() << MinTillEnd << " ";
                    // std::cout << flush;
                    }
                }
            }
            
            //update kp
            i=0;
            {
                const std::lock_guard<std::mutex> lock(_kp_mutex);
                kp = _kp;
                _kp += p_Primorial;
            }
            // {
            //     const std::lock_guard<std::mutex> lock(_cout_mutex);
            //     Log::out() << PId << "> " << " k: " <<  kp / p_Primorial << " kp: " << kp  << endl;
            // }


            if (std::is_same_v<T, unsigned long long>) {
                utils_mpz::mpz_set_ull(mpz_kp, kp);
            } else {
                utils_mpz::mpz_set_ul128(mpz_kp, kp);
            }

        }
    }

    auto TEnd = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(TEnd - TBegin);

    // if (iPId == 0) endwin();
    // endwin();

    // if (duration.count()!=0)
    // {
    //     const std::lock_guard<std::mutex> lock(_cout_mutex);
    //     Log::out() << PId << "> Sieve duration: " << duration.count() << " s = " << (float) duration.count() / 60.0f << " m" << "\n";
    // }

    RETURN(0);

}

template <class T>
unsigned int SieveGenerator<T>::Threads(const unsigned int Percent){
    const auto processor_count = std::thread::hardware_concurrency();
    _Threads = (unsigned int) round(( Percent * processor_count) / 100.0);
    if (_Threads>processor_count) _Threads = processor_count;
    if (_Threads<=0) _Threads = 1;
    return _Threads;
}

template <class T>
T SieveGenerator<T>::WorkMT(const T & Begin, const T & End, GeneratorFunctionAbstract & GF) {

    T res = 0;
    T k = Begin/p_Primorial;
    _kp = k * p_Primorial;  // primorial p multiplied by some integer k
    _Percent=-1.0;

    Log::out()  << "Sieve Begin: " << utils_str::FormatUInt(Begin) << "\n";        
    Log::out()  << "Sieve End  : " << utils_str::FormatUInt(End) <<  "\n";
    Log::out()  << "Sieve End-Begin: " << utils_str::FormatUInt(End - Begin) << "\n";        
    // Log::out()  << "End - UINT64_MAX: " << (uint128_t) End - (uint128_t) UINT64_MAX << endl;
    // Log::out()  << "End - 2 * UINT64_MAX: " << (uint128_t) End - (uint128_t) UINT64_MAX - (uint128_t) UINT64_MAX << endl;        
    // Log::out()  << "UINT64_MAX: " <<  UINT64_MAX << endl;        
    if ((uint128_t) End / ( (uint128_t) UINT64_MAX+  (uint128_t)1) >1) {
        Log::out() << "A serious warning!!! End looks too big." << "\n";
    }    
   
    Log::out() << "Primorials skipped: " << utils_str::FormatUInt(k) <<  "\n";
    Log::out() << "Multiple of primorial: " << utils_str::FormatUInt(_kp) <<  " ";
    Log::out() << "...till Begin: " << utils_str::FormatUInt(Begin - _kp) << "\n";
   
    const auto processor_count = std::thread::hardware_concurrency();
    // const size_t C_Threads = processor_count;
    // Log::out() << "Processor count detected: " << processor_count << " Threads to spawn: " << _Threads  << "\n";
    Log::out() << "Using " << (100 * _Threads)/processor_count << "%% CPU = spawning " << _Threads << " thread";
    if (_Threads > 1) Log::out() << "s";
    Log::out() << " out of " << processor_count << " available logical CPUs.\n";
    Log::out().ClearRight();
    

    // No two std::thread objects may represent the same thread of execution;
    // std::thread is not CopyConstructible or CopyAssignable, 
    // although it is MoveConstructible and MoveAssignable.
    // T (the container's element type) must meet the requirements of MoveInsertable and EmplaceConstructible.
    std::vector<std::thread> Threads;
    Threads.reserve(_Threads);

    // prepare all clones of GeneratorFunction before starting any thread
    std::vector<std::unique_ptr<GeneratorFunctionAbstract>> GFClones;
    GFClones.reserve(_Threads);
    for (unsigned int i = 0; i<_Threads; i++){
        GFClones.emplace_back(GF.clone());
    }

    // let's start all threads
    for (unsigned int i = 0; i<_Threads; i++){
        Threads.emplace_back(std::thread(&SieveGenerator::WorkMT_Thread, this, Begin, End, std::ref(GFClones[i]), std::to_string(i)));
    }

    for (auto &TS : Threads) {
        TS.join();
    }

    // consolidate all results into input GF object
    for (unsigned int i = 0; i < _Threads; i++){
        Log::out() << i << "> " << GF.PrimesCnt() << " + " << GFClones[i]->PrimesCnt();
        GF+=(*GFClones[i]);
        Log::out() << " = " << GF.PrimesCnt() << "\n";
    }
    Log::out() << "Multithreading Sieve Duration minutes: " << GF.DurationMinutes() << "\n";
 
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
