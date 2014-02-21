#pragma once
#include <memory>
#include <queue>
#include <vector>
#include <functional>
#include <utility>
//use Standard library concurrency when available
#define USE_STD_CONCURRENCY

#ifdef _MSC_VER
#if _MSC_VER < 1700
#undef USE_STD_CONCURRENCY
#endif
#endif

#if USE_STD_CONCURRENCY
#include <thread>
#include <mutex>
#define MUTEX std::unique_ptr<std::mutex>
#define NEW_MUTEX std::unique_ptr<std::mutex>(new mutex())
#define ACQUIRE_MUTEX(m) m->lock()
#define TRY_MUTEX(m) m->try_lock()
#define RELEASE_MUTEX(m) m->unlock()
#define THREAD_ID thread::id
#define LPVOID void*
#using std::thread;
#else
//no STL support for concurrency
#ifdef _WIN32
#define WIN32_CONCURRENCY
#include "windows.h"
#define MUTEX HANDLE
#define NEW_MUTEX CreateMutex(NULL,FALSE,NULL)
#define ACQUIRE_MUTEX(m) while(WaitForSingleObject(m,INFINITE)!=0) { }
#define TRY_MUTEX(m) 0==WaitForSingleObject(m,10)
#define RELEASE_MUTEX(m) ReleaseMutex(m)
#define thread_local __declspec(thread)
#define THREAD_ID DWORD
typedef HANDLE thread;
#else
#error "Unsupported compiler environment"
#endif
#endif

template<typename T>
class Future{
protected:
	struct Data{
	MUTEX m;
	bool started;
	bool complete;
	T result;
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data()){
		data->m = NEW_MUTEX;
		data->started = false;
		data->complete = false;
	}
	void start(){
		ACQUIRE_MUTEX(data->m);
		data->started = true;
	}
	void set(T val){
		data->result = val;
		data->complete = true;
		RELEASE_MUTEX(data->m);
	}
	inline bool complete(){
		return data->complete;
	}
	T val(){
		for(;;){
			ACQUIRE_MUTEX(data->m);
			if(data->complete == true){
				break;
			}
			RELEASE_MUTEX(data->m);
			Sleep(100);
		}
		RELEASE_MUTEX(data->m);
		return data->result;
	}
};

template<typename T>
class Future<Future<T>>{
protected:
	struct Data{
	MUTEX m;
	bool complete;
	bool hasThen;
	Future<T> result;
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data()),hasThen(false){
		data->m = NEW_MUTEX;
		data->complete = false;
	}
	void set(T val){
		data->result = val;
		data->complete = true;
		RELEASE_MUTEX(data->m);
	}
	inline bool complete(){
		return data->complete && result.complete();
	}
	T val(){
		for(;;){
			ACQUIRE_MUTEX(data->m);
			if(data->complete == true){
				break;
			}
			RELEASE_MUTEX(data->m);
			Sleep(100);
		}
		RELEASE_MUTEX(data->m);
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
	thread thread;
	THREAD_ID threadId;
	MUTEX mutex;
	bool stop;
	bool blocked;
	std::queue<std::function<void()>> workData; 
	WorkerThreadData(): stop(false),blocked(false),thread(NULL){
		mutex = NEW_MUTEX;
	}
};

struct DispatchData{
	std::vector<WorkerThreadData> workerThreads;
	std::queue<std::function<void()>> dispatchQueue;
	MUTEX dispatchMutex;
	//HANDLE dispatchSemaphore;
	/*HANDLE queuingThread;
	int roundRobinIndex;*/
};

template<typename T>
DWORD WINAPI awaitWorkerThreadProc(LPVOID lpParameter){
	std::pair<DispatchData*,Future<T>>* stuff = (std::pair<DispatchData*,Future<T>>*)lpParameter;
	DispatchData* data = stuff->first;
	while(true/*!stuff->second.complete()*/){
		ACQUIRE_MUTEX(data->dispatchMutex);
		if(!data->dispatchQueue.empty()){
			std::function<void()> workUnit = data->dispatchQueue.front();
			data->dispatchQueue.pop();
			RELEASE_MUTEX(data->dispatchMutex);
			workUnit();
		} else {
			RELEASE_MUTEX(data->dispatchMutex);
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
			f.start();
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
		thread_local static int depth;
		depth++; //track recursion depth
		//put upper bound on recursion depth to prevent stack overflow
		if(depth < 100){ 
			while(!result.complete()){
				//do some other work while we wait	
				if(TRY_MUTEX(sharedState.dispatchMutex)){
					if(!sharedState.dispatchQueue.empty()){
						std::function<void()> workUnit = sharedState.dispatchQueue.front();
						sharedState.dispatchQueue.pop();
						RELEASE_MUTEX(sharedState.dispatchMutex);
						workUnit();
					} else {
						RELEASE_MUTEX(sharedState.dispatchMutex);
					}
				}
			}
		} else {
			//spawn a new worker thread so we don't blow out our stack
			//auto data = make_pair(&sharedState,result);
			std::pair<DispatchData*,Future<T>>* data = new std::pair<DispatchData*,Future<T>>;
			data->first = &sharedState;
			data->second = result;
			#ifdef WIN32_CONCURRENCY
				thread worker = CreateThread(NULL,0,awaitWorkerThreadProc<T>,(LPVOID)data,0,NULL);
			#else
				thread worker(awaitWorkerThreadProc<T>,(LPVOID)data);
				worker.detach();
			#endif
			//block on the return value rather than the other thread
			//WaitForSingleObject(thread,INFINITE);
		}
		depth--;
		return result.val();
	}
	void onMain(std::function<void()> workUnit);
	bool processMainQueueUnit(); //returns true if work was done
};

extern ThreadPool glPool;