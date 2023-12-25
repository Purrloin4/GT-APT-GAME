#include "textviewcontroller.h"
#include <QTimer>

QLoggingCategory TextViewControllerCategory("textviewcontroller", QtDebugMsg);

void TextViewController::visualizeWorld(){
    auto myTiles = worldController->getTiles();
    auto myEnemies = worldController->getEnemies();
    auto myHealthpacks = worldController->getHealthpacks();

    // Determine the ASCII representation of different entities
    const QString horizontalBorder = "+---";
    const QString verticalEmptyTile = "|\u00A0\u00A0\u00A0";
    const QString verticalHealthPackTile = "| H ";
    const QString verticalEnemyTile = "| E ";
    const QString verticalProtagonistTile = "| P ";
    const QString verticalPEnemyTile = "| ¶ ";
    const QString verticalXEnemyTile = "| X ";

    // Loop through each row
    for (int y = 0; y < worldController->getRows(); ++y) {
        // Add the horizontal border for each tile in the row
        for (int x = 0; x < worldController->getCols(); ++x) {
            initialAsciiRepresentation += horizontalBorder;
        }
        initialAsciiRepresentation += "+"; // Add last '+' of every row
        initialAsciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'

        // Loop through each column
        for (int x = 0; x < worldController->getCols(); ++x) {
            // Check if the current position contains an entity (health pack, enemy, protagonist)
            auto isHealthPack = std::find_if(myHealthpacks.begin(), myHealthpacks.end(),
                                             [x, y](const auto &hp) { return hp->getXPos() == x && hp->getYPos() == y; });
            auto isPEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                                         [x, y](const auto &enemy) {
                                             if (auto pEnemy = std::dynamic_pointer_cast<PEnemy>(enemy)) {
                                                 return pEnemy->getXPos() == x && pEnemy->getYPos() == y;
                                             }
                                             return false;
                                         });
            auto isXEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                                         [x, y](const auto &enemy) {
                                             if (auto xEnemy = std::dynamic_pointer_cast<XEnemy>(enemy)) {
                                                 return xEnemy->getXPos() == x && xEnemy->getYPos() == y;
                                             }
                                             return false;
                                         });
            auto isEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                                        [x, y](const auto &enemy) { return enemy->getXPos() == x && enemy->getYPos() == y; });
            auto isProtagonist = (protagonist->getXPos() == x && protagonist->getYPos() == y);

            // Append the corresponding ASCII representation to the overall representation string
            if (isHealthPack != myHealthpacks.end()) {
                initialAsciiRepresentation += verticalHealthPackTile;
            } else if (isPEnemy != myEnemies.end()) {
                initialAsciiRepresentation += verticalPEnemyTile;
            } else if (isXEnemy != myEnemies.end()) {
                initialAsciiRepresentation += verticalXEnemyTile;
            } else if (isEnemy != myEnemies.end()) {
                initialAsciiRepresentation += verticalEnemyTile;
            } else if (isProtagonist) {
                initialAsciiRepresentation += verticalProtagonistTile;
            } else {
                initialAsciiRepresentation += verticalEmptyTile; // Spaces for empty tile
            }
        }
        initialAsciiRepresentation += "|"; // Add last '|' of every row
        initialAsciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'
    }

    // Add the horizontal border after the last row
    for (int x = 0; x < worldController->getCols(); ++x) {
        initialAsciiRepresentation += horizontalBorder;
    }
    initialAsciiRepresentation += "+"; // Add last '+' of map

    // Set ascii to other representations
    updatedAsciiRepresentation = initialAsciiRepresentation;
    oldAsciiRepresentation = initialAsciiRepresentation;

    // Create a widget to contain the text view
    textViewWidget = new QWidget;
    textLayout = new QVBoxLayout(textViewWidget);

    // Display the ASCII representation in a QTextEdit
    asciiTextEdit = new QTextEdit(initialAsciiRepresentation);
    asciiTextEdit->setFont(QFont("Courier")); // Set a monospaced font for better alignment
    asciiTextEdit->setReadOnly(true);

    // Set line wrap mode to NoWrap
    //asciiTextEdit->setLineWrapMode(QTextEdit::NoWrap); // Deze lijn zorgt voor delay wanneer movement

    asciiTextEdit->setPlainText(initialAsciiRepresentation);

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

