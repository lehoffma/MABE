//
// Created by Le on 16.11.2017.
//

#ifndef MABE_COLLISIONSTRATEGY_H
#define MABE_COLLISIONSTRATEGY_H

template<typename T>
class CollisionStrategy{
public:
    virtual bool hasCollided(T& to) = 0;
    virtual void collide(T& field) = 0;
};

#endif //MABE_COLLISIONSTRATEGY_H
