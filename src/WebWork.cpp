#include "WebWork.hpp"
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


std::string WebWork::PrepareWebPostString(WebGetWork &GetWork, GeneratorFunctionBitStatistics & BS){
    std::string post ("work=");
    post.append(std::to_string(GetWork.new_work_id));
    post.append(",");
    post.append(std::to_string(GetWork.asking_worker_id));
    post.append(",");
    post.append(std::to_string(GetWork.task_id));
    post.append(",");
    post.append(std::to_string(GetWork.c_power2));    
    post.append(",");
    post.append(std::to_string(GetWork.new_begin));
    post.append(",");
    post.append(std::to_string(GetWork.new_end));
    post.append(BS.WebPost_Serialize_CntPrimesWithOneAtPosition());
    return post;
}


WebGetWork WebWork::ParseJsonNewWork(const std::string &JSON){
    WebGetWork ret;
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
        cout << "Error parsing JSON: ";
        cout << JSON << endl;
        return ret;
    }
    return ret;
    // [{"task_id":"1","asking_worker_id":"1","new_begin":"0","new_end":"3","c_power2":"2","new_work_id":"62"}]   
}

WebGetWork WebWork::GetWebWork(WorkerStruct &w){
    std::string WebInput;
    WebGetWork GetWork;
    GetWork.ParsedOK=false;
    do {
        do {
            const char url[] = "https://prime17.000webhostapp.com/get_work.php";
            std::string GetWorkPostString ("worker_id=");
            GetWorkPostString.append(w.worker_id);
            WebInput = web.WebPost(url, GetWorkPostString);
            if (WebInput.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        } while (WebInput.empty());  
        std::string WebOutput = web.HTMLFindOutput(WebInput);
        if (WebOutput.empty()) {
        cout <<  "WebOutput is missing: " << WebInput << endl;
        } else {
            try {
                GetWork = ParseJsonNewWork(WebOutput);
            } catch (const std::exception& e){
                cout << "Error parsing JSON" << endl;
            }
        }
        if (!GetWork.ParsedOK) std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    } while (!GetWork.ParsedOK);
    return GetWork;
}

void WebWork::ProcessWebWork(WebGetWork &GetWork, WorkerStruct &w){

    cout << "Bit statistics for power: " << GetWork.c_power2 << " Offset Begin: " << GetWork.new_begin << " Offset End: " << GetWork.new_end << endl;
    if (GetWork.c_power2 <= 63) {
        GeneratorFunctionBitStatistics BSMT(GetWork.c_power2);
        SieveGenerator<unsigned long long> Sieve(C_SieveGeneratorDefaultMaxPrime);

        // w.LoadThreads();
        Sieve.Threads(w.ThreadsPct());

        unsigned long long Offset = static_cast<unsigned long long>(1) << GetWork.c_power2;
        Sieve.WorkMT(Offset + GetWork.new_begin, Offset + GetWork.new_end, BSMT);
        BSMT.SaveFile();

        // GetWebNewWorkHTMLPage();
        const char url[] = "https://prime17.000webhostapp.com/post_work.php";
        std::string PostString = PrepareWebPostString(GetWork, BSMT);
        cout << PostString << endl;
        std::string WebResponse = web.WebPost(url, PostString);
        // cout << WebResponse << endl;
        cout << web.HTMLFindOutput(WebResponse) << endl;

    } else {
        cout << "128 bit integers not ready yet, but it should be quite an easy task ..." << endl;
        // GeneratorFunctionBitStatistics BSMT(GetWork.c_power2);
        // SieveGenerator<uint128_t> Sieve(C_SieveGeneratorDefaultMaxPrime);
    }
}

void WebWork::WebBitStatisticsWork(WorkerStruct &w) {
    cout << endl;
    WebGetWork GetWork = GetWebWork(w);
    ProcessWebWork(GetWork, w);
    w.LoadThreads();
}