#include "textviewcontroller.h"
#include <QTimer>

QLoggingCategory TextViewControllerCategory("textviewcontroller", QtDebugMsg);

void TextViewController::visualizeWorld(){
    const auto& myTiles = worldController->getTiles();
    const auto& myEnemies = worldController->getEnemies();
    const auto& myHealthpacks = worldController->getHealthpacks();
    const auto& portalTile = worldController->getPortalTile();

    // Determine the ASCII representation of different entities
    const QString horizontalBorder = "+---";
    const QString verticalEmptyTile = "|\u00A0\u00A0\u00A0";
    const QString verticalHealthPackTile = "| H ";
    const QString verticalEnemyTile = "| E ";
    const QString verticalProtagonistTile = "| P ";
    const QString verticalPEnemyTile = "| ¶ ";
    const QString verticalXEnemyTile = "| X ";
    const QString verticalPortalTile = "| O ";

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
                [x, y](const auto &hp) { return hp->getXPos() == x && hp->getYPos() == y && hp->getValue() > 0; });

            auto isPEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                [x, y](const auto &enemy) {
                    if (auto pEnemy = std::dynamic_pointer_cast<PEnemy>(enemy)) {
                        return pEnemy->getXPos() == x && pEnemy->getYPos() == y && !pEnemy->getDefeated();
                    }
                    return false;
                });

            auto isXEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                [x, y](const auto &enemy) {
                    if (auto xEnemy = std::dynamic_pointer_cast<XEnemy>(enemy)) {
                        return xEnemy->getXPos() == x && xEnemy->getYPos() == y && !xEnemy->getDefeated();
                    }
                    return false;
                });

            auto isEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                [x, y](const auto &enemy) { return enemy->getXPos() == x && enemy->getYPos() == y && !enemy->getDefeated(); });

            auto isProtagonist = (protagonist->getXPos() == x && protagonist->getYPos() == y);
            auto isPortalTile = (portalTile->getXPos() == x && portalTile->getYPos() == y);


            // Append the corresponding ASCII representation to the overall representation string
            if (isHealthPack != myHealthpacks.end()) {
                initialAsciiRepresentation += verticalHealthPackTile;
            } else if (isPEnemy != myEnemies.end()) {
                initialAsciiRepresentation += verticalPEnemyTile;
            } else if (isXEnemy != myEnemies.end()) {
                initialAsciiRepresentation += verticalXEnemyTile;
            } else if (isEnemy != myEnemies.end()) {
                initialAsciiRepresentation += verticalEnemyTile;
            } else if (isPortalTile){
                initialAsciiRepresentation += verticalPortalTile;
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
    updatedAsciiRepresentationNoProt = initialAsciiRepresentation;
    updatedAsciiRepresentation = initialAsciiRepresentation;

    // Set line wrap mode to NoWrap
    //asciiTextEdit->setLineWrapMode(QTextEdit::NoWrap); // !!! Deze lijn zorgt voor delay wanneer movement !!!

    asciiTextEdit->setPlainText(initialAsciiRepresentation);
}

