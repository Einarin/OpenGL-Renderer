#pragma once
#include <memory>
#include <queue>
#include <vector>
#include <functional>
#include <utility>
#include <iostream>
#include "Semaphore.h"

//use Standard library thread when available
#define USE_STD_THREAD

//Visual Studio before 2012 doesn't have std::thread
#ifdef _MSC_VER
#if _MSC_VER < 1700
#undef USE_STD_THREAD
#endif
#endif

//GCC < 4.8 doesn't have thread_local
#ifdef __GNUC__
#if __GNUC__ < 4 || __GNUC__ == 4 && __GNUC_MINOR__ < 8
#define thread_local __thread
#endif
#endif

#ifdef USE_STD_THREAD
#include <thread>
#include <mutex>
#define MUTEX std::unique_ptr<std::mutex>
#define NEW_MUTEX std::unique_ptr<std::mutex>(new std::mutex)
#define ACQUIRE_MUTEX(m) m->lock()
#define TRY_MUTEX(m) m->try_lock()
#define RELEASE_MUTEX(m) m->unlock()
#define THREAD_ID thread::id
#define LPVOID void*
using std::thread;
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
#endif
#endif

template<typename T>
class Future{
protected:
	struct Data{
	    Semaphore s;
	    bool complete;
	    T result;
	    Data():complete(false){
	    }
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data())
	{}
	inline void set(T val){
		data->result = val;
		data->complete = true;
		data->s.post();
	}
	inline T operator=(T val){
		set(val);
		return val;
	}
	inline bool isDone(){
		return data->complete;
	}
	inline T wait(){
		while(!data->s.wait())
		{}
		data->s.post();
		return data->result;
	}
	inline operator T(){
		return wait();
	}
};

template<typename T>
class Future<Future<T>>{
	struct Data{
		Semaphore s;
		bool m_set;
		Future<T> child;
		Data():m_set(false){
		}
	};
	std::shared_ptr<Data> data;
public:
	Future():data(new Data)
	{}
	inline bool isDone(){
		return data->m_set && data->child.isDone();
	}
	inline void set(const Future<T>& result)
	{
		data->child = result;
		data->m_set = true;
		data->s.post();
	}
	inline Future<T> wait(){
		while(!data->s.wait())
		{}
		data->s.post();
		return data->child;
	}
	inline operator Future<T>(){
		return wait();
	}
};

template<>
class Future<void>{
	struct Data{
	Semaphore s;
	bool complete;
	Data():complete(false){
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
		data->s.post();
	}
	inline void wait(){
		while(!data->s.wait())
		{}
		data->s.post();
	}
};

struct WorkerThreadData{
public:
	thread mthread;
	THREAD_ID threadId;
	MUTEX mutex;
	bool blocked;
	std::queue<std::function<void()>> workData; 
	WorkerThreadData(): blocked(false){
		mutex = NEW_MUTEX;
	}
};

struct DispatchData{
	std::vector<WorkerThreadData> workerThreads;
	std::queue<std::function<void()>> dispatchQueue;
	MUTEX dispatchMutex;
	Semaphore dispatchSemaphore;
	bool stop;
};

template<typename T>
#ifdef USE_STD_THREAD
void awaitWorkerThreadProc(void* lpParameter){
#else
#ifdef _WIN32
DWORD WINAPI awaitWorkerThreadProc(LPVOID lpParameter){
#else
#error "Need thread invocation signature"
#endif
#endif
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
#ifndef USE_STD_THREAD
	return 0;
#endif
}

class ThreadPool{
private:
	std::shared_ptr<DispatchData> sharedState;
public:
	ThreadPool();
	ThreadPool(int numberOfThreads);
	~ThreadPool();
	bool working(){
		return sharedState->dispatchQueue.size() > 0;
	}
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
		Future<T> f;
		queue([f,func]()mutable{
			Future<T> result = func();
			f.set(result);
		});
		return f;
	}*/
	template<typename T>
	T await(Future<T> result){
		static thread_local int depth;
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
			std::cout << "Warning: worker thread stack has become excessive" << std::endl;
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

template<>
Future<void> ThreadPool::async<void>(std::function<void()> func);

class WorkQueue{
protected:
	std::queue<std::function<void()>> workQueue;
	MUTEX queueMutex;
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
	
	bool processQueueUnit(); //returns true if work was done
};

template<>
Future<void> WorkQueue::async<void>(std::function<void()> func);

extern ThreadPool CpuPool;
extern ThreadPool IoPool;
extern WorkQueue glQueue;
