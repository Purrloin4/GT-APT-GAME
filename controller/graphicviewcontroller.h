#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

class GraphicViewController : public ViewController
{
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :worldController(worldController){};
    QGraphicsView* visualizeWorld();
private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<QGraphicsScene> scene;
    std::unique_ptr<QGraphicsView> view;
    QGraphicsView* rawView;
};



#endif // GRAPHICVIEWCONTROLLER_H
