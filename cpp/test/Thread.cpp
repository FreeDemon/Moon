#include <iostream>
#include <IceUtil/Thread.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/RecMutex.h>
#include <IceUtil/Monitor.h>
#include <list>
#include <stdio.h>
#include <unistd.h>


using namespace std;

class WorkQueue : public IceUtil::Shared, public IceUtil::Monitor<IceUtil::RecMutex> {
	public:
		void putStr(string s);
		string getStr();
	private:
		list<string> _queue;
};

void WorkQueue::putStr(string s)
{
	IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(*this);
	_queue.push_back(s);
	//cout << "Queue Size Push: " << _queue.size() << endl;
	notify();
}

string WorkQueue::getStr()
{
	IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(*this);
	while(_queue.size() == 0){
		wait();
	}
	//cout << "Queue Size: " << _queue.size() << endl;
	string s = _queue.front();
	_queue.pop_front();
	return s;
}

typedef IceUtil::Handle<WorkQueue> WorkQueuePtr;

class ReaderThread : public IceUtil::Thread {
	public:
		ReaderThread(WorkQueuePtr wq) : _wq(wq) {};
		virtual void run();
	private:
		WorkQueuePtr _wq;
};

void ReaderThread::run()
{

	IceUtil::ThreadControl self;
	while(1){
		cout << "ID:" << self.id() <<" " << _wq->getStr() << endl;
	}
}

class WriterThread : public IceUtil::Thread {
	public:
		WriterThread(WorkQueuePtr wq) : _wq(wq) {};
		virtual void run();
	private:
		WorkQueuePtr _wq;
};
void WriterThread::run()
{
	for(int i = 0; i < 100; i++){
		IceUtil::ThreadControl self;
		char buf[256];
		sprintf(buf, "i=%d id=%ld", i, self.id());
		_wq->putStr(buf);
	}
}

class RefreshTask : public IceUtil::TimerTask {
public:
	RefreshTask(WorkQueuePtr wp) : _wp(wp) {};
	virtual void runTimerTask();
private:
	WorkQueuePtr _wp;
};

void RefreshTask::runTimerTask()
{
	_wp->putStr(IceUtil::Time::now().toDateTime());
}

int main()
{
	vector<IceUtil::ThreadControl> threads;
	WorkQueuePtr wq = new WorkQueue;
	IceUtil::ThreadPtr wt = new WriterThread(wq);
	threads.push_back(wt->start());
	IceUtil::ThreadPtr rt = new ReaderThread(wq);
	threads.push_back(rt->start());

	IceUtil::TimerPtr tm = new IceUtil::Timer;

	cout <<"-------1" <<  IceUtil::Time::now().toDateTime()<< endl;
	tm->scheduleRepeated(new RefreshTask(wq), IceUtil::Time::seconds(3));
	cout <<"-------2" <<  IceUtil::Time::now().toDateTime()<< endl;
	
	sleep(10);

	cout << IceUtil::generateUUID() << endl;
	tm->destroy();
	for(vector<IceUtil::ThreadControl>::iterator i = threads.begin(); i != threads.end(); ++i){
		i->join();
	}


	cout << IceUtil::Time::now().toDateTime()<< endl;

	while(1){
		sleep(1);
	}

	return 1;
}
