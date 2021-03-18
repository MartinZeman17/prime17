#include "WebWork.hpp"
#include "WorkerStruct.hpp"



void WebBitStatistics(WorkerStruct &w){

    WebWork WW;
    // todo break ??
    
    while(w.ThreadsPct()>0) {
        Log::out().init();
        WW.WebBitStatisticsWork(w);   
    } 
}
