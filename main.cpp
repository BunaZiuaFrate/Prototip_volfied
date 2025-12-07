#include "GameApp.h"
#include <iostream>

int main() {
    try {
        GameApp app;
        app.run();
    } catch (...) {
        return 1;
    }
    return 0;
}