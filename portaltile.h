#ifndef PORTALTILE_H
#define PORTALTILE_H

#include "world.h"

class PortalTile : public Tile {
public:
    PortalTile(int x, int y, QString destinationMap)
        : Tile(x, y, 0), destinationMap(destinationMap) {}

    QString getDestinationMap() const { return destinationMap; }
    bool getMainMapActive() { return mainMapActive; }

private:
    QString destinationMap;
    bool mainMapActive;
};

#endif // PORTALTILE_H
