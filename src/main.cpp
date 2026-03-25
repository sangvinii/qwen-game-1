#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include "simulation/Simulation.h"

Simulation* sim = nullptr;
bool running = true;

void signalHandler(int signum) {
    running = false;
    std::cout << "\n\n🛑 Simulation interrupted!" << std::endl;
}

int main() {
    std::cout << "🌍 Evolution World Simulator 🌍" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    // Регистрация обработчика сигналов
    signal(SIGINT, signalHandler);
    
    // Создание симуляции
    sim = new Simulation(30, 20);
    
    // Инициализация мира
    sim->init(15, 30);  // 15 существ, 30 растений
    
    std::cout << "World created: " << sim->world->width << "x" << sim->world->height << std::endl;
    std::cout << "Initial entities: 15" << std::endl;
    std::cout << "Initial plants: 30" << std::endl;
    std::cout << std::endl;
    std::cout << "Legend:" << std::endl;
    std::cout << "  h = Herbivore (травоядное)" << std::endl;
    std::cout << "  C = Carnivore (хищник)" << std::endl;
    std::cout << "  o = Omnivore (всеядное)" << std::endl;
    std::cout << "  * = Plant (растение)" << std::endl;
    std::cout << "  . = Empty ground" << std::endl;
    std::cout << std::endl;
    std::cout << "Press Ctrl+C to stop and save statistics" << std::endl;
    
    int ticksToRun = 100;  // Запустить на 100 тиков для демонстрации
    bool autoMode = true;  // Автоматический режим
    
    if (autoMode) {
        // Автоматическая симуляция
        while (running && sim->tick < ticksToRun) {
            sim->step();
            sim->render();
            
            // Демонстрация катастроф
            if (sim->tick == 30) {
                std::cout << "\n⚠️  WINTER IS COMING!" << std::endl;
                sim->triggerWinter(10);
            }
            
            if (sim->tick == 60) {
                std::cout << "\n☄️  METEOR INCOMING!" << std::endl;
                sim->triggerMeteor();
            }
            
            // Добавление ресурсов Богом
            if (sim->tick == 45) {
                std::cout << "\n🙏 GOD MODE: Adding resources!" << std::endl;
                sim->addResources(10);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } else {
        // Интерактивный режим (пошагово)
        char input;
        while (running) {
            std::cout << "\nPress Enter for next step, or type command:" << std::endl;
            std::cout << "  w - trigger winter" << std::endl;
            std::cout << "  m - trigger meteor" << std::endl;
            std::cout << "  a - add entities" << std::endl;
            std::cout << "  r - add resources" << std::endl;
            std::cout << "  q - quit" << std::endl;
            std::cout << "> ";
            
            std::cin >> input;
            
            if (input == 'q') break;
            else if (input == 'w') sim->triggerWinter(15);
            else if (input == 'm') sim->triggerMeteor();
            else if (input == 'a') sim->addEntities(5);
            else if (input == 'r') sim->addResources(10);
            
            sim->step();
            sim->render();
        }
    }
    
    // Сохранение статистики
    sim->saveStatsToFile("evolution_stats.csv");
    std::cout << "\n📊 Statistics saved to evolution_stats.csv" << std::endl;
    
    // Финальный отчёт
    std::cout << "\n=== FINAL REPORT ===" << std::endl;
    std::cout << "Total ticks: " << sim->tick << std::endl;
    std::cout << "Final population: " << sim->stats.totalEntities << std::endl;
    std::cout << "  Herbivores: " << sim->stats.herbivores << std::endl;
    std::cout << "  Carnivores: " << sim->stats.carnivores << std::endl;
    std::cout << "  Omnivores: " << sim->stats.omnivores << std::endl;
    std::cout << "Average Speed: " << sim->stats.avgSpeed << std::endl;
    std::cout << "Average Aggression: " << sim->stats.avgAggression << std::endl;
    std::cout << "Average Metabolism: " << sim->stats.avgMetabolism << std::endl;
    
    delete sim;
    
    return 0;
}
