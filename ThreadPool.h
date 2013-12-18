#pragma once
#include "windows.h"
#include <memory>
#include <queue>
#include <vector>
#include <functional>
#include <utility>

template<typename T>
class Future{
protected:
	struct Data{
	HANDLE s;
	bool complete;
	T result;
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data()){
		data->s = CreateSemaphore(NULL,0,1,NULL);
		data->complete = false;
	}
	void set(T val){
		data->result = val;
		data->complete = true;
		ReleaseSemaphore(data->s,1,NULL);
	}
	inline bool complete(){
		return data->complete;
	}
	T val(){
		while(!data->complete){
			if(!WaitForSingleObject(data->s,INFINITE)){
				ReleaseSemaphore(data->s,1,NULL);
			}
		}
		return data->result;
	}
};

template<typename T>
class Future<Future<T>>{
protected:
	struct Data{
	HANDLE s;
	bool complete;
	Future<T> result;
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data()){
		data->s = CreateSemaphore(NULL,0,1,NULL);
		data->complete = false;
	}
	void set(T val){
		data->result = val;
		data->complete = true;
		ReleaseSemaphore(s,1);
	}
	inline bool complete(){
		return data->complete && result.complete();
	}
	T val(){
		while(!complete){
			if(!WaitForSingleObject(data->s,INFINITE)){
				ReleaseSemaphore(data->s,1);
			}
		}
		return result.val();
	}
};

template<>
class Future<void>{
public:
	void set(void)
	{}
	void val()
	{}
};

struct WorkerThreadData{
public:
	HANDLE thread;
	DWORD threadId;
	HANDLE mutex;
	bool stop;
	bool blocked;
	std::queue<std::function<void()>> workData; 
	WorkerThreadData(): stop(false),blocked(false),thread(NULL){
		mutex = CreateMutex(NULL,FALSE,NULL);
	}
};

struct DispatchData{
	std::vector<WorkerThreadData> workerThreads;
	std::queue<std::function<void()>> dispatchQueue;
	HANDLE dispatchMutex;
	//HANDLE dispatchSemaphore;
	/*HANDLE queuingThread;
	int roundRobinIndex;*/
};

template<typename T>
DWORD WINAPI awaitWorkerThreadProc(LPVOID lpParameter){
	std::pair<DispatchData*,Future<T>>* stuff = (std::pair<DispatchData*,Future<T>>*)lpParameter;
	DispatchData* data = stuff->first;
	while(true/*!stuff->second.complete()*/){
		WaitForSingleObject(data->dispatchMutex,INFINITE);
		if(!data->dispatchQueue.empty()){
			std::function<void()> workUnit = data->dispatchQueue.front();
			data->dispatchQueue.pop();
			ReleaseMutex(data->dispatchMutex);
			workUnit();
		} else {
			ReleaseMutex(data->dispatchMutex);
			break; //no work to do, let this thread die
		}	
	}
	delete stuff;
	return 0;
}

class ThreadPool{
private:
	DispatchData sharedState;
	std::queue<std::function<void()>> mainQueue;
	HANDLE mainQueueMutex;
public:
	ThreadPool();
	ThreadPool(int numberOfThreads);
	void async(std::function<void()> workUnit);
	template<typename T>
	Future<T> async(std::function<T()> func){
		Future<T> f;
		async([f,func]()mutable{
			f.set(func());
		});
		return f;
	}
	/*template<typename T>
	Future<Future<T>> async(std::function<Future<T>()> func){
		Future f;
		async([&f,func]{
			f.set(func());
		});
	}*/
	template<typename T>
	T await(Future<T> result){
		__declspec(thread) static int depth;
		depth++; //track recursion depth
		//put upper bound on recursion depth to prevent stack overflow
		if(depth < 100){ 
			while(!result.complete()){
				//do some other work while we wait	
				if(!WaitForSingleObject(sharedState.dispatchMutex,10)){
					if(!sharedState.dispatchQueue.empty()){
						std::function<void()> workUnit = sharedState.dispatchQueue.front();
						sharedState.dispatchQueue.pop();
						ReleaseMutex(sharedState.dispatchMutex);
						workUnit();
					} else {
						ReleaseMutex(sharedState.dispatchMutex);
					}
				}
			}
		} else {
			//spawn a new worker thread so we don't blow out our stack
			//auto data = make_pair(&sharedState,result);
			std::pair<DispatchData*,Future<T>>* data = new std::pair<DispatchData*,Future<T>>;
			data->first = &sharedState;
			data->second = result;
			HANDLE thread = CreateThread(NULL,0,awaitWorkerThreadProc<T>,(LPVOID)data,0,NULL);
			//block on the return value rather than the other thread
			//WaitForSingleObject(thread,INFINITE);
		}
		depth--;
		return result.val();
	}
	void onMain(std::function<void()> workUnit);
	void processMainQueueUnit();
};

extern ThreadPool glPool;