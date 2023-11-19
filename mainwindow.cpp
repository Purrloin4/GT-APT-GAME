#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include "pathfinder.h"
#include "QLoggingCategory"

QLoggingCategory mainwindowCategory("mainwindow");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the world
    try {
        myWorld.createWorld(":/world_images/grobu.png", 1, 1, 0.25f);
        visualizeWorld(); // Visualize the created world
        auto startTile = std::make_unique<Tile>(0, 0, 0.0f);
        auto endTile = std::make_unique<Tile>(10, 10, 0.0f);

        findPathAndHighlight(scene,tileSize, std::move(startTile), std::move(endTile));
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
    this->scene = new QGraphicsScene(this);

    // Get tiles, enemies, and health packs from the world
    this->myTiles = myWorld.getTiles();
    auto enemies = myWorld.getEnemies();
    auto healthPacks = myWorld.getHealthPacks();

    // Adjust the size of the tiles in the visualization
    this->tileSize = 10; // Define the desired size for the tiles

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

    // Finally, set the scene in a graphics view
    QGraphicsView *view = new QGraphicsView(scene);
    setCentralWidget(view);
}

void MainWindow::findPathAndHighlight(QGraphicsScene* scene, int tileSize, std::unique_ptr<Tile> startTile, std::unique_ptr<Tile> endTile)
{
  std::vector<PathNode> pathNodes;
    for (const auto &tile : myTiles) {
        pathNodes.push_back(PathNode(*tile));
    }

  Comparator<PathNode> comp;

  PathNode startNode(*startTile);
  PathNode endNode(*endTile);

  auto width = (scene->width())/tileSize;

  std::vector<int> path = A_star(pathNodes, &startNode, &endNode, comp, width, 0.5);

  int xPos = startTile->getXPos();
  int yPos = startTile->getYPos();

  for (const auto &move : path) {
        // Determine the position based on the move
        switch (move) {
        case 0: xPos += 1; break;  // Move to the right
        case 1: xPos += 1; yPos += 1; break;  // Move to the bottom-right
        case 2: yPos += 1; break;  // Move down
        case 3: xPos -= 1; yPos += 1; break;  // Move to the bottom-left
        case 4: xPos -= 1; break;  // Move to the left
        case 5: xPos -= 1; yPos -= 1; break;  // Move to the top-left
        case 6: yPos -= 1; break;  // Move up
        case 7: xPos += 1; yPos -= 1; break;  // Move to the top-right
        default: break;
        }

        scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::red));
  }
}
