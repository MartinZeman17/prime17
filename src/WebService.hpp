#ifndef WEBSERVICE_HPP
#define WEBSERVICE_HPP

#include <iostream>
#include <string>

using std::endl;
using std::cout;


class WebService {
    private:
    const std::string C_NoInternet{"Internet connection is not available, however my creator wishes me to be be resilient and versatile. If I'm bothering you, just kill me. Ouch :-("};
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