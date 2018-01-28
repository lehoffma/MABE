//
// Created by Le on 14.01.2018.
//

#ifndef MABE_ORGANISMSTATECONTAINER_H
#define MABE_ORGANISMSTATECONTAINER_H

#include "../../../Organism/Organism.h"
#include "OrganismState.h"

class OrganismStateContainer{
public:
    std::shared_ptr<Organism> organism;
    std::vector<OrganismState> organismStates;

    OrganismStateContainer& setOrganism(const shared_ptr<Organism> &organism) {
        this->organism = organism;
        return *this;
    }

    OrganismStateContainer& setOrganismStates(const vector<OrganismState> &organismStates) {
        this->organismStates = organismStates;
        return *this;
    }
};


#endif //MABE_ORGANISMSTATECONTAINER_H
