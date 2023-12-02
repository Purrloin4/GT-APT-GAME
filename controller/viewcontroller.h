#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include "controller/worldcontroller.h"

class ViewController
{
    public:
        ViewController();
        ViewController(std::shared_ptr<WorldController> worldController)
            : worldController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())){}
        void drawBars();
        std::shared_ptr<WorldController> worldController;
        std::shared_ptr<QGraphicsScene> scene;
        QGraphicsView* rawView;
        virtual void drawProtagonist() = 0;
    private:

};

#endif // VIEWCONTROLLER_H
