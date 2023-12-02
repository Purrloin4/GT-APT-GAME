#include "graphicviewcontroller.h"
#include <iostream>

QGraphicsView* GraphicViewController::visualizeWorld()
{
    auto myTiles = worldController->getTiles();
    auto enemies = worldController->getEnemies();

    auto healthPacks = worldController->getHealthpacks();

    auto protagonist = worldController->getProtagonist();

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

    return rawView;

}

void GraphicViewController::drawProtagonist() {
    auto protagonistItem = worldController->getProtagonistItem();
    auto protagonist = worldController->getProtagonist();

    if (protagonistItem) {
        scene->removeItem(protagonistItem);
    }

    // Add visualization for protagonist
    protagonistItem = scene->addRect(protagonist->getXPos() * tileSize, protagonist->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::blue));
}
