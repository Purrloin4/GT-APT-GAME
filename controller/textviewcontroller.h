#ifndef TEXTVIEWCONTROLLER_H
#define TEXTVIEWCONTROLLER_H
#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

class TextViewController: public ViewController
{
public:
    TextViewController(std::shared_ptr<WorldController> worldController)
        : ViewController(worldController){}
public slots:
    void drawProtagonist() override;
    void handleDeath() override;
    void handleHealthPackTaken(int xPos, int yPos) override;
};

#endif // TEXTVIEWCONTROLLER_H
