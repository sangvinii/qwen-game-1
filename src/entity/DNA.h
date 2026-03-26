#ifndef DNA_H
#define DNA_H

#include <cstdlib>
#include <ctime>

struct DNA {
    int speed;        // 1-10
    int vision;       // 1-5 (радиус обзора)
    int aggression;   // 0-10 (0 - травоядное, 10 - хищник)
    int metabolism;   // 1-10 (сколько энергии тратит за ход)
    
    DNA() {
        speed = 1 + rand() % 10;
        vision = 1 + rand() % 5;
        aggression = rand() % 11;
        metabolism = 1 + rand() % 10;
    }
    
    DNA mutate() const {
        DNA child;
        child.speed = std::max(1, std::min(10, speed + (rand() % 3) - 1));
        child.vision = std::max(1, std::min(5, vision + (rand() % 3) - 1));
        child.aggression = std::max(0, std::min(10, aggression + (rand() % 3) - 1));
        child.metabolism = std::max(1, std::min(10, metabolism + (rand() % 3) - 1));
        return child;
    }
};

#endif
