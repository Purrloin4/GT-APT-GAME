#include "graphicviewcontroller.h"
#include <iostream>

QGraphicsView* GraphicViewController::visualizeWorld()
{
    auto myTiles = worldController->getTiles();
    auto enemies = worldController->getEnemies();

    auto healthPacks = worldController->getHealthpacks();

    auto protagonist = worldController->getProtagonist();

    // Adjust the size of the tiles in the visualization
    auto tileSize = 10; // Define the desired size for the tiles

    // Loop through each tile and set its color based on its value
    for (const auto &tile : myTiles) {
        int xPos = tile->getXPos();
        int yPos = tile->getYPos();
        double value = tile->getValue();

        // Determine the color of the tile based on its value
        QColor brush;
        if(std::isinf(value)){
            brush = Qt::black;
        }
        else {
            brush = QColor::fromRgbF(value, value, value);
        }
        scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), brush);
    }

    // Add visualization for enemies
    for (const auto &enemy : enemies) {
        scene->addRect(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::red));
    }

    // Add visualization for health packs
    for (const auto &healthPack : healthPacks) {
        scene->addRect(healthPack->getXPos() * tileSize, healthPack->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::green));
    }

    // Add visualization for protagonist
    scene->addRect(protagonist->getXPos() * tileSize, protagonist->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::blue));

//    // Add visualization for protagonist health bar
//    int healthBarWidth = tileSize * 2; // You can adjust the width as needed
//    int healthBarHeight = tileSize / 4; // You can adjust the height as needed
//    QRect healthBarRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize - tileSize / 2, healthBarWidth, healthBarHeight);
//    double healthRatio = static_cast<double>(protagonist.getHealth()) / static_cast<double>(maxEH);
//    QColor healthBarColor = QColor::fromRgbF(1.0 - healthRatio, healthRatio, 0.0); // Red to green gradient
//    scene->addRect(healthBarRect, QPen(Qt::black), QBrush(healthBarColor));

//    // Add visualization for protagonist energy bar
//    int energyBarWidth = tileSize * 2; // You can adjust the width as needed
//    int energyBarHeight = tileSize / 4; // You can adjust the height as needed
//    QRect energyBarRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize - tileSize / 2 - energyBarHeight, energyBarWidth, energyBarHeight);
//    double energyRatio = static_cast<double>(protagonist.getEnergy()) / static_cast<double>(maxEH);
//    QColor energyBarColor = QColor::fromRgbF(0.0, 0.0, 1.0 - energyRatio); // Blue to black gradient
//    scene->addRect(energyBarRect, QPen(Qt::black), QBrush(energyBarColor));

    return rawView;
}
