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
#include <algorithm>

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
    unsigned int _MaxPrime1 = C_Prime1;
    unsigned int _MaxPrime2 = C_Prime2;  
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
    T _SwitchPoint = 0;
    T SetSwitchPoint(const T & Begin, const T & End, unsigned int processor_count);



    uint32_t * SieveInit(const unsigned int SieveMaxPrime, uint32_t & MaxPrime, uint32_t & Primorial, size_t & CoprimesCount);    
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
            if (_TestArray1 != nullptr) {   
                _TestArray = _TestArray1;
                _TestArrayCount = _TestArrayCount1;
                _Primorial = _Primorial1;
                _MaxPrime = _MaxPrime1;
                _SieveLength = Sieve2Generator<T>::enSieveLength::SieveShort;
                // Log::out() << "Switch to short sieve.\n";
            }
        } 
        else if (DesiredSieveLength == Sieve2Generator<T>::enSieveLength::SieveLong){
            if (_TestArray2 != nullptr) {   
                _TestArray = _TestArray2;
                _TestArrayCount = _TestArrayCount2;
                _Primorial = _Primorial2;
                _MaxPrime = _MaxPrime2; 
                _SieveLength = Sieve2Generator<T>::enSieveLength::SieveLong;
                // Log::out() << "Switch to long sieve.\n";            
            }
        }
    }
    return _SieveLength;
};

