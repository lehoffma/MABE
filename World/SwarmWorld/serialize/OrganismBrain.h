//
// Created by Le on 12.12.2017.
//

#ifndef MABE_ORGANISMBRAIN_H
#define MABE_ORGANISMBRAIN_H

#include "../../../Brain/MarkovBrain/MarkovBrain.h"
#include "../../../Organism/Organism.h"

struct OrganismBrain {
    std::shared_ptr<MarkovBrain> brain;
    std::shared_ptr<Organism> organism;

    OrganismBrain(const shared_ptr<MarkovBrain> &brain, const shared_ptr<Organism> &organism) : brain(brain),
                                                                                                organism(organism) {}
};

#endif //MABE_ORGANISMBRAIN_H
