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

    this->connectSignalsAndSlots();


//    auto startTile = new Tile(0, 0, 0.0f);
//    auto endTile = new Tile(10, 10, 0.0f);
//    worldController->findPath(std::shared_ptr<Tile>(startTile),std::shared_ptr<Tile>(endTile));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
}

void MainWindow::connectSignalsAndSlots(){
    //visualize path
    connect(worldController.get(), &WorldController::pathFound,
            graphicViewController.get(), &GraphicViewController::visualizePath);
    //keypress
    connect(this, &MainWindow::keyPressed,
            worldController.get(), &WorldController::handleKeyPressEvent);
    //drawProtagonist
    connect(worldController.get(), &WorldController::drawProtagonist,
            graphicViewController.get(), &GraphicViewController::drawProtagonist);
    //handleDeath
    for (const auto &enemy : worldController->getEnemies() ){
        connect(enemy.get(), &Enemy::dead, graphicViewController.get(), &GraphicViewController::handleDeath);
        }
}

