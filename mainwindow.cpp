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
        myWorld.createWorld(":/world_images/grobu.png", 1, 1, 0.25f);
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

    // Add visualization code here to display the world using QGraphicsScene and QGraphicsView
    // For example, you can add rectangles or images representing tiles, enemies, and health packs to the scene.

    // Visualize the tiles
    for (const auto &tile : tiles) {
        int xPos = tile->getXPos();
        int yPos = tile->getYPos();
        if (std::isinf(tile->getValue())) {
            // Impassable area (wall)
            scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::darkGray));
        } else {
            // Passable area
            scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::lightGray));
        }
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
