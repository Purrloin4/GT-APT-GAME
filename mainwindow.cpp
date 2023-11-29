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
        myWorld.createWorld(":/world_images/grobu.png", 25, 25, 0.25f);
        visualizeWorldGraph(); // Visualize the created world
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
    enemies = myWorld.getEnemies();
    healthPacks = myWorld.getHealthPacks();

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

    // Add visualization for enemies (including defeated ones)
    for (const auto &enemy : enemies) {
        scene->addRect(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), enemy->getDefeated() ? QBrush(Qt::gray) : QBrush(Qt::red));
    }

    // Add visualization for health packs
    for (const auto &healthPack : healthPacks) {
        scene->addRect(healthPack->getXPos() * tileSize, healthPack->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::green));
    }

    drawProtagonist();

    drawBars();

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
    const QString horizontalBorder = "+---";
    const QString verticalEmptyTile = "|\u00A0\u00A0\u00A0";
    const QString verticalHealthPackTile = "| H ";
    const QString verticalEnemyTile = "| E ";
    const QString verticalProtagonistTile = "| P ";

    // Loop through each row
    for (int y = 0; y < myWorld.getRows(); ++y) {
        // Add the horizontal border for each tile in the row
        for (int x = 0; x < myWorld.getCols(); ++x) {
            asciiRepresentation += horizontalBorder;
        }
        asciiRepresentation += "+"; // Add last '+' of every row
        asciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'

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
        asciiRepresentation += "|"; // Add last '|' of every row
        asciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'
    }

    // Add the horizontal border after the last row
    for (int x = 0; x < myWorld.getCols(); ++x) {
        asciiRepresentation += horizontalBorder;
    }
    asciiRepresentation += "+"; // Add last '+' of map

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

void MainWindow::drawProtagonist() {
    // Remove the old position of the protagonist, if it exists
    if (protagonistItem) {
        scene->removeItem(protagonistItem);
    }

    // Add visualization for protagonist
    protagonistItem = scene->addRect(protagonist.getXPos() * tileSize, protagonist.getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::blue));
}

void MainWindow::drawBars() {
    // Define the position above the map for the bars
    int barX = 0; // X-coordinate
    int barY = -tileSize * 2; // Y-coordinate, adjust as needed

    // Calculate the remaining health as a percentage
    double healthRatio = static_cast<double>(protagonist.getHealth()) / static_cast<double>(maxEH);

    // Calculate the width of the health bar based on the remaining health
    int healthBarWidth = static_cast<int>(myWorld.getCols() * tileSize * healthRatio);

    // Clear the old health bar rectangle
    QRect oldHealthBarRect(barX, barY, myWorld.getCols() * tileSize, tileSize / 2); // Full width of the map
    scene->addRect(oldHealthBarRect, QPen(Qt::white), QBrush(Qt::white)); // Clear the old health bar

    // Add visualization for the sliding health bar
    QRect healthBarRect(barX, barY, healthBarWidth, tileSize / 2); // You can adjust the height as needed
    QColor healthBarColor = QColor::fromRgbF(1.0 - healthRatio, healthRatio, 0.0); // Red to green gradient
    scene->addRect(healthBarRect, QPen(Qt::black), QBrush(healthBarColor));

    // Add visualization for protagonist energy bar (similar to previous implementation)
    int energyBarWidth = myWorld.getCols() * tileSize; // Full width of the map
    int energyBarHeight = tileSize / 2; // You can adjust the height as needed
    QRect energyBarRect(barX, barY - energyBarHeight, energyBarWidth, energyBarHeight);
    double energyRatio = static_cast<double>(protagonist.getEnergy()) / static_cast<double>(maxEH);
    QColor energyBarColor = QColor::fromRgbF(0.0, 0.0, 1.0 - energyRatio); // Blue to black gradient
    scene->addRect(energyBarRect, QPen(Qt::black), QBrush(energyBarColor));
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

        // Redraw the protagonist and energy bar
        drawProtagonist();
        drawBars();

        // Check if we can attack an enemy or use a healthpack
        attackEnemy();
        useHealthpack();
  }
}

bool MainWindow::isValidPosition(int x, int y) {
  // Check if the new position is within the boundaries of the world
  return x >= 0 && x < myWorld.getCols() && y >= 0 && y < myWorld.getRows();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  // Set focus to the main window when the mouse is clicked on the map
  setFocus();

  // Get the position of the click in scene coordinates
  auto views = scene->views();
  if (!views.isEmpty()) {
        QPointF clickedPoint = views.first()->mapToScene(event->pos());

        // Convert the scene coordinates to tile coordinates
        int x = static_cast<int>(clickedPoint.x()) / tileSize - 1;
        int y = static_cast<int>(clickedPoint.y()) / tileSize - 1;

        // Check if the clicked position is within the boundaries of the world
        if (isValidPosition(x, y)) {
            // Call findPathAndHighlight with the clicked tile's position
            auto startTile = std::make_unique<Tile>(protagonist.getXPos(), protagonist.getYPos(), 0.0f);
            auto endTile = std::make_unique<Tile>(x, y, 0.0f);
            findPathAndHighlight(scene, tileSize, std::move(startTile), std::move(endTile));
            protagonist.setPos(x, y);
            drawProtagonist();
            attackEnemy();
            useHealthpack();
        }
  }

  // Call the base class implementation to ensure standard processing
  QMainWindow::mousePressEvent(event);
}

void MainWindow::attackEnemy()
{
  // Get the current position of the protagonist
  int x = protagonist.getXPos();
  int y = protagonist.getYPos();

  // Check if there is an enemy at the current position
  for (auto& enemy : enemies)
  {
        if (enemy->getXPos() == x && enemy->getYPos() == y)
        {
            if (enemy->getDefeated()){
                break;
            }
            // Perform the attack logic here
            if (protagonist.getHealth() > enemy->getValue())
            {
                // Protagonist has enough health to attack and defeat the enemy
                protagonist.setHealth(protagonist.getHealth()-enemy->getValue());
                enemy->setDefeated(true);
                scene->addRect(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::purple));
                drawProtagonist();
                drawBars();
            }
            else
            {
                // Protagonist doesn't have enough health to defeat the enemy
                protagonist.setHealth(0.0f);
                drawBars();
                QMessageBox::information(this, "Game Over", "You were defeated by the enemy!");
                // Exit the program when OK is pressed
                QCoreApplication::quit();
            }

            // Exit the function since the attack has been resolved
            return;
        }
  }
}

void MainWindow::useHealthpack()
{
  // Get the current position of the protagonist
  int x = protagonist.getXPos();
  int y = protagonist.getYPos();

  // Check if there is an enemy at the current position
  for (auto& pack : healthPacks)
  {
        if (pack->getXPos() == x && pack->getYPos() == y)
        {
            // Perform the attack logic here
            if (protagonist.getHealth() < maxEH)
            {
                // Protagonist has enough health to attack and defeat the enemy
                float newHealth = protagonist.getHealth() + pack->getValue();
                if (newHealth > maxEH) {
                    protagonist.setHealth(maxEH);
                } else {
                    protagonist.setHealth(newHealth);
                }
                scene->addRect(pack->getXPos() * tileSize, pack->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::purple));
                pack->setValue(0.0f);
                drawProtagonist();
                drawBars();
            }
            // Exit the function since the attack has been resolved
            return;
        }
  }
}
