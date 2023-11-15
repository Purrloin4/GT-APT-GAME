#ifndef PATHNODE_H
#define PATHNODE_H
#include "world.h"


template <typename T>
class PathNode {
public:
    Tile tile;
    float f;
    float g;
    float h;
    bool visited;
    T* prev;

    PathNode(Tile tile)
        :  tile(tile), f(0.0f), g(0.0f), h(0.0f), visited(false), prev(nullptr)  {
    }
};


#endif // PATHNODE_H
