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
using std::endl;
using std::cout;
using std::string;
using std::to_string;
using std::vector;


namespace Wizard { 

    WorkerStruct RegisterUserDB(std::string & email){
        WebService web;

        const char url[] = "https://prime17.000webhostapp.com/register_user.php";
        std::string PostString ("email=" + email);
        std::string WebResponse = web.WebPost(url, PostString);
        if (WebResponse.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        // cout <<WebResponse << endl;
        WebResponse=web.HTMLFindOutput(WebResponse);
        // cout << WebResponse <<endl;
        Json::Value root = utils::parseJsonStr(WebResponse);
        std::vector<WorkerStruct> workers;
        WorkerStruct::JsonWorkersToArr(root, workers);
        
        std::string UserName("");
        if (workers.size()==0){
            constexpr std::string_view C_InputUserName = "Wow, a new contributor! Nice to meet you. I know it is a pure charity so thank you very much. What is your name?";  
            cout << C_InputUserName << endl;
            getline(std::cin, UserName);
            utils_str::string_replace(UserName, ",", "");
        }

        if (workers.size()>0){
            UserName = workers[0].u_name;
            constexpr std::string_view C_InputSelectWorker = "Are you reinstalling one of the following computers(s)?\n"
                "If so, please enter the computer name or its id. Or if this is a new machine that I do not know yet, tell me its name please.";  
            cout << C_InputSelectWorker << endl;
            cout << "id" << "\t" << "computer" << "\t" << "YYYY-MM-DD HH:MM:SS" << endl;
            for (auto &w : workers){
                cout << w.worker_id << "\t" << w.w_name << "\t" << w.w_registered << endl;
            }
        } else {
            constexpr std::string_view C_InputNewWorker = "Let us measure the performance of different computers (workers). \n"
                "If you plan to have just one worker (which is perfectly fine) you may leave it blank, I will use your id/mail instead. \n"
                "So how do you call this computer?";        
            cout << C_InputNewWorker << endl;
        }
        std::string SelectedWorkerInput;
        getline(std::cin, SelectedWorkerInput);
        if (SelectedWorkerInput.empty()) SelectedWorkerInput = email;
        utils_str::string_replace(SelectedWorkerInput, ",", "");

        WorkerStruct SelectedWorker;
        SelectedWorker.worker_id="";
        for (auto &w : workers){
            if (w.worker_id == SelectedWorkerInput || w.w_name == SelectedWorkerInput) {
                SelectedWorker = w;
                return SelectedWorker;
            }
        }

        // I have a new worker name, get its seed and id from db
        // get threads strategy
        const char url_register_worker[] = "https://prime17.000webhostapp.com/register_worker.php";
        PostString = "email_worker_name=" + email + "," + SelectedWorkerInput + "," + UserName;
        WebResponse = web.WebPost(url_register_worker, PostString);
        if (WebResponse.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        // cout <<WebResponse << endl;
        WebResponse=web.HTMLFindOutput(WebResponse);
        // cout << WebResponse <<endl;

        Json::Value root2 = utils::parseJsonStr(WebResponse);
        std::vector<WorkerStruct> registered_worker;
        WorkerStruct::JsonWorkersToArr(root2, registered_worker);
        SelectedWorker=registered_worker[0];
        SelectedWorker.SaveToConfig();
        return SelectedWorker;
    }

    void ThreadSettings(WorkerStruct& worker){
        constexpr std::string_view C_CPU_01 = "How much of your CPU power are you willig to dedicate to Prime17? ";
        cout << C_CPU_01 << std::endl;
        const auto processor_count = std::thread::hardware_concurrency();
        std::string C_CPU_02 = "I have detected " + to_string(processor_count) + " logical CPU cores.";
        cout << C_CPU_02 << std::endl;
        constexpr std::string_view C_CPU_03 = " If you enter 100 (as in 100%) I will use all of them and your other tasks will be rather slow. "
            "If you enter 50 I will utilise approximately half of your CPU power and the rest will remain available for you work."
            "Enter a percentage in range 0-100";
        cout << C_CPU_03 << std::endl;
        
        std::string ThreadsPercent;
        getline(std::cin, ThreadsPercent); 

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
    
    WorkerStruct NewWorker(){
        constexpr std::string_view C_InputEmail = "Hi, I am Prime17. I wonder, have we already met? \nI would like to use your e-mail as an unique id in order to monitor your specific progress. "
                "If you enter an invalid email, I will still be satisfied, alas! if necessary, we will not be able to contact you in the future.\n"
                "Please enter your mail/ID: ";
        cout << C_InputEmail << std::endl;
        std::string email;
        getline(std::cin, email); 
        // email= "martin.zeman17@gmail.com";
        utils_str::string_replace(email, ",", "");
        WorkerStruct worker = RegisterUserDB(email);
        ThreadSettings(worker);
        return worker;
    }

}