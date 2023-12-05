#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include "pathfinder.h"
#include "QLoggingCategory"
#include <iostream>

#include "pathfinder.h"
QLoggingCategory pathfinderCategory("pathfinder", QtDebugMsg);

QLoggingCategory mainwindowCategory("mainwindow");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    worldController(std::make_shared<WorldController>()),
    graphicViewController(std::make_shared<GraphicViewController>(worldController))
{
    ui->setupUi(this);
    this->view = graphicViewController->visualizeWorld();
    graphicViewController->drawBars();
    setCentralWidget(view);


    auto startTile = new Tile(0, 0, 0.0f);
    auto endTile = new Tile(10, 10, 0.0f);
    auto path = worldController->findPath(std::unique_ptr<Tile>(startTile),std::unique_ptr<Tile>(endTile));
    graphicViewController->visualizePath(path, std::unique_ptr<Tile>(startTile));

    

}

MainWindow::~MainWindow()
{
    delete ui;
}

