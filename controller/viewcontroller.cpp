#include "viewcontroller.h"
#include <iostream>

ViewController::ViewController()
{
}

//void ViewController::drawBars(){
//    // Define the position above the map for the bars
//    int barX = 0; // X-coordinate
//    int barY = 0; // Y-coordinate, adjust as needed

//    // Calculate the remaining health as a percentage
//    double healthRatio = static_cast<double>(worldController->getWorld()->getProtagonist()->getHealth()) / static_cast<double>(worldController->getMaxEH());

//    // Calculate the width of the health bar based on the remaining health
//    int healthBarWidth = static_cast<int>(worldController->getWorld()->getCols() * tileSize * healthRatio);

//    // Clear the old health bar rectangle
//    QRect oldHealthBarRect(barX, barY, myWorld.getCols() * tileSize, tileSize / 2); // Full width of the map
//    scene->addRect(oldHealthBarRect, QPen(Qt::white), QBrush(Qt::white)); // Clear the old health bar

//    // Add visualization for the sliding health bar
//    QRect healthBarRect(barX, barY, healthBarWidth, tileSize / 2); // You can adjust the height as needed
//    QColor healthBarColor = QColor::fromRgbF(1.0 - healthRatio, healthRatio, 0.0); // Red to green gradient
//    scene->addRect(healthBarRect, QPen(Qt::black), QBrush(healthBarColor));

//    // Add visualization for protagonist energy bar (similar to previous implementation)
//    int energyBarWidth = myWorld.getCols() * tileSize; // Full width of the map
//    int energyBarHeight = tileSize / 2; // You can adjust the height as needed
//    QRect energyBarRect(barX, barY - energyBarHeight, energyBarWidth, energyBarHeight);
//    double energyRatio = static_cast<double>(protagonist.getEnergy()) / static_cast<double>(maxEH);
//    QColor energyBarColor = QColor::fromRgbF(0.0, 0.0, 1.0 - energyRatio); // Blue to black gradient
//    scene->addRect(energyBarRect, QPen(Qt::black), QBrush(energyBarColor));
//}
