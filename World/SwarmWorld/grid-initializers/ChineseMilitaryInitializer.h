//
// Created by Le on 02.11.2017.
//

#ifndef MABE_CHINESEMILITARYINITIALIZER_H
#define MABE_CHINESEMILITARYINITIALIZER_H


#include "GridInitializer.h"
#include "RandomInitializer.h"

class ChineseMilitaryInitializer : public RandomInitializer {
protected:
    /**
     * Calculates a list of positions that are in the same row or column as (at least) one of the agents of the given list
     * @param agentPositions
     * @param position
     * @return
     */
    std::vector<std::pair<int, int>> getAgentsOnSameRowOrColumn(
            const std::vector<std::pair<int, int>> &agentPositions,
            const std::pair<int, int> &position);

    /**
     * Calculates a weighted Position map.
     * The weight corresponds to the number of agents that are positioned on the same row or column as the position.
     *
     * Example weights map:
     *
     * A - agent (wont appear in the map), other values are the weights of the positions
     *
     *
     *     0 | 1 | 1 | 1 | 0
     *     2 | A | 3 | A | 2
     *     0 | 1 | 1 | 1 | 0
     *     1 | 2 | A | 1 | 1
     *     0 | 1 | 1 | 1 | 0
     *
     *
     * @param agentPositions
     * @param nonBlockedPositions
     * @param availableSlots
     * @return
     */
    std::map<std::pair<int, int>, int> getWeightedPositionMap(
            const std::vector<std::pair<int, int>> &agentPositions,
            const std::vector<std::pair<int, int>> &nonBlockedPositions,
            const std::vector<std::pair<int, int>> &availableSlots);


    /**
     *
     * Transforms the given map into a vector of pairs
     * @tparam K
     * @tparam V
     * @param map
     * @return
     */
    template<typename K, typename V>
    std::vector<std::pair<K, V>> getListFromMap(const std::map<K, V> &map);

    /**
     * Transforms the given map into a pair-vector and sorts it by weight in ascending order
     * @param weightedMap
     * @return
     */
    std::vector<std::pair<std::pair<int, int>, int>> getSortedWeightListFromMap(
            const std::map<std::pair<int, int>, int> &weightedMap
    );

    /**
     * Retrieves all weighted List items with the lowest weight.
     * The given list has to be sorted beforehand!
     * @param weightedList
     * @return
     */
    std::vector<std::pair<std::pair<int, int>, int>> getLowestWeightPositions(
            const std::vector<std::pair<std::pair<int, int>, int>> &weightedList
    );

public:
    std::pair<int, int> getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                        std::vector<std::pair<int, int>> availableSlots) override;
};


#endif //MABE_CHINESEMILITARYINITIALIZER_H
