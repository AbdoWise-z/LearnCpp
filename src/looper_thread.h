//a thread .. that loops (keeps calling a function)
// Author : Abdo Mohamed

#pragma once

#include<thread>

class LooperThread{
public:
    typedef void(*LOOPER_FUNCTION)(LooperThread*);
private:
    bool run;
    std::thread th;
    LOOPER_FUNCTION func;

    void runner();
public:
    LooperThread(LOOPER_FUNCTION f);
    
    bool isRunning();
    std::thread& getThread();
    LOOPER_FUNCTION getLooper();

    void stop();
    void waitFor();

};