#include "worldcontroller.h"
#include <iostream>
#include "QLoggingCategory"
#include "portaltile.h"
#include "qtimer.h"

QLoggingCategory WorldControllerCategory("worldController", QtDebugMsg);

WorldController::WorldController(QString map1, QString map2) {

    WorldState newState = createWorldState(map1);
    currentState = newState;
    loadWorldState(newState);

    portalMap = map2;

    this->protagonist = std::make_shared<Protagonist>();

    autoplayActive = false;
    energyRegenTimer = new QTimer(this);
    connect(energyRegenTimer, &QTimer::timeout, this, &WorldController::regenerateEnergy);
    energyRegenTimer->start(100);

}

WorldState WorldController::createWorldState(QString mapName){
    WorldState newState;
    try {
        newState.world = std::make_shared<World>();
        newState.world->createWorld(mapName, 8, 8, 0.25f);

        auto myTiles = newState.world->getTiles();
        for (const auto &tile : myTiles){
            auto sharedTile = std::make_shared<Tile>(tile->getXPos(), tile->getYPos(), tile->getValue());
            newState.tiles.push_back(sharedTile);
        }

        auto myHealthpacks = newState.world->getHealthPacks();
        for (const auto &healthpack : myHealthpacks){
            auto sharedHealthpack = std::make_shared<Tile>(healthpack->getXPos(),healthpack->getYPos(), healthpack->getValue());
            newState.healthpacks.push_back(sharedHealthpack);
        }

        auto myEnemies = newState.world->getEnemies();
        newState.nrOfEnemies = myEnemies.size();
        for (const auto &enemy : myEnemies){
            if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
                auto sharedPEnemy = std::make_shared<PEnemy>(pEnemy->getXPos(), pEnemy->getYPos(), enemy->getValue());
                newState.enemies.push_back(sharedPEnemy);
            } else {
                auto sharedEnemy = std::make_shared<Enemy>(enemy->getXPos(),enemy->getYPos(), enemy->getValue());
                newState.enemies.push_back(sharedEnemy);
            }
        }

        // Conversion of 25% of regual enemies to XEnemies
        int numXEnemies = static_cast<int>(0.25 * newState.nrOfEnemies);
        int i = 0;

        for (const auto &enemy : newState.enemies) {
            if (i == numXEnemies) {
                break;
            }
            // Replace the original enemy with the XEnemy
            auto xEnemy = std::make_shared<XEnemy>(enemy->getXPos(), enemy->getYPos(), enemy->getValue());
            newState.enemies[i] = xEnemy;
            i++;
        }

        for (const auto &tile : newState.tiles) {
            if (!isEnemy(tile->getXPos(), tile->getYPos()) && !isHealthPack(tile->getXPos(), tile->getYPos())) {
                newState.emptyTiles.push_back(tile);
            }
        }

        int index = rand() % newState.emptyTiles.size();
        Tile* randomTile = newState.emptyTiles[index].get();
        newState.portalTile = std::make_shared<PortalTile>(randomTile->getXPos(),randomTile->getYPos());

        newState.cols = newState.world->getCols();
        newState.rows = newState.world->getRows();
        emit connectSignals(newState.enemies);
        return newState;

    } catch (const std::exception& e) {
        // Handle any exceptions here
        std::cout << "Exeption during create of worldState" << std::endl;
        return newState;
    }
}

void WorldController::loadWorldState(const WorldState & newState){
    this->currentState = newState;
    world = currentState.world;
    tiles = currentState.tiles;
    healthpacks = currentState.healthpacks;
    enemies = currentState.enemies;
    emptyTiles = currentState.emptyTiles;
    portalTile = currentState.portalTile;
    nrOfEnemies = currentState.nrOfEnemies;
    cols = currentState.cols;
    rows = currentState.rows;
}

std::vector<int> WorldController::findPath(const std::shared_ptr<Tile> & startTile, const std::shared_ptr<Tile> & endTile) {
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

void WorldController::handleKeyPressEvent(QKeyEvent *event) {
      int newX = protagonist->getXPos();
      int newY = protagonist->getYPos();

      switch (event->key()) {
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
          emit drawProtagonist();
          emit drawBars();
          attackEnemy();
          useHealthpack();
          isPortal();
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
    if (isValidPosition(x, y)) {
        auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
        auto endTile = std::make_unique<Tile>(x, y, 0.0f);
        auto path = findPath(std::move(startTile), std::move(endTile));
        emit moveProtagonistPathSignal(path);
        emit drawProtagonist();
        emit drawBars();
        attackEnemy();
        useHealthpack();
        isPortal();
    }
}

void WorldController::attackEnemy() {
    int x = protagonist->getXPos();
    int y = protagonist->getYPos();

    for (auto& enemy : enemies) {
        if (enemy->getXPos() == x && enemy->getYPos() == y) {
        if (enemy->getDefeated()) {
            break;
        }
        if (protagonist->getHealth() > enemy->getValue()) { // Protagonist has enough health to attack and defeat the enemy
            protagonist->setHealth(protagonist->getHealth() - enemy->getValue());
            qCDebug(WorldControllerCategory) << "Protagonist health:" << protagonist->getHealth();
            if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) { // Check if the defeated enemy is a PEnemy
                pEnemy->poison();
                qCDebug(WorldControllerCategory) << "Attacked a poison enemy";
            } else if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) { // Check if enemy is an XEnemy
                if (!xEnemy->isHalfDead()) {
                    xEnemy->setHalfDead(true);
                } else {
                    xEnemy->setHalfDead(false);
                }
            } else {
                enemy->setDefeated(true);
                qCDebug(WorldControllerCategory) << &"Defeated an enemy, nrOfEnemies = " [nrOfEnemies];
            }
            emit drawBars();
        } else { // Protagonist doesn't have enough health to defeat the enemy
            protagonist->setHealth(0.0f);
            emit drawBars();
            qCDebug(WorldControllerCategory) << "Protagonist dead";
            emit gameOver();
        }
        return;
        }
    }
}

