//
// Created by Le on 15.10.2017.
//

#ifndef MABE_SWARMWORLDSERIALIZER_H
#define MABE_SWARMWORLDSERIALIZER_H


#include "Serializer.h"
#include "../model/OrganismState.h"
#include "../model/WorldLog.h"
#include "../../../Brain/MarkovBrain/MarkovBrain.h"

class SwarmWorldSerializer : public Serializer {
private:
    template<typename T>
    static std::string rowToCsvString(std::vector<T> values, std::function<std::string(T)> toString);

    template<typename T>
    static std::string nestedListToCsv(std::vector<std::vector<T>> values, std::function<std::string(T)> toString);

    template<typename ValueType, typename NestedValueType>
    static std::string nestedObjectToCsv(std::vector<ValueType> values,
                                         std::function<std::vector<NestedValueType>(ValueType)> toRow);

    template<typename ValueType, typename NestedValueType>
    static std::string nestedObjectToCsv(std::vector<ValueType> values,
                                         std::function<std::vector<NestedValueType>(ValueType)> toRow,
                                         std::function<std::string(NestedValueType)> toString);

public:
    void serialize() override;

    template<typename W>
    SwarmWorldSerializer &with(W value);

    SwarmWorldSerializer &withLocation(std::vector<std::pair<int, int>> locations, int gridX, int gridY);

    SwarmWorldSerializer &withBrain(MarkovBrain brain, int requiredInputs, int requiredOutputs);

    SwarmWorldSerializer() = default;

    SwarmWorldSerializer &operator=(const SwarmWorldSerializer &serializer) {
        this->serializers = serializer.serializers;
    }
};


#endif //MABE_SWARMWORLDSERIALIZER_H
