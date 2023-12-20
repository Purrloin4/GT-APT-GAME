#include "graphicviewcontroller.h"
#include <iostream>
#include "QLoggingCategory"

QLoggingCategory GraphicViewControllerCategory("graphicViewController", QtDebugMsg);

void GraphicViewController::visualizeWorld()
{
    auto myTiles = worldController->getTiles();
    auto enemies = worldController->getEnemies();

    auto healthPacks = worldController->getHealthpacks();

    auto protagonist = worldController->getProtagonist();

    // Loop through each tile and set its color based on its value
    for (const auto &tile : myTiles) {
        int xPos = tile->getXPos();
        int yPos = tile->getYPos();
        double value = tile->getValue();

        // Determine the color of the tile based on its value
        QColor brush;
        if(std::isinf(value)){
            brush = Qt::black;
        }
        else {
            brush = QColor::fromRgbF(value, value, value);
        }
        scene->addRect(xPos * tileSize, yPos * tileSize, tileSize, tileSize, QPen(Qt::black), brush);
    }

    // Add visualization for enemies
    for (const auto &enemy : enemies) {
        if (auto pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
            // Visualize PEnemy instances in yellow
            scene->addRect(pEnemy->getXPos() * tileSize, pEnemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::yellow));
        } else if (auto xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
            // Visualize PEnemy instances in orange
            scene->addRect(xEnemy->getXPos() * tileSize, xEnemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::orange));
        } else {
            // Visualize regular enemies in red
            scene->addRect(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::red));
        }
        QGraphicsTextItem *healthText = new QGraphicsTextItem(QString::number(enemy->getValue()));
        healthText->setDefaultTextColor(Qt::blue);
        healthText->adjustSize();
        healthText->setPos((enemy->getXPos()-0.5) * tileSize, (enemy->getYPos() + 0.2) * tileSize);
        scene->addItem(healthText);

        //To store the health text with the correct enemy
        TileVisualisation tileVis;
        tileVis.enemy = enemy.get();
        tileVis.enemyHealthText = healthText;
        tileVisualisations.push_back(tileVis);
    }

    // Add visualization for health packs
    for (const auto &healthPack : healthPacks) {
        scene->addRect(healthPack->getXPos() * tileSize, healthPack->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::green));
        QGraphicsTextItem *healthPackText = new QGraphicsTextItem(QString::number(healthPack->getValue()));
        healthPackText->setDefaultTextColor(Qt::blue);
        healthPackText->adjustSize();
        healthPackText->setPos((healthPack->getXPos()-0.5) * tileSize, (healthPack->getYPos() + 0.2) * tileSize);
        scene->addItem(healthPackText);


        //To store the health text with the correct enemy
        TileVisualisation tileVis;
        tileVis.tile = healthPack.get();
        tileVis.healthPackText = healthPackText;
        tileVisualisations.push_back(tileVis);
    }

    this->drawProtagonist();

    graphViewWidget = new QWidget;
    graphLayout = new QVBoxLayout(graphViewWidget);
    graphLayout->addWidget(rawView);
}

void GraphicViewController::drawProtagonist() {
    auto protagonistItem = worldController->getProtagonistItem();
    auto protagonist = worldController->getProtagonist();

    if (protagonistItem) {
        scene->removeItem(*protagonistItem);
    }

    // Add visualization for protagonist
    *protagonistItem = scene->addRect(protagonist->getXPos() * tileSize, protagonist->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(Qt::blue));
}

void GraphicViewController::visualizePath(std::vector<int> path, std::shared_ptr<Tile> startTile){
    for (const auto &tileVis : previousPath) {
        if (tileVis.graphicsItem) {
            scene->removeItem(tileVis.graphicsItem);
            delete tileVis.graphicsItem;
        }
    }
    previousPath.clear();

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

void GraphicViewController::handleDeath() {
    Enemy* enemy = qobject_cast<Enemy*>(sender());
    // Visualization of defeated enemy
    scene->addRect(enemy->getXPos() * tileSize, enemy->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::purple));
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        if (it->enemy==enemy){
            scene->removeItem(it->enemyHealthText);
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
            if (it->enemy == enemy) {
                scene->removeItem(it->graphicsItem);
                it = worldController->poisonedTiles.erase(it);
            } else {
                ++it;
            }
        }
    }
}


void GraphicViewController::handleHealthPackTaken(std::shared_ptr<Tile> pack){
     scene->addRect(pack->getXPos() * tileSize, pack->getYPos() * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(QColorConstants::Svg::purple));
    for (auto it = tileVisualisations.begin(); it != tileVisualisations.end();) {
        //if (it->tile->getXPos() == xPos && it->tile->getYPos() == yPos){
        if(it->tile == pack.get()){
            qCDebug(GraphicViewControllerCategory) << "found healthPackTile";
            scene->removeItem(it->healthPackText);
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
}

void GraphicViewController::zoomOut() {
    rawView->scale(0.9, 0.9);
}
