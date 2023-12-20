#ifndef XENEMY_H
#define XENEMY_H

#include "world.h"

class XEnemy : public Enemy {

    Q_OBJECT

protected:

    bool defeated;
    bool halfDead;
    static int halfDeadCount;
    QTimer *timer;

public:

    XEnemy(int xPosition, int yPosition, float strength);
    ~XEnemy();

    bool isHalfDead() const;
    void setHalfDead(bool value);
    void incrementHalfDeadCount() {halfDeadCount++;};
    void decrementHalfDeadCount() {halfDeadCount--;};
    int getHalfDeadCount() {return halfDeadCount;};
    void setDefeated(bool value);

signals:
    void timerExpired();
    void halfDeadSet();

};

#endif // XENEMY_H
