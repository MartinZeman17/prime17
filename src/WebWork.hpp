#ifndef WEBWORK_HPP
#define WEBWORK_HPP
// #define _HAS_STD_BYTE 0
// This order of includes works fine
// https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
// https://stackoverflow.com/questions/58059048/ambiguous-byte-definition-in-rpcndr-and-cstddef
#include <curl/curl.h>

#ifdef __MINGW64__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

#include "GeneratorFunctionBitStatistics.hpp"
#include "SieveGenerator.hpp"
#include "Sieve2Generator.hpp"

#include "WebService.hpp"
#include "WorkerStruct.hpp"

#include <iostream>
#include <string>
#include <cassert>



using std::endl;
using std::cout;

template <class T>
class clsNewWork {
    public:
    unsigned int task_id=0;
    unsigned long long asking_worker_id=0; 
    T new_begin=0;
    T new_end=0;
    unsigned int c_power2=0;
    unsigned long long new_work_id=0;
    bool ParsedOK=false;

    inline T Offset() const noexcept {
        return static_cast<unsigned long long>(1) << c_power2;
    }

    void LogHeader() const {
        Log::out() << "Started at:     " << utils::GetCurrentDateTime() << "\n";
        Log::out() << "Power:          " << c_power2 << "\n";
        assert(c_power2 <= 63);
        
        Log::out() << "Begin (offset): " << utils_str::FormatUInt(new_begin) << "\n";
        Log::out() << "End   (offset): " << utils_str::FormatUInt(new_end) << "\n";

        // unsigned long long Offset = static_cast<unsigned long long>(1) << c_power2;
        Log::out() << "Begin:         " << utils_str::FormatNumber( ((long double) 100.0 * (long double) new_begin ) / (long double) Offset(), 13,9) << "%\n";
        Log::out() << "End:           " << utils_str::FormatNumber( ((long double) 100.0 * (long double) new_end ) / (long double) Offset(), 13,9) << "%\n";
    }
};

// template <class T>
class WebWork {
    public:

    void WebBitStatisticsWork(WorkerStruct &w);
    
    private:

    std::string PrepareWebPostString(clsNewWork<uint64_t> &NewWork, GeneratorFunctionBitStatistics & BS);

    clsNewWork<uint64_t> ParseJsonNewWork(const std::string &JSON);

    clsNewWork<uint64_t> GetWebWork(WorkerStruct &w);

    void ProcessWebWork(clsNewWork<uint64_t> &NewWork, WorkerStruct &w);

};

#endif
