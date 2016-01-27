//
//  mcWorkerManager.cpp
//  Moon
//
//  Created by duj on 16/1/26.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#include "mcCore.h"


mcWorkerManager::~mcWorkerManager(){
	while (!_workers.empty())
	{
		mcWorkerPtr mcw =  _workers.front();
		_workers.pop_front();
	}

}

void mcWorkerManager::init(){

	for(int i = 0; i < _workerNum; i++){
		mcWorkerPtr mcw = new mcWorker;
		_workers.push_back(mcw);
		mcw->start();
	}
}

void mcWorkerManager::pushFD(int fd){
	if(_workers.size() <=_order){
		_order = 0;
	}
	PINT(_workers.size());
	PINT(_order);
	int i = 0;
	PSTR("WorkerManage get FD");
	for (std::list<mcWorkerPtr>::iterator it=_workers.begin(); it != _workers.end(); ++it){
		mcWorkerPtr mcw = *it;
		PINT(i-_order);
		if(i == _order){
			_order++;
			mcw->pushFD(fd);
			PINT(_order);
			PINT(i);
			return;
		}
		i++;
	}
}
