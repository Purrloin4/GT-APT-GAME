#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

struct EnemyVisualisation{
    Enemy enemy;
    QGraphicsTextItem* enemyHealthText;
};

class GraphicViewController : public ViewController
{
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :ViewController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())){}
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    void visualizeWorld() override;
    QGraphicsView* getRawView(){return rawView;};
    QVBoxLayout* getGraphLayout(){return graphLayout;};
public slots:
    void visualizePath(std::vector<int> path, std::shared_ptr<Tile> startTile);
    void drawProtagonist() override;
    void handleDeath() override;
    void handleHealthPackTaken(int xPos, int yPos) override;
    void drawBars();
signals:
    void pathVisualizationRequested(std::vector<int> path, std::shared_ptr<Tile> startTile);
private:
    int tileSize = 10;
    QVBoxLayout* graphLayout;
    QWidget* graphViewWidget;
};



#endif // GRAPHICVIEWCONTROLLER_H
