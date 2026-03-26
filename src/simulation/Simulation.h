#ifndef SIMULATION_H
#define SIMULATION_H

#include "World.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <map>

struct SimulationStats {
    int tick;
    int entityCount;
    int herbivoreCount;
    int predatorCount;
    int plantCount;
    double avgSpeed;
    double avgAggression;
    double avgMetabolism;
    int totalBirths;
    int totalDeaths;
    
    // Для обратной совместимости с main.cpp и Graphics.h
    int totalEntities;
    int herbivores;
    int carnivores;
    int omnivores;
    int plants;
    int generation;
    
    SimulationStats() : tick(0), entityCount(0), herbivoreCount(0), 
                   predatorCount(0), plantCount(0), avgSpeed(0), avgAggression(0),
                   avgMetabolism(0), totalBirths(0), totalDeaths(0),
                   totalEntities(0), herbivores(0), carnivores(0), 
                   omnivores(0), plants(0), generation(0) {}
};

class Simulation {
public:
    World* world;
    std::vector<Entity*> entities;
    std::vector<Plant*> plants;
    int tick;
    SimulationStats stats;
    std::vector<std::pair<int, SimulationStats>> history;
    
    // Катастрофы
    bool isWinter;
    int winterDuration;
    bool meteorActive;
    
    // Режим Бога
    float climateFactor;  // 1.0 = норма, <1 = холодно, >1 = жарко
    
    Simulation(int width, int height) 
        : tick(0), isWinter(false), winterDuration(0), 
          meteorActive(false), climateFactor(1.0f) {
        world = new World(width, height);
    }
    
    ~Simulation() {
        delete world;
    }
    
    void init(int entityCount, int plantCount) {
        srand(time(nullptr));
        
        for (int i = 0; i < entityCount; i++) {
            int x = rand() % world->width;
            int y = rand() % world->height;
            world->addEntity(new Entity(x, y));
        }
        
        world->spawnPlants(plantCount);
    }
    
    int findNearestFood(Entity* entity, int& bestX, int& bestY) {
        int bestDist = 999999;
        bool found = false;
        
        // Поиск в радиусе зрения
        for (int dy = -entity->dna.vision; dy <= entity->dna.vision; dy++) {
            for (int dx = -entity->dna.vision; dx <= entity->dna.vision; dx++) {
                int nx = entity->x + dx;
                int ny = entity->y + dy;
                
                if (!world->isValid(nx, ny)) continue;
                
                int dist = abs(dx) + abs(dy);
                if (dist >= bestDist) continue;
                
                // Травоядные ищут растения
                if (entity->type != EntityType::CARNIVORE) {
                    Plant* plant = world->getPlantAt(nx, ny);
                    if (plant && plant->alive) {
                        bestX = nx;
                        bestY = ny;
                        bestDist = dist;
                        found = true;
                    }
                }
                
                // Хищники ищут жертв
                if (entity->type != EntityType::HERBIVORE) {
                    Entity* other = world->getEntityAt(nx, ny);
                    if (other && other->alive && other != entity) {
                        // Хищники едят травоядных
                        if (entity->type == EntityType::CARNIVORE && 
                            other->type == EntityType::HERBIVORE) {
                            bestX = nx;
                            bestY = ny;
                            bestDist = dist;
                            found = true;
                        }
                        // Всеядные едят всё
                        if (entity->type == EntityType::OMNIVORE && 
                            other->type != EntityType::CARNIVORE) {
                            bestX = nx;
                            bestY = ny;
                            bestDist = dist;
                            found = true;
                        }
                    }
                }
            }
        }
        
        return found ? bestDist : -1;
    }
    
