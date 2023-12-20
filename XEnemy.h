#ifndef XENEMY_H
#define XENEMY_H

#include "world.h"

class XEnemy : public Enemy {

    Q_OBJECT

public:
    XEnemy(int xPosition, int yPosition, float strength);
    ~XEnemy();

};

#endif // XENEMY_H
