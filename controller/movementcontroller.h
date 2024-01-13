#ifndef MOVEMENTCONTROLLER_H
#define MOVEMENTCONTROLLER_H

#include <QObject>
#include <vector>
#include <memory>
#include <QLoggingCategory>
#include "worldcontroller.h"

class MovementController : public QObject {
    Q_OBJECT

signals:
    void posChanged();
public:
    MovementController(std::shared_ptr<WorldController> worldController)
        :  worldController(worldController) {
            protagonist = worldController->getProtagonist();
    }

public slots:
    void moveProtagonistPos(const int & x, const int & y);
    void moveProtagonistPath(const std::vector<int> & path);
private:
    std::shared_ptr<WorldController> worldController;
    std::shared_ptr<Protagonist> protagonist;
    bool isValidPosition(const int & x,const int & y);
};

#endif // MOVEMENTCONTROLLER_H
