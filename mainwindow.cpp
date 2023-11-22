#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include "pathfinder.h"
#include "QLoggingCategory"
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>

QLoggingCategory mainwindowCategory("mainwindow");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the world
    try {
        myWorld.createWorld(":/world_images/grobu.png", 25, 25, 0.25f);
        visualizeWorldText(); // Visualize the created world
        auto startTile = std::make_unique<Tile>(0, 0, 0.0f);
        auto endTile = std::make_unique<Tile>(20, 20, 0.0f);

        //findPathAndHighlight(scene,tileSize, std::move(startTile), std::move(endTile));
    } catch (const std::exception& e) {
        // Handle any exceptions here
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::visualizeWorldGraph()
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
        if (std::isinf(value)) {
            brush = Qt::black;
        } else {
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
    scene->addRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::blue));

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

    // Create buttons for switching between graphical and text views
    QPushButton *graphicalButton = new QPushButton("Graphical View");
    QPushButton *textButton = new QPushButton("Text View");

    // Connect button signals to corresponding slots
    connect(graphicalButton, &QPushButton::clicked, this, &MainWindow::showGraphicalView);
    connect(textButton, &QPushButton::clicked, this, &MainWindow::showTextView);

    // Create a layout for the buttons and graphics view
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view);
    layout->addWidget(graphicalButton);
    layout->addWidget(textButton);

    // Set the central widget to the layout
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
}

void MainWindow::visualizeWorldText()
{
    // Create a QString to hold the ASCII representation of the world
    QString asciiRepresentation;

    // Get tiles, enemies, and health packs from the world
    this->myTiles = myWorld.getTiles();
    auto enemies = myWorld.getEnemies();
    auto healthPacks = myWorld.getHealthPacks();

    // Create protagonist
    auto protagonist = Protagonist();

    // Adjust the size of the tiles in the visualization
    this->tileSize = 10; // Define the desired size for the tiles

    // Determine the ASCII representation of different entities
    const QString horizontalBorder = "+---+";
    const QString verticalEmptyTile = "|\u00A0\u00A0\u00A0|";
    const QString verticalHealthPackTile = "| H |";
    const QString verticalEnemyTile = "| E |";
    const QString verticalProtagonistTile = "| P |";

    // Loop through each row
    for (int y = 0; y < myWorld.getRows(); ++y) {
        // Add the horizontal border for each tile in the row
        for (int x = 0; x < myWorld.getCols(); ++x) {
            asciiRepresentation += horizontalBorder;
        }
        asciiRepresentation += QChar(0x2029);

        // Loop through each column
        for (int x = 0; x < myWorld.getCols(); ++x) {
            // Check if the current position contains an entity (health pack, enemy, protagonist)
            auto isHealthPack = std::find_if(healthPacks.begin(), healthPacks.end(),
                                             [x, y](const auto &hp) { return hp->getXPos() == x && hp->getYPos() == y; });
            auto isEnemy = std::find_if(enemies.begin(), enemies.end(),
                                        [x, y](const auto &enemy) { return enemy->getXPos() == x && enemy->getYPos() == y; });
            auto isProtagonist = (protagonist.getXPos() == x && protagonist.getYPos() == y);

            // Append the corresponding ASCII representation to the overall representation string
            if (isHealthPack != healthPacks.end()) {
                asciiRepresentation += verticalHealthPackTile;
            } else if (isEnemy != enemies.end()) {
                asciiRepresentation += verticalEnemyTile;
            } else if (isProtagonist) {
                asciiRepresentation += verticalProtagonistTile;
            } else {
                asciiRepresentation += verticalEmptyTile; // Spaces for empty tile
            }
        }
        asciiRepresentation += QChar(0x2029);
    }

    // Display the ASCII representation in a QTextEdit
    QTextEdit *asciiTextEdit = new QTextEdit(asciiRepresentation);
    asciiTextEdit->setFont(QFont("Courier")); // Set a monospaced font for better alignment

    // Create buttons for switching between graphical and text views
    QPushButton *graphicalButton = new QPushButton("Graphical View");
    QPushButton *textButton = new QPushButton("Text View");

    // Connect button signals to corresponding slots
    connect(graphicalButton, &QPushButton::clicked, this, &MainWindow::showGraphicalView);
    connect(textButton, &QPushButton::clicked, this, &MainWindow::showTextView);

    // Create a layout for the buttons and ASCII text edit
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(asciiTextEdit);
    layout->addWidget(graphicalButton);
    layout->addWidget(textButton);

    // Set the central widget to the layout
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
}







void MainWindow::showGraphicalView()
{
    visualizeWorldGraph();
}

void MainWindow::showTextView()
{
    visualizeWorldText();
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
