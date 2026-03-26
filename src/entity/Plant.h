#ifndef PLANT_H
#define PLANT_H

class Plant {
public:
    int x, y;
    int energyValue;
    bool alive;
    
    Plant(int startX, int startY, int energy = 20)
        : x(startX), y(startY), energyValue(energy), alive(true) {}
    
    void consume() {
        alive = false;
    }
};

#endif
