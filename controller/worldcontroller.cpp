#include "worldcontroller.h"
#include <iostream>
#include "QLoggingCategory"

QLoggingCategory WorldControllerCategory("worldController", QtDebugMsg);

WorldController::WorldController()
{
    // Create the world
    try {
        world = std::make_shared<World>();
        world->createWorld(":/world_images/worldmap.png", 20, 20, 0.25f);

        auto myTiles = world->getTiles();
        for (const auto &tile : myTiles){
            auto sharedTile = std::make_shared<Tile>(tile->getXPos(), tile->getYPos(), tile->getValue());
            this->tiles.push_back(sharedTile);
        }

        auto myHealthpacks = world->getHealthPacks();
        for (const auto &healthpack : myHealthpacks){
            auto sharedHealthpack = std::make_shared<Tile>(healthpack->getXPos(),healthpack->getYPos(), healthpack->getValue());
            this->healthpacks.push_back(sharedHealthpack);
        }

        auto myEnemies = world->getEnemies();
        for (const auto &enemy : myEnemies){
            auto sharedEnemy = std::make_shared<Enemy>(enemy->getXPos(),enemy->getYPos(), enemy->getValue());
            this->enemies.push_back(sharedEnemy);
        }
        this->protagonist = std::make_shared<Protagonist>();

        this->cols = world->getCols();
        this->rows = world->getRows();



    } catch (const std::exception& e) {
        // Handle any exceptions here
        std::cout << "Exeption during create world" << std::endl;
    }
}

void WorldController::findPath(std::shared_ptr<Tile> startTile, std::shared_ptr<Tile> endTile)
{
      std::vector<PathNode> pathNodes;
        for (const auto &tile : tiles) {
            pathNodes.push_back(PathNode(*tile));
        }

        auto weight = 0.1;
        auto minimalCost = 0.1;

        Comparator<PathNode> comp = PathNode::Comparator();

      PathNode startNode(*startTile);
      PathNode endNode(*endTile);

      std::vector<int> path = A_star(pathNodes, &startNode, &endNode, comp, cols, weight, minimalCost);

      emit pathFound(path, std::move(startTile));
}

void WorldController::handleKeyPressEvent(QKeyEvent *event){
        int newX = protagonist->getXPos();
        int newY = protagonist->getYPos();

        qCDebug(WorldControllerCategory) << event->key();

        switch (event->key()) {
        case Qt::Key_Left:
            qCDebug(WorldControllerCategory) << "left arrow key was pressed";
            newX = protagonist->getXPos() - 1;
            break;
        case Qt::Key_Right:
            qCDebug(WorldControllerCategory) << "right arrow key was pressed";
            newX = protagonist->getXPos() + 1;
            break;
        case Qt::Key_Up:
            qCDebug(WorldControllerCategory) << "up arrow key was pressed";
            newY = protagonist->getYPos() - 1;
            break;
        case Qt::Key_Down:
            qCDebug(WorldControllerCategory) << "down arrow key was pressed";
            newY = protagonist->getYPos() + 1;
            break;
        }

        // Check if the new position is within the boundaries of the world
        if (isValidPosition(newX, newY)) {
            // Update the protagonist's position only if it's a valid position
            protagonist->setXPos(newX);
            protagonist->setYPos(newY);

            // Redraw the protagonist and energy bar
//            drawProtagonist();
//            drawBars();

            // Check if we can attack an enemy or use a healthpack
//            attackEnemy();
//            useHealthpack();
      }

}

bool WorldController::isValidPosition(int x, int y) {
      return x >= 0 && x < cols && y >= 0 && y < rows;
}


std::shared_ptr<Tile> WorldController::getTile(int x, int y) const
{
    for (const auto& tile : tiles) {
        if (tile->getXPos() == x && tile->getYPos() == y) {
            return tile;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Tile> > WorldController::getTiles() const
{
    return tiles;
}

std::shared_ptr<Tile> WorldController::getHealthpack(int x, int y) const
{
    for (const auto& healthpack : healthpacks) {
        if (healthpack->getXPos() == x && healthpack->getYPos() == y) {
            return healthpack;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Tile> > WorldController::getHealthpacks() const
{
    return healthpacks;
}

std::shared_ptr<Enemy> WorldController::getEnemy(int x, int y) const
{
    for (const auto& enemy : enemies) {
        if (enemy->getXPos() == x && enemy->getYPos() == y) {
            return enemy;
        }
    }
    return nullptr;
}


std::vector<std::shared_ptr<Enemy> > WorldController::getEnemies() const
{
    return enemies;
}

std::shared_ptr<Protagonist> WorldController::getProtagonist() const
{
    return protagonist;
}

QGraphicsRectItem* WorldController::getProtagonistItem() const{
    return protagonistItem;
}

int WorldController::getRows() const
{
    return rows;
}

int WorldController::getCols() const
{
    return cols;
}

float WorldController::getMaxEH() const
{
    return maxEH;
}

bool WorldController::isHealthPack(int x, int y)
{
    for (const auto& healthpack : healthpacks) {
        if (healthpack->getXPos() == x && healthpack->getYPos() == y) {
            return true;
        }
    }
    return false;
}

bool WorldController::isEnemy(int x, int y)
{
    for (const auto& enemy : enemies) {
        if (enemy->getXPos() == x && enemy->getYPos() == y) {
            return true;
        }
    }
    return false;
}

bool WorldController::isPoisoned(int x, int y)
{
    // We do not yet have poisoned tiles
    return false;
}

std::shared_ptr<World> WorldController::getWorld(){
    return world;
}
