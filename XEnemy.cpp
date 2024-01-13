#include "XEnemy.h"
#include <iostream>
#include <QTimer>

int XEnemy::halfDeadCount = 0;
int XEnemy::totalEnemies = 0;

XEnemy::XEnemy(int xPosition, int yPosition, float strength)
    : Enemy(xPosition, yPosition, strength), defeated(false), halfDead(false)
{
    timer = new QTimer(this);
    totalEnemies++;
}

XEnemy::~XEnemy() {
    delete timer;
    totalEnemies--;
}

bool XEnemy::isHalfDead() const {
    return halfDead;
}

void XEnemy::setHalfDead(bool value) {
    halfDead = value;
    if (value) {
        emit halfDeadSet();
        incrementHalfDeadCount();
        std::cout << "starting timer for 10 seconds" << std::endl;
        timer->singleShot(10000, this, [=, this]() {
            if (!defeated) {
                setHalfDead(false);
                emit timerExpired();
            }
        });

        if (halfDeadCount == totalEnemies) {
            emit allHalfDead();
        }

    } else {
        decrementHalfDeadCount();
    }
}

void XEnemy::setDefeated() {
    this->Enemy::setDefeated(true);
    timer->stop();
    emit dead();
}
