#include "utils.hpp"


#ifdef __MINGW64__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

// #ifdef _WIN64
// #include <shlobj.h>
// #endif

#ifdef __unix__
#include <pwd.h>
#endif

#include <iostream>
#include <filesystem>

#include <cstdlib>
#include <unistd.h>

#include <fstream>


#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> 
#include <iomanip> // put_time
#include <string>

namespace fs = std::filesystem;

std::string utils::ExtractFilename( const std::string& path ) {
    return fs::path(path).stem();
}

std::string utils::getenvEx(const std::string &variable){
    try {
        const char *ret = getenv(variable.c_str());
        // The std::string class calls strlen when you use std::string(str), which will produce an access violation when passed a NULL string.
        if (!ret) return std::string();
        return std::string(ret);
    }
    catch (const std::exception& e){
    }
    return "";
}    

std::string utils::getConfigDir(){
    auto configDir = std::string("");
    
    #ifdef _WIN64
    configDir = getenvEx("APPDATA");

    #elif __unix__
    // https://specifications.freedesktop.org/basedir-spec/basedir-spec-0.6.html
    configDir = getenvEx("XDG_CONFIG_HOME");
    if (configDir.empty()) {
        configDir = getenvEx("HOME");
        if (configDir.empty()) {
            configDir = getpwuid(getuid())->pw_dir;
        }
    }
    #endif
    fs::path p(configDir);
    
    p /= C_AppNameConfigHomeSubdir; //.config
    fs::create_directory(p);

    p /= C_AppName; //prime17
    fs::create_directory(p);
    
    return p.string();
}

std::string utils::getConfigFile(){
    fs::path p(getConfigDir());
    p /= C_AppNameConfig;
    return p.string();
}

std::string utils::getLogFile(){
    fs::path p(getConfigDir());
    p /= C_AppLog;
    return p.string();
}


std::string utils::getConfigFileThreads(){
    fs::path p(getConfigDir());
    p /= C_AppNameConfigThreads;
    return p.string();
}


void utils::SaveJson(const Json::Value &JS, const std::string &ConfigFile){
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";

    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    // cout << ConfigFile << endl;
    std::ofstream outputFileStream(ConfigFile);
    writer -> write(JS, &outputFileStream);
}

Json::Value utils::LoadJsonFile(const std::string &ConfigFile) try {
    Json::Value root;
    std::ifstream rawJson(ConfigFile);
    if (!rawJson.good()) {
        const std::string C_Msg("Json file probably does not exist.\n");
        // Log::out().logRight(C_Msg);
        throw(C_Msg);   
    }
    rawJson >> root;
    // cout << root;
    return root;
} catch(...) {
    const std::string C_Msg("Load Json parsing error.\n");
    // Log::out().logRight(C_Msg);
    throw(C_Msg);   
}

Json::Value utils::parseJsonStr(const std::string & rawJson){
    Json::Value root;
    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    reader->parse(rawJson.c_str(), rawJson.c_str() + rawJson.length(), &root, &err);
    return root;
}


std::string utils::GetCurrentDateTime(){
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}


