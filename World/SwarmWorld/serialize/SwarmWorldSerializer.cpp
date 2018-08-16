//
// Created by Le on 15.10.2017.
//

#include "SwarmWorldSerializer.h"

#include "../util/StringUtils.h"
#include "../util/GridUtils.h"
#include "OrganismSerializer.h"


template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(std::vector<OrganismState> value) {
    std::vector<OrganismState> organisms = std::move(value);

    this->serializers.emplace_back(
            [organisms, this](Serializer serializer) {
                serializer.serializeToFile(
                        FileManager::outputDirectory,
                        "states.csv",
                        OrganismSerializer::nestedObjectToCsv<OrganismState, int>(organisms,
                                                                                  [](OrganismState state) -> std::vector<int> {
                                                                                      return state.state;
                                                                                  }) + "\n"
                );
            }
    );

    this->serializers.emplace_back(
            [organisms, this](Serializer serializer) {
                serializer.serializeToFile(
                        FileManager::outputDirectory,
                        "states_count.csv",
                        OrganismSerializer::nestedObjectToCsv<OrganismState, int>(organisms,
                                                                                  [](OrganismState state) -> std::vector<int> {
                                                                                      std::vector<int> row = std::vector<int>(
                                                                                              1);
                                                                                      row[0] = state.amount;
                                                                                      return row;
                                                                                  }) + "\n"
                );
            });


    return *this;
}


template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(std::vector<std::vector<OrganismState>> value) {
    auto organismStates = std::move(value);

    for (auto &organismState: organismStates) {
        this->with(organismState);
    }

    return *this;
}

template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(WorldLog value) {
    WorldLog worldLog = std::move(value);

    this->serializers.emplace_back([worldLog](Serializer serializer) {
        serializer.serializeToFile(FileManager::outputDirectory,
                                   "positions.csv",
                                   worldLog.serializeToString());
    });

    return *this;
}


template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(double value) {
    this->serializers.emplace_back(
            [value](Serializer serializer) {
                serializer.serializeToFile(FileManager::outputDirectory, "score.csv", std::to_string(value));
            }
    );

    return *this;
}

template<typename T>
SwarmWorldSerializer &SwarmWorldSerializer::with(T value) {
    return *this;
}

void SwarmWorldSerializer::serialize() {
    Serializer::serialize();
}


SwarmWorldSerializer &
SwarmWorldSerializer::withOrganismStates(const std::vector<std::shared_ptr<Agent>> &agents) {

    for (const auto &agent: agents) {

        this->serializers.emplace_back(
                [&agent, this](Serializer serializer) {
                    std::vector<std::string> states{};
                    std::vector<int> occurrences{};

                    std::vector<std::pair<std::string, int>> statesCopy{};

                    for (const auto &state: agent->getStates()) {
                        statesCopy.emplace_back(std::pair<std::string, int>{state.first, state.second});
                    }

                    //sort by amount
                    std::sort(statesCopy.begin(), statesCopy.end(),
                              [](std::pair<std::string, int> stateA, std::pair<std::string, int> stateB) -> int {
                                  return stateA.second > stateB.second;
                              });

                    for (const auto &state: statesCopy) {
                        states.emplace_back(state.first);
                        occurrences.emplace_back(state.second);
                    }


                    std::cout << "serializing states: " << states.size() << std::endl;
                    const auto fileName = "states_" + std::to_string(agent->getOrganism()->ID) + ".csv";
                    serializer.serializeToFile(
                            FileManager::outputDirectory,
                            fileName,
                            StringUtils::join(states, "\n")
                    );

                    const auto countFileName = "states_count_" + std::to_string(agent->getOrganism()->ID) + ".csv";
                    serializer.serializeToFile(
                            FileManager::outputDirectory,
                            countFileName,
                            StringUtils::join(occurrences, "\n")
                    );
                }
        );
    }

    return *this;
}


SwarmWorldSerializer &SwarmWorldSerializer::withLocation(std::vector<std::pair<int, int>> locations,
                                                         int gridX, int gridY) {
    std::string locationsAsString;
    std::stringstream locationStream;
    std::vector<std::pair<int, int>> locationsCopy = std::move(locations);
    std::sort(locationsCopy.begin(), locationsCopy.end());
    std::function<std::string(std::pair<int, int>)> pairToString = [](std::pair<int, int> pair) -> std::string {
        return std::to_string(pair.first) + "," + std::to_string(pair.second);
    };

    locationsAsString = StringUtils::join<std::pair<int, int>>(locationsCopy, "\n", pairToString);

    for (int i = 0; i < gridY; i++) {
        for (int j = 0; j < gridX; j++) {
            pair<int, int> location = {j, i};
            auto iterator = std::find_if(locationsCopy.begin(), locationsCopy.end(),
                                         [location](std::pair<int, int> pair) -> bool {
                                             return pair == location;
                                         });
            //the location is part of the agent-location array
            if (iterator != locationsCopy.end()) {
                locationStream << "A";
            } else {
                locationStream << "0";
            }
            locationStream << ",";
        }
        locationStream << "\n";
    }

    return dynamic_cast<SwarmWorldSerializer &>(
            this->withFileSerializer(FileManager::outputDirectory, "start_locations.csv", locationsAsString)
                    .withFileSerializer(FileManager::outputDirectory, "start_grid.csv", locationStream.str())
    );
}


SwarmWorldSerializer &SwarmWorldSerializer::withBrains(const std::vector<shared_ptr<Agent>> &agents,
                                                       const bool resetOutputs,
                                                       int requiredInputs,
                                                       int requiredOutputs) {

    for (const auto &agent: agents) {
        this->serializers.emplace_back([&agent, requiredInputs, requiredOutputs, resetOutputs](Serializer serializer) {
            auto markovBrain = dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain);
            auto serializedCM = OrganismSerializer::serializeConnectivityMatrix(markovBrain, requiredInputs,
                                                                                requiredOutputs, " ", "\n");
            auto fileName = "cm_" + std::to_string(agent->getOrganism()->ID) + ".csv";
            serializer.serializeToFile(FileManager::outputDirectory, fileName, serializedCM);

            auto serializedTpm = OrganismSerializer::serializeTransitionProbabilityMatrix(markovBrain, resetOutputs,
                                                                                          " ", "\n");
            fileName = "tpm_" + std::to_string(agent->getOrganism()->ID) + ".csv";
            serializer.serializeToFile(FileManager::outputDirectory, fileName, serializedTpm);
        });
    }

    return *this;
}
