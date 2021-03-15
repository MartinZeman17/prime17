#include "WebService.hpp"
#include "Log.hpp"


#include <iostream>
// #include <fstream>B

#include <string>
#include <cstring>

#include <chrono>
#include <thread>
#include <curl/curl.h>
// #include <stdbool.h>


size_t WebService::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
//     size_t written;
//     written = fwrite(ptr, size, nmemb, stream);
//     return written;
// }

// void downloadFile(const char* url, const char* fname) {
//     CURL *curl;
//     FILE *fp;
//     CURLcode res;
//     curl = curl_easy_init();
//     if (curl){
//         fp = fopen(fname, "wb");
//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
//         res = curl_easy_perform(curl);
//         curl_easy_cleanup(curl);
//         fclose(fp);
//         std::Log::out() << res << std::"\n";
//     }
// }


std::string WebService::WebPost(const char url[], std::string &PostString){
    CURLcode res;
    // endwin();
    std::string readBuffer("");
    do{
        CURL *curl;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        // curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);
        // std::cout << d->version << "\n"; //Ubuntu 20 7.68.0
        
        if (curl){
            // https://prime17.000webhostapp.com/index.html
            curl_easy_setopt(curl, CURLOPT_URL, url);
            // curl_easy_setopt(curl, CURLOPT_URL, "https://cuni.cz/UK-1.html");

            // curl_easy_setopt(curl, CURsLOPT_VERBOSE, 1L);

            if (_SpecialCommunication==HTTP_SpecialCommunication::UseNativeCA) {
                #ifdef __MINGW64__
                // curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
                // https://blog.usejournal.com/how-to-manually-update-curl-on-ubuntu-server-899476062ad6
                // UBUNTU: libcurl 7.68.0
                // Added in 7.71.0)
                #endif
            }
            else if (_SpecialCommunication==HTTP_SpecialCommunication::UseLocalCertificate) {
                curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-bundle.trust.crt");
            }
            else if (_SpecialCommunication==HTTP_SpecialCommunication::TurnOffSSL) {
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); //FALSE
            }
            // https://curl.se/docs/sslcerts.html
            // https://curl.se/libcurl/c/CURLOPT_SSL_OPTIONS.html
            // Not tested with proxy settings

            char errbuf[CURL_ERROR_SIZE];
            /* provide a buffer to store errors in */
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
            /* set the error buffer as empty before performing a request */
            errbuf[0] = 0;

            // Every std:string must exist until curl_easy_perform call. Othewrise order c_str() pointer to const char may be invalidated!!!
            // There is an option for libcurl to perform deep copy of input post parameters -CURLOPT_COPYPOSTFIELDS?
            // ToDo so this code is dirty and repeating blocks. How to solve it without malloc? Better not to bother in this case... 
            if (!PostString.empty()) curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PostString.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            if(res != CURLE_OK){
                readBuffer = "";
                
                if (res != CURLE_SSL_CACERT_BADFILE || _SpecialCommunication==HTTP_SpecialCommunication::TurnOffSSL) {
                    Log::out() << "libcurl: " << res << ": " << errbuf << "\n";            
                    Log::out() << C_NoInternet << "\n";
                }
                if (res != CURLE_SSL_CACERT_BADFILE) std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            }
            curl_easy_cleanup(curl);
            curl_global_cleanup(); //??
        }
    } while (res == CURLE_COULDNT_RESOLVE_HOST);
    return readBuffer;    
}

WebService::WebService(){
    do {
        _Initialized=false;
        const char url[] = "https://prime17.000webhostapp.com/index.html";
        std::string GetWorkPostString ("");

        _SpecialCommunication = HTTP_SpecialCommunication::NoSpecialSettings;
        std::string WebInput = WebPost(url, GetWorkPostString);
        if (!WebInput.empty()) {
            _Initialized=true;
            return;
        }

        _SpecialCommunication = HTTP_SpecialCommunication::UseNativeCA;
        WebInput = WebPost(url, GetWorkPostString);
        if (!WebInput.empty()) {
            Log::out() << "libcurl using the operating system's native CA store for certificate verification. Works only on Windows when built to use OpenSSL. This option is experimental and behavior is subject to change";
            _Initialized=true;
            return;
        }
        
        _SpecialCommunication = HTTP_SpecialCommunication::UseLocalCertificate;
        WebInput = WebPost(url, GetWorkPostString);
        if (!WebInput.empty()) {
            Log::out() << "libcurl using local certificate";
            _Initialized=true;
            return;
        }

        _SpecialCommunication = HTTP_SpecialCommunication::TurnOffSSL;
        WebInput = WebPost(url, GetWorkPostString);
        if (!WebInput.empty()) {
            Log::out() << "No certificate available. I feel depply sorry for turnig off SSL..." << "\n";
            _Initialized=true;
            return;
        }
        _SpecialCommunication = HTTP_SpecialCommunication::Failure;
        Log::out() << C_NoInternet << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    } while (_SpecialCommunication == HTTP_SpecialCommunication::Failure);
}

// std::string GetWebNewWorkHTMLPage(){
//     const char url[] = "https://prime17.000webhostapp.com/get_work.php";

//     CURL *curl;
//     CURLcode res;
//     std::string readBuffer("");
//     curl = curl_easy_init();
//     if (curl){
//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "worker_id=1");
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//         res = curl_easy_perform(curl);
//         if(res != CURLE_OK){
//             readBuffer = "";
//             // TODO cleanup and wait??
//             // curl_easy_strerror(res));
//             // curl_global_cleanup();
//         }

//         curl_easy_cleanup(curl);
//         // std::Log::out() << res << std::"\n";
//         std::Log::out() << readBuffer << std::"\n";
//     }
//     return readBuffer;
// }

std::string WebService::HTMLFindOutput(std::string &HTMLPage){
    constexpr std::string_view strB ("OUTPUT_BEGIN");
    constexpr std::string_view strE ("OUTPUT_END");  //ToDo const strings in cpp20

    std::size_t foundB = HTMLPage.find(strB);
    if (foundB==std::string::npos) return "";
    foundB+=strB.length();
    // if (HTMLPage[foundB] == '[') foundB++; // skip [
    std::size_t foundE = HTMLPage.find(strE);
    if (foundE==std::string::npos) return "";
    //move just before the text OUTPUT_END
    foundE--;
    
    // if (HTMLPage[foundE] == ']') foundE--; //skip ]
    return HTMLPage.substr(foundB, foundE - foundB + 1);        
}



