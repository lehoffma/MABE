//
// Created by Le on 09.11.2017.
//

#ifndef MABE_ORGANISMINFO_H
#define MABE_ORGANISMINFO_H


#include <utility>
#include <vector>

class OrganismInfo {
private:
    std::pair<int, int> location;
    double score;
    double waitForGoal;
    int facing;

public:
    const std::pair<int, int> &getLocation() const {
        return location;
    }

    OrganismInfo &setLocation(const std::pair<int, int> &location) {
        this->location.first = location.first;
        this->location.second = location.second;
        return *this;
    }

    double getScore() const {
        return score;
    }

    OrganismInfo &setScore(double score) {
        this->score = score;
        return *this;
    }

    double getWaitForGoal() const {
        return waitForGoal;
    }

    OrganismInfo &setWaitForGoal(double waitForGoal) {
        this->waitForGoal = waitForGoal;
        return *this;
    }

    int getFacing() const {
        return facing;
    }

    OrganismInfo &setFacing(int facing) {
        this->facing = facing;
        return *this;
    }

};


#endif //MABE_ORGANISMINFO_H
