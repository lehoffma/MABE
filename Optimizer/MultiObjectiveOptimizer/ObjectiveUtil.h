//
// Created by Le on 08.06.2018.
//

#ifndef MABE_OBJECTIVEUTIL_H
#define MABE_OBJECTIVEUTIL_H

#include "ObjectivePoint.h"
#include "../../Utilities/MTree.h"
#include "../../World/SwarmWorld/SwarmWorldParameters.h"
#include "MapUtil.h"

namespace MultiObjective {

    inline ObjectivePoint getRefPoint(const std::unordered_map<std::shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        ObjectivePoint refPoint{};

        for (auto const &objectiveEntry : objectiveMap) {
            const auto objective = objectiveEntry.first->getFormula();
            double referenceValue = 100000;

            //DM_AVE[movementPenalties],DM_AVE[collisions],DM_AVE[gatePassages]
            if (objective == "DM_AVE[movementPenalties]") {
                auto simulationUpdates = SwarmWorldParameters::worldUpdatesPL->get();
                auto invalidMovePenalty = SwarmWorldParameters::invalidMovePenaltyPL->get();

                //the worst would be doing an invalid move in every step of the simulation
                referenceValue = simulationUpdates * invalidMovePenalty * 1.01;
            } else if (objective == "DM_AVE[collisions]") {
                auto simulationUpdates = SwarmWorldParameters::worldUpdatesPL->get();

                //the worst would be colliding in every step of the simulation
                referenceValue = simulationUpdates * 1.01;
            } else if (objective == "DM_AVE[gatePassages]") {
                //the worst value is 0, and we choose a value slightly below that so the volume isn't 0
                referenceValue = -0.1;
            }

            refPoint[objective] = referenceValue;
        }

        return refPoint;
    }

    /**
     * Calculates the distance between two points for the given objective space
     */
    inline double distanceBetween(const ObjectivePoint &pointA,
                                  const ObjectivePoint &pointB,
                                  const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        if (objectiveMap.empty()) {
            return 0;
        }

        double distance = 1;

        for (auto const &objectiveEntry : objectiveMap) {
            auto objective = objectiveEntry.first->getFormula();
            auto valueA = Map::get(pointA, objective);
            auto valueB = Map::get(pointB, objective);

            auto length = std::pow(valueA - valueB, 2);

            distance += length;
        }

        return std::sqrt(distance);
    }
}

#endif //MABE_OBJECTIVEUTIL_H
