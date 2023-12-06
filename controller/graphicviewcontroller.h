#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"
#include <QObject>

class GraphicViewController : public QObject, public ViewController
{
    Q_OBJECT
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :ViewController(worldController){};
    QGraphicsView* visualizeWorld();
    void drawProtagonist() override;
public slots:
    void visualizePath(std::vector<int> path, std::shared_ptr<Tile> startTile);
signals:
    void pathVisualizationRequested(std::vector<int> path, std::shared_ptr<Tile> startTile);
private:
    int tileSize = 10;
};



#endif // GRAPHICVIEWCONTROLLER_H
