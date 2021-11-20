#include "WebWork.hpp"
#include "utils.hpp"


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



std::string WebWork::PrepareWebPostString(clsNewWork &NewWork, GeneratorFunctionBitStatistics & BS){
    std::string post ("work=");
    post.append(std::to_string(NewWork.new_work_id));
    post.append(",");
    post.append(std::to_string(NewWork.asking_worker_id));
    post.append(",");
    post.append(std::to_string(NewWork.task_id));
    post.append(",");
    post.append(std::to_string(NewWork.c_power2));    
    post.append(",");
    post.append(std::to_string(NewWork.new_begin));
    post.append(",");
    post.append(std::to_string(NewWork.new_end));
    post.append(BS.WebPost_Serialize_CntPrimesWithOneAtPosition());
    return post;
}


clsNewWork WebWork::ParseJsonNewWork(const std::string &JSON){
    clsNewWork ret;
    ret.ParsedOK = false;
    // TODO error handling does not work in the C world
    try{
        Json::Value obj = utils::parseJsonStr(JSON);
        // Json::Reader reader;
        // Json::Value obj;
        // reader.parse(JSON,obj);
        ret.task_id = (unsigned int)    std::stoul(obj[0]["task_id"].asString(), nullptr);
        ret.asking_worker_id =          std::stoll(obj[0]["asking_worker_id"].asString(), nullptr);
        ret.new_begin =                 std::stoll(obj[0]["new_begin"].asString(), nullptr);
        ret.new_end =                   std::stoll(obj[0]["new_end"].asString(), nullptr);
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
    // [{"task_id":"1","asking_worker_id":"1","new_begin":"0","new_end":"3","c_power2":"2","new_work_id":"62"}]   
}

clsNewWork WebWork::GetWebWork(WorkerStruct &w){
    std::string WebInput;
    clsNewWork NewWork;
    NewWork.ParsedOK=false;
    do {
        do {
            const char url[] = "https://prime17.000webhostapp.com/get_work.php";
            std::string NewWorkPostString ("worker_id=");
            NewWorkPostString.append(w.worker_id);
            WebInput = WebService::out().WebPost(url, NewWorkPostString);
            if (WebInput.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        } while (WebInput.empty());  
        std::string WebOutput = WebService::out().HTMLFindOutput(WebInput);
        if (WebOutput.empty()) {
        Log::out() <<  "WebOutput is missing: " << WebInput << "\n";
        } else {
            try {
                NewWork = ParseJsonNewWork(WebOutput);
            } catch (const std::exception& e){
                Log::out() << "Error parsing JSON" << "\n";
            }
        }
        if (!NewWork.ParsedOK) std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    } while (!NewWork.ParsedOK);
    return NewWork;
}

void WebWork::ProcessWebWork(clsNewWork &NewWork, WorkerStruct &w){
    NewWork.LogHeader();

    if (NewWork.c_power2 <= 63) {        
        GeneratorFunctionBitStatistics BSMT(NewWork.c_power2);
        SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);

        // w.LoadThreads();
        Sieve.Threads(w.ThreadsPct());
        Sieve.WorkMT(NewWork.Offset() + NewWork.new_begin, NewWork.Offset() + NewWork.new_end, BSMT);
        // BSMT.SaveFile(); //It does not make sense to save partial files.


        std::string PostString = PrepareWebPostString(NewWork, BSMT);
        // Log::out() << PostString << "\n";

        if (w.worker_id == "97") {
            Log::out() << "!!!! Yoga WSL worker 97 and Arnold WSL 94 do not post results to web !!!! \n";
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

void WebWork::WebBitStatisticsWork(WorkerStruct &w) {
    Log::out() << "\n";
    clsNewWork NewWork = GetWebWork(w);
    ProcessWebWork(NewWork, w);
    w.LoadThreads();
}