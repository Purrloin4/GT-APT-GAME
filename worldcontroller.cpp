#include "worldcontroller.h"

WorldController::WorldController()
{

}

std::vector<std::shared_ptr<Tile> > WorldController::getTiles() const
{
    return tiles;
}

std::vector<std::shared_ptr<Tile> > WorldController::getHealthpacks() const
{
    return healthpacks;
}

std::vector<std::shared_ptr<EnemyInfo> > WorldController::getEnemies() const
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
