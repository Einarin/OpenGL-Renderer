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
	HANDLE s;
	bool complete;
	T result;
	Data():complete(false){
		s = CreateSemaphore(NULL,0,1,NULL);
	}
	~Data(){
		CloseHandle(s);
	}
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data())
	{}
	inline void set(T val){
		data->result = val;
		data->complete = true;
		ReleaseSemaphore(data->s,1,NULL);
	}
	inline T operator=(T val){
		set(val);
		return val;
	}
	inline bool isDone(){
		return data->complete;
	}
	inline T wait(){
		while(0 != WaitForSingleObject(data->s,INFINITE))
		{}
		ReleaseSemaphore(data->s,1,NULL);
		return data->result;
	}
	inline operator T(){
		return wait();
	}
};

template<typename T>
class Future<Future<T>>{
	struct Data{
		HANDLE s;
		bool m_set;
		Future<T> child;
		Data():m_set(false){
		s = CreateSemaphore(NULL,0,1,NULL);
		}
		~Data(){
			CloseHandle(s);
		}
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data)
	{}
	inline bool isDone(){
		return data->m_set && data->child.isDone();
	}
	inline void set(Future<T> result)
	{
		data->child = result;
		data->m_set = true;
		ReleaseSemaphore(data->s,1,NULL);
	}
	inline Future<T> wait(){
		while(0 != WaitForSingleObject(data->s,INFINITE))
		{}
		ReleaseSemaphore(data->s,1,NULL);
		return data->child;
	}
	inline operator Future<T>(){
		return wait();
	}
	/*inline operator T(){
		return wait();
	}*/
};

template<>
class Future<void>{
	struct Data{
	HANDLE s;
	bool complete;
	Data():complete(false){
		s = CreateSemaphore(NULL,0,1,NULL);
	}
	~Data(){
		CloseHandle(s);
	}
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data())
	{}
	inline bool isDone(){
		return data->complete;
	}
	inline void set(void)
	{
		data->complete = true;
		ReleaseSemaphore(data->s,1,NULL);
	}
	inline void wait(){
		while(0 != WaitForSingleObject(data->s,INFINITE))
		{}
		ReleaseSemaphore(data->s,1,NULL);
	}
};

struct WorkerThreadData{
public:
	thread thread;
	THREAD_ID threadId;
	MUTEX mutex;
	bool blocked;
	std::queue<std::function<void()>> workData; 
	WorkerThreadData(): blocked(false),thread(NULL){
		mutex = NEW_MUTEX;
	}
};

struct DispatchData{
	std::vector<WorkerThreadData> workerThreads;
	std::queue<std::function<void()>> dispatchQueue;
	MUTEX dispatchMutex;
	HANDLE dispatchSemaphore;
	bool stop;
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
	std::shared_ptr<DispatchData> sharedState;
public:
	ThreadPool();
	ThreadPool(int numberOfThreads);
	~ThreadPool();
	void async(std::function<void()> workUnit);
	template<typename T>
	Future<T> async(std::function<T()> func){
		Future<T> f;
		async([f,func]()mutable{
			f.set(func());
		});
		return f;
	}
	template<>
	Future<void> async<void>(std::function<void()> func){
		Future<void> f;
		async([f,func]()mutable{
			func();
			f.set();
		});
		return f;
	}
	/*template<typename T>
	Future<Future<T>> async(std::function<Future<T>()> func){
		Future<T> f;
		queue([f,func]()mutable{
			Future<T> result = func();
			f.set(result);
		});
		return f;
	}*/
	template<typename T>
	T await(Future<T> result){
		thread_local static int depth;
		depth++; //track recursion depth
		//put upper bound on recursion depth to prevent stack overflow
		if(depth < 100){ 
			while(!result.isDone() && !sharedState->stop){
				//do some other work while we wait	
				if(TRY_MUTEX(sharedState->dispatchMutex)){
					if(!sharedState->dispatchQueue.empty()){
						std::function<void()> workUnit = sharedState->dispatchQueue.front();
						sharedState->dispatchQueue.pop();
						RELEASE_MUTEX(sharedState->dispatchMutex);
						workUnit();
					} else {
						RELEASE_MUTEX(sharedState->dispatchMutex);
					}
				}
			}
		} else {
			DebugBreak();
			//spawn a new worker thread so we don't blow out our stack
			//auto data = make_pair(&sharedState,result);
			std::pair<DispatchData*,Future<T>>* data = new std::pair<DispatchData*,Future<T>>;
			data->first = &*sharedState;
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
		//because result.done() is true it should be done now
		return result.wait();
	}
};

class WorkQueue{
protected:
	std::queue<std::function<void()>> workQueue;
	HANDLE queueMutex;
public:
	WorkQueue();
	void async(std::function<void()> workUnit);
	template<typename T>
	Future<T> async(std::function<T()> func){
		Future<T> f;
		async([f,func]()mutable{
			f.set(func());
		});
		return f;
	}
	template<>
	Future<void> async<void>(std::function<void()> func){
		Future<void> f;
		async([f,func]()mutable{
			func();
			f.set();
		});
		return f;
	}
	bool processQueueUnit(); //returns true if work was done
};

extern ThreadPool CpuPool;
extern ThreadPool IoPool;
extern WorkQueue glQueue;