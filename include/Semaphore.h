#pragma once
#ifdef _WIN32
#include <Windows.h>
#define semaphore_t HANDLE
#else
#include <semaphore.h>
#define semaphore_t sem_t
#endif
#include <iostream>
class Semaphore{
protected:
    semaphore_t s;
    int id;
public:
    Semaphore(int initialCount=1){
    static int num;
        #ifdef _WIN32
            s = CreateSemaphore(NULL,0,initialCount,NULL);
        #else
            sem_init(&s,0,initialCount);
        #endif
        id = num;
        num++;
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
        //std::cout << "posting 1 to sem " << id << std::endl;
    }
    void post(int count){
        #ifdef _WIN32
            ReleaseSemaphore(s,count,NULL);
        #else
            for(int i=0;i<count;i++){
                sem_post(&s);
            }
        #endif
        //std::cout << "posting " << count << " to sem " << id << std::endl;
    }
};

