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
    Protagonist protagonist;
    QGraphicsRectItem* protagonistItem = nullptr;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Tile>> healthPacks;
    const float maxEH = 100.0f;


    void visualizeWorldGraph();
    void visualizeWorldText();
    void findPathAndHighlight(QGraphicsScene* scene, int tileSize, std::unique_ptr<Tile> startTile, std::unique_ptr<Tile> endTile, float heurWeight, float minimalCost);
    void showGraphicalView();
    void showTextView();
    void keyPressEvent(QKeyEvent *event);
    void drawProtagonist();
    void drawBars();
    bool isValidPosition(int x, int y);
    void mousePressEvent(QMouseEvent *event);
    void attackEnemy();
    void useHealthpack();

};

#endif // MAINWINDOW_H
