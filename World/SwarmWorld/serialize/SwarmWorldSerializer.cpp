//
// Created by Le on 15.10.2017.
//

#include "SwarmWorldSerializer.h"

#include "../util/StringUtils.h"
#include "../util/GridUtils.h"

template<typename T>
std::string SwarmWorldSerializer::rowToCsvString(std::vector<T> values, std::function<std::string(T)> toString) {
    return StringUtils::join<T>(values, ",", toString);
}


template<typename T>
std::string SwarmWorldSerializer::nestedListToCsv(std::vector<std::vector<T>> values,
                                                  std::function<std::string(T)> toString) {
    if (!toString) {
        toString = [](T value) -> std::string {
            return std::to_string(value);
        };
    }

    return StringUtils::join<std::vector<T>>(values, "\n",
                                             [toString](std::vector<T> value) -> std::string {
                                                 return rowToCsvString(value, toString);
                                             });
}

template<typename ValueType, typename NestedValueType>
std::string SwarmWorldSerializer::nestedObjectToCsv(std::vector<ValueType> values,
                                                    std::function<std::vector<NestedValueType>(ValueType)> toRow,
                                                    std::function<std::string(NestedValueType)> toString) {
    std::vector<std::vector<NestedValueType>> nestedVectors = std::vector<std::vector<NestedValueType>>(values.size());

    std::transform(values.begin(), values.end(), nestedVectors.begin(), toRow);

    return nestedListToCsv(nestedVectors, toString);
}

template<typename ValueType, typename NestedValueType>
std::string SwarmWorldSerializer::nestedObjectToCsv(std::vector<ValueType> values,
                                                    std::function<std::vector<NestedValueType>(ValueType)> toRow) {
    return nestedObjectToCsv<ValueType, NestedValueType>(values, toRow, [](NestedValueType value) -> std::string {
        return std::to_string(value);
    });
}

template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(std::vector<OrganismState> value) {
    std::vector<OrganismState> organisms = std::move(value);

    this->serializers.emplace_back(
            [organisms, this](Serializer serializer) {
                serializer.serializeToFile(
                        FileManager::outputDirectory,
                        "states.csv",
                        nestedObjectToCsv<OrganismState, int>(organisms,
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
                        nestedObjectToCsv<OrganismState, int>(organisms,
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
SwarmWorldSerializer::withOrganismStates(std::vector<OrganismStateContainer> &value) {
    auto organismStates = std::move(value);

    for (auto &container: organismStates) {
        this->serializers.emplace_back(
                [container, this](Serializer serializer) {
                    const auto fileName = "states_" + std::to_string(container.organism->ID) + ".csv";
                    serializer.serializeToFile(
                            FileManager::outputDirectory,
                            fileName,
                            nestedObjectToCsv<OrganismState, int>(container.organismStates,
                                                                  [](OrganismState state) -> std::vector<int> {
                                                                      return state.state;
                                                                  }) + "\n"
                    );
                }
        );

        this->serializers.emplace_back(
                [container, this](Serializer serializer) {
                    const auto fileName = "states_count_" + std::to_string(container.organism->ID) + ".csv";
                    serializer.serializeToFile(
                            FileManager::outputDirectory,
                            fileName,
                            nestedObjectToCsv<OrganismState, int>(container.organismStates,
                                                                  [](OrganismState state) -> std::vector<int> {
                                                                      std::vector<int> row = std::vector<int>(
                                                                              1);
                                                                      row[0] = state.amount;
                                                                      return row;
                                                                  }) + "\n"
                    );
                });
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

/**
 *
 * @param brain
 * @param inputs
 * @param outputs
 */
std::string serializeConnectivityMatrix(MarkovBrain &brain, int inputs, int outputs) {
    int amountOfNodes = brain.nrNodes;
    vector<vector<int>> connectivityMatrix = brain.getConnectivityMatrix();
    vector<std::string> rows(connectivityMatrix.size());

    for (int i = 0; i < amountOfNodes; i++) {
        int j = 0;
        rows.emplace_back(
                StringUtils::join<int>(connectivityMatrix[i], " ",
                                       [&j, i, inputs, outputs](const int value) -> std::string {
                                           int val = value > 0;
                                           // DO NOT ALLOW CONNECTIONS TO INPUTS OR FROM OUTPUTS TO SOMEWHERE
                                           if (j < inputs) val = 0;
                                           if (i >= inputs && i < inputs + outputs) val = 0;

                                           j++;
                                           return std::to_string(val);
                                       }
                )
        );
    }

    return StringUtils::join(rows, "\n");
}

/**
 *
 * @param brain
 * @param requiredInputs
 * @param requiredOutputs
 * @return
 */
std::string serializeTransitionProbabilityMatrix(MarkovBrain &brain) {
    // EXPECT THAT HIDDEN NODES ARE IN THE END OF THE NODE LIST (VERIFIED)
    int amountOfNodes = brain.nrNodes;
    auto amountOfStates = static_cast<int>(pow(2, amountOfNodes));
    std::vector<std::vector<int>> mat = GridUtils::zerosVector<int>(amountOfNodes, amountOfStates);

    for (int i = 0; i < amountOfStates; i++) {
        brain.resetBrain();

        auto *array = new int[32];
        for (int j = 0; j < 32; ++j) {  // assuming a 32 bit int
            array[j] = i & (1 << j) ? 1 : 0;
        }

        for (int j = 0; j < amountOfNodes; j++) {
            if (j < brain.inputValues.size()) {
                brain.inputValues[j] = array[j];
                //} else if (j>=brain->inputValues.size() && j < brain->inputValues.size() + 2) {
                // MAKE SURE THAT OUTPUTS WILL NOT CAUSE ANYTHING (PYPHI-STUFF)
                //    brain->nodes[j] = 0;
            } else {
                // HIDDEN NODES
                brain.nodes[j] = array[j];
            }
        }
        brain.update();
        for (int j = 0; j < amountOfNodes; j++) {
            double &val = brain.nodes[j];

            mat[j][i] = (val > 0 ? 1 : 0);
        }
    }

    //i can't use the join method, because the indices are backwards :/
    stringstream ss;
    for (int i = 0; i < amountOfStates; i++) {
        for (int j = 0; j < amountOfNodes; j++) {
            if (j + 1 >= amountOfNodes) {
                ss << mat[j][i];
            } else {
                ss << mat[j][i] << ' ';

            }
        }
        ss << "\n";
    }

    return ss.str();
}

SwarmWorldSerializer &SwarmWorldSerializer::withBrains(std::vector<shared_ptr<OrganismBrain>> &brains,
                                                       int requiredInputs,
                                                       int requiredOutputs) {

    for (auto &brain: brains) {
        this->serializers.emplace_back([&brain, requiredInputs, requiredOutputs](Serializer serializer) {
            auto serializedCM = serializeConnectivityMatrix(*brain->brain, requiredInputs, requiredOutputs);
            auto fileName = "cm_" + std::to_string(brain->organism->ID) + ".csv";
            serializer.serializeToFile(FileManager::outputDirectory, fileName, serializedCM);
        });

        this->serializers.emplace_back([&brain](Serializer serializer) {
            auto serializedTpm = serializeTransitionProbabilityMatrix(*brain->brain);
            auto fileName = "tpm_" + std::to_string(brain->organism->ID) + ".csv";
            serializer.serializeToFile(FileManager::outputDirectory, fileName, serializedTpm);
        });
    }

    return *this;
}
