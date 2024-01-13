#include "movementcontroller.h"

QLoggingCategory MovementControllerCategory("MovementController", QtDebugMsg);

void MovementController::moveProtagonistPos(const int & x, const int & y)
{
    if (!isValidPosition(x, y)) {
        qCDebug(MovementControllerCategory) << "Invalid position";
        return;
    }

    auto oldTile = worldController->getTile(protagonist->getXPos(), protagonist->getYPos());
    auto newTile = worldController->getTile(x, y);
    float energyCost = std::abs(oldTile->getValue() - newTile->getValue());
    float fixedCost= 20.0/worldController->getRows();
    energyCost = energyCost+fixedCost;
    //qCDebug(MovementControllerCategory) << "Current energy" << protagonist->getEnergy() << ", energy cost: " << energyCost << fixedCost;
    if (protagonist->getEnergy() >= energyCost) {
        protagonist->setXPos(x);
        protagonist->setYPos(y);
        protagonist->setEnergy(protagonist->getEnergy() - energyCost);
        emit posChanged();
    }
    else {
        qCDebug(MovementControllerCategory) << "Not enough energy to move to tile";
    }
}

void MovementController::moveProtagonistPath(const std::vector<int> & path)
{
    int xPos = protagonist->getXPos();
    int yPos = protagonist->getYPos();
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
        moveProtagonistPos(xPos,yPos);
    }
}

bool MovementController::isValidPosition(const int & x, const int & y) {
    return x >= 0 && x < worldController->getCols() && y >= 0 && y < worldController->getRows();
}
