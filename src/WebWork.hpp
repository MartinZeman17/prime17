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


#include "utils.hpp"


#include <curl/curl.h>
#include <iostream>
#include <string>



using std::endl;
using std::cout;

template <class T>
class clsNewWork {
    public:
    unsigned int task_id=0;
    unsigned long long asking_worker_id=0; 
    T newBegin_=0;
    T newEnd_=0;
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
        
        Log::out() << "Begin (offset): " << utils_str::FormatUInt(newBegin_) << "\n";
        Log::out() << "End   (offset): " << utils_str::FormatUInt(newEnd_) << "\n";

        // unsigned long long Offset = static_cast<unsigned long long>(1) << c_power2;
        Log::out() << "Begin:         " << utils_str::FormatNumber( ((long double) 100.0 * (long double) newBegin_ ) / (long double) Offset(), 13,9) << "%\n";
        Log::out() << "End:           " << utils_str::FormatNumber( ((long double) 100.0 * (long double) newEnd_ ) / (long double) Offset(), 13,9) << "%\n";
    }
};

// template <class T>
class WebWork {
    public:

    void WebBitStatisticsWork(WorkerStruct &w);
    
    private:

    std::string PrepareWebPostString(clsNewWork<uint64_t> &NewWork, GeneratorFunctionBitStatistics<uint64_t> & BS);

    clsNewWork<uint64_t> ParseJsonNewWork(const std::string &JSON);

    clsNewWork<uint64_t> GetWebWork(WorkerStruct &w);

    void ProcessWebWork(clsNewWork<uint64_t> &NewWork, WorkerStruct &w);

};



// template <class T>
std::string WebWork::PrepareWebPostString(clsNewWork<uint64_t> &NewWork, GeneratorFunctionBitStatistics<uint64_t> & BS){
    std::string post ("work=");
    post.append(std::to_string(NewWork.new_work_id));
    post.append(",");
    post.append(std::to_string(NewWork.asking_worker_id));
    post.append(",");
    post.append(std::to_string(NewWork.task_id));
    post.append(",");
    post.append(std::to_string(NewWork.c_power2));    
    post.append(",");
    post.append(std::to_string(NewWork.newBegin_));
    post.append(",");
    post.append(std::to_string(NewWork.newEnd_));
    post.append(BS.WebPost_SerializeCntPrimesWithOneAtPosition_());
    return post;
}

// template <class T>
clsNewWork<uint64_t> WebWork::ParseJsonNewWork(const std::string &JSON){
    clsNewWork<uint64_t> ret;
    ret.ParsedOK = false;
    // TODO error handling does not work in> the C world
    try{
        Json::Value obj = utils::parseJsonStr(JSON);
        // Json::Reader reader;
        // Json::Value obj;
        // reader.parse(JSON,obj);
        ret.task_id = (unsigned int)    std::stoul(obj[0]["task_id"].asString(), nullptr);
        ret.asking_worker_id =          std::stoll(obj[0]["asking_worker_id"].asString(), nullptr);
        ret.newBegin_ =                 std::stoll(obj[0]["newBegin_"].asString(), nullptr);
        ret.newEnd_ =                   std::stoll(obj[0]["newEnd_"].asString(), nullptr);
        ret.c_power2 = (unsigned int)   std::stoul(obj[0]["power2"].asString(), nullptr);
        ret.new_work_id =               std::stoll(obj[0]["new_work_id"].asString(), nullptr);
        ret.ParsedOK = true;
    }  catch (const std::exception& e){
        ret.ParsedOK = false;
        Log::out() << "Error parsing JSON: ";
        Log::out() << JSON << "\n";
        return ret;
    }
    return ret;
    // [{"task_id":"1","asking_worker_id":"1","newBegin_":"0","newEnd_":"3","c_power2":"2","new_work_id":"62"}]   
}

// template <class T>
clsNewWork<uint64_t> WebWork::GetWebWork(WorkerStruct &w){
    std::string WebInput;
    clsNewWork<uint64_t> NewWork;
    NewWork.ParsedOK=false;
    bool bSleep=false;
    bool bSleep2=false;
    do {
        do {
            const char url[] = "https://prime17.000webhostapp.com/get_work.php";
            std::string NewWorkPostString ("worker_id=");
            NewWorkPostString.append(w.worker_id);
            WebInput = WebService::out().WebPost(url, NewWorkPostString);
            if (WebInput.empty()) {
                if (!bSleep){
                    Log::out() <<  "Internet outage. I am going to doze off for a moment...\n";
                    bSleep=true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
        } while (WebInput.empty());  
        std::string WebOutput = WebService::out().HTMLFindOutput(WebInput);
        if (WebOutput.empty()) {
        // Log::out() <<  "WebOutput is missing: " << WebInput << "\n";
        } else {
            try {
                NewWork = ParseJsonNewWork(WebOutput);
            } catch (const std::exception& e){
                Log::out() << "Error parsing JSON" << "\n";
            }
        }
        if (!NewWork.ParsedOK) {
            if (!bSleep2){
                Log::out() <<  "There is something wrong with an internet connection. I am going to doze off for a moment...\n";
                bSleep2=true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
    } while (!NewWork.ParsedOK);
    // Log::out() << "New work received" << "\n";
    return NewWork;
}

// template <class T>
void WebWork::ProcessWebWork(clsNewWork<uint64_t> &NewWork, WorkerStruct &w){
    NewWork.LogHeader();

    if (NewWork.c_power2 <= 63) {        
        GeneratorFunctionBitStatistics<uint64_t> BSMT(NewWork.c_power2);
        SieveGenerator<uint64_t> Sieve(C_SieveGeneratorDefaultMaxPrime);

        // w.LoadThreads();
        Sieve.Threads(w.ThreadsPct());
        Sieve.WorkMT(NewWork.Offset() + NewWork.newBegin_, NewWork.Offset() + NewWork.newEnd_, BSMT);
        // BSMT.SaveFile(); //It does not make sense to save partial files.


        std::string PostString = PrepareWebPostString(NewWork, BSMT);
        // Log::out() << PostString << "\n";

        if (w.worker_id == "97" || w.worker_id == "95") {
            Log::out() << "!!!! Yoga WSL worker 97 and Arnold WSL 95 do not post results to web !!!! \n";
        } else{
            #ifdef NDEBUG
            const char url[] = "https://prime17.000webhostapp.com/post_work.php";
            std::string WebResponse = WebService::out().WebPost(url, PostString);
            // Log::out() << web.HTMLFindOutput(WebResponse) << "\n";
            Log::out().logToFile(WebService::out().HTMLFindOutput(WebResponse));
            Log::out().logToFile("\n");
            #else
            Log::out() << "!!!! Debug version does not post results to web !!!! \n";
            #endif
        }


    } else {
        Log::out() << "128 bit integers not ready yet, but it is supposed to be quite an easy task ..." << "\n";
        // GeneratorFunctionBitStatistics BSMT(NewWork.c_power2);
        // SieveGenerator<uint128_t> Sieve(C_SieveGeneratorDefaultMaxPrime);
    }
}

// template <class T>
void WebWork::WebBitStatisticsWork(WorkerStruct &w) {
    Log::out() << "\n";
    clsNewWork<uint64_t> NewWork = GetWebWork(w);
    ProcessWebWork(NewWork, w);
    w.LoadThreads();
}





#endif
