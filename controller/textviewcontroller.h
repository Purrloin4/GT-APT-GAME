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
        commandHandlers["left"] = std::bind(&TextViewController::handleMoveCommand, this, "left");
        commandHandlers["right"] = std::bind(&TextViewController::handleMoveCommand, this, "right");
        commandHandlers["up"] = std::bind(&TextViewController::handleMoveCommand, this, "up");
        commandHandlers["down"] = std::bind(&TextViewController::handleMoveCommand, this, "down");
        commandHandlers["goto"] = std::bind(&TextViewController::handleGotoCommand, this);
        commandHandlers["attack"] = std::bind(&TextViewController::handleAttackCommand, this);
        commandHandlers["take"] = std::bind(&TextViewController::handleTakeCommand, this);
        commandHandlers["theme"] = std::bind(&TextViewController::handleThemeCommand, this);
        commandHandlers["help"] = std::bind(&TextViewController::handleHelpCommand, this);


        // Create a widget to contain the text view
        textViewWidget = new QWidget;
        textLayout = new QVBoxLayout(textViewWidget);

        // Display the ASCII representation in a QTextEdit
        asciiTextEdit = new QTextEdit(initialAsciiRepresentation);
        asciiTextEdit->setFont(QFont("Courier")); // Set a monospaced font for better alignment
        asciiTextEdit->setStyleSheet("background-color: white; color: black;");
        asciiTextEdit->setReadOnly(true);

        // Add the text view to the layout
        textLayout->addWidget(asciiTextEdit);

        // Use a QHBoxLayout to arrange the text box and button horizontally
        navigateLayout = new QHBoxLayout;

        // Add the text box to the layout
        navigateLineEdit = new QLineEdit;
        navigateLineEdit->setPlaceholderText("Enter your command");
        navigateLayout->addWidget(navigateLineEdit);

        // Enter to navigate
        connect(navigateLineEdit, &QLineEdit::returnPressed, this, &TextViewController::handleTextCommand);

        // Create a button
        navigateButton = new QPushButton("ENTER");
        connect(navigateButton, &QPushButton::clicked, this, &TextViewController::handleTextCommand);
        navigateLayout->addWidget(navigateButton);

        textLayout->addLayout(navigateLayout);

        // Add a QLabel for additional text
        commandMessageLabel = new QLabel;
        commandMessageLabel->setTextFormat(Qt::RichText);
        textLayout->addWidget(commandMessageLabel);
    }

    void visualizeWorld() override;
    QVBoxLayout* getTextLayout() {return textLayout;}

public slots:
    void drawProtagonist() override;
    void handleTextCommand();
    void zoomIn() override;
    void zoomOut() override;
    void animateWinner();
    void animateLoser();
private:
    // Map to associate command strings with handlers
    QMap<QString, std::function<void(const QStringList&)>> commandHandlers;
    bool isDarkTheme = true;

    void centerViewAroundProtagonist();

    // Command handling functions
    void handleGotoCommand();
    void handleMoveCommand(const QString& direction);
    void handleAttackCommand();
    void handleTakeCommand();
    void handleThemeCommand();
    void handleHelpCommand();
    void handleUnknownCommand();
    void commandCheckVisual(bool correctCommand);
    void completeCommand(const QString &completedCommand, const QStringList &parts);

    std::shared_ptr<Protagonist> protagonist = worldController->getProtagonist();
    float currentHealth = protagonist->getHealth();
    const int startProtagonistIndex = worldController->getCols()*4 + 4;
    int oldProtagonistIndex = worldController->getCols()*4 + 4;
    int newProtagonistIndex;
    QString initialAsciiRepresentation;
    QString updatedAsciiRepresentationNoProt;
    QString updatedAsciiRepresentation;
    QTextEdit* asciiTextEdit;
    QWidget* textViewWidget;
    QVBoxLayout* textLayout;
    QHBoxLayout* navigateLayout;
    QLineEdit* navigateLineEdit;
    QPushButton* navigateButton;
    QString navigateText;
    QLabel* commandMessageLabel;
};

#endif // TEXTVIEWCONTROLLER_H
