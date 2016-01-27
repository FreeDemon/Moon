//
//  mcWorkerManager.hpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#ifndef mcWorkerManager_hpp
#define mcWorkerManager_hpp

#include "mcCommon.h"
#include "mcCore.h"

using namespace std;
using namespace IceUtil;

class mcWorkerManager : public IceUtil::Shared {
public:
    mcWorkerManager(){ _workerNum = 2; };
    mcWorkerManager(int num) : _workerNum(num){};
    ~mcWorkerManager();
    void setWorkerNum(int num) { _workerNum = num <= 0? 2:num; }
    void init();
    void pushFD(int fd);
    
private:
    int  _workerNum;
    int _order;
    list<mcWorkerPtr> _workers;
};

typedef IceUtil::Handle<mcWorkerManager> mcWorkerManagerPtr;

#endif /* mcWorkerManager_hpp */
