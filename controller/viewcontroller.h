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
#include <QTextEdit>

class ViewController: public QObject
{
    Q_OBJECT
public:
    ViewController();
    ViewController(std::shared_ptr<WorldController> worldController)
        : worldController(worldController){}
    std::shared_ptr<WorldController> worldController;
    virtual void visualizeWorld() = 0;
    virtual void drawProtagonist() = 0;
    virtual void handleDeath() = 0;
    virtual void handlePoisonLevelUpdated(float poisonLevel) = 0;
    virtual void handleHealthPackTaken(int xPos, int yPos) = 0;
    QMap<PEnemy*, int> spreadRadii;
private:
};

#endif // VIEWCONTROLLER_H
