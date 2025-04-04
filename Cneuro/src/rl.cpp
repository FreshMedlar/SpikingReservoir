#include "rl.h"

int changeRandomInt(int toChange) {
    int change = getRandomInt(-1,1);
    switch(toChange) {
        case 0:
            maxConnectionStrenght += change;
            break;
        case 1:
            generalThreshold += change;
            break;
    }
    return change;
}

float changeRandomFloat(int toChange) {
    float change = 0.0f;
    switch (toChange) {
        case 0:
            change = getRandomFloat(-0.1f, 0.1f);
            omega += change;
            break;  
        case 1:
            change = getRandomFloat(-0.05f, 0.05f);
            alpha += change;
            break;
        case 2:
            change = getRandomFloat(-0.1f, 0.1f); 
            generalImpulse += change;
            break;
    }
    return change;
}