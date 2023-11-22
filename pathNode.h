#ifndef PATHNODE_H
#define PATHNODE_H
#include "world.h"


class PathNode {
public:
    Tile tile;
    float f;
    float g;
    float h;
    bool visited;
    PathNode* prev;

    PathNode(Tile tile)
        :  tile(tile), f(0.0f), g(0.0f), h(0.0f), visited(false), prev(nullptr)  {
    }

    float getValue(){
        return tile.getValue();
    }

    int getXPos() const{
        return tile.getXPos();
    }

    int getYPos() const{
        return tile.getYPos();
    }

    struct Comparator {
        bool operator()(const PathNode& lhs, const PathNode& rhs) const {
            return (lhs.f + lhs.h) > (rhs.f + rhs.h);
        }
    };
};


#endif // PATHNODE_H
