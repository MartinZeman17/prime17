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

using namespace primecount;

#include <vector>
#include <mutex>
#include <chrono>

// 23# fits into uint_32 datatype (36 495 360 coprimes to primorial 223 092 870)
// const std::vector<unsigned long long> C_Primorials({2,6,30, 210, 2310, 30030, 510510, 9699690, 223092870, 6469693230, 200560490130});
// const std::vector<unsigned long long> C_TestArraySizes({1,2,8,48, 480, 5760, 92160, 1658880, 36495360, 1021870080, 30656102400});
// const std::vector <long double> C_Effectivity({0.5, 0.333333, 0.266667, 0.228571, 0.207792,  0.191808, 0.180525, 0.171024, 0.163588, 0.157947, 0.152852});

// use Eratosthenes sieve with the following primorial
// bigger primorial = better efficiency, however at the end some threads have nothing to do
const unsigned int C_Sieve2GeneratorPrime1=19;
const unsigned int C_Sieve2GeneratorPrime2=23;

constexpr unsigned int C_Prime1 = 19;
constexpr unsigned int C_Prime2 = 23;

constexpr size_t C_TestArrayCoprimes19 = 1658880; // primorial 9699690;
constexpr size_t C_TestArrayCoprimes23 = 36495360; //primorial 223092870;


template <class T> 
class Sieve2Generator {
    public:
    enum class enSieveLength{
        SieveNotInitialized = 0,
        SieveShort,
        SieveLong 
    };

    private:
    const unsigned int _MaxPrime1 = C_Prime1;
    const unsigned int _MaxPrime2 = C_Prime2;  
    uint32_t _Primorial1;
    uint32_t _Primorial2;    
    uint32_t * _TestArray1 = nullptr;
    uint32_t * _TestArray2 = nullptr;
    size_t _TestArrayCount1 = C_TestArrayCoprimes19;
    size_t _TestArrayCount2 = C_TestArrayCoprimes23;

    // selected version for furter processing
    // change has not an immediate effect, it is rather a wish for further processing
    // all threads should store its own copies and not access this set as it si subject to change of other threads    
    // all changes by threads are protected by _Untouched_mutex. Changes by Constructor does not need locking.
    T _Untouched;
    std::mutex _Untouched_mutex;
    enSieveLength _SieveLength = enSieveLength::SieveNotInitialized;
    uint32_t * _TestArray=nullptr;
    size_t _TestArrayCount=0;
    uint32_t _Primorial=0;
    uint32_t _MaxPrime=0;
    T _SwitchPointOffset = 0; // 0 no switch point = start with shorter sieve since the beginning
    T SetSwitchPoint(const T & Begin, const T & End, unsigned int processor_count);



    void SieveInit(const unsigned int MaxPrime, uint32_t * DestArr, uint32_t & Primorial);
    void PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const;
    T Work2MT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract> & GF, const std::string PId);
    std::chrono::time_point<std::chrono::high_resolution_clock> _BeginTime; //  = std::high_resolution_clock::now(); 

    
    long double _Percent;
    std::mutex _Percent_mutex;
    
    std::mutex _cout_mutex;

    std::chrono::time_point<std::chrono::high_resolution_clock> _StartTime;

    unsigned int _Threads;
    unsigned int _ncursesY;
    unsigned int _ncursesX;

    public:
    // enSieveLength GetSieveLength() const noexcept { return _SieveLength; }
    enSieveLength SetSieveLength(const enSieveLength DesiredSieveLength) noexcept;
    // uint32_t Primorial() const noexcept { return _Primorial; }

    Sieve2Generator();
    ~Sieve2Generator();

    // T Work(const T & Begin, const T & End, GeneratorFunctionAbstract & GF) const;
    // multithreading version
    T Work2MT(const T & Begin, const T & End, GeneratorFunctionAbstract & GF);


    // const unsigned int & MaxPrime = p_MaxPrime;
    // // unsigned long long Primorial() const {return p_Primorial;};  
    // const uint32_t & Primorial = p_Primorial;

    unsigned int Threads(const unsigned int Percent);

    void ResetClock() noexcept;
    long double DurationMinutes() const noexcept;
    long double DurationSeconds() const noexcept;

};

// #include "SieveGeneratorT.cpp"


