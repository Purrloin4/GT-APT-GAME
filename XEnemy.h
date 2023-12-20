#ifndef XENEMY_H
#define XENEMY_H

#include "world.h"

class XEnemy : public Enemy {

    Q_OBJECT

protected:

    bool halfDead;

public:

    XEnemy(int xPosition, int yPosition, float strength);

    bool isHalfDead() const;
    void setHalfDead(bool value);

};

#endif // XENEMY_H
