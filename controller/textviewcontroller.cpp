#include "textviewcontroller.h"

QLoggingCategory TextViewControllerCategory("textviewcontroller", QtDebugMsg);

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

    // Use a QHBoxLayout to arrange the text box and button horizontally
    inputLayout = new QHBoxLayout;

    // Add the text box to the layout
    userInputLineEdit = new QLineEdit;
    inputLayout->addWidget(userInputLineEdit);

    // Create a button
    actionButton = new QPushButton("Perform Action"); // Adjust the button text as needed
    connect(actionButton, &QPushButton::clicked, this, &TextViewController::handleActionButtonClick);
    inputLayout->addWidget(actionButton);

    textLayout->addLayout(inputLayout);
}

void TextViewController::handleActionButtonClick(){
    // Handle the button click event
    // Retrieve text from the QLineEdit and store it in the member variable
    storedText = userInputLineEdit->text();

    // Print the text in the TextBox for debugging
    qCDebug(TextViewControllerCategory) << storedText;
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

void TextViewController::handleHealthPackTaken(std::shared_ptr<Tile> pack){
    //TODO
}

void TextViewController::handlePoisonLevelUpdated(float poisonLevel) {

}
