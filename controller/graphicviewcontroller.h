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
private:

};



#endif // GRAPHICVIEWCONTROLLER_H
