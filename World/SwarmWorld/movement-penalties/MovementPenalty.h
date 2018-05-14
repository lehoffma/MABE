//
// Created by Le on 14.05.2018.
//

#ifndef MABE_PENALTY_H
#define MABE_PENALTY_H

#include <functional>
#include "../model/Agent.h"

class MovementPenalty {
protected:
    double penalty = 0;

    bool historyContainsInvalidSequence(const std::vector<RelativeDirection> &directionHistory,
                                        const std::function<bool(RelativeDirection)> &isValid,
                                        int howFarBack) {
        auto historyLength = directionHistory.size();

        if (historyLength < howFarBack) {
            return false;
        }

        bool containsInvalidSequence = true;
        // apply penalty only if the agent didn't move in the last 3 timesteps
        for (size_t i = 0; i < howFarBack; i++) {
            auto historyEntry = directionHistory[historyLength - i - 1];
            if (isValid(historyEntry)) {
                //the agent moved, so this penalty doesn't apply
                containsInvalidSequence = false;
                break;
            }
        }

        return containsInvalidSequence;
    }

public:
    virtual double get(const std::shared_ptr<Agent> &agent) = 0;


    virtual double getBySequence(const std::shared_ptr<Agent> &agent,
                                 const std::function<bool(RelativeDirection)> &isValid,
                                 int howFarBack) {
        auto directionHistory = agent->getDirectionHistory();
        auto containsInvalidSequence = this->historyContainsInvalidSequence(directionHistory, isValid, howFarBack);

        return containsInvalidSequence ? penalty : 0;
    }

    explicit MovementPenalty(double penalty) : penalty(penalty) {

    }
};

#endif //MABE_PENALTY_H
