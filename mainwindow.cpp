#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "world.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    World world;
    QString path = ":/world_images/grobu.png";
    world.createWorld(path,2,3,2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

