#include <bits/stdc++.h>
#include "utility.h"
#include "MathUtils.h"

using namespace std;
using MathUtils::Expression;

ld f(ld x){
    return 1.0 / sinhl(x);
}

ld f(ld x , ld y){
    return y * x;
}

int main(){
    //this is just a test for some of the stuff I code
    try{
        PROFILER_TIMER("Create");

        MathUtils::addFunction1p("f" , f);
        MathUtils::addFunction2p("f" , f);
        
        Expression exp("sigma(e^n , 0 , 2)");

        cout << exp.eval() << endl;
        
    }catch(MathUtils::ExpressionError e){
        cout << "ExpError: " << e.error << " at " << e.where << endl;
    }

    return EXIT_SUCCESS;
}