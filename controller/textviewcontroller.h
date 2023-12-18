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
    void handleActionButtonClick();
private:
    QString asciiRepresentation;
    QTextEdit* asciiTextEdit;
    QWidget* textViewWidget;
    QVBoxLayout* textLayout;
    QHBoxLayout* inputLayout;
    QLineEdit* userInputLineEdit;
    QPushButton* actionButton;
};

#endif // TEXTVIEWCONTROLLER_H
