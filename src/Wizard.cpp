#include <iostream>
#include <filesystem>
#include <thread>
#include <fstream>

#ifdef __MINGW64__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

#include "WebService.hpp" 
#include "utils_str.hpp" 
#include "utils.hpp"
#include "WorkerStruct.hpp"


namespace fs = std::filesystem;
using std::string;
using std::to_string;
using std::vector;


namespace Wizard { 

    WorkerStruct RegisterUserDB(std::string & email){
        // WebService web;
        bool bSleep1=false;
        bool bSleep2=false;
        bool bSuccess = false;
        WorkerStruct SelectedWorker;
        do {
            const char url[] = "https://prime17.000webhostapp.com/register_user.php";
            std::string PostString ("email=" + email);
            std::string WebResponse = WebService::out().WebPost(url, PostString);
            if (WebResponse.empty()) {
                if (!bSleep1) {
                    Log::out() << "Look like a wrong time to connect to internet. Wait a bit please.\n";     
                    bSleep1=true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
            // Log::out() <<WebResponse << "\n";
            WebResponse=WebService::out().HTMLFindOutput(WebResponse);
            // Log::out() << WebResponse <<"\n";
            Json::Value root = utils::parseJsonStr(WebResponse);
            std::vector<WorkerStruct> workers;
            WorkerStruct::JsonWorkersToArr(root, workers);
            
            std::string UserName("");
            if (workers.size()==0){
                constexpr std::string_view C_InputUserName = "\nWow, a new contributor! Nice to meet you.\nI know it is a pure charity so thank you very much.\nWhat is your name?";  
                Log::out() << C_InputUserName << "\n";
                // getline(std::cin, UserName);
                UserName = Log::out().getlineLeft();
                utils_str::string_replace(UserName, ",", "");
                utils_str::string_replace(UserName, "\t", "");

                Log::out() << "Nice to meet you, " << UserName << "\n\n";
            }

            if (workers.size()>0){
                UserName = workers[0].u_name;
                constexpr std::string_view C_InputSelectWorker = "Are you reinstalling one of the following computers(s)?\n"
                    "If so, please enter the computer name or its id.\nOr if this is a new machine that I do not know yet, please tell me its name.\n";  
                Log::out() << C_InputSelectWorker << "\n";
                Log::out() << "id" << "\t" << "computer" << "\t" << "YYYY-MM-DD HH:MM:SS" << "\n";
                for (auto &w : workers){
                    Log::out() << w.worker_id << "\t" << w.w_name << "\t" << w.w_registered << "\n";
                }
            } else {
                constexpr std::string_view C_InputNewWorker = "\nLet us measure the performance of different computers (workers). \n"
                    "If you plan to have just one worker (which is perfectly fine) you may leave it blank, \nI will use your id/mail instead. \n"
                    "So how do you call this computer?";        
                Log::out() << C_InputNewWorker << "\n";
            }
            std::string SelectedWorkerInput;
            // getline(std::cin, SelectedWorkerInput);
            SelectedWorkerInput = Log::out().getlineLeft();
            if (SelectedWorkerInput.empty()) SelectedWorkerInput = email;
            utils_str::string_replace(SelectedWorkerInput, ",", "");  // remove split char just for safety reasons
            utils_str::string_replace(SelectedWorkerInput, "\t", "");

            SelectedWorker.worker_id="";
            for (auto &w : workers){
                if (w.worker_id == SelectedWorkerInput || w.w_name == SelectedWorkerInput) {
                    SelectedWorker = w;
                    SelectedWorker.SaveToConfig();
                    Log::out() << "Welcome back! Registering an old worker: " << w.worker_id << " " << w.u_name << "\n";
                    return SelectedWorker;
                }
            }

            // I have a new worker name, get its seed and id from db
            // get threads strategy
            const char url_register_worker[] = "https://prime17.000webhostapp.com/register_worker.php";
            PostString = "email_worker_name=" + email + "," + SelectedWorkerInput + "," + UserName;
            WebResponse = WebService::out().WebPost(url_register_worker, PostString);
            if (WebResponse.empty()) {
                if (!bSleep2) {
                    Log::out() << "We need an internet connection desperately. Please sip a cup of coffee first.\n";     
                    bSleep2=true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
            // Log::out() <<WebResponse << "\n";
            WebResponse=WebService::out().HTMLFindOutput(WebResponse);
            // Log::out() << WebResponse <<"\n";

            Json::Value root2 = utils::parseJsonStr(WebResponse);
            std::vector<WorkerStruct> registered_worker;
            WorkerStruct::JsonWorkersToArr(root2, registered_worker);
            SelectedWorker=registered_worker[0];

            constexpr std::string_view C_Proceed = "\n\nThis is a final check whether everything is ok. If you want to continue just press Enter.\n";                    
            Log::out() << C_Proceed << "\n";
            std::string Proceed = Log::out().getlineLeft();
            if (Proceed == "" || Proceed == "y" || Proceed == "Y" || Proceed == "yes" || Proceed == "Yes") bSuccess =true;
        } while (!bSuccess);
        Log::out() << "What a nice day, a new worker has been registered: " << SelectedWorker.w_name << "\n";
        SelectedWorker.SaveToConfig();
        
        return SelectedWorker;
    }

    void ThreadSettings(WorkerStruct& worker){
        constexpr std::string_view C_CPU_01 = "\nHow much of your CPU power are you willing to dedicate to Prime17?\n";
        Log::out() << C_CPU_01;
        const auto processor_count = std::thread::hardware_concurrency();
        std::string C_CPU_02 = "I have detected " + to_string(processor_count) + " logical CPU cores.";
        if (processor_count > 8) C_CPU_02.append(" Yummy!");
        Log::out() << C_CPU_02 << "\n";
        constexpr std::string_view C_CPU_03 = "If you enter 100 (as in 100%) I will use all of them and your other tasks will be rather slow.\n"
            "If you enter 50 I will utilise approximately half of your CPU power and the rest will remain available for you work.\n"
            "Enter a percentage in range 0-100:\n";
        Log::out() << C_CPU_03;
        
        std::string ThreadsPercent;
        // getline(std::cin, ThreadsPercent); 
        ThreadsPercent = Log::out().getlineLeft();

        long double ThreadsPct=100;
        try {
            ThreadsPct = stold(ThreadsPercent);
        } catch(const std::exception& e){
            ThreadsPct = 100;
        }
        if (ThreadsPct>100.0) ThreadsPct=100;
        if (ThreadsPct<0)  ThreadsPct=0;
    
        worker.ThreadsPct((int) ThreadsPct);
        worker.SaveToConfigThreads();
        
    }
    

    bool CheckWorker(WorkerStruct &w){
        // Log::out() << "Worker check start\n" ;
        bool bSleep1 = false;
        const char url[] = "https://prime17.000webhostapp.com/check_worker.php";
        std::string PostString(w.PrepareCheckWorkerPost());
        // Log::out() << PostString << "\n";
        std::string WebResponse = WebService::out().WebPost(url, PostString);
        utils_str::trim(WebResponse);
        if (WebResponse.empty()) {
            if (!bSleep1) {
                Log::out() << "Internet is crucial to check the worker. Please sip a cup of tea first. \n";     
                bSleep1=true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        } 
        WebResponse=WebService::out().HTMLFindOutput(WebResponse);
        Log::out() << WebResponse <<"\n";
        if (WebResponse=="[{\"status\":\"OK\"}]") {
            Log::out() << "Worker has been checked against Prime 17 web.\n" ;
            return true;
        }
        Log::out().logRight("Worker check failed. It could be lack of internet connection or may be someone was toying with the configuration file.\n");
        Log::out().logRight(WebResponse);
        Log::out().logRight("\n");
        return false;       
    }

    WorkerStruct NewWorker(){
        constexpr std::string_view C_InputEmail = "Hi, I am Prime17. I wonder, have we already met? \nI would like to use your e-mail as an unique id in order to monitor your specific progress. "
                "If you enter an invalid email, I will still be satisfied, alas! if necessary, we will not be able to contact you in the future.\n"
                "I will treat you mail with care and when neccessary I will replace some letters with an asterix sign *.\n"
                "Please do not use backspace (it is a known bug) and try to type everything carefully.\n"                
                "Please enter your mail/ID: ";
        Log::out() << C_InputEmail << "\n";
        std::string email;
        // getline(std::cin, email);
        email = Log::out().getlineLeft(); 
        // email= "martin.zeman17@gmail.com";
        utils_str::string_replace(email, ",", "");
        utils_str::string_replace(email, "\t", "");
        utils_str::string_replace(email, " ", "");

        Log::out() << "Thank you, your mail/ID is: " << email << "\n\n";
        WorkerStruct worker = RegisterUserDB(email);
        ThreadSettings(worker);
        return worker;
    }

}