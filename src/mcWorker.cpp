//
//  mcWorker.cpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#include "mcCore.h"

static void _pipe_readcb(struct bufferevent *bev, void *user_data)
{
	PSTR("Call pipe readcb");
        struct evbuffer *input = bufferevent_get_input(bev);
	struct event_base *_Base = (struct event_base *)user_data;
        int fd;
        evbuffer_copyout(input, &fd, sizeof(int));
	PINT(fd);
	evbuffer_drain(input, sizeof(int));

    	mcConnectionPtr c = new mcConnection(fd, _Base);
        c->enable();
}
void mcWorker::run()
{
    _Base = event_base_new();
  if (pipe(_ipc) == -1) {
	return;
   }
   int flag;
   if((flag = fcntl(_ipc[1], F_GETFL, 0)) == -1)
   {
      return;
   }
   if(fcntl(_ipc[1], F_SETFL, flag | O_NONBLOCK) == -1)
   {
      return ;
   }
    
    struct bufferevent *pev;
   pev = bufferevent_socket_new(_Base, _ipc[0], BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
   bufferevent_setcb(pev, _pipe_readcb, NULL, NULL, _Base);
   bufferevent_enable(pev, EV_READ);


   // while(1){
    //    int fd = _queue.get();
        PSTR("Worker get FD");
 
	event_base_dispatch(_Base);
        PSTR("Worker get Outer");
    //}
}

void mcWorker::pushFD(int fd){
    if(fd <= 0) return;    
   // _queue.put(fd);
    write(_ipc[1], &fd, sizeof(int));
    PINT(fd);
}
