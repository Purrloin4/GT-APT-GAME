#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"
#include <QTimer>
#include <QMovie>
#include <QGraphicsProxyWidget>

class GraphicViewController : public ViewController
{
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :ViewController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())){
        splatterAnimation = new QMovie(":/animations/splatter.gif");
        splatterAnimation->setScaledSize(QSize(tileSize*4, tileSize*4));
        splatterAnimation->setSpeed(50);
    }
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    void visualizeWorld() override;
    QGraphicsView* getRawView(){return rawView;}
    QVBoxLayout* getGraphLayout(){return graphLayout;}
    void drawPoisonSpread(PEnemy* pEnemy, float poisonLevel);
    void removePoisonedTiles(Enemy* enemy);
    int getTileSize() const;
    double getRelativeTileSize();
public slots:
    void visualizePath(std::vector<int> path, std::shared_ptr<Tile> startTile);
    void drawProtagonist() override;
    void handleDeath();
    void handlePoisonLevelUpdated(float poisonLevel);
    void handleHealthPackTaken(std::shared_ptr<Tile> pack);
    void zoomIn() override;
    void zoomOut() override;
    void handleAlive();
    void handleHalfDead();
signals:
    void pathVisualizationRequested(std::vector<int> path, std::shared_ptr<Tile> startTile);
private:
    QTimer *pathDeletionTimer = nullptr;
    const int tileSize = 50;
    QVBoxLayout* graphLayout;
    QWidget* graphViewWidget;
    std::vector<TileVisualisation> tileVisualisations;
    std::vector<TileVisualisation> previousPath;
    QGraphicsPixmapItem* healthPackTextureItem;
    QGraphicsPixmapItem* protagonistPixmapItem = nullptr;
    double relativeTileSize = 1.0;
    QMovie *splatterAnimation;
    void animateSplatter(int xPos,int yPos);
};



#endif // GRAPHICVIEWCONTROLLER_H
