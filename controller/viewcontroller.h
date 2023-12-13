#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QObject>
#include "controller/worldcontroller.h"
#include <QLayout>

class ViewController: public QObject
{
    Q_OBJECT
public:
    ViewController();
    ViewController(std::shared_ptr<WorldController> worldController)
        : worldController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())){}
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    virtual void visualizeWorld() = 0;
    virtual void drawProtagonist() = 0;
    virtual void handleDeath() = 0;
    virtual void handleHealthPackTaken(int xPos, int yPos) = 0;
private:

};

#endif // VIEWCONTROLLER_H
