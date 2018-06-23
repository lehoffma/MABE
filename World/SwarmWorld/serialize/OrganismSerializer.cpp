//
// Created by Le on 20.06.2018.
//

#include "OrganismSerializer.h"

void OrganismSerializer::addBrainToDataMap(const std::shared_ptr<AbstractBrain> &brain, DataMap &dataMap,
                                           int requiredInputs, int requiredOutputs) {
    auto serializedCM = std::string("\"[")
            .append(OrganismSerializer::serializeConnectivityMatrix(dynamic_cast<MarkovBrain &>(*brain), requiredInputs,
                                                                    requiredOutputs, ";", "|"))
            .append("]\"");

    auto serializedTpm = std::string("\"[")
            .append(OrganismSerializer::serializeTransitionProbabilityMatrix(dynamic_cast<MarkovBrain &>(*brain),
                                                                             ";", "|"))
            .append("]\"");

    dataMap.set("CM", serializedCM);
    dataMap.set("TPM", serializedTpm);
    dataMap.setOutputBehavior("CM", DataMap::FIRST);
    dataMap.setOutputBehavior("TPM", DataMap::FIRST);
}


void OrganismSerializer::addBrainToDataMap(const std::shared_ptr<Organism> &organism, int requiredInputs,
                                           int requiredOutputs) {
    OrganismSerializer::addBrainToDataMap(organism->brain, organism->dataMap, requiredInputs, requiredOutputs);
}

void OrganismSerializer::addStatesToDataMap(const std::shared_ptr<Organism> &organism,
                                            const std::unordered_map<std::string, int> &organismStates) {
    std::vector<std::string> serializedStates{};
    for (auto entry: organismStates) {
        auto state = entry.first;
        serializedStates.emplace_back(state);
    }

    auto csvStates = std::string("\"[")
            .append(StringUtils::join(serializedStates, "|"))
            .append("]\"");

    organism->dataMap.set("states", csvStates);
    organism->dataMap.setOutputBehavior("states", DataMap::FIRST);
}
