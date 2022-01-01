#include "WebService.hpp"
#include "Log.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <curl/curl.h>


size_t WebService::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}



std::string WebService::WebPost(const char url[], std::string &PostString){
    CURLcode res;
    // endwin();
    std::string readBuffer("");
    do {
        CURL *curl;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        // curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);
        // std::cout << d->version << "\n"; //Ubuntu 20 7.68.0
        
    
        // utils_str::string_replace(PostString, '104.18.108.8');
        if (curl){
            // https://prime17.000webhostapp.com/index.html
            curl_easy_setopt(curl, CURLOPT_URL, url);
            // curl_easy_setopt(curl, CURLOPT_URL, "https://cuni.cz/UK-1.html");

            // curl_easy_setopt(curl, CURsLOPT_VERBOSE, 1L);

            if (SpecialCommunication_==HTTPSpecialCommunication_::UseNativeCA) {
                #ifdef __MINGW64__
                // curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
                // https://blog.usejournal.com/how-to-manually-update-curl-on-ubuntu-server-899476062ad6
                // UBUNTU: libcurl 7.68.0
                // Added in 7.71.0)
                #endif
            }
            else if (SpecialCommunication_==HTTPSpecialCommunication_::UseLocalCertificate) {
                curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-bundle.trust.crt");
            }
            else if (SpecialCommunication_==HTTPSpecialCommunication_::TurnOffSSL) {
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
                unsigned long long Sleep=0;
                
                if (res != CURLE_SSL_CACERT_BADFILE || SpecialCommunication_==HTTPSpecialCommunication_::TurnOffSSL) {
                    Log::out() << "libcurl: " << res << ": " << errbuf << "\n";            
                    Log::out() << C_NoInternet << "\n";
                    Sleep = 5000;
                } else if (res == CURLE_COULDNT_RESOLVE_HOST)  {
                    Log::out() << "libcurl: " << res << ": " << errbuf << "\n";            
                    Log::out() << C_NoDNS << "\n";
                    Sleep = 5*60*1000;
                }
                if (res != CURLE_SSL_CACERT_BADFILE) std::this_thread::sleep_for(std::chrono::milliseconds(Sleep));
            }
            curl_easy_cleanup(curl);
            curl_global_cleanup();
        }
    } while (res == CURLE_COULDNT_RESOLVE_HOST);
    return readBuffer;    
}

WebService::WebService(){
    do {
        Initialized_=false;
        const char url[] = "https://prime17.000webhostapp.com/index.html";
        // const char url[] = "http://104.18.108.8/index.html";
        std::string NewWorkPostString ("");

        SpecialCommunication_ = HTTPSpecialCommunication_::NoSpecialSettings;
        std::string WebInput = WebPost(url, NewWorkPostString);
        if (!WebInput.empty()) {
            Initialized_=true;
            return;
        }

        SpecialCommunication_ = HTTPSpecialCommunication_::UseNativeCA;
        WebInput = WebPost(url, NewWorkPostString);
        if (!WebInput.empty()) {
            Log::out().logRight("libcurl using the operating system's native CA store for certificate verification. Works only on Windows when built to use OpenSSL.\nThis option is experimental and behavior is subject to change.\n");
            Initialized_=true;
            return;
        }
        
        SpecialCommunication_ = HTTPSpecialCommunication_::UseLocalCertificate;
        WebInput = WebPost(url, NewWorkPostString);
        if (!WebInput.empty()) {
            Log::out().logRight("libcurl using local certificate\n");
            Initialized_=true;
            return;
        }

        SpecialCommunication_ = HTTPSpecialCommunication_::TurnOffSSL;
        WebInput = WebPost(url, NewWorkPostString);
        if (!WebInput.empty()) {
            Log::out().logRight("No certificate available. I feel depply sorry for turnig off SSL...\n");
            Initialized_=true;
            return;
        }
        SpecialCommunication_ = HTTPSpecialCommunication_::Failure;
        Log::out() << C_NoInternet << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    } while (SpecialCommunication_ == HTTPSpecialCommunication_::Failure);
}


std::string WebService::HTMLFindOutput(std::string &HTMLPage){
    constexpr std::string_view strB ("OUTPUTBegin_");
    constexpr std::string_view strE ("OUTPUTEnd_");  //ToDo const strings in cpp20

    std::size_t foundB = HTMLPage.find(strB);
    if (foundB==std::string::npos) return "";
    foundB+=strB.length();
    std::size_t foundE = HTMLPage.find(strE);
    if (foundE==std::string::npos) return "";
    foundE--;
    
    return HTMLPage.substr(foundB, foundE - foundB + 1);        
}



//     std::string readBuffer("");
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

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
