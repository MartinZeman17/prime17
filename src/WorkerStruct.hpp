#ifndef WORKERSTRUCT_HPP
#define WORKERSTRUCT_HPP

#ifdef __MINGW64__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

#include <iostream>
#include <filesystem>
#include <thread>
#include <fstream>  
#include <string>

#include "utils.hpp"
   
class WorkerStruct{
    private:
    int _ThreadsPct;

    public:
    std::string worker_id;
    std::string w_seed;
    std::string w_name;
    std::string u_name;
    std::string w_registered;
    std::string u_registered;

    // const int &ThreadsPct = _ThreadsPct;
    
    const int& ThreadsPct() const { return _ThreadsPct; } 

    void ThreadsPct(const int& in) {
        if (in<=0) {
            _ThreadsPct=0;
        } else if (in>100) {
            _ThreadsPct=100;
        } else {
            _ThreadsPct = in; 
        }
    } 

    bool ThreadsPct(const std::string& in) {
        int Pct;
        try {
            Pct = std::stoi(in);
            ThreadsPct(Pct);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // void setThreadsPct

    WorkerStruct() = default;
    
    // WorkerStruct(const std::string & ConfigFile) {
    //     Json::Value JS = utils::LoadJsonFile(ConfigFile);
    // }

    #define C_JSON_worker_id  "worker_id"
    #define C_JSON_w_seed  "w_seed"
    #define C_JSON_u_name "u_name"
    #define C_JSON_w_name "w_name"
    #define C_JSON_w_registered "w_registered"
    #define C_JSON_u_registered "u_registered"

    bool LoadConfig() try {
        Json::Value JS = utils::LoadJsonFile(utils::getConfigFile());
        worker_id =  JS[C_JSON_worker_id].asString();
        w_seed = JS[C_JSON_w_seed].asString();
        u_name = JS[C_JSON_u_name].asString();
        w_name = JS[C_JSON_w_name].asString();
        w_registered = JS[C_JSON_w_registered].asString();
        u_registered = JS[C_JSON_u_registered].asString();
        
        return true;
    } catch(...){
        return false;
    }    

    void SaveToConfig() const {
        Json::Value JS;
        JS[C_JSON_worker_id] = worker_id;
        JS[C_JSON_w_seed] = w_seed;
        JS[C_JSON_w_name] = w_name;
        JS[C_JSON_u_name] = u_name;
        JS[C_JSON_w_registered] = w_registered;
        JS[C_JSON_u_registered] = u_registered;

        std::string ConfigFile=utils::getConfigFile();
        utils::SaveJson(JS, ConfigFile);
    }

    #define C_JSON_CPUPercent "CPUPercent"
    #define C_JSON_CPUPercentTmp "CPUPercentTmp"
    // const std::string_view C_JSON_CPUPercent ="CPUPErcent";

    void SaveToConfigThreads() const {
        Json::Value JS;
        JS[C_JSON_CPUPercent] = ThreadsPct();

        std::string ConfigFile=utils::getConfigFileThreads();
        utils::SaveJson(JS, ConfigFile);        
    }

    bool SaveToConfigThreadsTmp(std::string &ThreadsTmp) const {
        WorkerStruct w;
        if (!w.ThreadsPct(ThreadsTmp)) return false;
        Json::Value JS = utils::LoadJsonFile(utils::getConfigFileThreads());
        JS[C_JSON_CPUPercentTmp] = w.ThreadsPct();
        std::string ConfigFile=utils::getConfigFileThreads();
        utils::SaveJson(JS, ConfigFile);        
        return true;
    }

    bool LoadThreads() try {
        Json::Value JS = utils::LoadJsonFile(utils::getConfigFileThreads());
        if (JS.isMember(C_JSON_CPUPercentTmp)) {
            ThreadsPct(JS[C_JSON_CPUPercentTmp].asUInt());
            JS.removeMember(C_JSON_CPUPercentTmp);
            utils::SaveJson(JS, utils::getConfigFileThreads());
        } else{
            ThreadsPct(JS[C_JSON_CPUPercent].asUInt());
        }

        return true;
    } catch(...){
        return false;
    }    

    void static JsonWorkersToArr(Json::Value &root, std::vector<WorkerStruct> &workers) {
        Json::Value::ArrayIndex i = root.size();
        if (i==0) return;
        // std::string aa = root.toStyledString();
        
        for (Json::Value::iterator it=root.begin(); it!=root.end(); ++it) {
            WorkerStruct w;
            w.w_name= "";
            w.u_name= "";
            w.w_seed="";
            w.worker_id = "";
            w.w_registered = "";
            w.u_registered= "";
            if (it->isMember("worker_id")) w.worker_id = (*it)["worker_id"].asString(); 
            if (it->isMember("w_seed")) w.w_seed = (*it)["w_seed"].asString(); 
            if (it->isMember("w_name")) w.w_name = (*it)["w_name"].asString(); 
            if (it->isMember("u_name")) w.u_name = (*it)["u_name"].asString();
            if (it->isMember("w_registered")) w.w_registered = (*it)["w_registered"].asString();
            if (it->isMember("u_registered")) w.u_registered = (*it)["u_registered"].asString();
            workers.push_back(w);
        }
    }


    std::string PrepareCheckWorkerPost(){
        std::string ret = "worker=" + worker_id  + "," + w_seed + "," + w_name + "," + u_name;
        return ret;
    }
};

#endif