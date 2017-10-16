//
// Created by Le on 15.10.2017.
//

#ifndef MABE_ORGANISMSTATE_H
#define MABE_ORGANISMSTATE_H


#include <utility>
#include <vector>

class OrganismState {
public:
    std::vector<int> state;
    int amount;


    OrganismState(std::vector<int> state, int amount) : state(std::move(state)), amount(amount) {

    }
};

#endif //MABE_ORGANISMSTATE_H
