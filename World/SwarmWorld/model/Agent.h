//
// Created by Le on 09.11.2017.
//

#ifndef MABE_ORGANISMINFO_H
#define MABE_ORGANISMINFO_H


#include <utility>
#include <vector>
#include "../../../Organism/Organism.h"
#include "./Direction.h"

class Agent {
private:
    std::shared_ptr<Organism> organism;
    std::pair<int, int> location;
    std::vector<double> previousState{};
    double score;
    double waitForGoal;
    AbsoluteDirection facing{};

    double waitForGoalInterval;
public:
    explicit Agent(std::shared_ptr<Organism> organism, std::pair<int, int> location, double score,
                   double waitForGoal, AbsoluteDirection facing, double waitForGoalInterval) :
            organism(organism), location(std::move(location)), score(score), waitForGoal(waitForGoal), facing(facing),
            waitForGoalInterval(waitForGoalInterval) {

    }

    const std::pair<int, int> &getLocation() const {
        return location;
    }

    Agent &setLocation(const std::pair<int, int> &location) {
        this->location.first = location.first;
        this->location.second = location.second;
        return *this;
    }

    double getScore() const {
        return score;
    }

    Agent &setScore(double score) {
        this->score = score;
        return *this;
    }

    double getWaitForGoal() const {
        return waitForGoal;
    }

    Agent &setWaitForGoal(double waitForGoal) {
        this->waitForGoal = waitForGoal;
        return *this;
    }

    AbsoluteDirection getFacing() const {
        return facing;
    }

    Agent &setFacing(AbsoluteDirection facing) {
        this->facing = facing;
        return *this;
    }

    double getWaitForGoalInterval() const {
        return waitForGoalInterval;
    }

    Agent &setWaitForGoalInterval(double waitForGoalInterval) {
        this->waitForGoalInterval = waitForGoalInterval;
        return *this;
    }

    shared_ptr<Organism> getOrganism() const {
        return organism;
    }

    Agent &setWaitForGoalInterval(shared_ptr<Organism> organism) {
        this->organism = organism;
        return *this;
    }

    std::vector<double> getPreviousState() const {
        return previousState;
    }

    Agent &setPreviousState(std::vector<double> previousState) {
        this->previousState = previousState;
        return *this;
    }
};


#endif //MABE_ORGANISMINFO_H