template <class T>
Sieve2Generator<T>::Sieve2Generator(){
    ResetClock();

    _SieveLength = enSieveLength::SieveNotInitialized;

    _TestArray1 = SieveInit(C_Prime1, _MaxPrime1, _Primorial1, _TestArrayCount1);
    SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveShort);

    _TestArray2 = SieveInit(C_Prime2, _MaxPrime2, _Primorial2, _TestArrayCount2);
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
uint32_t * Sieve2Generator<T>::SieveInit(const unsigned int SieveMaxPrime, uint32_t & MaxPrime, uint32_t & Primorial, size_t & CoprimesCount) {

    assert(MaxPrime == C_Prime1 || MaxPrime == C_Prime2);
    
    MaxPrime = SieveMaxPrime;
    if (MaxPrime == 19) {
        CoprimesCount = C_TestArrayCoprimes19;
    } else if ( MaxPrime == 23 ) {
        CoprimesCount = C_TestArrayCoprimes23;
    }

    uint32_t * DestArr = (uint32_t *) malloc(CoprimesCount * sizeof(uint32_t)); 


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
    assert(TACount==_TestArrayCount1 || TACount==_TestArrayCount2);

    mpz_clear(mpz_primor);
    mpz_clear(mpz_res);

    long double MeasuredEffectivity = 100.0L * (long double) TACount / (long double) Primorial;

    Log::out() << "Sieve Prime:    " << MaxPrime << "\n";
    Log::out() << "Primorial:      " << utils_str::FormatUInt(Primorial) << "\n";
    Log::out() << "Coprimes #:     " << utils_str::FormatUInt(TACount) << " ";
    Log::out() << "Effectivity [%]:" << MeasuredEffectivity  << "\n"; 

    return DestArr;
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
    assert(Percent>=0.0);
    assert(Percent<=100.0);
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

    // perform an overflow test 
    {
        T OverflowTest1 = std::max(_Primorial1, _Primorial2); 
        T OverflowTest2 = OverflowTest1 + End;
        // prevent overflow in End + 1, End + _Primorial     
        if (!(OverflowTest2 > End && OverflowTest2 > OverflowTest1 && End + 1 > End)) {
            Log::out() << "Input parameters would lead to overflow in a sieve algorithm. Use template with bigger data type if possible (e.g. uint128_t). Mission aborted.\n";
            abort();
        }
    }


    while(true) {

        T kp;

        // local thread copies. _Untouched is locked, so is used directly (main purpose is to update _Untouched).  
        uint32_t * TestArray;
        uint32_t * pTestArrayBeyondEnd;  // Pointer to an invalid memory beyond last array item!!! This memory must not be read.
        size_t TestArrayCount;
        uint32_t Primorial;
        uint32_t MaxPrime;
        enSieveLength SieveLength;
        T Untouched=0;
        
        // Lock variables
        {
            const std::lock_guard<std::mutex> lock(_Untouched_mutex);

            // evaluate break condition first, ??? overflow on End +1 ???
            if (_Untouched > End) break;
 
            if (_Untouched == Begin) _StartTime = high_resolution_clock::now();

            // change of sieve length?
            SieveLength = _SieveLength;
            T SwitchPoint = _SwitchPointOffset + Begin;  // ??? overflow test, not needed

            if (_Untouched + _Primorial > SwitchPoint) {  // going to exceed switch point in this run //overflow test
                if (SieveLength != enSieveLength::SieveShort) {
                    SieveLength = SetSieveLength(enSieveLength::SieveShort);
                    {
                        const std::lock_guard<std::mutex> lock(_cout_mutex);
                        long double Perc = 100.0L* ( (long double)(_Untouched-Begin) / (long double)(End-Begin) );                        
                        Perc = (std::floor(Perc*10.0L))/10.0L;
                        Log::out() << PId << "> " <<"Switching sieve to: " << (unsigned int) SieveLength << " at: " <<  utils_str::FormatUInt(_Untouched)  << " = ~" << utils_str::FormatNumber(Perc, 4, 1) <<"%\n";
                        // << " SwitchPoint: " << utils_str::FormatUInt(SwitchPoint) << "\n";
                    }
                }
            }
            //fill in local thread copies of Sieve params
            TestArray = _TestArray;
            TestArrayCount = _TestArrayCount;
            pTestArrayBeyondEnd = &TestArray[TestArrayCount];  
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
                _Untouched = End+1;  // overflow test has been performed at the beginning
            }

            // {
            //     const std::lock_guard<std::mutex> lock(_cout_mutex);
            //     Log::out() << PId << "> " << "Sieve: " << (unsigned int) SieveLength << " k: " << utils_str::FormatUInt(k) << " kp: " << utils_str::FormatUInt(kp) << " Untouched: " << utils_str::FormatUInt(Untouched) << "\n";
            // }

        } // End of lock. _Untouched is updated, Untouched, kp and Sieve are set (having local copies to work on)

        // set mpz_kp
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

        // unsigned long long i=0;
        // if (kp < Untouched) {
        //     // seek in TestArray if Untouched is not a multiple of Primorial
        //     T tmp;            
        //     auto TTArrayBegin = high_resolution_clock::now();
        //     // search where to start with testing and set index i
        //     // for(; p_TestArray[i]+kp < Begin; i++);  // this for cycle works only for 64bit variables

        //     // todo use pointer ??
        //     // Beware of endless loop: 1. kp < Untouched 2. kp + Primorial > Untouched
        //     // ??? co kdyz je to posleni prvek??? and nevyjedu z pole???
        //     assert(TestArray[TestArrayCount-1]+kp >= Untouched);
        //     tmp = TestArray[i]+kp;  
        //     while (tmp < Untouched){
        //         ++i;
        //         tmp = TestArray[i]+kp;
        //     }

        //     auto TTArrayDuration = duration_cast<milliseconds>(high_resolution_clock::now() - TTArrayBegin);
        //     if (TTArrayDuration.count()> 0)
        //     {
        //         const std::lock_guard<std::mutex> lock(_cout_mutex);
        //         Log::out() << PId << "> Coprimes array seek: " << TTArrayDuration.count() << " ms\n";
        //     }
        // }

        // embedded binary search searching for value equal to or greater than BinarySearchValue 
        uint32_t *pCoprime;
        {
            if (kp == Untouched) {
                pCoprime = TestArray;
            } else {
                auto TTArrayBegin2 = high_resolution_clock::now();
                uint32_t BinarySearchValue = (uint32_t) (Untouched - kp);
                size_t B=0;
                size_t E=TestArrayCount-1;
                size_t M; 

                // BinarySearchValue = TestArray[E];
                assert(BinarySearchValue >= TestArray[B]);
                assert(BinarySearchValue <= TestArray[E]);

                do {
                    //M = B + (E - B) / 2; // safer code with respect to possible integer overflow in binary search, not needed here
                    M = (E + B) / 2;  // no overflow can happen for the coprimes number is ~1.7 mio for 19# and ~36.5 mio for 23#
                    if (TestArray[M] < BinarySearchValue){
                        B=M;
                    } else {
                        E=M;
                    }
                } while (B + 1 < E);
                // cycle was optimized as much as possible with cost of an extra step outside of a loop
                // for correct inputs == operator should be ok, but if value beeing searched is smaller than the first item, the first item is returned
                if (TestArray[B] >= BinarySearchValue) { 
                    pCoprime = &TestArray[B];            
                } else {
                    pCoprime = &TestArray[E];
                }

                auto TTArrayDuration2 = duration_cast<milliseconds>(high_resolution_clock::now() - TTArrayBegin2);
                if (TTArrayDuration2.count() > 0)
                {
                    const std::lock_guard<std::mutex> lock(_cout_mutex);
                    Log::out() << PId << "> Coprimes array binary seek: " << TTArrayDuration2.count() << " ms\n";
                }
            }
        }

        // i is now a valid index within the array bounds
        // pCoprime = &TestArray[i];
        // assert(pCoprime == &TestArray[i]); 


        // this is the core functionality consuming resources, be carefull
        while(true)
        {   
            uint32_t Coprime = *pCoprime; //TestArray[i];
            pCoprime++;
            T X = kp + Coprime;  // ??? overflow ???
            mpz_add_ui(mpz_X, mpz_kp, Coprime); //ok

            if(X > End) break;            
            
            if (GF->GenFunct(X, mpz_X)!=0) {
                res = X;
                RETURN(res);
            };
    
            // i++;
            // if(i == TestArrayCount) {
            //     // primorial range is done, return to outer cycle to read new primorial
            //     break;
            // }

            if(pCoprime == pTestArrayBeyondEnd) {
                // primorial range is done, return to outer cycle to read new primorial
                break;
            }


        }

        // print progress 
        // long double NewPerc = (100.0L*(long double)((kp + TestArray[i-1])-Begin)/(long double)(End-Begin));
        long double NewPerc = (100.0L*(long double)((kp + *(pCoprime-1))-Begin)/(long double)(End-Begin));
        if (NewPerc > 100.0) NewPerc = 100.0; // break on exceeding End leads greater value than 100%
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
                    const std::lock_guard<std::mutex> lock(_cout_mutex);
                    PrintProgress(iPId, _Percent, MinTillEnd);
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
T Sieve2Generator<T>::SetSwitchPoint(const T & Begin, const T & End, unsigned int threads){
    
    _SwitchPointOffset=0;   
    _SwitchPoint=0; 
    
    // if (threads <= 1 || _Primorial1 == 0 || _Primorial2 == 0) {
    // no competition, use large sieve as it is more effective
    // however for old and slow CPUs short sieve might be preferable due to more frequent updates

    if (_Primorial1 != 0 && _Primorial2 != 0) {
        T SPOffset;
        T Cnt2perCPU;
        T Primorials2 = Begin / _Primorial2;
        if (Primorials2 * _Primorial2 < Begin )  Primorials2++;
        T BeginOffset2 = Primorials2 * _Primorial2;  // overflow ???

        // overflow, Begin is near TMax barrier
        if (BeginOffset2 >= Begin) {
            T Len = 0;
            if (BeginOffset2 < End) {
                Len = ((End - BeginOffset2) + 1);
            }
            BeginOffset2 -= Begin; // make it an offset from the Beginning 

            T Cnt2 =  Len / _Primorial2;
            assert(threads>0);
            Cnt2perCPU = Cnt2 / threads;

            SPOffset =  Cnt2perCPU * _Primorial2 * threads;
            
            
            // if BeginOffset2 is too close to Primorial, may be harmful
            // todo finetuning for very small BeginOffset2 ??? compare with ending part
            // if (SPOffset > 0) SPOffset += BeginOffset2; 
            if (SPOffset>0) {
                SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveLong);
            } else {
                SetSieveLength(Sieve2Generator<T>::enSieveLength::SieveShort);
            }

            // long double SmallPrimorials = ((((End - (SPOffset + Begin) + 1)) / (long double) _Primorial1) ) /  (long double) threads; 
            long double SmallPrimorials = ((  (long double) ((End - (SPOffset + Begin) + 1)) / (long double) _Primorial1) ) /  (long double) threads; 
            Log::out() << "Each core is supposed to process " << Cnt2perCPU << " large primorial(s) and ~" << utils_str::trim_copy(utils_str::FormatNumber(SmallPrimorials, 3,1)) << " small primorials.\n";

            _SwitchPointOffset = SPOffset; 
            _SwitchPoint =  SPOffset + Begin;
        }
       

    }
    Log::out() << "Sieve switch: " << utils_str::FormatUInt(_SwitchPoint) << " offset: " << utils_str::FormatUInt(_SwitchPointOffset) << "\n";
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
    
    SetSwitchPoint(Begin, End, _Threads);

    _Untouched = Begin;


    // T k = Begin/_Primorial1;
    // T _kp = k * _Primorial1;  // primorial p multiplied by some integer k
    _Percent=-1.0;

    Log::out()  << "Sieve Begin:    " << utils_str::FormatUInt(Begin) << "\n";        
    Log::out()  << "Sieve End  :    " << utils_str::FormatUInt(End) <<  "\n";
    Log::out()  << "End-Begin  :    " << utils_str::FormatUInt(End - Begin) << "\n";        
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
    // GF.ResetClock();
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

    // long double PrimesRatioAfterSieve =  100.L * GF.PrimesCnt() / (( _TestArrayCount / (long double) _Primorial1) * (End - Begin + 1));
    long double PrimesRatioAfterSieve =  100.L * GF.PrimesCnt() / (( (long double) _TestArrayCount / (long double) _Primorial1) * (long double)(End - Begin + 1));
    Log::out() << "Primes: " << utils_str::FormatUInt(GF.PrimesCnt()) << "\n";
    // Log::out() << "Primes ratio (from interval): " << utils_str::FormatNumber(GF.PrimesCnt() * 100.0L / (End - Begin + 1), 6,3) << "%\n";
    Log::out() << "Primes ratio (from interval): " << utils_str::FormatNumber(GF.PrimesCnt() * 100.0L / (long double)(End - Begin + 1), 6,3) << "%\n";
    Log::out() << "Primes ratio (after sieve)  : " << utils_str::FormatNumber(PrimesRatioAfterSieve, 6,3) << "%\n";


    Log::out() << "Multithreading Sieve Duration [m]: " << GF.DurationMinutes() <<  " [s]:" << GF.DurationSeconds() <<"\n";
 
    if (res!=0) return res; 
    return 0;
}



#endif