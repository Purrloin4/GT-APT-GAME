#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

class GraphicViewController : public ViewController
{
public:
    std::shared_ptr<WorldController> worldController;

    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :worldController(worldController){};
    QGraphicsView visualizeWorld();
};

#endif // GRAPHICVIEWCONTROLLER_H
