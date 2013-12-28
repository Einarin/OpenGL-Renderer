#include "ThreadPool.h"

DWORD WINAPI DispatchThreadProc(LPVOID lpParameter);

DWORD WINAPI WorkerThreadProc(LPVOID lpParameter){
	DispatchData* data = (DispatchData*)lpParameter;
	bool run = true;
	while(run){
		ACQUIRE_MUTEX(data->dispatchMutex);
		/*if(data->stop){
			run = false;
		}*/
		if(!data->dispatchQueue.empty()){
			std::function<void()> workUnit = data->dispatchQueue.front();
			data->dispatchQueue.pop();
			RELEASE_MUTEX(data->dispatchMutex);
			workUnit();
		} else {
			RELEASE_MUTEX(data->dispatchMutex);
			//SuspendThread(data->thread);
			Sleep(0);
		}
		
	}
	return 0;
}

ThreadPool::ThreadPool(){
#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	int numberOfThreads = sysinfo.dwNumberOfProcessors;
#else
	int numberOfThreads = std::thread::hardware_concurrency();
	numberOfThreads = numberOfThreads == 0 ? 4 : numberOfThreads;
#endif

	sharedState.workerThreads.resize(numberOfThreads);

	sharedState.dispatchMutex = NEW_MUTEX;

	for(int i=0;i<numberOfThreads;i++)
	{
		WorkerThreadData& current = sharedState.workerThreads[i];
		#ifdef WIN32_CONCURRENCY
			current.thread = CreateThread(NULL,0,WorkerThreadProc,(LPVOID)&sharedState,0,&current.threadId);
		#else
			current.thread = thread(awaitWorkerThreadProc<T>,(LPVOID)data);
			current.threadId = current.thread.get_id();
			worker.detach();
		#endif
	}
	//sharedState.roundRobinIndex=0;
	
	//sharedState.dispatchSemaphore = CreateSemaphore(NULL,0,
	//sharedState.queuingThread = CreateThread(NULL,0,DispatchThreadProc,(LPVOID)&sharedState,0,NULL);
	mainQueueMutex = NEW_MUTEX;
}

ThreadPool::ThreadPool(int numberOfThreads){
	sharedState.workerThreads.resize(numberOfThreads);

	sharedState.dispatchMutex = NEW_MUTEX;

	for(int i=0;i<numberOfThreads;i++)
	{
		WorkerThreadData& current = sharedState.workerThreads[i];
		#ifdef WIN32_CONCURRENCY
			current.thread = CreateThread(NULL,0,WorkerThreadProc,(LPVOID)&sharedState,0,&current.threadId);
		#else
			current.thread = thread(awaitWorkerThreadProc<T>,(LPVOID)data);
			current.threadId = current.thread.get_id();
			worker.detach();
		#endif
	}
	//sharedState.roundRobinIndex=0;
	
	//sharedState.dispatchSemaphore = CreateSemaphore(NULL,0,
	//sharedState.queuingThread = CreateThread(NULL,0,DispatchThreadProc,(LPVOID)&sharedState,0,NULL);
	mainQueueMutex = NEW_MUTEX;
}

void ThreadPool::async(std::function<void()> workUnit){
	ACQUIRE_MUTEX(sharedState.dispatchMutex);
	sharedState.dispatchQueue.push(workUnit);
	RELEASE_MUTEX(sharedState.dispatchMutex);
	//ResumeThread(sharedState.queuingThread);
}

void ThreadPool::onMain(std::function<void()> workUnit){
	ACQUIRE_MUTEX(mainQueueMutex);
	mainQueue.push(workUnit);
	RELEASE_MUTEX(mainQueueMutex);
}

void ThreadPool::processMainQueueUnit(){
	ACQUIRE_MUTEX(mainQueueMutex);
	if(!mainQueue.empty()){
		std::function<void()> workUnit = mainQueue.front();
		mainQueue.pop();
		RELEASE_MUTEX(mainQueueMutex);
		workUnit();
	} else {
		RELEASE_MUTEX(mainQueueMutex);
	}
}

/*DWORD WINAPI DispatchThreadProc(LPVOID lpParameter){
	bool run = true;
	DispatchData* data = (DispatchData*)lpParameter;
    int length = 1;
	while(run){
		WaitForSingleObject(data->dispatchMutex,INFINITE);
		if(!data->dispatchQueue.empty()){
			std::function<void()> workUnit = data->dispatchQueue.front();
			data->dispatchQueue.pop();
			ReleaseMutex(data->dispatchMutex);
			//find a worker thread for this work unit
            int start = data->roundRobinIndex;
            
			for(;;){
				//we wait up to 10 ms for a worker thread to release it's mutex, before skipping over it
				DWORD result = WaitForSingleObject(data->workerThreads[data->roundRobinIndex].mutex,INFINITE);
                if(result == WAIT_OBJECT_0 && data->workerThreads[data->roundRobinIndex].workData.size() < length){ //success
					data->workerThreads[data->roundRobinIndex].workData.push(workUnit);
					ReleaseMutex(data->workerThreads[data->roundRobinIndex].mutex);
					ResumeThread(data->workerThreads[data->roundRobinIndex].thread);
					break;
				} else { //not a suitable thread
					ReleaseMutex(data->workerThreads[data->roundRobinIndex].mutex);
                    data->roundRobinIndex++;
				    data->roundRobinIndex %= data->workerThreads.size();
                    if(start == data->roundRobinIndex){//we checked every thread, relax selection criteria
                        length++;
                    }
				}
			}
            data->roundRobinIndex++;
			data->roundRobinIndex %= data->workerThreads.size();
		} else {
			ReleaseMutex(data->dispatchMutex);
            length = 1; //reset expected queue length
			SuspendThread(data->queuingThread);
		}
	}
	return 0;
}*/