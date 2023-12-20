#include "XEnemy.h"
#include <iostream>
#include <QTimer>

int XEnemy::halfDeadCount = 0;

XEnemy::XEnemy(int xPosition, int yPosition, float strength)
    : Enemy(xPosition, yPosition, strength), defeated(false), halfDead(false)
{
    timer = new QTimer(this);
}

XEnemy::~XEnemy() {
    delete timer;
}

bool XEnemy::isHalfDead() const {
    return halfDead;
}

void XEnemy::setHalfDead(bool value) {
    halfDead = value;
    if (value) {
        emit halfDeadSet();
        incrementHalfDeadCount();
        std::cout << "starting halfdead timer for 10 seconds" << std::endl;
        timer->singleShot(10000, this, [=, this]() { setHalfDead(false); emit timerExpired(); });
    } else {
        decrementHalfDeadCount();
    }
}

void XEnemy::setDefeated(bool value) {
    defeated = value;
    timer->stop();
    emit dead();
}
