//header that contains some useful functions / classes
//Author : Abdo Mohamed

#pragma once

#include <iostream> 
#include <chrono>
#include <memory.h>

#define pl(x) std::cout << x << std::endl
#define p(x) std::cout << x

#define ll long long
#define ull unsigned long long


template<typename T>
void sort(T* arr , int size , bool(* comparator)(T v1 , T v2) ){
    bool c = false;
    T temp;
    for (int i = 0;i < size;i++){
        c = false;

        for (int j = 0;j < size - 1;j++){
            if (comparator(arr[j] , arr[j+1])){
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
                c = true;
            }
        }

        if (!c)
            break;
    }
}

template<typename T , int s>
class Array{
private:
    T arr[s];
public:
    T& operator[](int i){
        return arr[i];
    }

    const T& operator[](int i) const{
        return arr[i];
    }

    constexpr int size() const{
        return s;
    }

    T& at(int pos){
        return arr[pos];
    }
};


void dispArray(int* arr , int size);


template<typename T>
void printBytes(T* t){
    char* ptr = (char*) t;
    for (int i = 0;i < sizeof(T);i++){
        p(*(ptr+i));
        p(" ");
    }
    pl("");
}

template<typename J , typename T>
bool instanceof(T* t){
    return dynamic_cast<J*>(t);
}


class Timer{
public:
    Timer(const char* name);
    void restart();
    ll stop(); //returns time in microsecounds
    ~Timer();
private:
    bool running = false;
    const char* name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

#define CONCAT_(x,y) x##y //contacts to 'string' passed to mactro
#define CONCAT(x,y) CONCAT_(x,y)
#define PROFILER_TIMER(name) Timer CONCAT(func_timer, __COUNTER__)(name)


//A class that can hold any data type
//Its better not to use it with temperary variables
class Any{

private:
int m_size = -1;
void* ptr = nullptr;
static const int s_max_stack_size = 32;
char* min_ptr[s_max_stack_size];

public:
    Any(){}
    
    template<typename T> Any(const T& t){
        operator=(t);
        //std::cout << "[Any] create (const T& t) [" << this << "]" << std::endl;
    }

    template<typename T> Any(const T* t){
        operator=(t);
        //std::cout << "[Any] create (const T* t) [" << this << "]" << std::endl;
    }

    Any(Any&& t){
        operator=(std::move(t));
        //std::cout << "[Any] create (Any&& t) [" << this << "]" << std::endl;
    }

    template<typename T> T& get(){
        return *getPtr<T>();
    }

    template<typename T> T* getPtr(){
        if (m_size != sizeof(T)){
            //std::cout << "return null: m_size = " << m_size << "\n";
            throw "[Any::getPtr()] type is not equal 'm_size != sizeof(T)' ";
            //return nullptr;
        }

        return m_size > s_max_stack_size ? ((T*) ptr) : ((T*) min_ptr);
    }

    template<typename T> const T& get() const{
        return *getPtr<T>();
    }

    template<typename T> const T* getPtr() const{
        if (m_size != sizeof(T)){
            //std::cout << "return null: m_size = " << m_size << "\n";
            throw "[Any::getPtr()] type is not equal 'm_size != sizeof(T)' ";
            //return nullptr;
        }

        return m_size > s_max_stack_size ? ((T*) ptr) : ((T*) min_ptr);
    }

    Any& operator=(const Any& a){
        if (ptr){
            free(ptr);
            ptr = nullptr;
        }

        m_size = a.m_size;
        if (m_size <= s_max_stack_size)
            memcpy(min_ptr , a.min_ptr , a.m_size);
        
        ptr = a.ptr;

        return *this;
    }

    Any& operator=(Any&& a){
        m_size = a.m_size;
        if (m_size <= s_max_stack_size)
            memcpy(min_ptr , a.min_ptr , a.m_size);
        
        ptr = a.ptr;

        a.ptr = nullptr;
        a.m_size = 0;
        
        return *this;
    }

    template<typename T> Any& operator=(const T& t){
        if (ptr){
            free(ptr);
            ptr = nullptr;
        }

        m_size = sizeof(T);
        
        if (m_size <= s_max_stack_size){
            memcpy(min_ptr , &t , m_size);
        }else{
            ptr = malloc(m_size);
            if (ptr){
                memcpy(ptr , &t , m_size);
            }
        }

        return *this;
    }

    template<typename T> Any& operator=(const T* t){
        if (ptr){
            free(ptr);
            ptr = nullptr;
        }

        m_size = sizeof(T*);
        
        if (m_size <= s_max_stack_size){
            memcpy(min_ptr , &t , m_size);
        }else{
            ptr = malloc(m_size);
            if (ptr){
                memcpy(ptr , &t , m_size);
            }
        }

        return *this;
    }

    ~Any(){
        if (ptr != nullptr){
            free(ptr);
            ptr = nullptr;
        }
    }
};