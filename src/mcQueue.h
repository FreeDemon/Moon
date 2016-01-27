//
//  mcQueue.h
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#ifndef mcQueue_h
#define mcQueue_h

#include "mcCommon.h"
#include "mcCore.h"

template<class T> class mcQueue : public IceUtil::Monitor<IceUtil::Mutex> {
public:
    mcQueue() : _waitingReaders(0) {}
    void put(const T& item) {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        _q.push_back(item);
        if (_waitingReaders)
            notify();
    }
    T get() {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        while (_q.size() == 0) {
            try {
                ++_waitingReaders;
                wait();
                --_waitingReaders;
            } catch (...) {
                --_waitingReaders;
                throw;
            } }
        T item = _q.front();
        _q.pop_front();
        return item;
    }
private:
    list<T> _q;
    short _waitingReaders;
};


#endif /* mcQueue_h */
