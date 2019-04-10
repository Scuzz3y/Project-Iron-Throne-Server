#include "pch.h"
#include "lightBringer.h"
#include "blackIce.h"
#include "arsenalHttp.h"

int main() {
    std::cout << "Hello from Main!" << std::endl;
    LightBringer bringit;
    bringit.BlackIceTesting();
    bringit.ArsenalHttpTesting();

    pause();
    return 0;
}
