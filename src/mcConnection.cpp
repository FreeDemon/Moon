//
//  mcConnection.cpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#include "mcCore.h"


static void _conn_readcb(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    
   PSTR("Read cb");
    while(1){
        size_t len = evbuffer_get_length(input);
        if(len < sizeof(uint32_t)){
            return;
        }
        
        char buf[1024] = {0};
        evbuffer_copyout(input, buf, len);
        PSTR(buf);
        evbuffer_drain(input, len);
	bufferevent_write(bev, buf, len);
    }
}

static void _conn_writecb(struct bufferevent *bev, void *user_data)
{
    
}

static void _conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
    
}

void mcConnection::enable(){
    
    PSTR("Connection enable");
    if(_Bev == NULL){
    
        _Bev = bufferevent_socket_new(_Base, _fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
        if (!_Bev) {
	    PSTR(evutil_gai_strerror(errno));
            return;
        }
    
        bufferevent_setcb(_Bev, _conn_readcb, _conn_writecb, _conn_eventcb, this);
    }
    
    bufferevent_enable(_Bev, EV_READ);

    PSTR("Connection enable2 222222" );
    
    return;
}

void mcConnection::disable(){
    if(_Bev){
        bufferevent_disable(_Bev, EV_READ);
    }
}
