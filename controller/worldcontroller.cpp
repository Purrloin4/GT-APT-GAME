#include "worldcontroller.h"
#include <iostream>


WorldController::WorldController()
    : start{0, 0}, exit{30,30} // start always (0,0), exit (30,30) for worldmap.png
{
    // Create the world
    try {
        world = std::make_shared<World>();
        world->createWorld(":/world_images/worldmap.png", 1, 1, 0.25f);

        auto myTiles = world->getTiles();
        for (const auto &tile : myTiles){
            auto sharedTile = std::make_shared<Tile>(tile->getXPos(), tile->getYPos(), tile->getValue());
            this->tiles.push_back(sharedTile);
        }


    } catch (const std::exception& e) {
        // Handle any exceptions here
        std::cout << "Exeption during create world" << std::endl;
    }
}

point WorldController::getStart()
{
    return start;
}

point WorldController::getExit()
{
    return exit;
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

int WorldController::getHeight() const
{
    return height;
}

int WorldController::getWidth() const
{
    return width;
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
