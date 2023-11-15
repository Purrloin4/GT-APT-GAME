#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the world
    try {
        myWorld.createWorld(":/world_images/worldmap4.png", 1, 1, 0.25f);
        visualizeWorld(); // Visualize the created world
    } catch (const std::exception& e) {
        // Handle any exceptions here
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::visualizeWorld()
{
    // Create a graphics scene
    QGraphicsScene *scene = new QGraphicsScene(this);

    // Get tiles, enemies, and health packs from the world
    auto tiles = myWorld.getTiles();
    auto enemies = myWorld.getEnemies();
    auto healthPacks = myWorld.getHealthPacks();

    // Adjust the size of the tiles in the visualization
    const int tileSize = 10; // Define the desired size for the tiles

    // Loop through each tile and set its color based on its value
    for (const auto &tile : tiles) {
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

    // Finally, set the scene in a graphics view
    QGraphicsView *view = new QGraphicsView(scene);
    setCentralWidget(view);
}
