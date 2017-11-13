//
// Created by Le on 09.11.2017.
//

#ifndef MABE_ORGANISMINFOUTIL_H
#define MABE_ORGANISMINFOUTIL_H

#include <utility>
#include <vector>
#include <algorithm>
#include "../model/OrganismInfo.h"

class OrganismInfoUtil {
public:
    static std::vector<std::pair<int, int>> getLocations(const std::vector<OrganismInfo> &organismInfos) {
        std::vector<std::pair<int, int>> locations(organismInfos.size());

        std::transform(organismInfos.begin(), organismInfos.end(), locations.begin(),
                       [](const OrganismInfo &info) -> std::pair<int, int> {
                           return info.getLocation();
                       });

        return locations;
    }


    static std::vector<double> getScores(const std::vector<OrganismInfo> &organismInfos) {
        std::vector<double> locations(organismInfos.size());

        std::transform(organismInfos.begin(), organismInfos.end(), locations.begin(),
                       [](const OrganismInfo &info) -> double {
                           return info.getScore();
                       });

        return locations;
    }
};

#endif //MABE_ORGANISMINFOUTIL_H
