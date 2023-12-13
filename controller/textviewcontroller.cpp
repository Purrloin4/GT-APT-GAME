#include "textviewcontroller.h"

void TextViewController::visualizeWorld(){
    auto myTiles = worldController->getTiles();
    auto myEnemies = worldController->getEnemies();
    auto myHealthpacks = worldController->getHealthpacks();
    auto protagonist = worldController->getProtagonist();

    // Determine the ASCII representation of different entities
    const QString horizontalBorder = "+---";
    const QString verticalEmptyTile = "|\u00A0\u00A0\u00A0";
    const QString verticalHealthPackTile = "| H ";
    const QString verticalEnemyTile = "| E ";
    const QString verticalProtagonistTile = "| P ";

    // Loop through each row
    for (int y = 0; y < worldController->getRows(); ++y) {
        // Add the horizontal border for each tile in the row
        for (int x = 0; x < worldController->getCols(); ++x) {
            asciiRepresentation += horizontalBorder;
        }
        asciiRepresentation += "+"; // Add last '+' of every row
        asciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'

        // Loop through each column
        for (int x = 0; x < worldController->getCols(); ++x) {
            // Check if the current position contains an entity (health pack, enemy, protagonist)
            auto isHealthPack = std::find_if(myHealthpacks.begin(), myHealthpacks.end(),
                                             [x, y](const auto &hp) { return hp->getXPos() == x && hp->getYPos() == y; });
            auto isEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                                        [x, y](const auto &enemy) { return enemy->getXPos() == x && enemy->getYPos() == y; });
            auto isProtagonist = (protagonist->getXPos() == x && protagonist->getYPos() == y);

            // Append the corresponding ASCII representation to the overall representation string
            if (isHealthPack != myHealthpacks.end()) {
                asciiRepresentation += verticalHealthPackTile;
            } else if (isEnemy != myEnemies.end()) {
                asciiRepresentation += verticalEnemyTile;
            } else if (isProtagonist) {
                asciiRepresentation += verticalProtagonistTile;
            } else {
                asciiRepresentation += verticalEmptyTile; // Spaces for empty tile
            }
        }
        asciiRepresentation += "|"; // Add last '|' of every row
        asciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'
    }

    // Add the horizontal border after the last row
    for (int x = 0; x < worldController->getCols(); ++x) {
        asciiRepresentation += horizontalBorder;
    }
    asciiRepresentation += "+"; // Add last '+' of map

    // Create a widget to contain the text view
    textViewWidget = new QWidget;
    textLayout = new QVBoxLayout(textViewWidget);

    // Display the ASCII representation in a QTextEdit
    asciiTextEdit = new QTextEdit(asciiRepresentation);
    asciiTextEdit->setFont(QFont("Courier")); // Set a monospaced font for better alignment

    // Set line wrap mode to NoWrap
    //asciiTextEdit->setLineWrapMode(QTextEdit::NoWrap);

    asciiTextEdit->setPlainText(asciiRepresentation);

    // Add the text view to the layout
    textLayout->addWidget(asciiTextEdit);

    // Set the widget as the content of the second tab
    //tabWidget->widget(1)->setLayout(textLayout);

}

void TextViewController::drawProtagonist() {
    // Update ASCII representation for Text view
    QString updatedAsciiRepresentation = asciiRepresentation;

    // Find the index corresponding to the old protagonist's position in the ASCII representation
    // >>>>>>>> onderstaande lijn is nog ni volledig juist >>>>>>>>>>>
    int oldProtagonistIndex = worldController->getCols()*4 + 4*worldController->getProtagonist()->getXPos() + 4*(worldController->getProtagonist()->getYPos()-1) + 2*worldController->getCols()*4*(worldController->getProtagonist()->getYPos()-1);// + 3*myWorld.getCols()*protagonist.getYPos();

    // Replace the old representation of the protagonist with an empty tile
    updatedAsciiRepresentation.replace(oldProtagonistIndex, 1, "X"); // "X" moet nog "\u00A0" worden, maar makkelijker om debuggen voor nu

    // Find the index corresponding to the new protagonist's position in the ASCII representation
    int newProtagonistIndex = worldController->getCols()*4 + 4*worldController->getProtagonist()->getXPos() + 2*worldController->getCols()*4*worldController->getProtagonist()->getYPos() + 4*worldController->getProtagonist()->getYPos()+4;

    // Replace the empty tile with the representation of the protagonist
    updatedAsciiRepresentation.replace(newProtagonistIndex, 1, "P");

    // Display the updated ASCII representation in the QTextEdit
    asciiTextEdit->setPlainText(updatedAsciiRepresentation);

}

void TextViewController::handleDeath(){
    //TODO
}

void TextViewController::handleHealthPackTaken(int xPos, int yPos){
    //TODO
}

void TextViewController::handlePoisonLevelUpdated(float poisonLevel) {

}
