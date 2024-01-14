#include "graphicviewcontroller.h"
#include <iostream>
#include "QLoggingCategory"
#include "qlabel.h"
#include "qtimer.h"

QLoggingCategory GraphicViewControllerCategory("graphicViewController", QtDebugMsg);

void GraphicViewController::visualizeWorld()
{
    const auto& myTiles = worldController->getTiles();
    const auto& enemies = worldController->getEnemies();
    const auto& healthPacks = worldController->getHealthpacks();
    const auto& protagonist = worldController->getProtagonist();
    const auto& portalTile = worldController->getPortalTile();

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
            scene->addItem(std::move(brickItem));
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
    scene->addItem(std::move(portalItem));

    tileVisualisations.clear();
    // Add visualization for enemies
    for (const auto &enemy : enemies) {
        QGraphicsPixmapItem *enemyItem;
        if (enemy->getDefeated()) continue;
        if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
            enemyItem = new QGraphicsPixmapItem(PEnemyTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            enemyItem->setPos(pEnemy->getXPos() * tileSize, pEnemy->getYPos() * tileSize);
            scene->addItem(enemyItem);
        } else if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
            if (xEnemy->getDefeated()) continue;
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
        scene->addItem(std::move(healthText));

        //To store the health text with the correct enemy
        TileVisualisation tileVis;
        tileVis.enemy = enemy.get();
        tileVis.enemyHealthText = healthText;
        tileVis.texturePixmapItem = std::move(enemyItem);
        tileVisualisations.push_back(tileVis);
    }

    QPixmap healthPackTexture(":/texture_images/healthpack.png");

    // Add visualization for health packs
    for (const auto &healthPack : healthPacks) {
        if (healthPack->getValue() == 0) continue;
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
        tileVis.healthPackText = std::move(healthPackText);
        tileVis.texturePixmapItem = std::move(healthPackItem);
        tileVisualisations.push_back(tileVis);
    }

    this->drawProtagonist();
}

