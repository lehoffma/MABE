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

class SwarmWorldVisualizer {
public:
    /**
     *
     */
    static void serializeEndResult(const vector<shared_ptr<Organism>> &population, const WorldLog &worldLog,
                                   vector<OrganismStateContainer> &organismStates, int requiredInputs,
                                   int requiredOutputs, SwarmWorldSerializer &serializer) {
        std::cout << "serialize start" << std::endl;
        //todo dont just write the average to the file
        double score = std::accumulate(population.begin(), population.end(), 0,
                                       [](const double acc, const shared_ptr<Organism> val) {
                                           return acc + val->dataMap.getAverage("score");
                                       }) / population.size();
        serializeResult(population, worldLog, organismStates, requiredInputs, requiredOutputs, serializer, score);
        std::cout << "serialize end" << std::endl;
    }

    /**
     *
     */
    static void serializeResult(const vector<shared_ptr<Organism>> &organisms, const WorldLog &worldLog,
                                vector<OrganismStateContainer> &organismStates, int requiredInputs, int requiredOutputs,
                                SwarmWorldSerializer &serializer, double globalScore) {

        // write all tpm's/cm's
        std::vector<std::shared_ptr<OrganismBrain>> brains{};
        for (auto &organism: organisms) {
            shared_ptr<MarkovBrain> brain = dynamic_pointer_cast<MarkovBrain>(organism->brain->makeCopy());
            shared_ptr<OrganismBrain> organismBrain = make_shared<OrganismBrain>(brain, organism);
            brains.push_back(organismBrain);
        }

        //sort states by amount
        for (auto &stateContainer: organismStates) {
            sort(stateContainer.organismStates.begin(), stateContainer.organismStates.end(),
                 [](OrganismState stateA, OrganismState stateB) -> int {
                     return stateA.amount > stateB.amount;
                 });
        }

        serializer
                .with(worldLog)
                .with(globalScore)
                .withOrganismStates(organismStates)
                .withBrains(brains, requiredInputs, requiredOutputs)
                .serialize();
    }


    static void serializeCompleteWorldUpdate(const vector<shared_ptr<Organism>> &population, WorldLog &worldLog,
                                             const vector<shared_ptr<Agent>> &agents,
                                             vector<OrganismStateContainer> &organismStates,
                                             int amountOfCopies, int t) {
        int currentIndex = 0;
        for (const auto &org: population) {
            //todo test
            for (int organismIdx = currentIndex;
                 organismIdx < (currentIndex + amountOfCopies); organismIdx++) {
                organismStates[currentIndex].organism = org;
                serializeWorldUpdate(org, worldLog, agents, organismStates[currentIndex].organismStates,
                                     amountOfCopies, organismIdx, t);
            }
            currentIndex += amountOfCopies;
        }
    }

    /**
     *
     */
    static void serializeWorldUpdate(const shared_ptr<Organism> &org, WorldLog &worldLog,
                                     const vector<shared_ptr<Agent>> &agents, vector<OrganismState> &organismStates,
                                     int copies, int orgIndex, int t) {

        //write organism's data into world log
        worldLog[orgIndex][t]
                .setX(agents[orgIndex]->getLocation().first)
                .setY(agents[orgIndex]->getLocation().second)
                .setFacing(agents[orgIndex]->getFacing())
                .setScore(agents[orgIndex]->getScore());


        //state of the current organism
        vector<double> nodes = dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes;
        vector<int> state(nodes.size());
        transform(nodes.begin(), nodes.end(), state.begin(),
                  [](double nodeValue) { return nodeValue > 0 ? 1 : 0; });

        //search for index of first state of states of other organisms we've added so far that is completely equal
        //to the state data of the organism we're currently analyzing
        long long int index;
        auto iterator = find_if(organismStates.begin(), organismStates.end(),
                                [state](const OrganismState &entry) -> bool {
                                    return entry.state == state;
                                });
        //state is not part of the states => add them to the list
        if (iterator == organismStates.end()) {
            organismStates.push_back(*new OrganismState(state, 1));
        }
            //or just increase frequency count if already part of the list
        else {
            index = std::distance(organismStates.begin(), iterator);
            organismStates[index].amount++;
        }
    }
};

#endif //MABE_SWARMWORLDVISUALIZER_H
