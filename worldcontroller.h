#ifndef WORLDCONTROLLER_H
#define WORLDCONTROLLER_H

#include "world.h"
#include <memory>
#include <vector>


struct point {
    int xCoordinate;
    int yCoordinate;
};


class WorldController
{
public:
    WorldController();

    point getStart();
    point getExit();

    std::shared_ptr<Tile> getTile(int x, int y) const;
    std::vector<std::shared_ptr<Tile> > getTiles() const;

    std::shared_ptr<Tile> getHealthpack(int x, int y) const;
    std::vector<std::shared_ptr<Tile> > getHealthpacks() const;

    std::shared_ptr<Enemy> getEnemy(int x, int y) const;
    std::vector<std::shared_ptr<Enemy> > getEnemies() const;

    std::shared_ptr<Protagonist> getProtagonist() const;

    int getHeight() const;
    int getWidth() const;

    bool isHealthPack(int x, int y);
    bool isEnemy(int x, int y);
    bool isPoisoned(int x, int y);

private:
    point start;
    point exit;
    std::shared_ptr<World> world;
    std::vector<std::shared_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Tile>> healthpacks;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::shared_ptr<Protagonist> protagonist;
    int height;
    int width;
};

#endif // WORLDCONTROLLER_H
