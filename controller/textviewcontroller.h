#ifndef TEXTVIEWCONTROLLER_H
#define TEXTVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"

class TextViewController: public ViewController
{
public:
    TextViewController(std::shared_ptr<WorldController> worldController)
        : ViewController(worldController){}
    void visualizeWorld() override;
    QVBoxLayout* getTextLayout(){return textLayout;}
public slots:
    void drawProtagonist() override;
    void handleDeath() override;
    void handlePoisonLevelUpdated(float poisonLevel) override;
    void handleHealthPackTaken(std::shared_ptr<Tile> pack) override;
    void handleMoveButtonClick();
    void handleNavigateButtonClick();
private:
    std::shared_ptr<Protagonist> protagonist = worldController->getProtagonist();
    float currentHealth = protagonist->getHealth();
    const int startProtagonistIndex = worldController->getCols()*4 + 4;
    int oldProtagonistIndex = worldController->getCols()*4 + 4;
    int newProtagonistIndex;
    QString initialAsciiRepresentation;
    QString oldAsciiRepresentation;
    QString updatedAsciiRepresentation;
    QTextEdit* asciiTextEdit;
    QWidget* textViewWidget;
    QVBoxLayout* textLayout;
    QHBoxLayout* moveLayout;
    QHBoxLayout* navigateLayout;
    QLineEdit* moveLineEdit;
    QLineEdit* navigateLineEdit;
    QPushButton* moveButton;
    QPushButton* navigateButton;
    QString moveText;
    QString navigateText;
};

#endif // TEXTVIEWCONTROLLER_H
