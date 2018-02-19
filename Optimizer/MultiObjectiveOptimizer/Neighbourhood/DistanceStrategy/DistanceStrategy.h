//
// Created by Le on 19.02.2018.
//

#ifndef MABE_DISTANCESTRATEGY_H
#define MABE_DISTANCESTRATEGY_H

class DistanceStrategy {
public:
    virtual double distance(int x1, int y1, int x2, int y2) = 0;
};

#endif //MABE_DISTANCESTRATEGY_H
