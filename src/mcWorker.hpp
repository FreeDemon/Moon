//
//  mcWorker.hpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#ifndef mcWorker_hpp
#define mcWorker_hpp

#include "mcCommon.h"
#include "mcCore.h"

using namespace std;
using namespace IceUtil;

class mcWorker :  public IceUtil::Thread {
public:
//    mcWorker();
//    ~mcWorker();
    
    virtual void run();
    void pushFD(int fd);
    
private:
   int _ipc[2];
    mcQueue<int> _queue;
    struct event_base *_Base;
    IceUtil::ThreadControl _thread;
};

typedef IceUtil::Handle<mcWorker> mcWorkerPtr;

#endif /* mcWorker_hpp */
