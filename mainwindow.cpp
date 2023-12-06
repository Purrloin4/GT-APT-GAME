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
    graphicViewController(std::make_shared<GraphicViewController>(worldController)),
    textViewController(std::make_shared<TextViewController>(worldController))

{
    ui->setupUi(this);
    graphicViewController->visualizeWorld();
    setCentralWidget(graphicViewController->getRawView());

    this->connectSignalsAndSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
}

void MainWindow::gameOverMessage(){
    QMessageBox::information(this, "Game Over", "You were defeated by the enemy!");
    QCoreApplication::quit();
}

void MainWindow::connectSignalsAndSlots(){
    //visualize path
    connect(worldController.get(), &WorldController::pathFound,
            graphicViewController.get(), &GraphicViewController::visualizePath);
    //keypress
    connect(this, &MainWindow::keyPressed,
            worldController.get(), &WorldController::handleKeyPressEvent);
    //drawProtagonist
    connect(worldController.get(), &WorldController::drawProtagonist, // world geeft al een emit op poschange dus kan beter
            graphicViewController.get(), &GraphicViewController::drawProtagonist);
    //handleDeath
    for (const auto &enemy : worldController->getEnemies() ){
        connect(enemy.get(), &Enemy::dead, graphicViewController.get(), &GraphicViewController::handleDeath);
    }
    //drawBars
    connect(worldController.get(), &WorldController::drawBars, //world geeft een emit op health- en energychange dus kan beter
            graphicViewController.get(), &GraphicViewController::drawBars);
    //gameOverMessage
    connect(worldController.get(), &WorldController::gameOver,
            this, &MainWindow::gameOverMessage);
    //healthPackTaken
    connect(worldController.get(), &WorldController::healthPackTaken,
            graphicViewController.get(), &GraphicViewController::handleHealthPackTaken);
}

