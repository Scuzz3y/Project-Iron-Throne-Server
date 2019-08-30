#include "pch.h"
#include "lightBringer.h"
#include "blackIce.h"
#include "arsenalHttp.h"

int main() {
    // Initialize Manager Module
    LightBringer bringit;

    // Initialize Puppet Modules
    BlackIce blackIce(&bringit);
    std::thread(&BlackIce::Init, &blackIce).detach();
    
    // Initialize Master Modules
    ArsenalHttp arsenal(&bringit);
    std::thread(&ArsenalHttp::Start, &arsenal).detach();

    // Call CLI
    bringit.InitCli();

    return 0; 
}
