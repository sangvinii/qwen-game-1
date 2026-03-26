#pragma once

#include <SFML/Graphics.hpp>
#include "World.h"
#include "Entity.h"
#include "Plant.h"
#include <vector>
#include <string>
#include <sstream>

class Graphics {
private:
    sf::RenderWindow* window;
    sf::Font font;
    sf::Text statsText;
    sf::Text infoText;
    
    int cellSize;
    int offsetX;
    int offsetY;
    
    // Цвета
    sf::Color grassColor;
    sf::Color plantColor;
    sf::Color herbivoreColor;
    sf::Color predatorColor;
    sf::Color winterColor;
    
public:
    Graphics(int worldWidth, int worldHeight, int cellSize = 20) 
        : cellSize(cellSize) {
        
        int width = worldWidth * cellSize;
        int height = worldHeight * cellSize + 150; // Место для статистики
        
        window = new sf::RenderWindow(sf::VideoMode(width, height), 
            "Эволюционный Симулятор - SFML", sf::Style::Titlebar | sf::Style::Close);
        window->setFramerateLimit(30);
        
        // Загрузка шрифта (используем системный)
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf")) {
            // Если не найдён, пробуем другой путь
            if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf")) {
                // Если всё ещё не найдён, создаём простой текст без шрифта
                std::cout << "Шрифт не найден, используем упрощённый режим" << std::endl;
            }
        }
        
        // Настройка текста статистики
        statsText.setFont(font);
        statsText.setCharacterSize(14);
        statsText.setFillColor(sf::Color::White);
        statsText.setPosition(10, worldHeight * cellSize + 10);
        
        infoText.setFont(font);
        infoText.setCharacterSize(12);
        infoText.setFillColor(sf::Color::Yellow);
        infoText.setPosition(10, worldHeight * cellSize + 80);
        
        // Цвета
        grassColor = sf::Color(34, 139, 34);      // ForestGreen
        plantColor = sf::Color(0, 255, 0);         // Ярко-зелёный
        herbivoreColor = sf::Color(30, 144, 255);  // DodgerBlue
        predatorColor = sf::Color(255, 69, 0);     // OrangeRed
        winterColor = sf::Color(200, 200, 255, 100); // Полупрозрачный белый/синий
    }
    
    ~Graphics() {
        delete window;
    }
    
    bool isOpen() const {
        return window->isOpen();
    }
    
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window->close();
            
            // Обработка клавиш для режима бога
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::W:
                        std::cout << "[Режим Бога] Активирована зима" << std::endl;
                        break;
                    case sf::Keyboard::M:
                        std::cout << "[Режим Бога] Вызван метеорит" << std::endl;
                        break;
                    case sf::Keyboard::P:
                        std::cout << "[Режим Бога] Добавлены растения" << std::endl;
                        break;
                    case sf::Keyboard::E:
                        std::cout << "[Режим Бога] Добавлены травоядные" << std::endl;
                        break;
                    case sf::Keyboard::R:
                        std::cout << "[Режим Бога] Добавлены хищники" << std::endl;
                        break;
                    case sf::Keyboard::Escape:
                        window->close();
                        break;
                }
            }
        }
    }
    
    void render(const World& world, const std::vector<Entity*>& entities, 
                const std::vector<Plant*>& plants, const SimulationStats& stats,
                bool isWinter) {
        
        window->clear(grassColor);
        
        // Отрисовка растений
        for (const auto& plant : plants) {
            sf::RectangleShape rect(sf::Vector2f(cellSize, cellSize));
            rect.setPosition(plant->x * cellSize, plant->y * cellSize);
            rect.setFillColor(plantColor);
            window->draw(rect);
        }
        
        // Отрисовка существ
        for (const auto& entity : entities) {
            sf::CircleShape circle(cellSize / 2.0f - 1);
            circle.setPosition(entity->x * cellSize + 1, entity->y * cellSize + 1);
            
            // Цвет зависит от типа
            if (entity->isPredator()) {
                circle.setFillColor(predatorColor);
            } else {
                circle.setFillColor(herbivoreColor);
            }
            
            // Размер зависит от здоровья
            float healthRatio = static_cast<float>(entity->health) / entity->getMaxHealth();
            circle.setRadius((cellSize / 2.0f - 1) * (0.5f + 0.5f * healthRatio));
            
            window->draw(circle);
        }
        
        // Эффект зимы
        if (isWinter) {
            sf::RectangleShape winterOverlay(
                sf::Vector2f(world.getWidth() * cellSize, world.getHeight() * cellSize));
            winterOverlay.setFillColor(winterColor);
            window->draw(winterOverlay);
        }
        
        // Отрисовка статистики
        updateStatsText(stats);
        window->draw(statsText);
        
        // Отрисовка подсказок
        updateInfoText();
        window->draw(infoText);
        
        window->display();
    }
    
    void updateStatsText(const SimulationStats& stats) {
        std::ostringstream oss;
        oss << "=== СТАТИСТИКА ===" << "\n";
        oss << "Тик: " << stats.tick << "\n";
        oss << "Существа: " << stats.entityCount << " (Трав: " << stats.herbivoreCount 
            << ", Хищ: " << stats.predatorCount << ")" << "\n";
        oss << "Растения: " << stats.plantCount << "\n";
        oss << "Рождений: " << stats.totalBirths << " | Смертей: " << stats.totalDeaths << "\n";
        oss << "Ср. скорость: " << stats.avgSpeed << "\n";
        oss << "Ср. агрессия: " << stats.avgAggression << "\n";
        oss << "Ср. метаболизм: " << stats.avgMetabolism << "\n";
        
        statsText.setString(oss.str());
    }
    
    void updateInfoText() {
        std::ostringstream oss;
        oss << "=== УПРАВЛЕНИЕ (Режим Бога) ===" << "\n";
        oss << "W - Зима" << "\n";
        oss << "M - Метеорит" << "\n";
        oss << "P - Добавить растения" << "\n";
        oss << "E - Добавить травоядных" << "\n";
        oss << "R - Добавить хищников" << "\n";
        oss << "ESC - Выход" << "\n";
        
        infoText.setString(oss.str());
    }
    
    void close() {
        window->close();
    }
};