void TextViewController::drawProtagonist() {
    // Replace the old representation of the protagonist with an empty tile
    updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "\u00A0");

    // Check what was under old position
    if (initialAsciiRepresentation.at(oldProtagonistIndex) == 'E') {
        qCDebug(TextViewControllerCategory) << "Previous position was E";
        updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "\u00A0");
        //updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "\u0332E"); // Underlined 'E'
    } else if (initialAsciiRepresentation.at(oldProtagonistIndex) == 'H') {
        qCDebug(TextViewControllerCategory) << "Previous position was H";
        qCDebug(TextViewControllerCategory) << "Health before HP =" << currentHealth;
        if (currentHealth < 100.0) {
            updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "\u00A0");
            //updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "\u0332H");  // Underlined 'H'
        } else {
            updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "H");
        }
    } else {
        qCDebug(TextViewControllerCategory) << "Previous position was empty";
    }

    // Find the index corresponding to the new protagonist's position in the ASCII representation
    newProtagonistIndex = worldController->getCols()*4 + 4*worldController->getProtagonist()->getXPos() + 2*worldController->getCols()*4*worldController->getProtagonist()->getYPos() + 4*worldController->getProtagonist()->getYPos()+4;

    // Place the 'P' character at the new position
    updatedAsciiRepresentation.replace(newProtagonistIndex, 1, "P");

    // Display the updated ASCII representation in the QTextEdit
    asciiTextEdit->setPlainText(updatedAsciiRepresentation);

    // Update old with new
    oldProtagonistIndex = newProtagonistIndex;
    oldAsciiRepresentation = updatedAsciiRepresentation;

    // Do not update health when you enter H-tile
    if (initialAsciiRepresentation.at(newProtagonistIndex) != 'H') {
        currentHealth = protagonist->getHealth();
    }
}

void TextViewController::zoomIn() {
    QFont currentFont = asciiTextEdit->font();
    currentFont.setPointSize(currentFont.pointSize() + 1);
    asciiTextEdit->setFont(currentFont);

}

void TextViewController::zoomOut() {
    QFont currentFont = asciiTextEdit->font();
    currentFont.setPointSize(currentFont.pointSize() - 1);
    asciiTextEdit->setFont(currentFont);
}

void TextViewController::handleTextCommand() {
    navigateText = navigateLineEdit->text();

    // Split the command into parts
    QStringList parts = navigateText.split(" ", Qt::SkipEmptyParts);

    // Check if the command has at least one part
    if (!parts.isEmpty()) {
        // Find the corresponding handler in the map
        auto handler = commandHandlers.find(parts[0].toLower());

        // If the handler is found, call it with the remaining parts
        if (handler != commandHandlers.end()) {
            commandCheckVisual(true);
            handler.value()(parts.mid(1));
        } else {
            // Handle unknown command
            commandCheckVisual(false);
            handleUnknownCommand();
        }
    }
}

void TextViewController::handleMoveCommand(const QString &direction) {
    qCDebug(TextViewControllerCategory) << direction << " action was triggered";

    // Your logic for handling the move command based on the direction
    int newX = protagonist->getXPos();
    int newY = protagonist->getYPos();

    if (direction == "left") {
        newX = protagonist->getXPos() - 1;
    } else if (direction == "right") {
        newX = protagonist->getXPos() + 1;
    } else if (direction == "up") {
        newY = protagonist->getYPos() - 1;
    } else if (direction == "down") {
        newY = protagonist->getYPos() + 1;
    }

    // Check if the new position is within the boundaries of the world
    if(newX >= 0 && newX < worldController->getCols() && newY >= 0 && newY < worldController->getRows()) {
        // Update the protagonist's position only if it's a valid position
        protagonist->setXPos(newX);
        protagonist->setYPos(newY);

        // Redraw the protagonist and energy bar
        emit worldController->drawProtagonist();
        emit worldController->drawBars();

        // Check if you can attack an enemy or use a healthpack
        worldController->attackEnemy();
        worldController->useHealthpack();
    }

    commandMessageLabel->setText("Protagonist moved <b>" + direction + "</b>");
}

