#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "world.h"
#include "pathNode.h"
#include "controller/worldcontroller.h"
#include "controller/graphicviewcontroller.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    World myWorld;
    int tileSize;
    std::vector<std::unique_ptr<Tile>> myTiles;
    std::shared_ptr<WorldController> worldController;


    void visualizeWorld();
    void findPathAndHighlight(QGraphicsScene* scene, int tileSize, std::unique_ptr<Tile> startTile, std::unique_ptr<Tile> endTile);

};

#endif // MAINWINDOW_H
