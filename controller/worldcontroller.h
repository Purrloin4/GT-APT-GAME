#ifndef WORLDCONTROLLER_H
#define WORLDCONTROLLER_H

#include "portaltile.h"
#include "world.h"
#include <memory>
#include <stack>
#include <vector>
#include <QImage>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include "pathNode.h"
#include "pathfinder function.h"
#include "XEnemy.h"

struct TileVisualisation{
    int spreadXPos;
    int spreadYPos;
    float poisonLevel;
    Enemy* enemy;
    Tile* tile;
    QGraphicsTextItem* enemyHealthText;
    QGraphicsTextItem* healthPackText;
    QGraphicsRectItem* graphicsItem;
    QGraphicsPixmapItem* texturePixmapItem;
};

struct WorldState{
    std::shared_ptr<World> world;
    std::vector<std::shared_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Tile>> emptyTiles;
    std::vector<std::shared_ptr<Tile>> healthpacks;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::shared_ptr<PortalTile> portalTile;
    int rows;
    int cols;
    int nrOfEnemies;
};


class WorldController : public QObject
{
    Q_OBJECT
signals:
    void pathFound(std::vector<int> path, std::shared_ptr<Tile> startTile);
    void drawProtagonist();
    void drawBars();
    void gameOver();
    void gameWon();
    void healthPackTaken(std::shared_ptr<Tile> pack);
    void moveProtagonistPosSignal(int x, int y);
    void moveProtagonistPathSignal(std::vector<int> path);
    void portalUsed();
public:
    WorldController(QString map1, QString map2);

    std::vector<int> findPath(const std::shared_ptr<Tile> & startTile,const std::shared_ptr<Tile> & endTile);
    bool isValidPosition(int x, int y) {return x >= 0 && x < cols && y >= 0 && y < rows;};
    void attackEnemy();
    void useHealthpack();

    std::shared_ptr<World> getWorld() {return world;};

    std::shared_ptr<Tile> getTile(int x, int y) const;
    std::vector<std::shared_ptr<Tile> > getTiles() const {return tiles;};

    std::shared_ptr<Tile> getHealthpack(int x, int y) const;
    std::vector<std::shared_ptr<Tile> > getHealthpacks() const {return healthpacks;};

    std::shared_ptr<Enemy> getEnemy(int x, int y) const;
    std::vector<std::shared_ptr<Enemy> > getEnemies() const {return enemies;};

    std::shared_ptr<Protagonist> getProtagonist() const {return protagonist;};

    std::shared_ptr<PortalTile> getPortalTile() const {return portalTile;};

    std::vector<TileVisualisation> poisonedTiles;

    int getHeursticFactor() const {return heursticFactor;};
    int getHeightFactor() const {return heightFactor;};

    void setHeuristicFactor(int factor){heursticFactor = factor;};
    void setHeightFactor(int factor){heightFactor = factor;};

    int getRows() const {return rows;};
    int getCols() const {return cols;};

    float getMaxEH() const {return maxEH;};

    bool isHealthPack(int x, int y);
    bool isEnemy(int x, int y);

    std::shared_ptr<Enemy> getNearestEnemy();
    std::shared_ptr<Tile> getNearestHealthpack();

    void autoplayStep();

    void isPortal();

    void generateNewWorld(QString map);

    bool isPoisoned();

public slots:
    void handleKeyPressEvent(QKeyEvent *event);
    void handleMousePressEvent(int x, int y);
    void handleAllHalfDead();
    void checkPoisonDamage();
    void handleAutoplay();
    void handleDeath();
private:
    std::shared_ptr<World> world;
    std::vector<std::shared_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Tile>> healthpacks;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::shared_ptr<Protagonist> protagonist;
    std::vector<std::shared_ptr<Tile>> emptyTiles;
    std::shared_ptr<PortalTile> portalTile;
    int rows;
    int cols;
    const float maxEH = 100.0f;
    double heursticFactor = 1.1f;
    double heightFactor = 1.1f;
    QTimer* energyRegenTimer;
    void regenerateEnergy();
    int nrOfEnemies;
    bool autoplayActive;
    WorldState currentState;
    std::stack<WorldState> otherStates;
    WorldState createWorldState(QString mapName);
    void loadWorldState(const WorldState & state);
    void updateProtagonistPositionToPortal();
};

#endif // WORLDCONTROLLER_H
