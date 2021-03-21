#ifndef WEBSERVICE_HPP
#define WEBSERVICE_HPP

#include <iostream>
#include <string>
#include "Log.hpp"

// class Log final: public Singleton<Log>
class WebService final: public Singleton<WebService>  {
    private:
    const std::string C_NoInternet{"Internet connection is not available, however my creator wishes me to be be resilient and versatile. If I'm bothering you, just kill me. Ouch :-(\n"};
    const std::string C_NoDNS{"This is a typicall situation after multiple kills when the web server detects multiple dangling connections and as a safety measure refuses further connections. Fortunately after timeouts connections will expire and communicaton may resume as normal. Let's wait 5 minutes and see..."};

    enum class HTTP_SpecialCommunication 
    {
        NoSpecialSettings,
        UseNativeCA,
        UseLocalCertificate,        
        TurnOffSSL,
        Failure
    };

    bool _Initialized = false;

    HTTP_SpecialCommunication _SpecialCommunication;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    
    public:

    WebService();
    std::string WebPost(const char url[], std::string &PostString);
    std::string static HTMLFindOutput(std::string &HTMLPage);
};

#endif