//
// Created by Le on 15.10.2017.
//

#include "SwarmWorldSerializer.h"

#include <utility>

#include "../../../Utilities/Data.h"
#include "../util/StringUtils.h"

template<typename T>
std::string SwarmWorldSerializer::rowToCsvString(std::vector<T> values, std::function<std::string(T)> toString) {
    std::vector<std::string> stringValues = std::vector<std::string>(values.size());

    std::transform(values.begin(), values.end(), stringValues.begin(), toString);
//                   [toString](T &value) -> std::string {
//                       return toString(value);
//                   });

    return StringUtils::join(stringValues, ",");
}


template<typename T>
std::string
SwarmWorldSerializer::nestedListToCsv(std::vector<std::vector<T>> values, std::function<std::string(T)> toString) {
    if (!toString) {
        toString = [](T value) -> std::string {
            return std::to_string(value);
        };
    }

    std::vector<std::string> rows(values.size());
    std::transform(values.begin(), values.end(), rows.begin(),
                   [toString](std::vector<T> value) -> std::string {
                       return rowToCsvString(value, toString);
                   });

    return StringUtils::join(rows, "\n");
}

template<typename ValueType, typename NestedValueType>
std::string SwarmWorldSerializer::nestedObjectToCsv(std::vector<ValueType> values,
                                                    std::function<std::vector<NestedValueType>(ValueType)> toRow,
                                                    std::function<std::string(NestedValueType)> toString) {

    std::vector<std::vector<NestedValueType>> nestedVectors = std::vector<std::vector<NestedValueType>>(values.size());

    std::transform(values.begin(), values.end(), nestedVectors.begin(), toRow);

    /* todo remove
                   [toRow](ValueType &value) -> NestedValueType {
                       return toRow(value);
                   }
     */

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
