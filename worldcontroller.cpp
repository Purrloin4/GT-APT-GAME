#include "worldcontroller.h"

WorldController::WorldController()
{

}

std::shared_ptr<Tile> WorldController::getTile() const
{
    return nullptr;
}

std::vector<std::shared_ptr<Tile> > WorldController::getTiles() const
{
    return tiles;
}

std::shared_ptr<Tile> WorldController::getHealthpack() const
{
    return nullptr;
}

std::vector<std::shared_ptr<Tile> > WorldController::getHealthpacks() const
{
    return healthpacks;
}

std::shared_ptr<Enemy> WorldController::getEnemy() const
{
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
    return false;
}

bool WorldController::isEnemy(int x, int y)
{
    return false;
}

bool WorldController::isPoisined(int x, int y)
{
    return false;
}
