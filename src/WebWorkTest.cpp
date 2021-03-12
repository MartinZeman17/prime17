#include "WebWork.hpp"
#include "WorkerStruct.hpp"



void WebBitStatistics(WorkerStruct &w){

    WebWork WW;
    // todo break ??
    
    while(w.ThreadsPct()>0) {
        WW.WebBitStatisticsWork(w);   
    } 
}
