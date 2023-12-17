#include "controller/windowcontroller.h"

void WindowController::setupWindow(){
    windowWidget = new QWidget;
    windowLayout = new QHBoxLayout(windowWidget);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(rawView);

    QFormLayout *controlLayout = new QFormLayout;
    QPushButton *button = new QPushButton("AutoPlay");

    QLabel *heuristicFactorLabel = new QLabel("Heuristic Factor");
    QLineEdit *heuristicFactor = new QLineEdit;
    controlLayout->addRow(heuristicFactorLabel, heuristicFactor);

    QLabel *heightFactorLabel = new QLabel("Height Factor");
    QLineEdit *heightFactor = new QLineEdit;
    controlLayout->addRow(heightFactorLabel, heightFactor);

    controlLayout->addWidget(button);

    windowLayout->addLayout(infoLayout);
    windowLayout->addLayout(controlLayout);
    drawBars();
}

void WindowController::drawBars(){
    auto tileSize = 10;

    int barX = 0;
    int barY = 0;

    // Calculate the remaining health as a percentage
    double healthRatio = static_cast<double>(worldController->getProtagonist()->getHealth()) / static_cast<double>(worldController->getMaxEH());

    // Calculate the width of the health bar based on the remaining health
    int healthBarWidth = static_cast<int>(worldController->getCols() * tileSize * healthRatio);

    // Clear the old health bar rectangle
    QRect oldHealthBarRect(barX, barY, worldController->getCols() * tileSize, tileSize); // Full width of the map
    scene->addRect(oldHealthBarRect, QPen(Qt::white), QBrush(Qt::white)); // Clear the old health bar

    // Add visualization for the sliding health bar
    QRect healthBarRect(barX, barY, healthBarWidth, tileSize / 2); // You can adjust the height as needed
    QColor healthBarColor = QColor::fromRgbF(1.0 - healthRatio, healthRatio, 0.0); // Red to green gradient
    scene->addRect(healthBarRect, QPen(Qt::black), QBrush(healthBarColor));

    // Add visualization for protagonist energy bar (similar to previous implementation)
    int energyBarWidth = worldController->getCols() * tileSize; // Full width of the map
    int energyBarHeight = tileSize; // You can adjust the height as needed
    QRect energyBarRect(barX, barY - energyBarHeight, energyBarWidth, energyBarHeight);
    double energyRatio = static_cast<double>(worldController->getProtagonist()->getEnergy()) / static_cast<double>(worldController->getMaxEH());
    QColor energyBarColor = QColor::fromRgbF(0.0, 0.0, 1.0 - energyRatio); // Blue to black gradient
    scene->addRect(energyBarRect, QPen(Qt::black), QBrush(energyBarColor));
}
