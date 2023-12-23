#include "movementcontroller.h"

QLoggingCategory MovementControllerCategory("MovementController", QtDebugMsg);

void MovementController::moveProtagonistPos(int x, int y)
{
    auto oldTile = worldController->getTile(protagonist->getXPos(), protagonist->getYPos());
    auto newTile = worldController->getTile(x, y);
    double energyCost = std::abs(oldTile->getValue() - newTile->getValue());
    qCDebug(MovementControllerCategory) << "Currennt energy" << protagonist->getEnergy() << ", enenrgy cost: " << energyCost;
    if (protagonist->getEnergy() >= energyCost) {
        protagonist->setXPos(x);
        protagonist->setYPos(y);
        protagonist->setEnergy(protagonist->getEnergy() - energyCost);
    }
    else {
        qCDebug(MovementControllerCategory) << "Not enough energy to move to tile";
    }
}

void MovementController::moveProtagonistPath(std::vector<int> path, std::shared_ptr<Tile> startTile)
{
    int xPos = protagonist->getXPos();
    int yPos = protagonist->getYPos();
    auto oldTile = startTile;
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
        auto newTile = worldController->getTile(xPos, yPos);
        double energyCost = std::abs(oldTile->getValue() - newTile->getValue());
        if (protagonist->getEnergy() >= energyCost) {
            protagonist->setXPos(xPos);
            protagonist->setYPos(yPos);
            protagonist->setEnergy(protagonist->getEnergy() - energyCost);
        }
        else {
            qCDebug(MovementControllerCategory) << "Not enough energy to move to tile";
            break;
        }
        oldTile = newTile;
    }
}
