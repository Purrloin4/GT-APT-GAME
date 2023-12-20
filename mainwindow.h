#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "world.h"
#include "controller/worldcontroller.h"
#include "controller/graphicviewcontroller.h"
#include "controller/textviewcontroller.h"
#include "controller/windowcontroller.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void keyPressed(QKeyEvent *event);
    void mousePressed(int x, int y);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
        void gameOverMessage();
private:
    Ui::MainWindow *ui;
    World myWorld;
    int tileSize;
    std::vector<std::unique_ptr<Tile>> myTiles;
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<GraphicViewController> graphicViewController;
    std::shared_ptr<TextViewController> textViewController;
    std::shared_ptr<WindowController> windowController;
    QGraphicsView* view;

    void connectSignalsAndSlots();
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

};

#endif // MAINWINDOW_H
