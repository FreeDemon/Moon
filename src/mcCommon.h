//
//  mcCommon.h
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#ifndef mcCommon_h
#define mcCommon_h

#include    <unistd.h>
//#include    <sys/epoll.h>
//#include <sys/timerfd.h>
#include    <sys/select.h>
#include    <sys/types.h>   /* basic system data types */
#include    <sys/socket.h>  /* basic socket definitions */
#include    <sys/time.h>    /* timeval{} for select() */
#include    <time.h>                /* timespec{} for pselect() */
#include    <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include    <arpa/inet.h>   /* inet(3) functions */
#include    <errno.h>
#include    <fcntl.h>               /* for nonblocking */
#include    <netdb.h>
#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <sys/stat.h>    /* for S_xxx file mode constants */
#include    <sys/uio.h>             /* for iovec{} and readv/writev */
#include    <unistd.h>
#include    <sys/wait.h>
#include    <sys/un.h>              /* for Unix domain sockets */
#include    <net/if.h>
#include    <sys/ioctl.h>
#include    <netinet/tcp.h>
#include    <fcntl.h>
#include    <stdint.h>
#include    <assert.h>
#include    <setjmp.h>
#include    <execinfo.h>
#include    <pthread.h>
#include    <dirent.h>
#include    <stdarg.h>
#include    <iostream>
#include    <string>
#include    <list>

#include <IceUtil/IceUtil.h>
#include <IceUtil/Monitor.h>

#include "display.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"
#include "event2/thread.h"

#endif /* mcCommon_h */
