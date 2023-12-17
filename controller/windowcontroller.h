#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QObject>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "worldcontroller.h"

class WindowController: public QObject
{
    Q_OBJECT
public:
    WindowController(std::shared_ptr<WorldController> worldController)
        : worldController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())) {
        connect(worldController.get(), &WorldController::drawBars, this, &WindowController::drawBars);
    }
    void setupWindow();
    void updateHealthBar();
    void updateEnergyBar();
    QVBoxLayout* getWindowLayout(){return windowLayout;};
public slots:
    void drawBars();

private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    QVBoxLayout* windowLayout;
    QWidget* windowWidget;
};

#endif
