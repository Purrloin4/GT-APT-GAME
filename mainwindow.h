#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "world.h"
#include "controller/worldcontroller.h"
#include "controller/graphicviewcontroller.h"
#include "controller/textviewcontroller.h"
#include "controller/windowcontroller.h"
#include "controller/movementcontroller.h"


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
    MainWindow(QWidget *parent = nullptr); // ctor - default width and height
    MainWindow(int width, int height, QWidget *parent = nullptr); // ctor - specified width and height
    ~MainWindow();
public slots:
        void gameOverMessage();
        void gameWonMessage();
        void handlePortalUsed();
private:
    Ui::MainWindow *ui;
    int tileSize;
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<GraphicViewController> graphicViewController;
    std::shared_ptr<TextViewController> textViewController;
    std::shared_ptr<WindowController> windowController;
    std::shared_ptr<MovementController> movementController;
    QGraphicsView* view;

    void connectSignalsAndSlots();
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

};

#endif // MAINWINDOW_H