void TextViewController::handleGotoCommand() {
    qCDebug(TextViewControllerCategory) << "Goto command triggered!";

    // Retrieve text from the QLineEdit and store it in the member variable
    navigateText = navigateLineEdit->text();

    // Print the text in the TextBox for debugging
    qCDebug(TextViewControllerCategory) << navigateText;

    // Split the string into a QStringList using the space as a delimiter
    QStringList values = navigateText.split(' ');

    // Check if there are exactly three parts (including "goto")
    if (values.size() == 3 && values[0].toLower() == "goto") {
        // Convert the second and third parts to integers
        int x = values[1].toInt();
        int y = values[2].toInt();

        qCDebug(TextViewControllerCategory) << x << "---> x value";
        qCDebug(TextViewControllerCategory) << y << "---> y value";
        // Check if the command is correct (add your specific conditions here)
        if (worldController->isValidPosition(x - 1, y - 1)) {
            worldController->handleMousePressEvent(x - 1, y - 1);

            commandMessageLabel->setText("Protagonist moved to <b>(" + QString::number(x) + "," + QString::number(y) + ")</b>");
        }
        else {
            commandMessageLabel->setText("<font color='red'>Error: Cannot go to the specified position. Coordinates <b>(" + QString::number(x) + "," + QString::number(y) + ")</b> are outside the world boundaries.<br>"
                                                 "World boundaries: X [1, " + QString::number(worldController->getCols()) + "], Y [1, " + QString::number(worldController->getRows()) + "]</font>");

        }
    }
    else {
        commandMessageLabel->setText("<font color='red'>Error: Invalid coordinates after 'goto' command. Use 'goto x y'.</font>");
    }
}

void TextViewController::handleAttackCommand() {
    qCDebug(TextViewControllerCategory) << "Attack nearest enemy command triggered";

    auto enemy = worldController->getNearestEnemy();
    protagonist->setXPos(enemy->getXPos());
    protagonist->setYPos(enemy->getYPos());

    // Redraw the protagonist and energy bar
    emit worldController->drawProtagonist();
    emit worldController->drawBars();

    // Check if you can attack an enemy
    worldController->attackEnemy();

    commandMessageLabel->setText("Protagonist attacks the nearest enemy at coordinates: <b>(" + QString::number(protagonist->getXPos() + 1) + ", " + QString::number(protagonist->getYPos() + 1) + ")</b>");
}

void TextViewController::handleTakeCommand() {
    qCDebug(TextViewControllerCategory) << "Take nearest health pack command triggered";

    if(protagonist->getHealth() < 100.0) {
        commandMessageLabel->setText("Protagonist takes the nearest health pack at coordinates: <b>(" + QString::number(protagonist->getXPos() + 1) + ", " + QString::number(protagonist->getYPos() + 1) + ")</b>");
    }
    else {
        commandMessageLabel->setText("Protagonist is already at maximum health (100%). Coordinates: <b>(" + QString::number(protagonist->getXPos()) + ", " + QString::number(protagonist->getYPos()) + ")</b>");
    }

    auto pack = worldController->getNearestHealthpack();
    protagonist->setXPos(pack->getXPos());
    protagonist->setYPos(pack->getYPos());

    // Redraw the protagonist and energy bar
    emit worldController->drawProtagonist();
    emit worldController->drawBars();

    worldController->useHealthpack();
}

void TextViewController::handleHelpCommand() {
    qCDebug(TextViewControllerCategory) << "Help command triggered";

    // Update the additional text in the QLabel
    commandMessageLabel->setText("Available commands:");

    // Append each command to the QLabel
    for (const auto& command : commandHandlers.keys()) {
        commandMessageLabel->setText(commandMessageLabel->text() + "\n  - " + command);
    }
}

void TextViewController::handleUnknownCommand() {
    qCDebug(TextViewControllerCategory) << "Unknown command";

    // Retrieve text from the QLineEdit and store it in the member variable
    navigateText = navigateLineEdit->text();
    commandMessageLabel->setText("<font color='red'>Error: Unknown command: <b>" + navigateText + "</b><br>Enter \"help\" to see available commands!</font>");
}

void TextViewController::commandCheckVisual(bool correctCommand) {
    // Change the background color of the button temporarily
    if (correctCommand) {
        // Green color for correct command
        navigateLineEdit->setStyleSheet("background-color: lightgreen;");
    } else {
        // Red color for incorrect command
        navigateLineEdit->setStyleSheet("background-color: lightcoral;");
    }

    // Set up a QTimer to revert the color after a short delay (e.g., 500 milliseconds)
    QTimer::singleShot(250, this, [this]() {
        // Restore the default background color
        navigateLineEdit->setStyleSheet("");
        navigateLineEdit->setText("");
    });
}