void WorldController::useHealthpack() {
    int x = protagonist->getXPos();
    int y = protagonist->getYPos();

    for (auto& pack : healthpacks) {
        if (pack->getXPos() == x && pack->getYPos() == y) {
            if (protagonist->getHealth() < maxEH) {
                float newHealth = protagonist->getHealth() + pack->getValue();
                if (newHealth > maxEH) {
                    protagonist->setHealth(maxEH);
                } else {
                    protagonist->setHealth(newHealth);
                }
                emit healthPackTaken(pack);
                pack->setValue(0.0f);
                emit drawBars();
            }
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

std::shared_ptr<Tile> WorldController::getTile(int x, int y) const {
    for (const auto& tile : tiles) {
        if (tile->getXPos() == x && tile->getYPos() == y) {
            return tile;
        }
    }
    return nullptr;
}

std::shared_ptr<Tile> WorldController::getHealthpack(int x, int y) const {
    for (const auto& healthpack : healthpacks) {
        if (healthpack->getXPos() == x && healthpack->getYPos() == y) {
            return healthpack;
        }
    }
    return nullptr;
}

std::shared_ptr<Enemy> WorldController::getEnemy(int x, int y) const {
    for (const auto& enemy : enemies) {
        if (enemy->getXPos() == x && enemy->getYPos() == y) {
            return enemy;
        }
    }
    return nullptr;
}


bool WorldController::isHealthPack(int x, int y) {
    for (const auto& healthpack : healthpacks) {
        if (healthpack->getXPos() == x && healthpack->getYPos() == y) {
            return true;
        }
    }
    return false;
}

bool WorldController::isEnemy(int x, int y) {
    for (const auto& enemy : enemies) {
        if (enemy->getXPos() == x && enemy->getYPos() == y) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Enemy> WorldController::getNearestEnemy() {
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

std::shared_ptr<Tile> WorldController::getNearestHealthpack() {
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
    static QTimer timer;
    if (!autoplayActive) {
        autoplayActive = true;
        qCDebug(WorldControllerCategory) << "Autoplay was activated!";
        connect(&timer, &QTimer::timeout, this, &WorldController::autoplayStep);
        timer.start(500);
    } else {
        autoplayActive = false;
        qCDebug(WorldControllerCategory) << "Autoplay was deactivated!";
        timer.stop();
        disconnect(&timer, &QTimer::timeout, this, &WorldController::autoplayStep);
    }
}

void WorldController::autoplayStep() {
    if (isPoisoned()) {
        return;
    }
    if (nrOfEnemies != 0) {
        auto enemy = getNearestEnemy();
        if (enemy) {
            if (protagonist->getHealth() > enemy->getValue()) {
                auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
                auto endTile = std::make_unique<Tile>(enemy->getXPos(), enemy->getYPos(), 0.0f);
                auto path = findPath(std::move(startTile), std::move(endTile));
                emit moveProtagonistPathSignal(path);
                attackEnemy();
                emit drawProtagonist();
                emit drawBars();
            } else {
                auto pack = getNearestHealthpack();
                if (pack) {
                    auto startTile = std::make_unique<Tile>(protagonist->getXPos(), protagonist->getYPos(), 0.0f);
                    auto endTile = std::make_unique<Tile>(pack->getXPos(), pack->getYPos(), 0.0f);
                    auto path = findPath(std::move(startTile), std::move(endTile));
                    emit moveProtagonistPathSignal(path);
                    useHealthpack();
                    emit drawProtagonist();
                    emit drawBars();
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
    Enemy* enemy = dynamic_cast<Enemy*>(sender());
    if (enemy) {
        disconnect(enemy, &Enemy::dead, this, &WorldController::handleDeath);
    }
    qCDebug(WorldControllerCategory) << "nrOfEnemies changed:" << nrOfEnemies;
    if (nrOfEnemies == 0) {
        emit gameWon();
    }
}

void WorldController::isPortal() {
    int x = protagonist->getXPos();
    int y = protagonist->getYPos();

    if (portalTile->getXPos() == x && portalTile->getYPos() == y) {
        qCDebug(WorldControllerCategory) << "Entered a portal tile!";

        if (firstBool) {
            WorldState secondState = createWorldState(portalMap);
            otherStates.push(secondState);
            firstBool = false;
        }
        WorldState newState = otherStates.top();
        otherStates.pop();
        otherStates.push(currentState);
        loadWorldState(newState);
        emit portalUsed();
        updateProtagonistPositionToPortal();
    }
}

void WorldController::updateProtagonistPositionToPortal(){
    protagonist->setXPos(portalTile->getXPos());
    protagonist->setYPos(portalTile->getYPos());
    emit drawProtagonist();
}

bool WorldController::isPoisoned() {
    for (const auto &enemy : enemies){
        if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
            if (pEnemy->getXPos() == protagonist->getXPos() && pEnemy->getYPos() == protagonist->getYPos()) {
                if (!pEnemy->getDefeated()) {
                    return true;
                }
            }
        }
    }
    return false;
}
