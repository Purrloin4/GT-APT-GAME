#ifndef PORTALTILE_H
#define PORTALTILE_H

#include "world.h"

class PortalTile : public Tile {
public:
    PortalTile(int x, int y, const std::string& destinationMap)
        : Tile(x, y, 0), destinationMap(destinationMap) {}

    const std::string& getDestinationMap() const { return destinationMap; }

private:
    std::string destinationMap;
};

#endif // PORTALTILE_H