void GraphicViewController::drawProtagonist() {
    auto protagonist = worldController->getProtagonist();

    if (protagonistPixmapItem == nullptr){
        QPixmap protagonistTexture(":/texture_images/protagonist.png");
        protagonistPixmapItem = new QGraphicsPixmapItem(protagonistTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        protagonistPixmapItem->setZValue(1);
        scene->addItem(protagonistPixmapItem);
    }

    int centerPosX = (rawView->viewport()->width() - 1) / 2;
    int centerPosY = (rawView->viewport()->height() - 1) / 2;

    int protagonistX = protagonist->getXPos() * tileSize;
    int protagonistY = protagonist->getYPos() * tileSize;

    int scrollX = protagonistX - centerPosX;
    int scrollY = protagonistY - centerPosY;

    rawView->horizontalScrollBar()->setValue(scrollX / getRelativeTileSize());
    rawView->verticalScrollBar()->setValue(scrollY / getRelativeTileSize());

    protagonistPixmapItem->setPos(protagonistX, protagonistY);
}

void GraphicViewController::visualizePath(const std::vector<int>& path, const std::shared_ptr<Tile>& startTile){
    if (pathDeletionTimer) {
        pathDeletionTimer->stop();
        delete pathDeletionTimer;
        pathDeletionTimer = nullptr;

        for (const auto &tileVis : previousPath) {
            if (tileVis.graphicsItem) {
                try {
                    scene->removeItem(tileVis.graphicsItem);
                    delete tileVis.graphicsItem;
                } catch (...) {
                }
            }
        }
        previousPath.clear();
    }

    pathDeletionTimer = new QTimer(this);

    connect(pathDeletionTimer, &QTimer::timeout, this, [this]() {
        for (const auto &tileVis : previousPath) {
            if (tileVis.graphicsItem) {
                try {
                    scene->removeItem(tileVis.graphicsItem);
                    delete tileVis.graphicsItem;
                } catch (...) {
                }
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
    const Enemy* enemy = qobject_cast<Enemy*>(sender());
    QPixmap xEnemyTexture(":/texture_images/XEnemy.png");
    QGraphicsPixmapItem *xEnemyItem = new QGraphicsPixmapItem(xEnemyTexture.scaled(tileSize*0.9, tileSize*0.9, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    xEnemyItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
    scene->addItem(xEnemyItem);
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            scene->removeItem(it->texturePixmapItem);
            it->texturePixmapItem = std::move(xEnemyItem);
            break;
        }
        else{
            ++it;
        }
    }
}

void GraphicViewController::handleHalfDead() {
    const Enemy* enemy = qobject_cast<Enemy*>(sender());
    animateSplatter(enemy->getXPos(), enemy->getYPos());
    QPixmap XEnemyHalfTexture(":/texture_images/XEnemy_half.png");
    QGraphicsPixmapItem *halfXEnemyItem = new QGraphicsPixmapItem(XEnemyHalfTexture.scaled(tileSize*0.8, tileSize*0.8, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    halfXEnemyItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
    scene->addItem(halfXEnemyItem);
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            try {
                scene->removeItem(it->texturePixmapItem);
                it->texturePixmapItem = std::move(halfXEnemyItem);
            } catch (...) {
            }
            break;
        }
        else{
            ++it;
        }
    }
}

void GraphicViewController::handleDeath() {
    Enemy* enemy = qobject_cast<Enemy*>(sender());
    animateSplatter(enemy->getXPos(), enemy->getYPos());
    QPixmap tombStoneTexture(":/texture_images/tombstone.png");
    QGraphicsPixmapItem *tombStoneItem = new QGraphicsPixmapItem(tombStoneTexture.scaled(tileSize, tileSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    tombStoneItem->setPos(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize);
    scene->addItem(std::move(tombStoneItem));
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            try {
                scene->removeItem(it->enemyHealthText);
                scene->removeItem(it->texturePixmapItem);
            } catch (...) {
            }
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
                try {
                    scene->removeItem(it->graphicsItem);
                } catch (...) {
                }
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
            animateHealing(it->tile->getXPos(), it->tile->getYPos());
            qCDebug(GraphicViewControllerCategory) << "found healthPackTile";
            try {
                scene->removeItem(it->healthPackText);
                scene->removeItem(it->texturePixmapItem);
            } catch (...) {
            }
            break;
        }
        else{
            ++it;
        }
    }
}

void GraphicViewController::handlePoisonLevelUpdated(float poisonLevel) {
    PEnemy* pEnemy = qobject_cast<PEnemy*>(sender());
    animateSplatter(pEnemy->getXPos(), pEnemy->getYPos());
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

    spreadRadii.insert(pEnemy, spreadRadius + 1);
}

void GraphicViewController::animateSplatter(int xPos,int yPos){
    QLabel *splatterAnimationLabel = new QLabel();
    splatterAnimationLabel->setAttribute( Qt::WA_TranslucentBackground, true );
    splatterAnimationLabel->setMovie(splatterAnimation);
    splatterAnimation->start();

    QGraphicsProxyWidget* splatterProxy = scene->addWidget(std::move(splatterAnimationLabel));

    splatterProxy->setPos((xPos-1.5) * tileSize, (yPos- 1.5) * tileSize );
    splatterProxy->setZValue(2);

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, [splatterProxy]() {
        if (splatterProxy) {
            QGraphicsScene* scene = splatterProxy->scene();
            if (scene) {
                try {
                    scene->removeItem(splatterProxy);
                } catch (...) {
                }
                splatterProxy->deleteLater();
            }
        }
    });

    timer->start(1000);
}

void GraphicViewController::animateHealing(int xPos,int yPos){
    QLabel *healingAnimationLabel = new QLabel();
    healingAnimationLabel->setAttribute( Qt::WA_TranslucentBackground, true );
    healingAnimationLabel->setMovie(healingAnimation);
    healingAnimation->start();

    QGraphicsProxyWidget* healingProxy = scene->addWidget(std::move(healingAnimationLabel));

    healingProxy->setPos((xPos-0.5) * tileSize, (yPos- 0.5) * tileSize );
    healingProxy->setZValue(2);

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, [healingProxy]() {
        if (healingProxy) {
            QGraphicsScene* scene = healingProxy->scene();
            if (scene) {
                scene->removeItem(healingProxy);
                healingProxy->deleteLater();
            }
        }
    });

    timer->start(1000);
}

void GraphicViewController::animateExplosions(){
    qCDebug(GraphicViewControllerCategory) << "animateExplosions";
    for (int i = 0; i < 10; ++i) {
        QTimer::singleShot(i * 500, this, [this]() {
            int xPos = rand() % worldController->getCols();
            int yPos = rand() % worldController->getRows();
            QLabel *explosionAnimationLabel = new QLabel();
            explosionAnimationLabel->setAttribute( Qt::WA_TranslucentBackground, true );
            explosionAnimationLabel->setMovie(explosionAnimation);
            explosionAnimation->start();

            QGraphicsProxyWidget* explosionProxy = scene->addWidget(std::move(explosionAnimationLabel));

            explosionProxy->setPos((xPos - 4) * tileSize, (yPos - 4) * tileSize );
            explosionProxy->setZValue(2);

            QTimer* timer = new QTimer(this);
            timer->setSingleShot(true);

            connect(timer, &QTimer::timeout, this, [explosionProxy]() {
                if (explosionProxy) {
                    QGraphicsScene* scene = explosionProxy->scene();
                    if (scene) {
                        try {
                            scene->removeItem(explosionProxy);
                        } catch (...) {
                        }
                        explosionProxy->deleteLater();
                    }
                }
            });

            timer->start(2000);
        });
    }
}

void GraphicViewController::animateFireworks(){
    qCDebug(GraphicViewControllerCategory) << "animateFireworks";
    for (int i = 0; i < 10; ++i) {
        QTimer::singleShot(i * 500, this, [i, this]() {
            int xPos = rand() % worldController->getCols();
            int yPos = rand() % worldController->getRows();
            QLabel *fireworksAnimationLabel = new QLabel();
            fireworksAnimationLabel->setAttribute( Qt::WA_TranslucentBackground, true );
            if (i % 3 == 0){
                fireworksAnimationLabel->setMovie(fireworksAnimation1);
                fireworksAnimation1->start();
            }
            else if (i % 3 == 1){
                fireworksAnimationLabel->setMovie(fireworksAnimation2);
                fireworksAnimation2->start();
            }
            else{
                fireworksAnimationLabel->setMovie(fireworksAnimation3);
                fireworksAnimation3->start();
            }

            QGraphicsProxyWidget* fireworksProxy = scene->addWidget(std::move(fireworksAnimationLabel));

            fireworksProxy->setPos((xPos - 4) * tileSize, (yPos - 4) * tileSize );
            fireworksProxy->setZValue(2);

            QTimer* timer = new QTimer(this);
            timer->setSingleShot(true);

            connect(timer, &QTimer::timeout, this, [fireworksProxy]() {
                if (fireworksProxy) {
                    QGraphicsScene* scene = fireworksProxy->scene();
                    if (scene) {
                        try {
                            scene->removeItem(fireworksProxy);
                        } catch (...) {
                        }
                        fireworksProxy->deleteLater();
                    }
                }
            });
            timer->start(2000);
        });
    }
}

void GraphicViewController::clearScene() {
    if (scene) {
        QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem* item : items) {
            if (item == protagonistPixmapItem) {
                continue;
            }
            scene->removeItem(item);
        }
    }
}
