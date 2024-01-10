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
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>

class ViewController: public QObject
{
    Q_OBJECT
public:
    virtual ~ViewController() = default;

    virtual void visualizeWorld() = 0;
    virtual void drawProtagonist() = 0;
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;

    QMap<PEnemy*, int> spreadRadii;

protected:
    ViewController(std::shared_ptr<WorldController> worldController)
        : worldController(worldController) {}

    std::shared_ptr<WorldController> worldController;

private:

};

#endif // VIEWCONTROLLER_H