void TextViewController::drawProtagonist() {
    // Replace the old representation of the protagonist with an empty tile
    updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "\u00A0");

    // Check what was under old position
    if (updatedAsciiRepresentationNoProt.at(oldProtagonistIndex) == 'E'
        || updatedAsciiRepresentationNoProt.at(oldProtagonistIndex) == QChar(0x00B6)
        || updatedAsciiRepresentationNoProt.at(oldProtagonistIndex) == 'X') {
        qCDebug(TextViewControllerCategory) << "Previous position was one of the enemies";
        updatedAsciiRepresentationNoProt.replace(oldProtagonistIndex, 1, "\u00A0");
    } else if (updatedAsciiRepresentationNoProt.at(oldProtagonistIndex) == 'H') {
        qCDebug(TextViewControllerCategory) << "Previous position was H";
        qCDebug(TextViewControllerCategory) << "Health before HP =" << currentHealth;
        if (currentHealth < 100.0) {
            updatedAsciiRepresentationNoProt.replace(oldProtagonistIndex, 1, "\u00A0");
        } else {
            updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "H");
        }
    } else if (initialAsciiRepresentation.at(oldProtagonistIndex) == 'O') {
        updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "O");
    } else {
        qCDebug(TextViewControllerCategory) << "Previous position was empty";
    }

    // Find the index corresponding to the new protagonist's position in the ASCII representation
    newProtagonistIndex = worldController->getCols()*4 + 4*worldController->getProtagonist()->getXPos() + 2*worldController->getCols()*4*worldController->getProtagonist()->getYPos() + 4*worldController->getProtagonist()->getYPos()+4;

    // Check the character at the current position
    QChar currentChar = updatedAsciiRepresentationNoProt.at(newProtagonistIndex);

    // Change the color based on the character at the current position
    if (currentChar == 'E' || currentChar == QChar(0x00B6) || currentChar == 'X') {
        asciiTextEdit->setTextColor(Qt::red);
    } else if (currentChar == 'H') {
        asciiTextEdit->setTextColor(Qt::green);
    } else if (currentChar == 'O') {
        asciiTextEdit->setTextColor(Qt::magenta);
    } else {
        asciiTextEdit->setTextColor(isDarkTheme ? Qt::black : Qt::white);
    }

    // Place the 'P' character at the new position
    updatedAsciiRepresentation.replace(newProtagonistIndex, 1, "P");

    // Display the updated ASCII representation in the QTextEdit
    asciiTextEdit->setPlainText(updatedAsciiRepresentation);

    // Update old with new
    oldProtagonistIndex = newProtagonistIndex;

    // Do not update health when you enter H-tile
    if (initialAsciiRepresentation.at(newProtagonistIndex) != 'H') {
        currentHealth = protagonist->getHealth();
    }

    centerViewAroundProtagonist();
}

