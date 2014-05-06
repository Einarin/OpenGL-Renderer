#pragma once
#ifdef _WIN32
#include <Windows.h>
#define semaphore_t HANDLE
#else
#include <semaphore.h>
#define semaphore_t sem_t
#endif
class Semaphore{
protected:
    semaphore_t s;
public:
    Semaphore(int initialCount=1){
        #ifdef _WIN32
            s = CreateSemaphore(NULL,0,initialCount,NULL);
        #else
            sem_init(&s,0,initialCount);
        #endif
    }
    ~Semaphore(){
        #ifdef _WIN32
            CloseHandle(s);
        #else
            sem_destroy(&s);
        #endif
    }
    inline bool wait(){
        #ifdef _WIN32
            return 0==WaitForSingleObject(s,INFINITE);
        #else
            return 0==sem_wait(&s);
        #endif
    }
    inline void post(){
        #ifdef _WIN32
            ReleaseSemaphore(s,1,NULL);
        #else
            sem_post(&s);
        #endif
    }
    void post(int count){
        #ifdef _WIN32
            ReleaseSemaphore(s,count,NULL);
        #else
            for(int i=0;i<count;i++){
                sem_post(&s);
            }
        #endif
    }
};

