//
//  main.cpp
//  Moon
//
//  Created by duj on 16/1/24.
//  Copyright © 2016年 FreeDemon. All rights reserved.
//

#include <iostream>
#include "mcCore.h"


int main(int argc, const char * argv[]) {
    // insert code here...
    evthread_use_pthreads();
    mcEngine *mce = new mcEngine("./cf");
    if(mce){
	mce->init();
    }
    while(1){
	PSTR("00");
	sleep(1);
	}
    return 0;
}
