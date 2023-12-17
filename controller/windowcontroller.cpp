#include "controller/windowcontroller.h"

//void WindowController::updateHealthBar() {
//    // Clear the layout
//    QLayoutItem *item;
//    while ((item = additionalLayout->takeAt(0)) != nullptr) {
//        delete item->widget();
//        delete item;
//    }

//    // Draw the health bar
//    QProgressBar *healthBar = new QProgressBar;
//    healthBar->setRange(0, 100);
//    healthBar->setValue(worldController->getProtagonist()->getHealth());
//    additionalLayout->addWidget(healthBar);
//}

//void WindowController::updateEnergyBar() {
//    // Clear the layout
//    QLayoutItem *item;
//    while ((item = additionalLayout->takeAt(0)) != nullptr) {
//        delete item->widget();
//        delete item;
//    }

//    // Draw the energy bar
//    QProgressBar *energyBar = new QProgressBar;
//    energyBar->setRange(0, 100);
//    energyBar->setValue(worldController->getProtagonist()->getEnergy());
//    additionalLayout->addWidget(energyBar);
//}

//void WindowController::drawBars(){
//    updateEnergyBar();
//    updateHealthBar();
//}

void WindowController::setupWindow(){
    windowWidget = new QWidget;
    windowLayout = new QVBoxLayout(windowWidget);
    windowLayout->addWidget(rawView);
    drawBars();
}

void WindowController::drawBars(){
    auto tileSize = 10;

    // Define the position above the map for the bars
    int barX = 0; // X-coordinate
    int barY = -tileSize * 2; // Y-coordinate, adjust as needed

    // Calculate the remaining health as a percentage
    double healthRatio = static_cast<double>(worldController->getProtagonist()->getHealth()) / static_cast<double>(worldController->getMaxEH());

    // Calculate the width of the health bar based on the remaining health
    int healthBarWidth = static_cast<int>(worldController->getCols() * tileSize * healthRatio);

    // Clear the old health bar rectangle
    QRect oldHealthBarRect(barX, barY, worldController->getCols() * tileSize, tileSize / 2); // Full width of the map
    scene->addRect(oldHealthBarRect, QPen(Qt::white), QBrush(Qt::white)); // Clear the old health bar

    // Add visualization for the sliding health bar
    QRect healthBarRect(barX, barY, healthBarWidth, tileSize / 2); // You can adjust the height as needed
    QColor healthBarColor = QColor::fromRgbF(1.0 - healthRatio, healthRatio, 0.0); // Red to green gradient
    scene->addRect(healthBarRect, QPen(Qt::black), QBrush(healthBarColor));

    // Add visualization for protagonist energy bar (similar to previous implementation)
    int energyBarWidth = worldController->getCols() * tileSize; // Full width of the map
    int energyBarHeight = tileSize / 2; // You can adjust the height as needed
    QRect energyBarRect(barX, barY - energyBarHeight, energyBarWidth, energyBarHeight);
    double energyRatio = static_cast<double>(worldController->getProtagonist()->getEnergy()) / static_cast<double>(worldController->getMaxEH());
    QColor energyBarColor = QColor::fromRgbF(0.0, 0.0, 1.0 - energyRatio); // Blue to black gradient
    scene->addRect(energyBarRect, QPen(Qt::black), QBrush(energyBarColor));
}
