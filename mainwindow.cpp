#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include "QLoggingCategory"
#include <QLabel>
#include <QScreen>
#include "controller/windowcontroller.h"

QLoggingCategory mainwindowCategory("mainwindow");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    worldController(std::make_shared<WorldController>()),
    graphicViewController(std::make_shared<GraphicViewController>(worldController)),
    textViewController(std::make_shared<TextViewController>(worldController)),
    windowController(std::make_shared<WindowController>(worldController)),
    movementController(std::make_shared<MovementController>(worldController))
{
    ui->setupUi(this);

    // Show the main window maximized
    this->showMaximized();

    graphicViewController->visualizeWorld();
    textViewController->visualizeWorld();
    windowController->setupWindow();

    setFocus();

    // Create a vertical layout for the main window
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create tabs
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QWidget, "Graphical View");
    tabWidget->addTab(new QWidget, "Text View");

    // Add the tab widget to the main layout
    mainLayout->addWidget(tabWidget);

    // Create additional widget
    QWidget *additionalWidget = new QWidget(this);

    // Add the additional widget to the main layout
    mainLayout->addWidget(additionalWidget);

    mainLayout->setStretch(0, 5); // tabWidget
    mainLayout->setStretch(1, 1); // additionalWidget

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

MainWindow::MainWindow(int width, int height, QWidget *parent)
    : MainWindow(parent)  // Call the default constructor with default values
{
    // Get the primary screen
    QScreen *screen = QGuiApplication::primaryScreen();
    // Get the available geometry of the screen
    QRect screenGeometry = screen->availableGeometry();
    // Calculate the center position
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;

    // Set the geometry of the main window
    setGeometry(x, y, width, height);
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
        int x = static_cast<int>(clickedPoint.x()) / 10 - 2;
        int y = static_cast<int>(clickedPoint.y()) / 10 - 4;

        emit mousePressed(x, y);
    }

}

void MainWindow::gameOverMessage(){
    QMessageBox::information(this, "Game Over", "You were defeated by the enemy!");
    QCoreApplication::quit();
}

void MainWindow::gameWonMessage() {
    QMessageBox::information(this, "Completed!", "You have defeated all enemies! Congrats!");
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
        connect(enemy.get(), &Enemy::dead, worldController.get(), &WorldController::handleDeath);
        if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
            connect(pEnemy, &PEnemy::poisonLevelUpdated, graphicViewController.get(), &GraphicViewController::handlePoisonLevelUpdated);
        }
        if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
            connect(xEnemy, &XEnemy::timerExpired, graphicViewController.get(), &GraphicViewController::handleAlive);
            connect(xEnemy, &XEnemy::halfDeadSet, graphicViewController.get(), &GraphicViewController::handleHalfDead);
            connect(xEnemy, &XEnemy::allHalfDead, worldController.get(), &WorldController::handleAllHalfDead);
        }
    }
    //drawBars
    connect(worldController.get(), &WorldController::drawBars,
            windowController.get(), &WindowController::drawBars);
    //gameOverMessage
    connect(worldController.get(), &WorldController::gameOver,
            this, &MainWindow::gameOverMessage);
    //gameWonMessage
    connect(worldController.get(), &WorldController::gameWon,
            this, &MainWindow::gameWonMessage);
    //healthPackTaken
    connect(worldController.get(), &WorldController::healthPackTaken,
            graphicViewController.get(), &GraphicViewController::handleHealthPackTaken);
    //zoomInGraphic
    connect(windowController.get(), &WindowController::zoomInSignal,
            graphicViewController.get(), &GraphicViewController::zoomIn);
    //zoomOutGraphic
    connect(windowController.get(), &WindowController::zoomOutSignal,
            graphicViewController.get(), &GraphicViewController::zoomOut);
    //autoplay
    connect(windowController.get(), &WindowController::autoplaySignal,
            worldController.get(), &WorldController::handleAutoplay);
    //zoomInText
    connect(windowController.get(), &WindowController::zoomInSignal,
            textViewController.get(), &TextViewController::zoomIn);
    //zoomOutText
    connect(windowController.get(), &WindowController::zoomOutSignal,
            textViewController.get(), &TextViewController::zoomOut);
    //moveProtagonistPos
    connect(worldController.get(), &WorldController::moveProtagonistPosSignal,
            movementController.get(), &MovementController::moveProtagonistPos);
    //moveProtagonistPath
    connect(worldController.get(), &WorldController::moveProtagonistPathSignal,
            movementController.get(), &MovementController::moveProtagonistPath);
    //checkPoisonDamage
    connect(movementController.get(), &MovementController::posChanged,
             worldController.get(), &WorldController::checkPoisonDamage);
}
