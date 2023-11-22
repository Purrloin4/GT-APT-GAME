#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include "pathfinder.h"
#include "QLoggingCategory"
#include <QKeyEvent>
#include <QMouseEvent>

QLoggingCategory mainwindowCategory("mainwindow");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFocus();

    // Create the world
    try {
        myWorld.createWorld(":/world_images/grobu.png", 1, 1, 0.25f);
        visualizeWorld(); // Visualize the created world
        auto startTile = std::make_unique<Tile>(0, 0, 0.0f);
        auto endTile = std::make_unique<Tile>(20, 20, 0.0f);

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
    const float maxEH = 100.0f; // Define the value of maxEH
    auto enemies = myWorld.getEnemies();
    auto healthPacks = myWorld.getHealthPacks();

    // Create protagonist
    auto protagonist = Protagonist();

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

    drawProtagonist();

    // Add visualization for protagonist health bar
    int healthBarWidth = tileSize * 2; // You can adjust the width as needed
    int healthBarHeight = tileSize / 4; // You can adjust the height as needed
    QRect healthBarRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize - tileSize / 2, healthBarWidth, healthBarHeight);
    double healthRatio = static_cast<double>(protagonist.getHealth()) / static_cast<double>(maxEH);
    QColor healthBarColor = QColor::fromRgbF(1.0 - healthRatio, healthRatio, 0.0); // Red to green gradient
    scene->addRect(healthBarRect, QPen(Qt::black), QBrush(healthBarColor));

    // Add visualization for protagonist energy bar
    int energyBarWidth = tileSize * 2; // You can adjust the width as needed
    int energyBarHeight = tileSize / 4; // You can adjust the height as needed
    QRect energyBarRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize - tileSize / 2 - energyBarHeight, energyBarWidth, energyBarHeight);
    double energyRatio = static_cast<double>(protagonist.getEnergy()) / static_cast<double>(maxEH);
    QColor energyBarColor = QColor::fromRgbF(0.0, 0.0, 1.0 - energyRatio); // Blue to black gradient
    scene->addRect(energyBarRect, QPen(Qt::black), QBrush(energyBarColor));

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

  Comparator<PathNode> comp = PathNode::Comparator();

  PathNode startNode(*startTile);
  PathNode endNode(*endTile);

  std::vector<int> path = A_star(pathNodes, &startNode, &endNode, comp, myWorld.getCols(), 0.5);

  int xPos = startTile->getXPos();
  int yPos = startTile->getYPos();

  for (const auto &move : path) {
        // Determine the position based on the move
        switch (move) {
        case 0: yPos -= 1; break;  // Move up
        case 1: xPos += 1; yPos -= 1; break;  // Move to the top-right
        case 2: xPos += 1; break;  // Move to the right
        case 3: xPos += 1; yPos += 1; break;  // Move to the bottom-right
        case 4: yPos += 1; break;  // Move down
        case 5: xPos -= 1; yPos += 1; break;  // Move to the bottom-left
        case 6: xPos -= 1; break;  // Move to the left
        case 7: xPos -= 1; yPos -= 1; break;  // Move to the top-left
        default: break;
        }

        scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::red));
  }
}

void MainWindow::drawProtagonist() {
  // Remove the old position of the protagonist, if it exists
  if (protagonistItem) {
        scene->removeItem(protagonistItem);
  }

  // Add visualization for protagonist
  protagonistItem = scene->addRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::blue));
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  int newX = protagonist.getXPos();
  int newY = protagonist.getYPos();

  switch (event->key()) {
  case Qt::Key_Left: // If the left arrow key was pressed
        newX = protagonist.getXPos() - 1;
        break;
  case Qt::Key_Right: // If the right arrow key was pressed
        newX = protagonist.getXPos() + 1;
        break;
  case Qt::Key_Up: // If the up arrow key was pressed
        newY = protagonist.getYPos() - 1;
        break;
  case Qt::Key_Down: // If the down arrow key was pressed
        newY = protagonist.getYPos() + 1;
        break;
  default:
        QMainWindow::keyPressEvent(event);
  }

  // Check if the new position is within the boundaries of the world
  if (isValidPosition(newX, newY)) {
        // Update the protagonist's position only if it's a valid position
        protagonist.setXPos(newX);
        protagonist.setYPos(newY);

        // Redraw the protagonist
        drawProtagonist();
  }
}

bool MainWindow::isValidPosition(int x, int y) {
  // Check if the new position is within the boundaries of the world
  return x >= 0 && x < myWorld.getCols() && y >= 0 && y < myWorld.getRows();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  // Set focus to the main window when the mouse is clicked on the map
  setFocus();

  // Handle other mouse press events if needed: Movement of protagonist via mouse

  // Call the base class implementation to ensure standard processing
  QMainWindow::mousePressEvent(event);
}
