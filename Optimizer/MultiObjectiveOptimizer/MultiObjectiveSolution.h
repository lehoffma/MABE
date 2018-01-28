//
// Created by Le on 25.01.2018.
//

#ifndef MABE_MULTIOBJECTIVESOLUTION_H
#define MABE_MULTIOBJECTIVESOLUTION_H

#include "../../Organism/Organism.h"

struct MultiObjectiveSolution {
    std::shared_ptr<Organism> organism;
    double crowdingDistance;
    int rank;

    MultiObjectiveSolution(const shared_ptr<Organism> &organism, double crowdingDistance, int rank) : organism(
            organism), crowdingDistance(crowdingDistance), rank(rank) {}

    /**
     * Partial ordering according to Deb et. al
     * @param lhs
     * @param rhs
     * @return
     */
    friend bool operator>=(const MultiObjectiveSolution &lhs, const MultiObjectiveSolution &rhs) {
        return (lhs.rank < rhs.rank) || ((lhs.rank == rhs.rank) && lhs.crowdingDistance > rhs.crowdingDistance);
    }
};

#endif //MABE_MULTIOBJECTIVESOLUTION_H
