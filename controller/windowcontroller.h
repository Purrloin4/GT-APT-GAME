#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
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
    QHBoxLayout* getWindowLayout(){return windowLayout;};
public slots:
    void drawBars();

private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    QHBoxLayout* windowLayout;
    QVBoxLayout* controlLayout;
    QVBoxLayout* infoLayout;
    QWidget* windowWidget;
};

#endif
