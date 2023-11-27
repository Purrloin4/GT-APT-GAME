#ifndef GRAPHICVIEWCONTROLLER_H
#define GRAPHICVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

class GraphicViewController : public ViewController
{
public:
    GraphicViewController(std::shared_ptr<WorldController> worldController)
        :worldController(worldController){};
    std::unique_ptr<QGraphicsView> visualizeWorld();
private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<QGraphicsScene> scene;
};



#endif // GRAPHICVIEWCONTROLLER_H
