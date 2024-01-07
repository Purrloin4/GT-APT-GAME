#include "worldcontroller.h"
#include <iostream>
#include "QLoggingCategory"
#include "portaltile.h"
#include "qtimer.h"

QLoggingCategory WorldControllerCategory("worldController", QtDebugMsg);

WorldController::WorldController()
{
    // Create the world
    try {
        world = std::make_shared<World>();
        world->createWorld(":/world_images/worldmap.png", 8, 8, 0.25f);

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
        nrOfEnemies = myEnemies.size();
        for (const auto &enemy : myEnemies){
            if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
                auto sharedPEnemy = std::make_shared<PEnemy>(pEnemy->getXPos(), pEnemy->getYPos(), enemy->getValue());
                this->enemies.push_back(sharedPEnemy);
            } else {
                auto sharedEnemy = std::make_shared<Enemy>(enemy->getXPos(),enemy->getYPos(), enemy->getValue());
                this->enemies.push_back(sharedEnemy);
            }
        }

        for (const auto &tile : tiles) {
            if (!isEnemy(tile->getXPos(), tile->getYPos()) && !isHealthPack(tile->getXPos(), tile->getYPos())) {
                emptyTiles.push_back(tile);
            }
        }

        int index = rand() % emptyTiles.size();
        Tile* randomTile = emptyTiles[index].get();
        this->portalTile = std::make_shared<PortalTile>(randomTile->getXPos(),randomTile->getYPos(),":/world_images/worldmap.png");

        // Conversion of 25% of regual enemies to XEnemies
        int numXEnemies = static_cast<int>(0.25 * nrOfEnemies);
        int i = 0;

        for (const auto &enemy : enemies) {
            if (i == numXEnemies) {
                break;
            }

            // Create XEnemy from Enemy
            auto xEnemy = std::make_shared<XEnemy>(enemy->getXPos(), enemy->getYPos(), enemy->getValue());

            // Replace the original enemy with the XEnemy
            this->enemies[i] = xEnemy;
            i++;
        }

        autoplayActive = false;

        this->protagonist = std::make_shared<Protagonist>();

        this->cols = world->getCols();
        this->rows = world->getRows();

        energyRegenTimer = new QTimer(this);
        connect(energyRegenTimer, &QTimer::timeout, this, &WorldController::regenerateEnergy);
        energyRegenTimer->start(100);

    } catch (const std::exception& e) {
        // Handle any exceptions here
        std::cout << "Exeption during create world" << std::endl;
    }
}

std::vector<int> WorldController::findPath(std::shared_ptr<Tile> startTile, std::shared_ptr<Tile> endTile)
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
      return path;
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
      if (newX != protagonist->getXPos() || newY != protagonist->getYPos()){
          emit moveProtagonistPosSignal(newX, newY);

          // Redraw the protagonist and energy bar
          emit drawProtagonist();
          emit drawBars();

          // Check if we can attack an enemy or use a healthpack
          attackEnemy();
          useHealthpack();
      }
}

void WorldController::checkPoisonDamage() {
      int newX = protagonist->getXPos();
      int newY = protagonist->getYPos();
      for (auto it = poisonedTiles.begin(); it != poisonedTiles.end();) {
            if (it->spreadXPos == newX && it->spreadYPos == newY) {
            float newHealth = protagonist->getHealth() - it->poisonLevel;
            emit drawBars();
            if (newHealth > 0) {
                protagonist->setHealth(newHealth);
            } else {
                protagonist->setHealth(0);
                emit gameOver();
            }
            break;
            } else {
            ++it;
            }
      }
}

void WorldController::handleMousePressEvent(int x, int y) {

    // Check if the clicked position is within the boundaries of the world
    if (isValidPosition(x, y)) {
        // Call findPath with the clicked tile's position
        auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
        auto endTile = std::make_unique<Tile>(x, y, 0.0f);
        auto path = findPath(std::move(startTile), std::move(endTile));
        emit moveProtagonistPathSignal(path);
        emit drawProtagonist();
        emit drawBars();
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
                qCDebug(WorldControllerCategory) << "Protagonist health:" << protagonist->getHealth();
                // Check if the defeated enemy is a PEnemy
                if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
                    // Call the poison method for PEnemy
                    pEnemy->poison();
                    qCDebug(WorldControllerCategory) << "Attacked a poison enemy";
                // Check if enemy is an XEnemy
                } else if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
                    if (!xEnemy->isHalfDead()) {
                        xEnemy->setHalfDead(true);
                    } else {
                        xEnemy->setHalfDead(false);
                    }
                } else {
                    enemy->setDefeated(true);
                    qCDebug(WorldControllerCategory) << &"Defeated an enemy, nrOfEnemies = " [nrOfEnemies];
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

void WorldController::handleAllHalfDead() {
    for (auto& enemy : enemies) {
        if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
            xEnemy->setDefeated(true);
        }
    }
}

