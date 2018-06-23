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
    std::vector<std::pair<int, int>> fieldHistory{};
    std::vector<RelativeDirection> directionHistory{};
    std::unordered_map<std::string, int> states{};
    std::pair<int, int> location;
    double score;
    int gatePassages = 0;
    int collisions = 0;
    double movementPenalties = 0;
    int waitForGoal;
    AbsoluteDirection facing{};

    int waitForGoalInterval;
public:
    explicit Agent(std::shared_ptr<Organism> organism, std::pair<int, int> location, double score,
                   int waitForGoal, AbsoluteDirection facing, int waitForGoalInterval) :
            organism(std::move(organism)), location(std::move(location)), score(score), waitForGoal(waitForGoal),
            facing(facing),
            waitForGoalInterval(waitForGoalInterval) {

    }

    const std::pair<int, int> &getLocation() const {
        return location;
    }

    Agent &setLocation(const std::pair<int, int> &location) {
        this->location.first = location.first;
        this->location.second = location.second;
        this->fieldHistory.push_back(location);
        return *this;
    }

    const std::vector<std::pair<int, int>> &getHistory() {
        return this->fieldHistory;
    };

    double getScore() const {
        return score;
    }

    Agent &setScore(double score) {
        this->score = score;
        return *this;
    }

    int getWaitForGoal() const {
        return waitForGoal;
    }

    Agent &setWaitForGoal(int waitForGoal) {
        this->waitForGoal = waitForGoal;
        return *this;
    }

    void decrementWaitForGoal() {
        if (this->waitForGoal > 0) {
            this->waitForGoal--;
        }
    }

    AbsoluteDirection getFacing() const {
        return facing;
    }

    Agent &setFacing(AbsoluteDirection facing) {
        this->facing = facing;
        return *this;
    }

    int getWaitForGoalInterval() const {
        return waitForGoalInterval;
    }


    shared_ptr<Organism> getOrganism() const {
        return organism;
    }

    Agent &setOrganism(shared_ptr<Organism> organism) {
        this->organism = std::move(organism);
        return *this;
    }

    int getGatePassages() const {
        return gatePassages;
    }

    Agent &setGatePassages(int gatePassages) {
        this->gatePassages = gatePassages;
        return *this;
    }

    int getCollisions() const {
        return collisions;
    }

    Agent &setCollisions(int collisions) {
        this->collisions = collisions;
        return *this;
    }

    double getMovementPenalties() const {
        return movementPenalties;
    }

    Agent &setMovementPenalties(double movementPenalties) {
        this->movementPenalties = movementPenalties;
        return *this;
    }

    const vector<RelativeDirection> &getDirectionHistory() const {
        return directionHistory;
    }

    Agent &setDirectionHistory(const vector<RelativeDirection> &directionHistory) {
        this->directionHistory = directionHistory;
        return *this;
    }

    Agent &addToDirectionHistory(RelativeDirection direction) {
        this->directionHistory.emplace_back(direction);
        this->organism->dataMap.append("directionHistory", direction);
        return *this;
    }

    std::unordered_map<std::string, int>& getStates(){
        return this->states;
    }

    Agent& setStates(const std::unordered_map<std::string, int>& states){
        this->states = states;
        return *this;
    }
};


#endif //MABE_ORGANISMINFO_H
