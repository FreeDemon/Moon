//
//  mcEngine.hpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#ifndef mcEngine_hpp
#define mcEngine_hpp

#include "mcCommon.h"
#include "mcCore.h"

using namespace std;
using namespace IceUtil;

enum LISTENER_TYPE{
    LST_HTTP,
    LST_NORMAL
};

class mcEngine {
public:
    mcEngine(string filepath):_ConfigFile(filepath){};
    ~mcEngine();
    
    bool init();
    PropertiesPtr getConfig();
    void dispatchFD(int fd);
    
private:
    
    struct evconnlistener *_initListener(string ip, int port);
    string _ConfigFile;
    PropertiesPtr _Config;
    struct event_base *_Base;
    struct evconnlistener *_PublicListener;
    mcWorkerManagerPtr   _wkm;
    //struct evconnlistener *__AdminListener;
};


#endif /* mcEngine_hpp */
