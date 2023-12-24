#ifndef MOVEMENTCONTROLLER_H
#define MOVEMENTCONTROLLER_H

#include <QObject>
#include <vector>
#include <memory>
#include <QLoggingCategory>
#include "worldcontroller.h"

class MovementController : public QObject {
    Q_OBJECT


public:
    MovementController(std::shared_ptr<WorldController> worldController)
        :  worldController(worldController) {
            protagonist = worldController->getProtagonist();
    }

public slots:
    void moveProtagonistPos(int x, int y);
    void moveProtagonistPath(std::vector<int> path);
private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<Protagonist> protagonist;
};

#endif // MOVEMENTCONTROLLER_H
