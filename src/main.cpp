#include <bits/stdc++.h>
#include "MathUtils.h"

#define pl(x) std::cout << x << "\n"
#define p(x) std::cout << x

#define ll long long

using std::cin;
using std::cout;

int main(){
    try{
        MathUtils::Expression e("(2x + 5)(x - 1)");
        e["x"] = 1; //set the value of x .. notic that this must be called atleast once to avoid throw an Error
        cout << "f(1) = " << e.eval() << "\n";

    }catch (MathUtils::ExpressionError e){
        cout << "Error : " << e.error << "\n";
    }
    return EXIT_SUCCESS;
}