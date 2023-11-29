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
    , ui(new Ui::MainWindow),
    worldController(std::make_shared<WorldController>()),
    graphicViewController(std::make_shared<GraphicViewController>(worldController))
{
    ui->setupUi(this);
    this->view = graphicViewController->visualizeWorld();
    setCentralWidget(view.get());


//    auto startTile = std::make_unique<Tile>(0, 0, 0.0f);
//    auto endTile = std::make_unique<Tile>(myWorld.getCols()-1, myWorld.getRows()-1, 0.0f);
//    findPathAndHighlight(scene,tileSize, std::move(startTile), std::move(endTile));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::findPathAndHighlight(QGraphicsScene* scene, int tileSize, std::unique_ptr<Tile> startTile, std::unique_ptr<Tile> endTile)
{
  std::vector<PathNode> pathNodes;
    for (const auto &tile : myTiles) {
        pathNodes.push_back(PathNode(*tile));
    }

    auto weight = 0.1;
    auto minimalCost = 0.1;

    Comparator<PathNode> comp = PathNode::Comparator();

  PathNode startNode(*startTile);
  PathNode endNode(*endTile);

  std::vector<int> path = A_star(pathNodes, &startNode, &endNode, comp, myWorld.getCols(), weight, minimalCost);

  int xPos = startTile->getXPos();
  int yPos = startTile->getYPos();

  for (const auto &move : path) {
        // Determine the position based on the move
        switch (move) {
        case 0: yPos -= 1; break;  // Move up
        case 1: xPos -= 1; yPos -= 1; break;  // Move to the top-right
        case 2: xPos -= 1; break;  // Move to the right
        case 3: xPos -= 1; yPos += 1; break;  // Move to the bottom-right
        case 4: yPos += 1; break;  // Move down
        case 5: xPos += 1; yPos += 1; break;  // Move to the bottom-left
        case 6: xPos += 1; break;  // Move to the left
        case 7: xPos += 1; yPos -= 1; break;  // Move to the top-left
        default: break;
        }

        scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::red));
  }
}
