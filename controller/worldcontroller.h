#ifndef WORLDCONTROLLER_H
#define WORLDCONTROLLER_H

#include "world.h"
#include <memory>
#include <vector>
#include <QImage>
#include <QGraphicsRectItem>
#include "pathNode.h"
#include "pathfinder.h"


struct point {
    int xCoordinate;
    int yCoordinate;
};


class WorldController : public QObject
{
    Q_OBJECT
signals:
    void pathFound(std::vector<int> path, std::shared_ptr<Tile> startTile);
public:
    WorldController();

    void findPath(std::shared_ptr<Tile> startTile, std::shared_ptr<Tile> endTile);

    std::shared_ptr<World> getWorld();

    std::shared_ptr<Tile> getTile(int x, int y) const;
    std::vector<std::shared_ptr<Tile> > getTiles() const;

    std::shared_ptr<Tile> getHealthpack(int x, int y) const;
    std::vector<std::shared_ptr<Tile> > getHealthpacks() const;

    std::shared_ptr<Enemy> getEnemy(int x, int y) const;
    std::vector<std::shared_ptr<Enemy> > getEnemies() const;

    std::shared_ptr<Protagonist> getProtagonist() const;

    QGraphicsRectItem* getProtagonistItem() const;

    int getRows() const;
    int getCols() const;


    float getMaxEH() const;

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
    int rows;
    int cols;
    const float maxEH = 100.0f;
    QGraphicsRectItem* protagonistItem = nullptr;
};

#endif // WORLDCONTROLLER_H
