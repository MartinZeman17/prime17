#ifndef UTILS_HPP
#define UTILS_HPP


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


namespace fs = std::filesystem;


namespace utils {

    constexpr std::string_view C_AppName = "Prime17"; 
    constexpr std::string_view C_AppNameConfig = "Prime17.json";
    constexpr std::string_view C_AppLog = "Prime17.log";

    constexpr std::string_view C_AppNameConfigThreads = "threads.json"; 
    constexpr std::string_view C_AppNameConfigHomeSubdir =".config";

    std::string static getenvEx(const std::string &variable){
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

    std::string static getConfigDir(){
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

    std::string static getConfigFile(){
        fs::path p(getConfigDir());
        p /= C_AppNameConfig;
        return p.string();
    }

    std::string static getLogFile(){
        fs::path p(getConfigDir());
        p /= C_AppLog;
        return p.string();
    }


    std::string static getConfigFileThreads(){
        fs::path p(getConfigDir());
        p /= C_AppNameConfigThreads;
        return p.string();
    }


    void static SaveJson(const Json::Value &JS, const std::string &ConfigFile){
        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        builder["indentation"] = "   ";

        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        // cout << ConfigFile << endl;
        std::ofstream outputFileStream(ConfigFile);
        writer -> write(JS, &outputFileStream);
    }

    Json::Value static LoadJsonFile(const std::string &ConfigFile) try {
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
    
    Json::Value static parseJsonStr(const std::string & rawJson){
        Json::Value root;
        JSONCPP_STRING err;
        Json::CharReaderBuilder builder;
        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        reader->parse(rawJson.c_str(), rawJson.c_str() + rawJson.length(), &root, &err);
        return root;
    }

}
#endif