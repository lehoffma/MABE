//
// Created by Le on 15.10.2017.
//

#ifndef MABE_SWARMWORLDSERIALIZER_H
#define MABE_SWARMWORLDSERIALIZER_H


#include "Serializer.h"
#include "../model/OrganismState.h"
#include "../model/WorldLog.h"
#include "../../../Brain/MarkovBrain/MarkovBrain.h"
#include "../model/Agent.h"
#include "OrganismBrain.h"
#include "../model/OrganismStateContainer.h"

class SwarmWorldSerializer : public Serializer {
private:
public:
    void serialize() override;

    template<typename W>
    SwarmWorldSerializer &with(W value);

    SwarmWorldSerializer& withOrganismStates(const std::vector<std::shared_ptr<Agent>> &agents);

    SwarmWorldSerializer &withLocation(std::vector<std::pair<int, int>> locations, int gridX, int gridY);

    SwarmWorldSerializer &withBrains(const std::vector<shared_ptr<Agent>> &agents,
                                     const bool resetOutputs,
                                     int requiredInputs,
                                     int requiredOutputs);

    SwarmWorldSerializer() = default;

    SwarmWorldSerializer &operator=(const SwarmWorldSerializer &serializer) {
        this->serializers = serializer.serializers;
    }
};


#endif //MABE_SWARMWORLDSERIALIZER_H