    void moveEntity(Entity* entity) {
        if (!entity->alive) return;
        
        int foodX = -1, foodY = -1;
        int foodDist = -1;
        
        // Если голоден - ищем еду
        if (entity->energy < 50) {
            foodDist = findNearestFood(entity, foodX, foodY);
        }
        
        int newX = entity->x;
        int newY = entity->y;
        
        // Логика поведения (Этап 5)
        if (foodDist > 0 && foodDist <= entity->dna.vision) {
            // Идти к еде
            int dx = foodX - entity->x;
            int dy = foodY - entity->y;
            
            if (dx != 0) newX += (dx > 0 ? 1 : -1);
            if (dy != 0) newY += (dy > 0 ? 1 : -1);
        } else {
            // Случайное движение или исследование
            int moves[4][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}};
            int moveIdx = rand() % 4;
            
            newX += moves[moveIdx][0];
            newY += moves[moveIdx][1];
        }
        
        // Проверка границ и занятости
        if (!world->isValid(newX, newY) || !world->isEmpty(newX, newY)) {
            return; // Не двигаться
        }
        
        entity->x = newX;
        entity->y = newY;
        
        // Трата энергии на движение (зависит от скорости)
        entity->energy -= entity->dna.speed / 3;
    }
    
    void eat(Entity* entity) {
        if (!entity->alive) return;
        
        // Поиск еды на текущей клетке
        if (entity->type != EntityType::CARNIVORE) {
            Plant* plant = world->getPlantAt(entity->x, entity->y);
            if (plant && plant->alive) {
                entity->eat(plant->energyValue);
                plant->consume();
                return;
            }
        }
        
        // Хищники и всеядные могут атаковать
        if (entity->type != EntityType::HERBIVORE) {
            // Проверяем соседей на наличие жертв
            int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
            
            for (auto d : dirs) {
                int nx = entity->x + d[0];
                int ny = entity->y + d[1];
                
                Entity* prey = world->getEntityAt(nx, ny);
                if (prey && prey->alive && prey != entity) {
                    bool canAttack = false;
                    
                    if (entity->type == EntityType::CARNIVORE && 
                        prey->type == EntityType::HERBIVORE) {
                        canAttack = true;
                    }
                    if (entity->type == EntityType::OMNIVORE && 
                        prey->type == EntityType::HERBIVORE) {
                        canAttack = true;
                    }
                    
                    if (canAttack) {
                        // Атака зависит от агрессии
                        int attackChance = entity->dna.aggression * 5;
                        if (rand() % 100 < attackChance) {
                            entity->eat(prey->energy);
                            prey->alive = false;
                            prey->health = 0;
                            return;
                        }
                    }
                }
            }
        }
    }
    
    void reproduce() {
        std::vector<Entity*> newborns;
        
        for (auto entity : world->entities) {
            if (!entity->alive) continue;
            
            if (entity->canReproduce()) {
                // Поиск свободного места рядом
                int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
                
                for (auto d : dirs) {
                    int nx = entity->x + d[0];
                    int ny = entity->y + d[1];
                    
                    if (world->isValid(nx, ny) && world->isEmpty(nx, ny)) {
                        Entity* child = entity->reproduce(nx, ny);
                        if (child) {
                            newborns.push_back(child);
                            break;
                        }
                    }
                }
            }
        }
        
        for (auto baby : newborns) {
            world->addEntity(baby);
        }
    }
    
    void applyDisasters() {
        // Зима
        if (isWinter) {
            winterDuration--;
            climateFactor = 0.7f;
            
            for (auto e : world->entities) {
                if (e->alive) {
                    e->energy -= 2; // Дополнительный расход зимой
                }
            }
            
            if (winterDuration <= 0) {
                isWinter = false;
                climateFactor = 1.0f;
            }
        }
        
        // Метеорит
        if (meteorActive) {
            int casualties = world->entities.size() / 3;
            for (int i = 0; i < casualties && !world->entities.empty(); i++) {
                int idx = rand() % world->entities.size();
                world->entities[idx]->alive = false;
                world->entities[idx]->health = 0;
            }
            meteorActive = false;
        }
    }
    
    void updateStatistics() {
        stats.totalEntities = 0;
        stats.herbivores = 0;
        stats.carnivores = 0;
        stats.omnivores = 0;
        stats.plants = 0;
        
        double totalSpeed = 0, totalAgg = 0, totalMet = 0;
        
        for (auto e : world->entities) {
            if (!e->alive) continue;
            
            stats.totalEntities++;
            totalSpeed += e->dna.speed;
            totalAgg += e->dna.aggression;
            totalMet += e->dna.metabolism;
            
            if (e->type == EntityType::HERBIVORE) stats.herbivores++;
            else if (e->type == EntityType::CARNIVORE) stats.carnivores++;
            else stats.omnivores++;
        }
        
        for (auto p : world->plants) {
            if (p->alive) stats.plants++;
        }
        
        if (stats.totalEntities > 0) {
            stats.avgSpeed = totalSpeed / stats.totalEntities;
            stats.avgAggression = totalAgg / stats.totalEntities;
            stats.avgMetabolism = totalMet / stats.totalEntities;
        }
        
        stats.generation = tick;
        
        // Сохранение истории
        history.push_back({tick, stats});
    }
    
    void triggerWinter(int duration) {
        isWinter = true;
        winterDuration = duration;
    }
    
    void triggerMeteor() {
        meteorActive = true;
    }
    
    void setClimate(float factor) {
        climateFactor = factor;
    }
    
    void addEntities(int count) {
        for (int i = 0; i < count; i++) {
            int x = rand() % world->width;
            int y = rand() % world->height;
            if (world->isEmpty(x, y)) {
                world->addEntity(new Entity(x, y));
            }
        }
    }
    
    void addResources(int count) {
        world->spawnPlants(count);
    }
    
    void step() {
        tick++;
        
        // Обновление существ
        for (auto entity : world->entities) {
            entity->update();
        }
        
        // Движение и действия
        for (auto entity : world->entities) {
            if (entity->alive) {
                moveEntity(entity);
                eat(entity);
            }
        }
        
        // Размножение
        reproduce();
        
        // Применение катастроф
        applyDisasters();
        
        // Очистка мёртвых
        world->clearDeadEntities();
        world->clearDeadPlants();
        
        // Новые растения
        if (tick % 3 == 0) {
            world->spawnPlants(2 + rand() % 3);
        }
        
        // Статистика
        updateStatistics();
    }
    
    void render() const {
        std::cout << "\n=== Tick: " << tick << " ===" << std::endl;
        if (isWinter) std::cout << "❄️ WINTER (" << winterDuration << ") ❄️" << std::endl;
        
        world->render();
        
        std::cout << "\n📊 Statistics:" << std::endl;
        std::cout << "  Entities: " << stats.totalEntities 
                  << " (h:" << stats.herbivores 
                  << " C:" << stats.carnivores 
                  << " o:" << stats.omnivores << ")" << std::endl;
        std::cout << "  Plants: " << stats.plants << std::endl;
        std::cout << "  Avg Speed: " << stats.avgSpeed << std::endl;
        std::cout << "  Avg Aggression: " << stats.avgAggression << std::endl;
        std::cout << "  Avg Metabolism: " << stats.avgMetabolism << std::endl;
    }
    
    void saveStatsToFile(const std::string& filename) {
        std::ofstream file(filename);
        file << "tick,total,herbivores,carnivores,omnivores,plants,avgSpeed,avgAggression,avgMetabolism\n";
        
        for (auto& h : history) {
            file << h.first << ","
                 << h.second.totalEntities << ","
                 << h.second.herbivores << ","
                 << h.second.carnivores << ","
                 << h.second.omnivores << ","
                 << h.second.plants << ","
                 << h.second.avgSpeed << ","
                 << h.second.avgAggression << ","
                 << h.second.avgMetabolism << "\n";
        }
        
        file.close();
    }
};

#endif
