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
#include <QLabel>
#include "controller/windowcontroller.h"

QLoggingCategory pathfinderCategory("pathfinder", QtDebugMsg);

QLoggingCategory mainwindowCategory("mainwindow");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    worldController(std::make_shared<WorldController>()),
    graphicViewController(std::make_shared<GraphicViewController>(worldController)),
    textViewController(std::make_shared<TextViewController>(worldController)),
    windowController(std::make_shared<WindowController>(worldController))
{
    ui->setupUi(this);
    graphicViewController->visualizeWorld();
    textViewController->visualizeWorld();
    windowController->setupWindow();

    // Create a vertical layout for the main window
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Create tabs
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QWidget, "Graphical View");
    tabWidget->addTab(new QWidget, "Text View");

    // Add the tab widget to the main layout
    mainLayout->addWidget(tabWidget);

    // Create additional widget
    QWidget *additionalWidget = new QWidget;
    QLabel *additionalLabel = new QLabel("Health and energy");
    additionalLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout *additionalLayout = new QVBoxLayout;
    additionalLayout->addWidget(additionalLabel);

    // Add the additional widget to the main layout
    mainLayout->addWidget(additionalWidget);

    // Set the main layout for the main window
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Set the layout for the first tab
    tabWidget->widget(0)->setLayout(graphicViewController->getGraphLayout());
    tabWidget->widget(1)->setLayout(textViewController->getTextLayout());

    additionalWidget->setLayout(windowController->getWindowLayout());

    connectSignalsAndSlots();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    emit keyPressed(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    // Set focus to the main window when the mouse is clicked on the map
    setFocus();

    // Get the position of the click in scene coordinates
    auto views = graphicViewController->scene->views();
    if (!views.isEmpty()) {
        QPointF clickedPoint = views.first()->mapToScene(event->pos());

        // Convert the scene coordinates to tile coordinates
        int x = static_cast<int>(clickedPoint.x()) / 10 - 1;
        int y = static_cast<int>(clickedPoint.y()) / 10 - 1;

        emit mousePressed(x, y);
    }

}

void MainWindow::gameOverMessage(){
    QMessageBox::information(this, "Game Over", "You were defeated by the enemy!");
    QCoreApplication::quit();
}

void MainWindow::connectSignalsAndSlots() {
    //visualize path
    connect(worldController.get(), &WorldController::pathFound,
            graphicViewController.get(), &GraphicViewController::visualizePath);
    //keypress
    connect(this, &MainWindow::keyPressed,
            worldController.get(), &WorldController::handleKeyPressEvent);
    //mousepress
    connect(this, &MainWindow::mousePressed,
            worldController.get(), &WorldController::handleMousePressEvent);
    //drawProtagonistGraph
    connect(worldController.get(), &WorldController::drawProtagonist,
            graphicViewController.get(), &GraphicViewController::drawProtagonist);
    //drawProtagonistText
    connect(worldController.get(), &WorldController::drawProtagonist,
            textViewController.get(), &TextViewController::drawProtagonist);
    //handleDeath & handlePoisonLevelUpdated
    for (const auto &enemy : worldController->getEnemies() ){
        connect(enemy.get(), &Enemy::dead, graphicViewController.get(), &GraphicViewController::handleDeath);
        if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
            connect(pEnemy, &PEnemy::poisonLevelUpdated, graphicViewController.get(), &GraphicViewController::handlePoisonLevelUpdated);
        }
    }
//    //drawBars
//    connect(worldController.get(), &WorldController::drawBars,
//            textViewController.get(), &ViewController::drawBars);
    //gameOverMessage
    connect(worldController.get(), &WorldController::gameOver,
            this, &MainWindow::gameOverMessage);
    //healthPackTaken
    connect(worldController.get(), &WorldController::healthPackTaken,
            graphicViewController.get(), &GraphicViewController::handleHealthPackTaken);
}

