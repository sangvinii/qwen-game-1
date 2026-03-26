#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include "simulation/Simulation.h"
#include "graphics/Graphics.h"

Simulation* sim = nullptr;
Graphics* gfx = nullptr;
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
    
    // Создание графического окна
    gfx = new Graphics(sim->world->getWidth(), sim->world->getHeight(), 25);
    
    std::cout << "World created: " << sim->world->getWidth() << "x" << sim->world->getHeight() << std::endl;
    std::cout << "Initial entities: 15" << std::endl;
    std::cout << "Initial plants: 30" << std::endl;
    std::cout << std::endl;
    std::cout << "Graphical mode started!" << std::endl;
    std::cout << "Use keyboard controls in the window:" << std::endl;
    std::cout << "  W - Winter" << std::endl;
    std::cout << "  M - Meteor" << std::endl;
    std::cout << "  P - Add Plants" << std::endl;
    std::cout << "  E - Add Herbivores" << std::endl;
    std::cout << "  R - Add Predators" << std::endl;
    std::cout << "  ESC - Quit" << std::endl;
    std::cout << std::endl;
    
    int ticksToRun = 1000;  // Запустить на 1000 тиков
    bool autoMode = true;  // Автоматический режим
    
    if (autoMode) {
        // Автоматическая симуляция с графикой
        while (running && gfx->isOpen() && sim->tick < ticksToRun) {
            gfx->pollEvents();
            
            sim->step();
            
            // Получение статистики
            sim->updateStatistics();
            
            // Отрисовка
            gfx->render(*sim->world, sim->world->entities, sim->world->plants, sim->stats, sim->isWinter);
            
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
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
    std::cout << "Average Speed: " << sim->stats.avgSpeed << std::endl;
    std::cout << "Average Aggression: " << sim->stats.avgAggression << std::endl;
    std::cout << "Average Metabolism: " << sim->stats.avgMetabolism << std::endl;
    
    delete gfx;
    delete sim;
    
    return 0;
}
