#pragma once

#include<iostream>
#include<vector>
#include<chrono>

class CWidget{
public:
    CWidget(){}
    virtual void draw(float delta){
        std::cout << "CWidget::draw(float) was called \n";
    } //do nothing
    template<typename T> static inline void print(const T &t){
        std::cout << t;
    }
};

class Console{
private:
    std::vector<CWidget*> widgets;
    std::chrono::time_point<std::chrono::high_resolution_clock> time;
    float delta = 0;

public:
    void addItem(CWidget& w);
    void removeItem(int i);
    void setItem(CWidget& w , int i);

    void draw();
    float getDelta();

    Console(std::vector<CWidget*>& items);
    Console(){}

};
