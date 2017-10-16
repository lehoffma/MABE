//
// Created by Le on 15.10.2017.
//

#ifndef MABE_SWARMWORLDSERIALIZER_H
#define MABE_SWARMWORLDSERIALIZER_H


#include "Serializer.h"
#include "../model/OrganismState.h"
#include "../model/WorldLog.h"

class SwarmWorldSerializer : public Serializer {
public:
    std::vector<OrganismState> &organismStates;
    WorldLog &worldLog;
    double globalScore{};

    void serialize() override;

    template<typename T>
    SwarmWorldSerializer& with(T value);

    SwarmWorldSerializer():worldLog(*new WorldLog()), organismStates(*new std::vector<OrganismState>()){
    }

    SwarmWorldSerializer& operator=(const SwarmWorldSerializer& serializer){
        this->organismStates = serializer.organismStates;
        this->worldLog = serializer.worldLog;
        this->globalScore = serializer.globalScore;
    }
};


#endif //MABE_SWARMWORLDSERIALIZER_H