void TextViewController::centerViewAroundProtagonist() {
    //    // >>>>>>>>>>>>>>>>>>>> Character dimensions debug >>>>>>>>>>>>>>>>>>>>
    //    // Assuming you have a QTextEdit named "asciiTextEdit"
    //    QFont currentFont = asciiTextEdit->currentFont();

    //    // Get font family and size
    //    QString fontFamily = currentFont.family();
    //    int fontSize = currentFont.pointSize();

    //    // Print or use the font information as needed
    //    qDebug() << "Font Family: " << fontFamily;
    //    qDebug() << "Font Size: " << fontSize;

    //    QFontMetrics fontMetrics(currentFont);
    //    int charWidth = fontMetrics.horizontalAdvance('+');
    //    int charHeight = fontMetrics.height();
    //    qDebug() << "Width of character: " << charWidth;
    //    qDebug() << "Height of characters: " << charHeight;
    //    // <<<<<<<<<<<<<<<<<<<< Character dimensions debug <<<<<<<<<<<<<<<<<<<<

    // Get the dimensions of the visible area within the QTextEdit
    int textviewWidth = asciiTextEdit->viewport()->width();
    int textviewHeight = asciiTextEdit->viewport()->height();

    // Calculate the center position in the QTextEdit representation
    int centerPosX = (textviewWidth - 1) / 2;
    qCDebug(TextViewControllerCategory) << "--------> centerPosX : " << centerPosX;
    int centerPosY = (textviewHeight - 1) / 2;
    qCDebug(TextViewControllerCategory) << "--------> centerPosY : " << centerPosY;

    // Calculate dimensions of character
    QFont currentFont = asciiTextEdit->currentFont();
    QFontMetrics fontMetrics(currentFont);
    int charWidth = fontMetrics.horizontalAdvance('+');
    int charHeight = fontMetrics.height();
    qDebug() << "Width of character: " << charWidth;
    qDebug() << "Height of characters: " << charHeight;

    // Set dimensions of tile
    int tileWidth = charWidth*4;
    int tileHeight = charHeight*2;

    // Calculate the desired scroll positions
    int protagonistX = protagonist->getXPos()*tileWidth;
    qCDebug(TextViewControllerCategory) << "--------> protPosX : " << protagonistX;
    int protagonistY = protagonist->getYPos()*tileHeight;
    qCDebug(TextViewControllerCategory) << "--------> protPosY : " << protagonistY;

    // Calculate the scroll positions to center the view around the protagonist
    int scrollX = protagonistX - centerPosX;
    int scrollY = protagonistY - centerPosY;

    // Ensure the scroll positions are within bounds
    scrollX = qMax(0, scrollX);
    scrollY = qMax(0, scrollY);

    // Set the scroll positions
    asciiTextEdit->horizontalScrollBar()->setValue(scrollX);
    asciiTextEdit->verticalScrollBar()->setValue(scrollY);
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

void TextViewController::animateWinner() {
    qCDebug(TextViewControllerCategory) << "WINNER";
    // Green screen
    asciiTextEdit->setStyleSheet("background-color: green; color: green;");
    asciiTextEdit->setTextColor(Qt::green);

//    // Happy smiley
//    asciiTextEdit->setStyleSheet("background-color: black; color: green;");
//    asciiTextEdit->setTextColor(Qt::green);
//    QString happySmiley = "                   __ooooooooo__";
//    happySmiley += QChar(0x2029);
//    happySmiley += "              oOOOOOOOOOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "          oOOOOOOOOOOOOOOOOOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "       oOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "     oOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "   oOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "  oOOOOOOOOOOO*  *OOOOOOOOOOOOOO*  *OOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += " oOOOOOOOOOOO      OOOOOOOOOOOO      OOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += " oOOOOOOOOOOOOo  oOOOOOOOOOOOOOOo  oOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "oOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "oOOOO     OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO     OOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += "oOOOOOO OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO OOOOOOo";
//    happySmiley += QChar(0x2029);
//    happySmiley += " *OOOOO  OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO  OOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += " *OOOOOO  *OOOOOOOOOOOOOOOOOOOOOOOOOOOOO*  OOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "  *OOOOOO  *OOOOOOOOOOOOOOOOOOOOOOOOOOO*  OOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "   *OOOOOOo  *OOOOOOOOOOOOOOOOOOOOOOO*  oOOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "     *OOOOOOOo  *OOOOOOOOOOOOOOOOO*  oOOOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "       *OOOOOOOOo  *OOOOOOOOOOO*  oOOOOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "          *OOOOOOOOo           oOOOOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "              *OOOOOOOOOOOOOOOOOOOOO*";
//    happySmiley += QChar(0x2029);
//    happySmiley += "                  *ooooooooooooo*";
//    asciiTextEdit->setPlainText(happySmiley);
}

void TextViewController::animateLoser() {
    qCDebug(TextViewControllerCategory) << "DOOD";
    // Red screen
    asciiTextEdit->setStyleSheet("background-color: red; color: red;");
    asciiTextEdit->setTextColor(Qt::red);

//    // Magere Hein
//    asciiTextEdit->setStyleSheet("background-color: black; color: red;");
//    asciiTextEdit->setTextColor(Qt::red);
//    QString sadSmiley = "    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⣤⣤⣤⣤⣤⣤⣤⣴⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⣿";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣿⣿⣿⠿⠿⠟⠿⠿⠿⠛⠿⠿⢿⣿⣿⣿⣷⣽";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⠿⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠿⠋⠁⠀⠀⠀⣷⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⡆";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠘⠁⠀⠀⠀⠀⠀⠀⣿⣿⣿⣾⣿⣷⣶⣶⣤⣄⠀⠀⠀⠀⠀⠀⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡤⠀⠀⠀⠀⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⢹⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⣀⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⣀⣿⣿⢷⡄";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣾⣿⣿⣭⣽⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢽⣶⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⣿⡏⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣶⣿⣿⣿⣿⣿⣿⣿⡆⠈⠙⠛⢿⡿⠟";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢙⣿⣿⣿⣯⣉⣉⣹⢿⢿⣯⣼⣧⠀⠻⣿⣾⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⡿⣸⡎⢿⣿⣿⣆⠀⠀⣿⡇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⡇⣿⡡⢴⣿⣿⣿⡆⠀⣿⠁";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣉⣿⣿⣿⣿⣿⣿⣿⣴⣿";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠀⠀⠀⠀⠀⠀⠀⣀⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣾⣿⣿⣿⣿⣿⣿⣿⣿⡏";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⣤⣤⣤⣤⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇";
//    sadSmiley += QChar(0x2029);
//    sadSmiley += "⠈⠙⠛⠛⠻⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠛⠿⠿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠛⠛⠋⠁";
//    asciiTextEdit->setPlainText(sadSmiley);
}

void TextViewController::handleTextCommand() {
    navigateText = navigateLineEdit->text();

    // Split the command into parts
    QStringList parts = navigateText.split(" ", Qt::SkipEmptyParts);

    // Check if the command has at least one part
    if (!parts.isEmpty()) {
        QString commandPrefix = parts[0].toLower();

        // Find the corresponding handler in the map
        auto handler = commandHandlers.find(commandPrefix);

        // If the handler is found, call it with the remaining parts
        if (handler != commandHandlers.end()) {
            commandCheckVisual(true);
            handler.value()(parts.mid(1));
        } else {
            // Handle unknown command or implement tab completion
            QStringList matchingCommands;
            for (auto iter = commandHandlers.begin(); iter != commandHandlers.end(); ++iter) {
                if (iter.key().startsWith(commandPrefix)) {
                    matchingCommands.append(iter.key());
                }
            }

            if (matchingCommands.isEmpty()) {
                // Handle unknown command
                commandCheckVisual(false);
                handleUnknownCommand();
            } else if (matchingCommands.size() == 1) {
                // If there is only one matching command, auto-complete the command
                completeCommand(matchingCommands.first(), parts);
            }
        }
    }
}

void TextViewController::completeCommand(const QString &completedCommand, const QStringList &parts) {
    commandCheckVisual(true);

    if (completedCommand.toLower() == "goto" && parts.size() == 3) {
        // If the completed command is "goto" and there is one missing parameter, append it
        navigateLineEdit->setText(completedCommand + " " + parts[1] + " " + parts[2]);
    } else {
        navigateLineEdit->setText(completedCommand);
    }

    // Simulate press of the "ENTER" key to execute the auto-completed command
    QTimer::singleShot(0, this, &TextViewController::handleTextCommand);
}

void TextViewController::handleMoveCommand(const QString &direction) {
    qCDebug(TextViewControllerCategory) << direction << " action was triggered";

    // Handling the move command based on the direction
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
        worldController->isPortal();
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

    if (protagonist->getHealth() >= worldController->getMaxEH()) {
        commandMessageLabel->setText("<font color='red'>Protagonist is already at maximum health (100%).</font>");
        return;
    }

    auto nearestHealthPack = worldController->getNearestHealthpack();
    if (nearestHealthPack) {
        commandMessageLabel->setText("Protagonist takes the nearest health pack at coordinates: <b>(" + QString::number(nearestHealthPack->getXPos() + 1) + ", " + QString::number(nearestHealthPack->getYPos() + 1) + ")</b>");

        protagonist->setXPos(nearestHealthPack->getXPos());
        protagonist->setYPos(nearestHealthPack->getYPos());

        // Redraw the protagonist and energy bar
        emit worldController->drawProtagonist();
        emit worldController->drawBars();

        worldController->useHealthpack();
    } else {
        commandMessageLabel->setText("<font color='red'>Uh-oh! The world has run out of health packs. The protagonist is in trouble!</font>");
    }
}

void TextViewController::handleThemeCommand() {
    // Toggle between light and dark themes
    isDarkTheme = !isDarkTheme;

    if (!isDarkTheme) {
        asciiTextEdit->setStyleSheet("background-color: black; color: white;");
        commandMessageLabel->setText("Theme is set to <b>Dark</b>");
    } else {
        asciiTextEdit->setStyleSheet("background-color: white; color: black;");
        commandMessageLabel->setText("Theme is set to <b>Light</b>");
    }

    drawProtagonist();
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

void TextViewController::clearTextWorld() {
    initialAsciiRepresentation.clear();
}
