#include <iostream>


#define pl(x) std::cout << x << "\n"
#define p(x) std::cout << x

#define ll long long

using std::cin;
using std::cout;

void fake_return(){
    pl("It worked!");
}

void expliot(){
    int i1 = 5;
    int i2 = 6;
    int i3 = 7;
    int i4 = 8;

    cout << "i1=" << &i1 << " (" << ((ll) &i1) << ")\n";
    cout << "i2=" << &i2 << " (" << ((ll) &i2) << ")\n";
    cout << "i3=" << &i3 << " (" << ((ll) &i3) << ")\n";
    cout << "i4=" << &i4 << " (" << ((ll) &i4) << ")\n";

    ll* address = (ll*) &i1 + 1;

    printf("return address = %llx\n" , *address);

    address = (ll*) fake_return;

    printf("fake address   = %llx\n" , *address);
}

int main(){
    pl("did this work ?");
    expliot();
    pl("no it didn't");
    return 0;
}