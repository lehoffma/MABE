//
// Created by Le on 19.02.2018.
//

#ifndef MABE_CHEBYSHEVDISTANCESTRATEGY_H
#define MABE_CHEBYSHEVDISTANCESTRATEGY_H


#include "DistanceStrategy.h"

class ChebyshevDistanceStrategy: public DistanceStrategy{
public:
    double distance(int x1, int y1, int x2, int y2) override;
};


#endif //MABE_CHEBYSHEVDISTANCESTRATEGY_H
