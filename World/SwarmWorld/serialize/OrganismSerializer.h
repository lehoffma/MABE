//
// Created by Le on 20.06.2018.
//

#ifndef MABE_ORGANISMSERIALIZER_H
#define MABE_ORGANISMSERIALIZER_H


#include <string>
#include "../../../Brain/MarkovBrain/MarkovBrain.h"
#include "../util/StringUtils.h"
#include "../util/GridUtils.h"
#include "../../../Organism/Organism.h"
#include "../model/OrganismState.h"
#include "../model/Agent.h"

namespace OrganismSerializer {

    template<typename T>
    inline std::string rowToCsvString(std::vector<T> values, std::function<std::string(T)> toString) {

        return StringUtils::join<T>(values, ",", toString);
    }

    template<typename T>
    inline std::string nestedListToCsv(std::vector<std::vector<T>> values, std::function<std::string(T)> toString) {
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
    inline std::string nestedObjectToCsv(std::vector<ValueType> values,
                                         std::function<std::vector<NestedValueType>(ValueType)> toRow) {
        return nestedObjectToCsv<ValueType, NestedValueType>(values, toRow, [](NestedValueType value) -> std::string {
            return std::to_string(value);
        });
    };

    template<typename ValueType, typename NestedValueType>
    inline std::string nestedObjectToCsv(std::vector<ValueType> values,
                                         std::function<std::vector<NestedValueType>(ValueType)> toRow,
                                         std::function<std::string(NestedValueType)> toString) {
        std::vector<std::vector<NestedValueType>> nestedVectors = std::vector<std::vector<NestedValueType>>(
                values.size());

        std::transform(values.begin(), values.end(), nestedVectors.begin(), toRow);

        return nestedListToCsv(nestedVectors, toString);
    };


    /**
     *
     * @param brain
     * @param inputs
     * @param outputs
     */
    inline std::string serializeConnectivityMatrix(MarkovBrain &brain, int inputs, int outputs,
                                                   const std::string &columnDelimiter,
                                                   const std::string &rowDelimiter) {
        int amountOfNodes = brain.nrNodes;
        auto connectivityMatrix = brain.getConnectivityMatrix();
        vector<std::string> rows(connectivityMatrix.size());

        for (int i = 0; i < amountOfNodes; i++) {
            int j = 0;
            rows.emplace_back(
                    StringUtils::join<int>(connectivityMatrix[i], columnDelimiter,
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

        return StringUtils::join(rows, rowDelimiter);
    }

    /**
     *
     * @param brain
     * @param requiredInputs
     * @param requiredOutputs
     * @return
     */
    inline std::string serializeTransitionProbabilityMatrix(MarkovBrain &brain,
                                                            const std::string &columnDelimiter,
                                                            const std::string &rowDelimiter) {
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

            delete[] array;
        }

        //i can't use the join method, because the indices are backwards :/
        stringstream ss;
        for (int i = 0; i < amountOfStates; i++) {
            for (int j = 0; j < amountOfNodes; j++) {
                if (j + 1 >= amountOfNodes) {
                    ss << mat[j][i];
                } else {
                    ss << mat[j][i] << columnDelimiter;

                }
            }
            ss << rowDelimiter;
        }

        return ss.str();
    }

    inline std::vector<int> getState(const std::shared_ptr<Organism> &org) {
        //state of the current organism
        vector<double> nodes = dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes;
        vector<int> state(nodes.size());
        transform(nodes.begin(), nodes.end(), state.begin(),
                  [](double nodeValue) { return (nodeValue > 0); });

        return state;
    }

    inline std::string getStringState(const std::shared_ptr<Organism> &org) {
        //state of the current organism
        vector<double> nodes = dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes;


        return StringUtils::join(nodes, ";", [](double nodeValue) -> std::string {
            return std::to_string(Bit(nodeValue));
         });
    }

    inline void updateStates(const std::shared_ptr<Organism> &org, std::unordered_map<std::string, int> &states) {
        //state of the current organism
        auto stringState = getStringState(org);
        states[stringState]++;
    }


    inline void updateOrganismState(const std::shared_ptr<Organism> &org,
                                    std::vector<OrganismState> &organismStates) {

        //state of the current organism
        auto state = getState(org);

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

    void addStatesToDataMap(
            const std::shared_ptr<Organism> &organism,
            const std::unordered_map<std::string, int> &organismStates
    );


    void addBrainToDataMap(
            const std::shared_ptr<AbstractBrain> &brain,
            DataMap &dataMap,
            int requiredInputs,
            int requiredOutputs
    );

    void addBrainToDataMap(
            const std::shared_ptr<Organism> &organism,
            int requiredInputs,
            int requiredOutputs
    );
};


#endif //MABE_ORGANISMSERIALIZER_H
