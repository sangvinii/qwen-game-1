#ifndef ENTITY_H
#define ENTITY_H

#include "DNA.h"
#include <cstdlib>

enum class EntityType {
    HERBIVORE,
    CARNIVORE,
    OMNIVORE
};

class Entity {
public:
    int x, y;
    int health;
    int energy;
    int age;
    DNA dna;
    EntityType type;
    bool alive;
    int id;
    
    static int nextId;
    
    Entity(int startX, int startY) 
        : x(startX), y(startY), health(100), energy(50), age(0), alive(true), id(nextId++) {
        
        // Тип зависит от агрессии
        if (dna.aggression <= 3) {
            type = EntityType::HERBIVORE;
        } else if (dna.aggression >= 7) {
            type = EntityType::CARNIVORE;
        } else {
            type = EntityType::OMNIVORE;
        }
    }
    
    Entity(int startX, int startY, DNA parentDNA)
        : x(startX), y(startY), health(50), energy(30), age(0), alive(true), 
          dna(parentDNA.mutate()), id(nextId++) {
        
        if (dna.aggression <= 3) {
            type = EntityType::HERBIVORE;
        } else if (dna.aggression >= 7) {
            type = EntityType::CARNIVORE;
        } else {
            type = EntityType::OMNIVORE;
        }
    }
    
    void update() {
        if (!alive) return;
        
        age++;
        // Трата энергии зависит от метаболизма
        energy -= dna.metabolism;
        health -= (dna.metabolism / 5);
        
        if (energy <= 0 || health <= 0) {
            alive = false;
        }
    }
    
    void eat(int amount) {
        energy += amount;
        health += amount / 2;
        if (energy > 100) energy = 100;
        if (health > 100) health = 100;
    }
    
    bool canReproduce() const {
        return energy > 70 && age > 10 && alive;
    }
    
    Entity* reproduce(int newX, int newY) {
        if (!canReproduce()) return nullptr;
        
        energy -= 30;
        return new Entity(newX, newY, dna);
    }
    
    char getSymbol() const {
        if (!alive) return ' ';
        switch (type) {
            case EntityType::HERBIVORE: return 'h';
            case EntityType::CARNIVORE: return 'C';
            case EntityType::OMNIVORE: return 'o';
        }
        return '?';
    }
};

int Entity::nextId = 0;

#endif
