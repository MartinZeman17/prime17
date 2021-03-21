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
#include "WebService.hpp"
#include "WorkerStruct.hpp"

#include <iostream>
#include <string>



using std::endl;
using std::cout;

    struct WebGetWork {
        unsigned int task_id=0;
        unsigned long long asking_worker_id=0; 
        unsigned long long new_begin=0;
        unsigned long long new_end=0;
        unsigned int c_power2=0;
        unsigned long long new_work_id=0;
        bool ParsedOK=false;
    };

class WebWork {
    public:

    void WebBitStatisticsWork(WorkerStruct &w);
    
    private:
    // WebService web;

    std::string PrepareWebPostString(WebGetWork &GetWork, GeneratorFunctionBitStatistics & BS);

    WebGetWork ParseJsonNewWork(const std::string &JSON);

    WebGetWork GetWebWork(WorkerStruct &w);

    void ProcessWebWork(WebGetWork &GetWork, WorkerStruct &w);

};

#endif
