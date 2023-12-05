#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

class GraphicViewController : public ViewController
{
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :ViewController(worldController){};
    QGraphicsView* visualizeWorld();
    void drawProtagonist() override;
    void visualizePath(std::vector<int> path, std::unique_ptr<Tile> startTile);
private:
    int tileSize = 10;
};



#endif // GRAPHICVIEWCONTROLLER_H