void WorldController::regenerateEnergy() {
    static int lastX = protagonist->getXPos();
    static int lastY = protagonist->getYPos();

    if (lastX == protagonist->getXPos() && lastY == protagonist->getYPos()) {
        double newEnergy = protagonist->getEnergy() + 0.5;
        if (newEnergy > getMaxEH()) {
            newEnergy = getMaxEH();
        }
        protagonist->setEnergy(newEnergy);
        emit drawBars();
    }

    lastX = protagonist->getXPos();
    lastY = protagonist->getYPos();
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


std::vector<std::shared_ptr<Enemy>> WorldController::getEnemies() const
{
    return enemies;
}

std::shared_ptr<Protagonist> WorldController::getProtagonist() const
{
    return protagonist;
}

std::shared_ptr<PortalTile> WorldController::getPortalTile() const
{
    return portalTile;
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

std::shared_ptr<World> WorldController::getWorld(){
    return world;
}

std::shared_ptr<Enemy> WorldController::getNearestEnemy()  {
    std::shared_ptr<Enemy> nearestEnemy = nullptr;
    int minDistance = INT_MAX;
    int protagonistX = protagonist->getXPos();
    int protagonistY = protagonist->getYPos();

    for (const auto& enemy : enemies) {
        if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
            if (!xEnemy->isHalfDead()) {
                if (!xEnemy->getDefeated()) {
                    int distance = abs(xEnemy->getXPos() - protagonistX) + abs(xEnemy->getYPos() - protagonistY);
                    if (distance < minDistance) {
                        minDistance = distance;
                        nearestEnemy = enemy;
                    }
                }
            }
        } else {
            if (!enemy->getDefeated()) {
                int distance = abs(enemy->getXPos() - protagonistX) + abs(enemy->getYPos() - protagonistY);
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestEnemy = enemy;
                }
            }
        }
    }
    return nearestEnemy;
}

std::shared_ptr<Tile> WorldController::getNearestHealthpack()  {
    std::shared_ptr<Tile> nearestHealthpack = nullptr;
    int minDistance = INT_MAX;
    int protagonistX = protagonist->getXPos();
    int protagonistY = protagonist->getYPos();

    for (const auto& healthpack : healthpacks) {
        if (healthpack->getValue() != 0) {
            int distance = abs(healthpack->getXPos() - protagonistX) + abs(healthpack->getYPos() - protagonistY);
            if (distance < minDistance) {
                minDistance = distance;
                nearestHealthpack = healthpack;
            }
        }
    }

    return nearestHealthpack;
}

void WorldController::handleAutoplay() {
    static QTimer timer; // Timer for autoplay

    if (!autoplayActive) {
        autoplayActive = true;
        qCDebug(WorldControllerCategory) << "Autoplay was activated!";

        // Connect the timer to a method that performs one step of the autoplay logic
        connect(&timer, &QTimer::timeout, this, &WorldController::autoplayStep);

        // Start the timer to call autoplayStep every 500 milliseconds
        timer.start(500);
    } else {
        autoplayActive = false;
        qCDebug(WorldControllerCategory) << "Autoplay was deactivated!";

        // Stop the timer
        timer.stop();

        // Disconnect the timer from autoplayStep
        disconnect(&timer, &QTimer::timeout, this, &WorldController::autoplayStep);
    }
}

void WorldController::autoplayStep() {
    if (nrOfEnemies != 0) {
        auto enemy = getNearestEnemy();
        if (enemy) {
            if (protagonist->getHealth() > enemy->getValue()) {
                auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
                auto endTile = std::make_unique<Tile>(enemy->getXPos(), enemy->getYPos(), 0.0f);
                auto path = findPath(std::move(startTile), std::move(endTile));
                emit moveProtagonistPathSignal(path);
                attackEnemy();
            } else {
                auto pack = getNearestHealthpack();
                if (pack) {
                    auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
                    auto endTile = std::make_unique<Tile>(pack->getXPos(), pack->getYPos(), 0.0f);
                    auto path = findPath(std::move(startTile), std::move(endTile));
                    emit moveProtagonistPathSignal(path);
                    useHealthpack();
                } else {
                    handleAutoplay();
                }
            }
        } else {
            handleAutoplay();
        }
    } else {
        handleAutoplay();
    }
}

void WorldController::handleDeath() {
    nrOfEnemies--;
    Enemy* enemy = qobject_cast<Enemy*>(sender());
    if (enemy) {
        disconnect(enemy, &Enemy::dead, this, &WorldController::handleDeath);
    }
    qCDebug(WorldControllerCategory) << "nrOfEnemies changed:" << nrOfEnemies;
    if (nrOfEnemies == 0) {
        emit gameWon();
    }
}

void WorldController::isPortal() {

    // Get the current position of the protagonist
    int x = protagonist->getXPos();
    int y = protagonist->getYPos();

    if (portalTile->getXPos() == x && portalTile->getYPos() == y) {
        // TODO handle portaltile
    }
}
