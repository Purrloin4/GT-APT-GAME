#ifndef TEXTVIEWCONTROLLER_H
#define TEXTVIEWCONTROLLER_H

#include "controller/viewcontroller.h"
#include "controller/worldcontroller.h"
#include <functional>
#include <QStringList>
#include <QMap>
#include <QLabel>

class TextViewController: public ViewController
{
public:
    TextViewController(std::shared_ptr<WorldController> worldController)
        : ViewController(worldController)
    {
        // Add available commands and their corresponding handlers to the map
        commandHandlers["goto"] = std::bind(&TextViewController::handleGotoCommand, this);
        commandHandlers["attack"] = std::bind(&TextViewController::handleAttackCommand, this);
        commandHandlers["take"] = std::bind(&TextViewController::handleTakeCommand, this);
        commandHandlers["help"] = std::bind(&TextViewController::handleHelpCommand, this);
    }

    void visualizeWorld() override;
    QVBoxLayout* getTextLayout() {return textLayout;}

public slots:
    void drawProtagonist() override;
    void handleDeath() override;
    void handlePoisonLevelUpdated(float poisonLevel) override;
    void handleHealthPackTaken(std::shared_ptr<Tile> pack) override;
    void handleMoveButtonClick();
    void handleTextCommand();
    void zoomIn() override;
    void zoomOut() override;
private:
    // Map to associate command strings with handlers
    QMap<QString, std::function<void(const QStringList&)>> commandHandlers;

    // Command handling functions
    void handleGotoCommand();
    void handleAttackCommand();
    void handleTakeCommand();
    void handleHelpCommand();
    void handleUnknownCommand();

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
    QLabel* additionalTextLabelNavigate;
};

#endif // TEXTVIEWCONTROLLER_H
