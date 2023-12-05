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


    auto startTile = std::make_unique<Tile>(0, 0, 0.0f);
    auto endTile = std::make_unique<Tile>(myWorld.getCols()-1, myWorld.getRows()-1, 0.0f);
    findPath(scene,tileSize, std::move(startTile), std::move(endTile));
    

}

MainWindow::~MainWindow()
{
    delete ui;
}

