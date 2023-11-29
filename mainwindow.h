#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "world.h"
#include "pathNode.h"



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
    QGraphicsScene* scene;
    std::vector<std::unique_ptr<Tile>> myTiles;
    std::vector<std::unique_ptr<Enemy>> myEnemies;
    std::vector<std::unique_ptr<Tile>> myHealthpacks;
    Protagonist protagonist;
    QGraphicsRectItem* protagonistItem = nullptr;


    void visualizeWorldGraph();
    void visualizeWorldText();
    void findPathAndHighlight(QGraphicsScene* scene, int tileSize, std::unique_ptr<Tile> startTile, std::unique_ptr<Tile> endTile);
    void showGraphicalView();
    void showTextView();
    void keyPressEvent(QKeyEvent *event);
    void drawProtagonist();
    bool isValidPosition(int x, int y);
    void mousePressEvent(QMouseEvent *event);

};

#endif // MAINWINDOW_H
