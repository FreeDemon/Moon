#include <iostream>
#include <unistd.h>
#include "IceUtil/IceUtil.h"
#include "IceUtil/Properties.h"


using namespace IceUtil;
using namespace std;

class ReaderThread : public IceUtil::Thread {
	virtual void run() {
		for(int i = 0; i < 100; i++){
			cout << i << endl;
		}
	}
};

int main()
{
	IceUtil::PropertiesPtr p = new IceUtil::Properties;

	p->load("./cf");


	cout << p->getProperty("test") << endl;;
	cout << p->getProperty("test1") << endl;;
	cout << p->getPropertyAsInt("test1") << endl;;
	vector<string> vs = p->getPropertyAsList("test2");	

	for (std::vector<string>::iterator it = vs.begin(); it != vs.end(); ++it)
    	std::cout << '[' << *it << ']' << endl;

	//IceUtil::ThreadPtr t = new ReaderThread;

	//t->start();

	p = NULL;
	/*while(1){
		sleep(1);
	}*/


	return 0;

}
