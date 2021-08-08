#pragma once
#include "console.h"
#include <cmath>

class CText: public CWidget{
public:
    std::string text;
    void draw(float delta){
        print(text);
    }
    CText(std::string t): text(t) {}
    CText(){
        text = "";
    }
};

class CProgressBar: public CWidget{
public:
    char progress_char = '=';
    int max = 100;
    int progress = 50;
    int size = 20;

    void draw(float delta){
        int count = (int) ((float) progress / max * size);
        while(count--){
            print(progress_char);
        }
    }
};

class CProgress: public CWidget{
public:
    float passed = 0;
    float anim_duration = 0.6;
    std::vector<char> items = {'\\' , '|' , '/' , '-'};

    void draw(float delta){
        if (items.size() == 0)
            return;
        
        passed += delta;

        if (passed >= anim_duration / items.size() * (items.size() + 0.45f)){
            passed = - anim_duration / items.size() * 0.45f;
        }

        int index = (int) round( passed * items.size() / anim_duration);
        print(items[index]);

        
    }   
};
