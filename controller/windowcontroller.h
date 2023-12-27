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
signals:
    void zoomInSignal();
    void zoomOutSignal();
    void autoplaySignal();
public:
    WindowController(std::shared_ptr<WorldController> worldController)
        : worldController(worldController), scene(std::make_shared<QGraphicsScene>()), rawView(new QGraphicsView(scene.get())) {}
    void setupWindow();
    void updateHealthBar();
    void updateEnergyBar();
    QHBoxLayout* getWindowLayout(){return windowLayout;};
public slots:
    void drawBars();
    void handleTextChange(const QString &text);
    void zoomIn();
    void zoomOut();
    void autoplay();
private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<QGraphicsScene> scene;
    QGraphicsView* rawView;
    QHBoxLayout* windowLayout;
    QVBoxLayout* controlLayout;
    QVBoxLayout* infoLayout;
    QWidget* windowWidget;
    QLineEdit* tileSizeEdit;
    QLineEdit* heuristicFactorEdit;
    QLineEdit* heightFactorEdit;
    QPushButton* autoPlayButton;
    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;
};

#endif
