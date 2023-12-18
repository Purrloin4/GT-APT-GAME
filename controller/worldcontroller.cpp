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
            if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
                auto sharedPEnemy = std::make_shared<PEnemy>(pEnemy->getXPos(), pEnemy->getYPos(), enemy->getValue());
                this->enemies.push_back(sharedPEnemy);
            } else {
                auto sharedEnemy = std::make_shared<Enemy>(enemy->getXPos(),enemy->getYPos(), enemy->getValue());
                this->enemies.push_back(sharedEnemy);
            }
        }

        this->protagonist = std::make_shared<Protagonist>();
        this->protagonistItem = std::make_shared<QGraphicsRectItem*>();

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

        Comparator<PathNode> comp = PathNode::Comparator();

      PathNode startNode(*startTile);
      PathNode endNode(*endTile);

      std::vector<int> path = A_star(pathNodes, &startNode, &endNode, comp, cols, heursticFactor, heightFactor);

      emit pathFound(path, std::move(startTile));
}

void WorldController::handleKeyPressEvent(QKeyEvent *event){
        int newX = protagonist->getXPos();
        int newY = protagonist->getYPos();

        switch (event->key()) {
            // arrow keys can act up so ZQSD also possible #Azerty koning
            case Qt::Key_Q:
            case Qt::Key_Left:
                qCDebug(WorldControllerCategory) << "left key was pressed";
                newX = protagonist->getXPos() - 1;
                break;
            case Qt::Key_D:
            case Qt::Key_Right:
                qCDebug(WorldControllerCategory) << "right key was pressed";
                newX = protagonist->getXPos() + 1;
                break;
            case Qt::Key_Z:
            case Qt::Key_Up:
                qCDebug(WorldControllerCategory) << "up key was pressed";
                newY = protagonist->getYPos() - 1;
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                qCDebug(WorldControllerCategory) << "down key was pressed";
                newY = protagonist->getYPos() + 1;
                break;
        }

        // Check if the new position is within the boundaries of the world
        if (isValidPosition(newX, newY)) {
            // Update the protagonist's position only if it's a valid position
            protagonist->setXPos(newX);
            protagonist->setYPos(newY);

            // Redraw the protagonist and energy bar
            emit drawProtagonist();
            emit drawBars();

            // Check if we can attack an enemy or use a healthpack
            attackEnemy();
            useHealthpack();
      }
}

void WorldController::handleMousePressEvent(int x, int y) {

    // Check if the clicked position is within the boundaries of the world
    if (isValidPosition(x, y)) {
        // Call findPathAndHighlight with the clicked tile's position
        auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
        auto endTile = std::make_unique<Tile>(x, y, 0.0f);
        findPath(std::move(startTile), std::move(endTile));
        protagonist->setPos(x, y);
        emit drawProtagonist();
        attackEnemy();
        useHealthpack();
    }
}

bool WorldController::isValidPosition(int x, int y) {
      return x >= 0 && x < cols && y >= 0 && y < rows;
}

void WorldController::attackEnemy(){
      // Get the current position of the protagonist
      int x = protagonist->getXPos();
      int y = protagonist->getYPos();

      // Check if there is an enemy at the current position
      for (auto& enemy : enemies) {
            if (enemy->getXPos() == x && enemy->getYPos() == y) {
            if (enemy->getDefeated()) {
                break;
            }
            // Perform the attack logic here
            if (protagonist->getHealth() > enemy->getValue()) {
                // Protagonist has enough health to attack and defeat the enemy
                protagonist->setHealth(protagonist->getHealth() - enemy->getValue());
                qCDebug(WorldControllerCategory) << "Protagonist healt:" << protagonist->getHealth();
                // Check if the defeated enemy is a PEnemy
                if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
                    // Call the poison method for PEnemy
                    pEnemy->poison();
                    qCDebug(WorldControllerCategory) << "Attacked a poison enemy";
                } else {
                    enemy->setDefeated(true);
                    qCDebug(WorldControllerCategory) << "Defeated an enemy";
                }

                emit drawProtagonist();
                emit drawBars();
            } else {
                // Protagonist doesn't have enough health to defeat the enemy
                protagonist->setHealth(0.0f);
                emit drawBars();
                qCDebug(WorldControllerCategory) << "Protagonist dead";
                emit gameOver();
            }

            // Exit the function since the attack has been resolved
            return;
            }
      }

}

void WorldController::useHealthpack()
{
      // Get the current position of the protagonist
      int x = protagonist->getXPos();
      int y = protagonist->getYPos();

      // Check if there is an enemy at the current position
      for (auto& pack : healthpacks)
      {
            if (pack->getXPos() == x && pack->getYPos() == y)
            {
            // Perform the attack logic here
            if (protagonist->getHealth() < maxEH)
            {
                // Protagonist has enough health to attack and defeat the enemy
                float newHealth = protagonist->getHealth() + pack->getValue();
                if (newHealth > maxEH) {
                    protagonist->setHealth(maxEH);
                } else {
                    protagonist->setHealth(newHealth);
                }
                emit healthPackTaken(pack);
                pack->setValue(0.0f);
                emit drawProtagonist();
                emit drawBars();
            }
            // Exit the function since the attack has been resolved
            return;
            }
      }
}

int WorldController::getHeursticFactor() const
{
    return heursticFactor;
}

int WorldController::getHeightFactor() const
{
    return heightFactor;
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

std::shared_ptr<QGraphicsRectItem*> WorldController::getProtagonistItem() const{
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
