//
//  mcConnection.hpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#ifndef mcConnection_hpp
#define mcConnection_hpp

#include "mcCommon.h"
#include "mcCore.h"

using namespace std;
using namespace IceUtil;


class mcConnection : public IceUtil::Shared{
public:
    mcConnection(int fd, struct event_base *base) : _fd(fd),_Base(base),_Bev(NULL){};
    //~mcConnection();
    
    void enable();
    void disable();
    
private:
    int _fd;
    struct event_base *_Base;
    struct bufferevent *_Bev;
    
    //void _conn_readcb(struct bufferevent *bev, void *user_data);
    //void _conn_writecb(struct bufferevent *bev, void *user_data);
    //void _conn_eventcb(struct bufferevent *bev, short events, void *user_data);
};
typedef IceUtil::Handle<mcConnection> mcConnectionPtr;

#endif /* mcConnection_hpp */
