#include "XEnemy.h"

XEnemy::XEnemy(int xPosition, int yPosition, float strength)
    : Enemy(xPosition, yPosition, strength), halfDead(false)
    {}

bool XEnemy::isHalfDead() const {
    return halfDead;
}

void XEnemy::setHalfDead(bool value) {
    halfDead = value;
}