// called from constructor and threads where _Untouched_mutex locks other threads and prevents changes
// for more complex scenarios consider an independent lock or better  recursive_mutex
template <class T>
typename Sieve2Generator<T>::enSieveLength Sieve2Generator<T>::SetSieveLength(const Sieve2Generator<T>::enSieveLength DesiredSieveLength) noexcept {
    assert(DesiredSieveLength != Sieve2Generator<T>::enSieveLength::SieveNotInitialized); 
    
    if (DesiredSieveLength != _SieveLength) {
        if (DesiredSieveLength == Sieve2Generator<T>::enSieveLength::SieveShort) {
            _TestArray = _TestArray1;
            _TestArrayCount = _TestArrayCount1;
            _Primorial = _Primorial1;
            _MaxPrime = _MaxPrime1;
            _SieveLength = Sieve2Generator<T>::enSieveLength::SieveShort;
            Log::out() << "Switch to short sieve.\n";
        } 
        else if (DesiredSieveLength == Sieve2Generator<T>::enSieveLength::SieveLong){
            _TestArray = _TestArray2;
            _TestArrayCount = _TestArrayCount2;
            _Primorial = _Primorial2;
            _MaxPrime = _MaxPrime2; 
            _SieveLength = Sieve2Generator<T>::enSieveLength::SieveLong;
            Log::out() << "Switch to long sieve.\n";            
        }
    }
    return _SieveLength;
};

template <class T>
Sieve2Generator<T>::Sieve2Generator(){
    // ResetClock();

    _SieveLength = enSieveLength::SieveNotInitialized;
    _TestArray1  = (uint32_t *) malloc(C_TestArrayCoprimes19 * sizeof(uint32_t)); 
    _TestArray2  = (uint32_t *) malloc(C_TestArrayCoprimes23 * sizeof(uint32_t)); 

    SieveInit(C_Prime1, _TestArray1, _Primorial1);
    SieveInit(C_Prime2, _TestArray2, _Primorial2);
    SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveLong);
    assert(_SieveLength != enSieveLength::SieveNotInitialized);
}

template <class T>
Sieve2Generator<T>::~Sieve2Generator(){
    free(_TestArray1);
    _TestArray1 = nullptr;
    free(_TestArray2);
    _TestArray2 = nullptr;
}

template <class T> 
void Sieve2Generator<T>::SieveInit(const unsigned int MaxPrime, uint32_t * DestArr, uint32_t & Primorial) {

    assert(MaxPrime == C_Prime1 || MaxPrime == C_Prime2);

    mpz_t mpz_primor;
    mpz_t mpz_res;
    mpz_init2(mpz_primor,64);
    mpz_init2(mpz_res,64);
    mpz_primorial_ui(mpz_primor, MaxPrime); 

    assert(mpz_cmp_ui(mpz_primor, UINT32_MAX) <= 0); //SieveGenerator cannot initialize for primorial exceding 32 bits.
    Primorial = (uint32_t) utils_mpz::mpz_get<unsigned long long>(mpz_primor); // fits into 32 bits

    // fill Test array with coprimes to primorial
    unsigned long long TACount = 0;
    for (uint32_t i = 1; i<Primorial;){
        mpz_gcd_ui(mpz_res, mpz_primor, i);
        if(mpz_cmp_ui(mpz_res,1) == 0) {
            (*DestArr) = i;
            ++DestArr;
            TACount++;
        }
        i+=2;
    }
    assert(TACount==_TestArrayCount1 || TACount==_TestArrayCount2);

    mpz_clear(mpz_primor);
    mpz_clear(mpz_res);

    long double MeasuredEffectivity = 100.0L * (long double) TACount / (long double) Primorial;

    Log::out() << "Sieve Prime: " << MaxPrime << " ";
    Log::out() << "Primorial: " << utils_str::FormatUInt(Primorial) << "\n";
    Log::out() << "Coprimes #: " << utils_str::FormatUInt(TACount) << " ";
    Log::out() << "Effectivity [%]: " << MeasuredEffectivity  << "\n"; 
}

template <class T>
unsigned int Sieve2Generator<T>::Threads(const unsigned int Percent){
    const auto processor_count = std::thread::hardware_concurrency();
    _Threads = (unsigned int) round(( Percent * processor_count) / 100.0);
    if (_Threads>processor_count) _Threads = processor_count;
    if (_Threads<=0) _Threads = 1;
    return _Threads;
}

template <class T>
void Sieve2Generator<T>::ResetClock() noexcept {
        _BeginTime = std::chrono::high_resolution_clock::now(); 
 }

