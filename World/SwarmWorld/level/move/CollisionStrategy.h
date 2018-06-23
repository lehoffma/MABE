//
// Created by Le on 16.11.2017.
//

#ifndef MABE_COLLISIONSTRATEGY_H
#define MABE_COLLISIONSTRATEGY_H

template<typename T>
class CollisionStrategy{
public:
    virtual bool hasCollided(const std::shared_ptr<T>& to) = 0;
    virtual void collide(const std::shared_ptr<T>& field) = 0;
};

#endif //MABE_COLLISIONSTRATEGY_H
