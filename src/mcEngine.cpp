//
//  mcEngine.cpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#include "mcCore.h"


using namespace std;
using namespace IceUtil;


bool mcEngine::init(){
    if(_ConfigFile.empty()){
        PSTR("Config File is empty");
        return false;
    }
    
    _Config = new IceUtil::Properties;
    _Config->load(_ConfigFile);
 
    string public_ip = _Config->getProperty("public_ip");
    int public_port = _Config->getPropertyAsInt("public_port");
    
    if(public_ip.empty() || public_port <= 0){
        PSTR("Parse config file error");
        return false;
    }
    
    _Base = event_base_new();
    if(!_Base){
        return false;
    }

    _PublicListener = _initListener(public_ip, public_port);
    if(!_PublicListener){
        return false;
    }

    _wkm = new mcWorkerManager(_Config->getPropertyAsInt("workers"));
    _wkm->init();

     event_base_dispatch(_Base);
    
}

void mcEngine::dispatchFD(int fd)
{
	_wkm->pushFD(fd);
}

void mc_public_listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                           struct sockaddr *sa, int socklen, void *user_data)
{
    mcEngine *mce = (mcEngine *)user_data;
    mce->dispatchFD(fd);;
    PSTR("Engine get a FD");
}


struct evconnlistener *mcEngine::_initListener(string ip, int port){
    struct sockaddr_in sin;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_addr.s_addr = inet_addr(ip.c_str());
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    
    struct evconnlistener *licListener = evconnlistener_new_bind(_Base, mc_public_listener_cb, (void *)this,
                                              LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
                                              (struct sockaddr*)&sin,
                                              sizeof(sin));
    if(!licListener){
        PSTR("Could not create a listener!");
        return NULL;
    }

    return licListener;
}
