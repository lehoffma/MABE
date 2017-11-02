//
// Created by Le on 02.11.2017.
//

#include <map>
#include <algorithm>
#include "ChineseMilitaryInitializer.h"


std::vector<std::pair<int, int>> ChineseMilitaryInitializer::getAgentsOnSameRowOrColumn(
        const std::vector<std::pair<int, int>> &agentPositions,
        const std::pair<int, int> &position
) {
    std::vector<std::pair<int, int>> agentsOnSameRowOrColumn = std::vector<std::pair<int, int>>();

    std::copy_if(agentPositions.begin(), agentPositions.end(), std::back_inserter(agentsOnSameRowOrColumn),
                 [position](std::pair<int, int> slot) -> bool {
                     //at least one value is equal to the given position
                     return slot.first == position.first || slot.second == position.second;
                 });

    return agentsOnSameRowOrColumn;
}


std::map<std::pair<int, int>, int> ChineseMilitaryInitializer::getWeightedPositionMap(
        const std::vector<std::pair<int, int>> &agentPositions,
        const std::vector<std::pair<int, int>> &nonBlockedPositions,
        const std::vector<std::pair<int, int>> &availableSlots
) {

    std::map<std::pair<int, int>, int> map = std::map<std::pair<int, int>, int>();

    //for every nonBlockedPosition
    for (auto &&position : nonBlockedPositions) {
        auto iterator = map.find(position);

        if (iterator == map.end()) {
            map[position] = 0;
        }

        std::vector<std::pair<int, int>> agents = this->getAgentsOnSameRowOrColumn(agentPositions,
                                                                                   position);

        //increase value by how many agents are on the same row or column
        map[position] += agents.size();
    }


    return map;
}

template<typename K, typename V>
std::vector<std::pair<K, V>> ChineseMilitaryInitializer::getListFromMap(const std::map<K, V> &map) {
    std::vector<std::pair<K, V>> pairList;
    for (auto &&item : map) {
        pairList.emplace_back(item);
    }
    return pairList;
}


std::vector<std::pair<std::pair<int, int>, int>> ChineseMilitaryInitializer::getSortedWeightListFromMap(
        const std::map<std::pair<int, int>, int> &weightedMap
) {
    std::vector<std::pair<std::pair<int, int>, int>> weightedList = this->getListFromMap(weightedMap);
    //sort by weight in ascending order
    std::sort(weightedList.begin(), weightedList.end(),
              [](std::pair<std::pair<int, int>, int> a, std::pair<std::pair<int, int>, int> b) -> bool {
                  return a.second < b.second;
              });

    return weightedList;
}


std::vector<std::pair<std::pair<int, int>, int>> ChineseMilitaryInitializer::getLowestWeightPositions(
        const std::vector<std::pair<std::pair<int, int>, int>> &weightedList
) {
    std::vector<std::pair<std::pair<int, int>, int>> lowestWeightList;

    //since the list is sorted, the lowest weight has to be the one of the first value in the list
    int lowestWeight = weightedList[0].second;

    //retrieve all weighted List items that have the same weight as the first item
    std::copy_if(weightedList.begin(), weightedList.end(), std::back_inserter(lowestWeightList),
                 [lowestWeight](std::pair<std::pair<int, int>, int> weightListItem) -> bool {
                     return weightListItem.second == lowestWeight;
                 });

    return lowestWeightList;
}


std::pair<int, int> ChineseMilitaryInitializer::getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                                                std::vector<std::pair<int, int>> availableSlots) {

    //get positions not occupied by a agent
    std::vector<std::pair<int, int>> nonBlockedPositions = this->getFreePositions(agentPositions, availableSlots);

    //get the weighted position map (higher values the more agents are on the same row/column)
    std::map<std::pair<int, int>, int> weightedMap = this->getWeightedPositionMap(agentPositions,
                                                                                  nonBlockedPositions,
                                                                                  availableSlots);

    //convert to list and sort it by weight
    std::vector<std::pair<std::pair<int, int>, int>> weightedList = this->getSortedWeightListFromMap(weightedMap);

    //choose the positions with the lowest weights
    std::vector<std::pair<std::pair<int, int>, int>> lowestWeightList = this->getLowestWeightPositions(weightedList);

    //choose a random position from the lowest weight list
    auto randomIndex = this->getRandomIndex(0, lowestWeightList.size() - 1);
    return lowestWeightList[randomIndex].first;
}
