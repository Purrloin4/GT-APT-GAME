#ifndef PORTALTILE_H
#define PORTALTILE_H

#include "world.h"

class PortalTile : public Tile {
public:
    PortalTile(int x, int y, QString destinationMap)
        : Tile(x, y, 0), destinationMap(destinationMap), mainMapActive(true) {}
    PortalTile(int x, int y)
        : Tile(x,y,0) {};
    QString getDestinationMap() const { return destinationMap; }
    bool getMainMapActive() { return mainMapActive; }
    void setMainMapActive(bool value) { mainMapActive = value; }

private:
    QString destinationMap;
    bool mainMapActive;
};

#endif // PORTALTILE_H
