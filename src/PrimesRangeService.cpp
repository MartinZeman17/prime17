#include "PrimesRangeService.hpp"
#include <mutex>


// static variable must be defined outside of header, beware: declaration is not a definition
std::vector<std::weak_ptr<PrimesRange>> PrimesRangeService::p_Obj;
std::mutex PrimesRangeService::p_Obj_mutex;  

std::shared_ptr<PrimesRange> PrimesRangeService::FindExisting(const unsigned long long Begin, const unsigned long long End){  
    // lock should be probably considered as active operation changing content (state of weak pointer) -> ToDo: verify
    // ToDo: dead object should be removed
    const std::lock_guard<std::mutex> lock(p_Obj_mutex);

    for(auto &obj : PrimesRangeService::p_Obj){
        if (auto shObj = obj.lock()) {
            if (shObj->Begin == Begin && shObj->End == End) return shObj;
        }
    }
    return nullptr;
}

std::shared_ptr<PrimesRange> PrimesRangeService::GetPrimesRange(const unsigned long long Begin, const unsigned long long End){
    
    std::shared_ptr<PrimesRange> res = PrimesRangeService::FindExisting(Begin, End);
    if (res) return res;

    // suitable object not found -> create new

    // this syntax creates object directly within shared pointer class which prevents an object copy 
    auto shared = std::make_shared<PrimesRange>(Begin, End);
    
    std::weak_ptr<PrimesRange> weak = shared;

    // lock vector when making changes    
    const std::lock_guard<std::mutex> lock(p_Obj_mutex);
    p_Obj.push_back(weak);

    return shared;
}