template <class T>
long double Sieve2Generator<T>::DurationMinutes() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    return duration.count() / (1000.0l * 60.0l);
}

template <class T>
long double Sieve2Generator<T>::DurationSeconds() const noexcept {
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _BeginTime);
    return duration.count() / (1000.0l);
}


template <class T>
void Sieve2Generator<T>::PrintProgress(const unsigned int &PId, const long double &Percent, const long double &MinTillEnd) const{
    if (Log::out().win_right == nullptr) return;
    
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
    RowMsg << "% ";
    if (MinTillEnd>0) {
        RowMsg << utils_str::FormatNumber(MinTillEnd, 4, 1);
        RowMsg << "m ";
    }

    {
        // const std::lock_guard<std::mutex> lock(_cout_mutex);
        // mvwprintw(Log::out().win_right, row, 0, RowMsg.str().c_str());
        mvwaddstr(Log::out().win_right, row, 0, RowMsg.str().c_str());
        wrefresh(Log::out().win_right);
    }
}


template <class T>
T Sieve2Generator<T>::Work2MT_Thread(const T & Begin, const T & End, std::unique_ptr<GeneratorFunctionAbstract> & GF, const std::string PId) {
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

    mpz_init2(mpz_kp, 65);
    mpz_init2(mpz_X, 65);
    T res = 0;

    while(true) {

        T kp;

        // local thread copies. _Untouched is locked, so is used directly (main purpose is to update _Untouched).  
        uint32_t * TestArray;
        size_t TestArrayCount;
        uint32_t Primorial;
        uint32_t MaxPrime;
        // T SwitchPointOffset; 
        T Untouched=0;
        
        // Lock variables
        {
            const std::lock_guard<std::mutex> lock(_Untouched_mutex);
            if (_Untouched == Begin) _StartTime = high_resolution_clock::now();

            // change of sieve length?
            if (_Untouched >= (_SwitchPointOffset + Begin)) {
                SetSieveLength(enSieveLength::SieveShort);
            }
            //fill in local thread copies of Sieve params
            TestArray = _TestArray;
            TestArrayCount = _TestArrayCount;
            Primorial = _Primorial;
            MaxPrime = _MaxPrime;
            // SwitchPointOffset = _SwitchPointOffset;
            
            T k = _Untouched / Primorial;
            kp = k * Primorial;

            T tmp = TestArray[TestArrayCount-1];
            tmp += kp; 
            if (tmp < _Untouched){
                // Untouched are is very close to the next multiple of Primorial 
                // so whole TestArray would be exhausted and variable i would out be of bounds
                // all numbers between kp + last item of TestArray and the next multiple of primorial must have common divisor (not primes) 
                // -> go directly to the next primorial and leave i index at 0
                // skip coprimes and move to the next primorial
                // The probability is very low: Primorial -1 and Primorial are typically coprimes and so Primorial -1 should be the last TestArray number.
                // However it happens when Begin is Multiple of Primorial decreased by one. ToDo Test.
                kp += Primorial;
                _Untouched = kp; 
                {
                    const std::lock_guard<std::mutex> lock(_cout_mutex);
                    Log::out() << PId << "> " << "Begin after last TestArray item. Wow !!! Is Begin multiple of Primorial decreased by 1?" << "\n";
                }
            }
            // load local copy of Untouched
            Untouched = _Untouched;
            // Update _Untouched for another thread to process 
            _Untouched = kp + Primorial; // by the way it is not a prime
            // overflow test
            if (_Untouched < Untouched) {
                _Untouched = End;  // todo test
            }

        } // _Untouched and Sieve is updated, having local copies to work on


        unsigned long long i=0;
        if (kp < Untouched) {
            T tmp;            
            auto TTArrayBegin = high_resolution_clock::now();
            // search where to start with testing and set index i
            // for(; p_TestArray[i]+kp < Begin; i++);  // this for cycle works only for 64bit variables

            // todo use pointer ??
            // Beware of endless loop: 1. kp < Untouched 2. kp + Primorial > Untouched
            tmp = TestArray[i]+kp;
            while (tmp < Untouched){
                ++i;
                tmp = TestArray[i]+kp;
            }

            auto TTArrayDuration = duration_cast<milliseconds>(high_resolution_clock::now() - TTArrayBegin);
            if (TTArrayDuration.count()> 0)
            {
                const std::lock_guard<std::mutex> lock(_cout_mutex);
                Log::out() << "Coprimes array seek: " << TTArrayDuration.count() << " ms\n";
            }
        }



        // The branch that fails the constexpr condition will not be compiled for the given template instantiation. Or I hope so...
        // But it looks like constexpr is not needed, both binaries are identical.
        // if (std::is_same_v<T, unsigned long long>) {
        if constexpr(std::is_same_v<T, unsigned long long>) {
            utils_mpz::mpz_set_ull(mpz_kp, kp);
        } else {
            utils_mpz::mpz_set_ul128(mpz_kp, kp);
        }
        
        // MaxPrime = 2, Primorial = 2, TestArray = [1] -> ??? ToDo test
        // Beg = 0, k=0 - should test 0+1, k=1, should test 1*2+1, k=3  
        assert(Primorial>MaxPrime);

        // ToDo c++20 constexpr vector
        assert(MaxPrime<=43);
        static const std::vector <unsigned long long> C_Primorial2Primes({2,3,5,7,11,13,17,19,23,29,31,37,41,43});
        if (Untouched <= MaxPrime) {
            // test all primes from primorial construction between Begin and End as the algorithm does not test them
            for (auto & Prime : C_Primorial2Primes) {
                if (Untouched<=Prime) {
                    if (Prime <= End && Prime <= MaxPrime) {
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
        uint32_t *pCoprime = TestArray; 
        while(true)
        {   
            uint32_t Coprime = *pCoprime;//TestArray[i];
            pCoprime++;
            T X = kp + Coprime;
            mpz_add_ui(mpz_X, mpz_kp, Coprime); //ok

            if(X > End) break;            
            
            if (GF->GenFunct(X, mpz_X)!=0) {
                res = X;
                RETURN(res);
            };
    
            i++;
            if(i == TestArrayCount) {
                // primorial range is done, return to outer cycle to read new primorial
                break;
            }
        }

        // print progress 
        long double NewPerc = (100.0L*(long double)((kp + TestArray[i-1])-Begin)/(long double)(End-Begin));
        NewPerc = (std::floor(NewPerc*10.0L))/10.0L;
        {
            const std::lock_guard<std::mutex> lock(_Percent_mutex);
            if(NewPerc !=_Percent){
                _Percent = NewPerc;
                long double MinTillEnd;
                if (NewPerc<1.5) {
                    MinTillEnd = 0;
                } else {
                    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - _StartTime);
                    MinTillEnd = ceill(((duration.count() / NewPerc) * (100.0L-NewPerc))/6000.0L) / 10.0L;
                }
                {
                // const std::lock_guard<std::mutex> lock(_cout_mutex);
                PrintProgress(iPId, _Percent, MinTillEnd);
                // Log::out() << MinTillEnd << " ";
                // std::cout << flush;
                }
            }
        }
    }

    auto TEnd = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(TEnd - TBegin);

    if (duration.count()!=0)
    {
        const std::lock_guard<std::mutex> lock(_cout_mutex);
        Log::out() << PId << "> Sieve duration: " << duration.count() << " s = " << utils_str::FormatNumber( (float) duration.count() / 60.0f, 1,1) << " m" << "\n";
    }

    RETURN(0);

}



template <class T>
T Sieve2Generator<T>::SetSwitchPoint(const T & Begin, const T & End, unsigned int processor_count){

    if (processor_count <= 1) {
        // no competition, use large sieve as it is more effective
        // however for old and slow CPUs short sieve might be preferable due to more frequent updates
        _SwitchPointOffset=0;    
    } else {
        T Primorials2 = Begin / _Primorial2;
        if (Primorials2 * _Primorial2 < Begin )  Primorials2++;
        T BeginOffset2 = Primorials2 * _Primorial2;
        
        T Len = 0;
        if (BeginOffset2 < End) {
            Len = (End - BeginOffset2 + 1);
        }
        BeginOffset2 -= Begin; // make it an offset from the Beginning 

        uint64_t Cnt2 =  Len / _Primorial2;
        assert(processor_count>0);
        uint64_t Cnt2perCPU = Cnt2 / processor_count;

        T SPOffset =  Cnt2perCPU * _Primorial2 * processor_count;
        
        // if BeginOffset2 is too close to Primorial, may be harmful
        // todo finetuning for very small BeginOffset2 ??? compare with ending part
        // if (SPOffset > 0) SPOffset += BeginOffset2; 
        if (SPOffset>0) {
            SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveLong);
        } else {
            SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveShort);
        }

        long double SmallPrimorials = ((((End - (SPOffset + Begin) + 1)) / (long double) _Primorial1) ) /  (long double) processor_count; 
        Log::out() << "Each core is supposed to process " << Cnt2perCPU << " whole large primorial(s) and ~" << utils_str::trim_copy(utils_str::FormatNumber(SmallPrimorials, 3,1)) << " small primorials.\n";

        _SwitchPointOffset = SPOffset; 
    }
    Log::out() << "Sieve-switch offset at: " << _SwitchPointOffset << "\n";
    return _SwitchPointOffset;
}


template <class T>
T Sieve2Generator<T>::Work2MT(const T & Begin, const T & End, GeneratorFunctionAbstract & GF) {
    ResetClock();
    T res = 0;

    const auto processor_count = std::thread::hardware_concurrency();
    Log::out() << "Using " << (unsigned int) (100 * _Threads)/processor_count << "% CPU = spawning " << _Threads << " thread";
    if (_Threads > 1) Log::out() << "s";
    Log::out() << " out of " << processor_count << " available logical CPUs.\n";
    
    SetSwitchPoint(Begin, End, processor_count);

    _Untouched = Begin;


    // T k = Begin/_Primorial1;
    // T _kp = k * _Primorial1;  // primorial p multiplied by some integer k
    _Percent=-1.0;

    Log::out()  << "Sieve Begin: " << utils_str::FormatUInt(Begin) << "\n";        
    Log::out()  << "Sieve End  : " << utils_str::FormatUInt(End) <<  "\n";
    Log::out()  << "End-Begin  : " << utils_str::FormatUInt(End - Begin) << "\n";        
    if ((uint128_t) End / ( (uint128_t) UINT64_MAX+  (uint128_t)1) >1) {
        Log::out() << "A serious warning!!! End looks too big." << "\n";
    }    
   
    // Log::out() << "Primorials skipped: " << utils_str::FormatUInt(k) <<  "\n";
    // Log::out() << "Multiple of primorial: " << utils_str::FormatUInt(_kp) <<  " ";
    // Log::out() << "...till Begin: " << utils_str::FormatUInt(Begin - _kp) << "\n";
    
    Log::out().ClearRight();
   


    // prepare all clones of GeneratorFunction before starting any thread
    std::vector<std::unique_ptr<GeneratorFunctionAbstract>> GFClones;
    GFClones.reserve(_Threads);
    for (unsigned int i = 0; i<_Threads; i++){
        GFClones.emplace_back(GF.clone());
    }

    if (_Threads == 1) {
        Work2MT_Thread(Begin, End, std::ref(GFClones[0]), "0");
    } else {    

        // No two std::thread objects may represent the same thread of execution;
        // std::thread is not CopyConstructible or CopyAssignable, 
        // although it is MoveConstructible and MoveAssignable.
        // T (the container's element type) must meet the requirements of MoveInsertable and EmplaceConstructible.
        std::vector<std::thread> Threads;
        Threads.reserve(_Threads);

        // let's start all threads
        for (unsigned int i = 0; i<_Threads; i++){
            Threads.emplace_back(std::thread(&Sieve2Generator::Work2MT_Thread, this, Begin, End, std::ref(GFClones[i]), std::to_string(i)));
        }

        for (auto &TS : Threads) {
            TS.join();
        }
    }

    // consolidate all results into input GF object
    for (unsigned int i = 0; i < _Threads; i++){
        Log::out() << i << "> " << utils_str::FormatUInt(GF.PrimesCnt()) << " + " << utils_str::FormatUInt(GFClones[i]->PrimesCnt());
        GF+=(*GFClones[i]);
        Log::out() << " = " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    }

    long double PrimesRatioAfterSieve =  100.L * GF.PrimesCnt() / (( _TestArrayCount / (long double) _Primorial1) * (End - Begin + 1));
    Log::out() << "Primes: " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    Log::out() << "Primes ratio (from interval): " << utils_str::FormatNumber(GF.PrimesCnt() * 100.0L / (End - Begin + 1), 6,3) << "%\n";
    Log::out() << "Primes ratio (after sieve)  : " << utils_str::FormatNumber(PrimesRatioAfterSieve, 6,3) << "%\n";


    Log::out() << "Multithreading Sieve Duration [m]: " << GF.DurationMinutes() << "\n";
 
    if (res!=0) return res; 
    return 0;
}



#endif