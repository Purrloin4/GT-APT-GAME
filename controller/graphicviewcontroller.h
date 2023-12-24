#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"
#include <QTimer>

class GraphicViewController : public ViewController
{
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :ViewController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())){}
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    void visualizeWorld() override;
    QGraphicsView* getRawView(){return rawView;}
    QVBoxLayout* getGraphLayout(){return graphLayout;}
    void drawPoisonSpread(PEnemy* pEnemy, float poisonLevel);
    void removePoisonedTiles(Enemy* enemy);
public slots:
    void visualizePath(std::vector<int> path, std::shared_ptr<Tile> startTile);
    void drawProtagonist() override;
    void handleDeath() override;
    void handlePoisonLevelUpdated(float poisonLevel) override;
    void handleHealthPackTaken(std::shared_ptr<Tile> pack) override;
    void zoomIn() override;
    void zoomOut() override;
    void handleAlive();
    void handleHalfDead();
signals:
    void pathVisualizationRequested(std::vector<int> path, std::shared_ptr<Tile> startTile);
private:
    QTimer *pathDeletionTimer = nullptr;
    int tileSize = 10;
    QVBoxLayout* graphLayout;
    QWidget* graphViewWidget;
    std::vector<TileVisualisation> tileVisualisations;
    std::vector<TileVisualisation> previousPath;
    QGraphicsPixmapItem* healthPackTextureItem;
};



#endif // GRAPHICVIEWCONTROLLER_H
