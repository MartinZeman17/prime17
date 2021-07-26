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
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> 
#include <iomanip> // put_time
#include <string>

namespace fs = std::filesystem;

namespace utils{

    constexpr std::string_view C_AppName = "Prime17"; 
    constexpr std::string_view C_AppNameConfig = "Prime17.json";
    constexpr std::string_view C_AppLog = "Prime17.log";

    // std::string AppName;

    constexpr std::string_view C_AppNameConfigThreads = "threads.json"; 
    constexpr std::string_view C_AppNameConfigHomeSubdir =".config";

    std::string ExtractFilename( const std::string& path );

    std::string getenvEx(const std::string &variable);

    std::string getConfigDir();

    std::string getConfigFile();

    std::string getLogFile();

    std::string getConfigFileThreads();

    void SaveJson(const Json::Value &JS, const std::string &ConfigFile);

    Json::Value LoadJsonFile(const std::string &ConfigFile);

    Json::Value parseJsonStr(const std::string & rawJson);

    std::string GetCurrentDateTime();

}

#endif

