//Looper thread impl
// Author : Abdo Mohamed

#include "looper_thread.h"
#include <thread>

LooperThread::LooperThread(LOOPER_FUNCTION func) :
    func(func) , th(std::thread(LooperThread::runner , this)) , run(true)
{
}

void LooperThread::runner(){
    while(run){
        func(this); //execute the function
    }
}

bool LooperThread::isRunning(){
    return run;
}

std::thread& LooperThread::getThread(){
    return th;
}

void LooperThread::stop(){
    run = false;
}

void LooperThread::waitFor(){
    th.join();
}