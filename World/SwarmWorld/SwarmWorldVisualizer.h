//
// Created by Le on 14.05.2018.
//

#ifndef MABE_SWARMWORLDVISUALIZER_H
#define MABE_SWARMWORLDVISUALIZER_H

#include "model/OrganismStateContainer.h"
#include "model/WorldLog.h"
#include "model/Agent.h"
#include "serialize/OrganismBrain.h"
#include "serialize/Serializer.h"
#include "serialize/SwarmWorldSerializer.h"
#include "serialize/OrganismSerializer.h"

class SwarmWorldVisualizer {
public:
    /**
     *
     */
    static void serializeEndResult(const vector<shared_ptr<Agent>> &agents, const WorldLog &worldLog,
                                   int requiredInputs, int requiredOutputs,
                                   std::unique_ptr<SwarmWorldSerializer> &serializer) {
        std::cout << "serialize start" << std::endl;
        //todo dont just write the average to the file
        double score = std::accumulate(agents.begin(), agents.end(), 0,
                                       [](const double acc, const shared_ptr<Agent> val) {
                                           return acc + val->getOrganism()->dataMap.getAverage("score");
                                       }) / agents.size();

        serializeResult(agents, worldLog, requiredInputs, requiredOutputs, serializer, score);
        std::cout << "serialize end" << std::endl;
    }

    /**
     *
     */
    static void serializeResult(const vector<shared_ptr<Agent>> &agents, const WorldLog &worldLog,
                                int requiredInputs, int requiredOutputs,
                                std::unique_ptr<SwarmWorldSerializer> &serializer, double globalScore) {
        serializer
                ->with(worldLog)
                .with(globalScore)
                .withOrganismStates(agents)
                .withBrains(agents, requiredInputs, requiredOutputs)
                .serialize();
    }


    static void serializeCompleteWorldUpdate(const vector<shared_ptr<Agent>> &agents, WorldLog &worldLog, int t) {
        int currentIndex = 0;
        for (const auto &agent: agents) {
            serializeWorldUpdate(agent, worldLog, currentIndex, t);
            currentIndex++;
        }
    }

    /**
     *
     */
    static void serializeWorldUpdate(const shared_ptr<Agent> &agent, WorldLog &worldLog, int orgIndex, int t) {

        //write organism's data into world log
        worldLog[orgIndex][t]
                .setX(agent->getLocation().first)
                .setY(agent->getLocation().second)
                .setFacing(agent->getFacing())
                .setScore(agent->getScore());

    }
};

#endif //MABE_SWARMWORLDVISUALIZER_H
