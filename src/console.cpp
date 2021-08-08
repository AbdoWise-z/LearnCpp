#include "console.h"

//CLASS CONSOLE

Console::Console(std::vector<CWidget*>& items){
    for(int i= 0;i < items.size();i++){
        widgets.emplace_back(items[i]);
    }

    time = std::chrono::high_resolution_clock::now();
}

void Console::draw(){
    system("cls");

    auto end = std::chrono::high_resolution_clock::now();
    delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - time).count() / 1000.0f;
    if (delta < 1E-6)
        delta = 1E-6;
    
    time = end;

    if (delta > 15){
        std::cout << "[Warn] delta=" << delta << "\n";
        delta = 0.001;
    }

    for (int i = 0;i < widgets.size();i++){
        CWidget* w = widgets[i];
        w->draw(delta);
    }

    std::cout.flush(); //make sure its emplty
}

void Console::addItem(CWidget& w){
    widgets.emplace_back(&w);
}

void Console::removeItem(int i){
    widgets.erase(widgets.begin() + i);
}

void Console::setItem(CWidget& w , int i){
    widgets[i] = &w;
}

float Console::getDelta(){
    return delta;
}

//END CLASS CONSOLE 

