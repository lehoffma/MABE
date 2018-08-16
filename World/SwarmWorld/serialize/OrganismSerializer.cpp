//
// Created by Le on 20.06.2018.
//

#include "OrganismSerializer.h"

void OrganismSerializer::addBrainToDataMap(const std::shared_ptr<AbstractBrain> &brain, DataMap &dataMap,
                                           const bool resetOutputs,
                                           int requiredInputs, int requiredOutputs) {
    auto markovBrain = std::dynamic_pointer_cast<MarkovBrain>(brain->makeCopy());

    std::string serializedCM = std::string("\"[")
            .append(OrganismSerializer::serializeConnectivityMatrix(markovBrain, requiredInputs,
                                                                    requiredOutputs, ";", "|"))
            .append("]\"");

    std::string serializedTpm = std::string("\"[")
            .append(OrganismSerializer::serializeTransitionProbabilityMatrix(markovBrain, resetOutputs, ";", "|"))
            .append("]\"");

    dataMap.set("CM", serializedCM);
    dataMap.set("TPM", serializedTpm);
    dataMap.setOutputBehavior("CM", DataMap::FIRST);
    dataMap.setOutputBehavior("TPM", DataMap::FIRST);
}


void OrganismSerializer::addBrainToDataMap(const std::shared_ptr<Organism> &organism,
                                           const bool resetOutputs,
                                           int requiredInputs,
                                           int requiredOutputs) {
    OrganismSerializer::addBrainToDataMap(organism->brain, organism->dataMap, resetOutputs, requiredInputs,
                                          requiredOutputs);
}

void OrganismSerializer::addStatesToDataMap(const std::shared_ptr<Organism> &organism,
                                            const std::unordered_map<std::string, int> &organismStates) {

    std::vector<std::pair<std::string, int>> statesCopy{};

    for (const auto &state: organismStates) {
        statesCopy.emplace_back(std::pair<std::string, int>{state.first, state.second});
    }

    //sort by amount
    std::sort(statesCopy.begin(), statesCopy.end(),
              [](std::pair<std::string, int> stateA, std::pair<std::string, int> stateB) -> int {
                  return stateA.second > stateB.second;
              });

    std::vector<std::string> serializedStates{};
    for (auto entry: statesCopy) {
        auto state = entry.first;
        serializedStates.emplace_back(state);
    }

    auto csvStates = std::string("\"[")
            .append(StringUtils::join(serializedStates, "|"))
            .append("]\"");

    organism->dataMap.set("states", csvStates);
    organism->dataMap.setOutputBehavior("states", DataMap::FIRST);
}
