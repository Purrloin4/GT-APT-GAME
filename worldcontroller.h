#ifndef WORLDCONTROLLER_H
#define WORLDCONTROLLER_H

#include "world.h"
#include <memory>
#include <vector>

class WorldController
{
public:
    WorldController();

    std::shared_ptr<Tile> getTile() const;
    std::vector<std::shared_ptr<Tile> > getTiles() const;

    std::shared_ptr<Tile> getHealthpack() const;
    std::vector<std::shared_ptr<Tile> > getHealthpacks() const;

    std::shared_ptr<Enemy> getEnemy() const;
    std::vector<std::shared_ptr<Enemy> > getEnemies() const;

    std::shared_ptr<Protagonist> getProtagonist() const;

    int getHeight() const;
    int getWidth() const;

    bool isHealthPack(int x, int y);
    bool isEnemy(int x, int y);
    bool isPoisined(int x, int y);

private:
    std::shared_ptr<World> world;
    std::vector<std::shared_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Tile>> healthpacks;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::shared_ptr<Protagonist> protagonist;
    int height;
    int width;
};

#endif // WORLDCONTROLLER_H
