#include "utility.h"

using namespace std;

void dispArray(int* arr , int size){
    for (int i = 0;i < 7;i++){
        cout << arr[i] << " ";
    }
}

Timer::Timer(const char* name){
    this->name = name;
    restart();
}

void Timer::restart(){
    running = true;
    start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer(){
    if (running){
        cout << "[timer/" << name << "] duration=" << stop() << "\n";
    }
}

ll Timer::stop(){
    auto end = std::chrono::high_resolution_clock::now();
    ll dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    running = false;
    return dur;
}
