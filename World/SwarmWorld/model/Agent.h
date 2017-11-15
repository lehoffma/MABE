//
// Created by Le on 09.11.2017.
//

#ifndef MABE_ORGANISMINFO_H
#define MABE_ORGANISMINFO_H


#include <utility>
#include <vector>

class Agent {
private:
    std::pair<int, int> location;
    double score;
    double waitForGoal;
    int facing;

    double waitForGoalInterval;
public:
    Agent(pair<int, int> location, double score, double waitForGoal, int facing, double waitForGoalInterval) :
            location(std::move(location)), score(score), waitForGoal(waitForGoal), facing(facing),
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

    int getFacing() const {
        return facing;
    }

    Agent &setFacing(int facing) {
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
};


#endif //MABE_ORGANISMINFO_H
