#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cstdlib>
#include <cstring>
#include "Entity.h"
#include "Plant.h"

class World {
public:
    int width;
    int height;
    std::vector<std::vector<char>> map;
    std::vector<Entity*> entities;
    std::vector<Plant*> plants;
    
    World(int w, int h) : width(w), height(h) {
        map.resize(height, std::vector<char>(width, '.'));
    }
    
    ~World() {
        for (auto e : entities) delete e;
        for (auto p : plants) delete p;
    }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    bool isValid(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
    
    bool isEmpty(int x, int y) const {
        if (!isValid(x, y)) return false;
        for (auto e : entities) {
            if (e->alive && e->x == x && e->y == y) return false;
        }
        return true;
    }
    
    Entity* getEntityAt(int x, int y) const {
        for (auto e : entities) {
            if (e->alive && e->x == x && e->y == y) return e;
        }
        return nullptr;
    }
    
    Plant* getPlantAt(int x, int y) const {
        for (auto p : plants) {
            if (p->alive && p->x == x && p->y == y) return p;
        }
        return nullptr;
    }
    
    void addEntity(Entity* entity) {
        entities.push_back(entity);
    }
    
    void addPlant(Plant* plant) {
        plants.push_back(plant);
    }
    
    void spawnPlants(int count) {
        for (int i = 0; i < count; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (isEmpty(x, y)) {
                addPlant(new Plant(x, y, 15 + rand() % 15));
            }
        }
    }
    
    void clearDeadEntities() {
        auto it = entities.begin();
        while (it != entities.end()) {
            if (!(*it)->alive) {
                delete *it;
                it = entities.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void clearDeadPlants() {
        auto it = plants.begin();
        while (it != plants.end()) {
            if (!(*it)->alive) {
                delete *it;
                it = plants.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void render() {
        // Очистка карты
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                map[y][x] = '.';
            }
        }
        
        // Растения
        for (auto p : plants) {
            if (p->alive && isValid(p->x, p->y)) {
                map[p->y][p->x] = '*';
            }
        }
        
        // Существa
        for (auto e : entities) {
            if (e->alive && isValid(e->x, e->y)) {
                map[e->y][e->x] = e->getSymbol();
            }
        }
        
        // Вывод
        std::cout << "+";
        for (int x = 0; x < width; x++) std::cout << "-";
        std::cout << "+" << std::endl;
        
        for (int y = 0; y < height; y++) {
            std::cout << "|";
            for (int x = 0; x < width; x++) {
                std::cout << map[y][x];
            }
            std::cout << "|" << std::endl;
        }
        
        std::cout << "+";
        for (int x = 0; x < width; x++) std::cout << "-";
        std::cout << "+" << std::endl;
    }
};

#endif
