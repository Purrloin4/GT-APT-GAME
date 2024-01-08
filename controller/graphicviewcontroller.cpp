#include "graphicviewcontroller.h"
#include <iostream>
#include "QLoggingCategory"
#include "qtimer.h"

QLoggingCategory GraphicViewControllerCategory("graphicViewController", QtDebugMsg);

void GraphicViewController::visualizeWorld()
{
    auto myTiles = worldController->getTiles();
    auto enemies = worldController->getEnemies();
    auto healthPacks = worldController->getHealthpacks();
    auto protagonist = worldController->getProtagonist();
    auto portalTile = worldController->getPortalTile();

    QPixmap brickTexture(":/texture_images/brickwall.jpg");

    // Loop through each tile and set its color based on its value
    for (const auto &tile : myTiles) {
        int xPos = tile->getXPos();
        int yPos = tile->getYPos();
        double value = tile->getValue();

        // Determine the color of the tile based on its value
        QColor brush;
        if(std::isinf(value)){
            //brush = Qt::black;
            QGraphicsPixmapItem *brickItem = new QGraphicsPixmapItem(brickTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            brickItem->setPos(xPos * tileSize, yPos * tileSize);
            scene->addItem(brickItem);
        }
        else {
            brush = QColor::fromRgbF(value, value, value);
            scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), brush);
        }

    }

    QPixmap enemyTexture(":/texture_images/enemy.png");
    QPixmap PEnemyTexture(":/texture_images/PEnemy.png");
    QPixmap XEnemyTexture(":/texture_images/XEnemy.png");
    QPixmap portalTexture(":/texture_images/portal.png");

    QGraphicsPixmapItem *portalItem;
    portalItem = new QGraphicsPixmapItem(portalTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    portalItem->setPos(portalTile->getXPos() * tileSize, portalTile->getYPos() * tileSize);
    scene->addItem(portalItem);

    // Add visualization for enemies
    for (const auto &enemy : enemies) {
        QGraphicsPixmapItem *enemyItem;
        if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
            enemyItem = new QGraphicsPixmapItem(PEnemyTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            enemyItem->setPos(pEnemy->getXPos() * tileSize, pEnemy->getYPos() * tileSize);
            scene->addItem(enemyItem);
        } else if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
            enemyItem = new QGraphicsPixmapItem(XEnemyTexture.scaled(tileSize*0.9, tileSize*0.9, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            enemyItem->setPos(xEnemy->getXPos() * tileSize, xEnemy->getYPos() * tileSize);
            scene->addItem(enemyItem);
        } else {
            enemyItem = new QGraphicsPixmapItem(enemyTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            enemyItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
            scene->addItem(enemyItem);
        }
        QGraphicsTextItem *healthText = new QGraphicsTextItem(QString::number(enemy->getValue()));
        healthText->setDefaultTextColor(Qt::blue);
        healthText->setTextWidth(100);
        QFont font;
        font.setPointSize(20);
        healthText->setFont(font);
        healthText->setPos((enemy->getXPos()) * tileSize, (enemy->getYPos() + 1) * tileSize);
        scene->addItem(healthText);

        //To store the health text with the correct enemy
        TileVisualisation tileVis;
        tileVis.enemy = enemy.get();
        tileVis.enemyHealthText = healthText;
        tileVis.texturePixmapItem = enemyItem;
        tileVisualisations.push_back(tileVis);
    }

    QPixmap healthPackTexture(":/texture_images/healthpack.png");

    // Add visualization for health packs
    for (const auto &healthPack : healthPacks) {
        QGraphicsPixmapItem *healthPackItem = new QGraphicsPixmapItem(healthPackTexture.scaled(tileSize*0.95, tileSize*0.95, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        healthPackItem->setPos(healthPack->getXPos() * tileSize, healthPack->getYPos() * tileSize);
        scene->addItem(healthPackItem);

        QGraphicsTextItem *healthPackText = new QGraphicsTextItem(QString::number(healthPack->getValue()));
        healthPackText->setDefaultTextColor(Qt::blue);
        healthPackText->setTextWidth(100);
        QFont font;
        font.setPointSize(20);
        healthPackText->setFont(font);
        healthPackText->setPos((healthPack->getXPos()) * tileSize, (healthPack->getYPos() + 1) * tileSize);
        scene->addItem(healthPackText);


        //To store the health text with the correct enemy
        TileVisualisation tileVis;
        tileVis.tile = healthPack.get();
        tileVis.healthPackText = healthPackText;
        tileVis.texturePixmapItem = healthPackItem;
        tileVisualisations.push_back(tileVis);
    }

    this->drawProtagonist();

    graphViewWidget = new QWidget;
    graphLayout = new QVBoxLayout(graphViewWidget);
    graphLayout->addWidget(rawView);
}

void GraphicViewController::drawProtagonist() {
    auto protagonist = worldController->getProtagonist();

    if (protagonistPixmapItem == nullptr){
        QPixmap protagonistTexture(":/texture_images/protagonist.png");
        protagonistPixmapItem = new QGraphicsPixmapItem(protagonistTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        protagonistPixmapItem->setZValue(1);
        scene->addItem(protagonistPixmapItem);
    }

    // Calculate the desired scroll positions
    int centerPosX = (rawView->viewport()->width() - 1) / 2;
    int centerPosY = (rawView->viewport()->height() - 1) / 2;

    int protagonistX = protagonist->getXPos() * tileSize;
    int protagonistY = protagonist->getYPos() * tileSize;

    int scrollX = protagonistX - centerPosX;
    int scrollY = protagonistY - centerPosY;

    // Set the scroll positions
    rawView->horizontalScrollBar()->setValue(scrollX);
    rawView->verticalScrollBar()->setValue(scrollY);

    protagonistPixmapItem->setPos(protagonistX, protagonistY);
}

void GraphicViewController::visualizePath(std::vector<int> path, std::shared_ptr<Tile> startTile){
    if (pathDeletionTimer) {
        pathDeletionTimer->stop();
        delete pathDeletionTimer;
        pathDeletionTimer = nullptr;

        for (const auto &tileVis : previousPath) {
            if (tileVis.graphicsItem) {
                scene->removeItem(tileVis.graphicsItem);
                delete tileVis.graphicsItem;
            }
        }
        previousPath.clear();
    }

    pathDeletionTimer = new QTimer(this);

    connect(pathDeletionTimer, &QTimer::timeout, [this]() {
        for (const auto &tileVis : previousPath) {
            if (tileVis.graphicsItem) {
                scene->removeItem(tileVis.graphicsItem);
                delete tileVis.graphicsItem;
            }
        }
        previousPath.clear();

        pathDeletionTimer->stop();
        delete pathDeletionTimer;
        pathDeletionTimer = nullptr;
    });

    pathDeletionTimer->start(2000);

    auto xPos = startTile->getXPos();
    auto yPos = startTile->getYPos();
    for (const auto &move : path) {
        // Determine the position based on the move
        switch (move) {
        case 0: yPos -= 1; break;  // Move up
        case 1: xPos -= 1; yPos -= 1; break;  // Move to the top-right
        case 2: xPos -= 1; break;  // Move to the right
        case 3: xPos -= 1; yPos += 1; break;  // Move to the bottom-right
        case 4: yPos += 1; break;  // Move down
        case 5: xPos += 1; yPos += 1; break;  // Move to the bottom-left
        case 6: xPos += 1; break;  // Move to the left
        case 7: xPos += 1; yPos -= 1; break;  // Move to the top-left
        default: break;
        }
        TileVisualisation tileVis;
        tileVis.tile = worldController->getTile(xPos, yPos).get();
        tileVis.graphicsItem = scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::red));
        previousPath.push_back(tileVis);
    }
}

void GraphicViewController::handleAlive() {
    Enemy* enemy = qobject_cast<Enemy*>(sender());
    QPixmap xEnemyTexture(":/texture_images/XEnemy.png");
    QGraphicsPixmapItem *xEnemyItem = new QGraphicsPixmapItem(xEnemyTexture.scaled(tileSize*0.9, tileSize*0.9, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    xEnemyItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
    scene->addItem(xEnemyItem);
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            scene->removeItem(it->texturePixmapItem);
            it->texturePixmapItem = xEnemyItem;
            break;
        }
        else{
            ++it;
        }
    }
}

void GraphicViewController::handleHalfDead() {
    Enemy* enemy = qobject_cast<Enemy*>(sender());
    QPixmap XEnemyHalfTexture(":/texture_images/XEnemy_half.png");
    QGraphicsPixmapItem *halfXEnemyItem = new QGraphicsPixmapItem(XEnemyHalfTexture.scaled(tileSize*0.8, tileSize*0.8, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    halfXEnemyItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
    scene->addItem(halfXEnemyItem);
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            scene->removeItem(it->texturePixmapItem);
            it->texturePixmapItem = halfXEnemyItem;
            break;
        }
        else{
            ++it;
        }
    }
}

void GraphicViewController::handleDeath() {
    Enemy* enemy = qobject_cast<Enemy*>(sender());
    QPixmap tombStoneTexture(":/texture_images/tombstone.png");
    QGraphicsPixmapItem *tombStoneItem = new QGraphicsPixmapItem(tombStoneTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    tombStoneItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
    scene->addItem(tombStoneItem);
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            scene->removeItem(it->enemyHealthText);
            scene->removeItem(it->texturePixmapItem);
            break;
        }
        else{
            ++it;
        }
    }

    // Remove poisoned tiles and their color from the scene
    removePoisonedTiles(enemy);

}

void GraphicViewController::removePoisonedTiles(Enemy* enemy) {
    if (PEnemy* pEnemy = dynamic_cast<PEnemy*>(enemy)) {
        for (auto it = worldController->poisonedTiles.begin(); it != worldController->poisonedTiles.end();) {
            if (it->enemy == pEnemy) {
                scene->removeItem(it->graphicsItem);
                it = worldController->poisonedTiles.erase(it);
            } else {
                ++it;
            }
        }
    }
}


void GraphicViewController::handleHealthPackTaken(std::shared_ptr<Tile> pack){
     //scene->addRect(pack->getXPos() * tileSize, pack->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::purple));
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        //if (it->tile->getXPos() == xPos && it->tile->getYPos() == yPos){
        if(it->tile == pack.get()){
            qCDebug(GraphicViewControllerCategory) << "found healthPackTile";
            scene->removeItem(it->healthPackText);
            scene->removeItem(it->texturePixmapItem);
            break;
         }
        else{
            ++it;
        }
    }
}

void GraphicViewController::handlePoisonLevelUpdated(float poisonLevel) {
    PEnemy* pEnemy = qobject_cast<PEnemy*>(sender());
    qDebug() << "Poison level updated:" << poisonLevel << "Sender position:" << pEnemy->getXPos() << pEnemy->getYPos();

    // If the PEnemy is not in the map, initialize its spread radius to 1
    if (!spreadRadii.contains(pEnemy)) {
        spreadRadii.insert(pEnemy, 1);
    }

    drawPoisonSpread(pEnemy, poisonLevel);

}

void GraphicViewController::drawPoisonSpread(PEnemy* pEnemy, float poisonLevel) {
    int spreadRadius = spreadRadii.value(pEnemy);

    qDebug() << "Drawing poison at position:" << pEnemy->getXPos() << pEnemy->getYPos() << "with level:" << poisonLevel;

    // Calculate the color based on poison level
    int greenValue = static_cast<int>((100.0 - poisonLevel) * 255 / 100.0);
    QColor poisonColor = QColor(0, greenValue, 0);

    // Spread poison in a square pattern around the given position
    for (int i = -spreadRadius; i <= spreadRadius; ++i) {
        for (int j = -spreadRadius; j <= spreadRadius; ++j) {
            int spreadX = pEnemy->getXPos() + i;
            int spreadY = pEnemy->getYPos() + j;

            // Check if the position is within the bounds of the scene
            if (spreadX >= 0 && spreadX < worldController->getCols() && spreadY >= 0 && spreadY < worldController->getRows()) {
                TileVisualisation poisonedTile;
                poisonedTile.spreadXPos = spreadX;
                poisonedTile.spreadYPos = spreadY;
                poisonedTile.graphicsItem = scene->addRect(spreadX * tileSize, spreadY * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(poisonColor));
                poisonedTile.enemy = pEnemy;
                poisonedTile.poisonLevel = poisonLevel;
                worldController->poisonedTiles.push_back(poisonedTile);
            }
        }
    }

    drawProtagonist();

    // Update the spread radius for the next iteration
    spreadRadii.insert(pEnemy, spreadRadius + 1);
}

void GraphicViewController::zoomIn() {
    rawView->scale(1.1, 1.1);
    relativeTileSize = relativeTileSize / 1.1;
    qCDebug(GraphicViewControllerCategory) << "relativeTilesize:" << relativeTileSize;
}

void GraphicViewController::zoomOut() {
    rawView->scale(0.9, 0.9);
    relativeTileSize = relativeTileSize / 0.9;
    qCDebug(GraphicViewControllerCategory) << "relativeTilesize:" << relativeTileSize;
}

int GraphicViewController::getTileSize() const{
    return tileSize;
}

double GraphicViewController::getRelativeTileSize(){
    return relativeTileSize;
}

