//
// Created by Le on 12.02.2018.
//

#ifndef MABE_TOURNAMENTSELECTOR_H
#define MABE_TOURNAMENTSELECTOR_H

#include <vector>
#include "functional"
#include "../../Utilities/Random.h"

class TournamentSelector {
public:
    template<typename T>
    static T select(
            const int tournamentSize,
            const int populationSize,
            const std::function<T(int)> organismSupplier,
            const std::function<bool(T, T)> compare
    );

    template<typename T>
    static T binary(const int populationSize,
                    const std::function<T(int)> organismSupplier,
                    const std::function<bool(T, T)> compare);
};


template<typename T>
T TournamentSelector::binary(
        const int populationSize,
        const std::function<T(int)> organismSupplier,
        const std::function<bool(T, T)> compare
) {
    return select(2, populationSize, organismSupplier, compare);
}


template<typename T>
T TournamentSelector::select(const int tournamentSize, const int populationSize,
                             const std::function<T(int)> organismSupplier, const std::function<bool(T, T)> compare) {
    if (tournamentSize == 0 || populationSize == 0) {
        return nullptr;
    }

    std::vector<int> indices{};
    auto size = std::min(tournamentSize, populationSize);
    while (indices.size() < size) {
        auto nextIndex = -1;
        //generate random values until a different one was found
        while (std::find(indices.begin(), indices.end(), nextIndex = Random::getIndex(populationSize)) !=
               indices.end()) {}
        indices.push_back(nextIndex);
    }

    std::vector<T> values(indices.size());
    std::transform(indices.begin(), indices.end(), values.begin(),organismSupplier);

    std::sort(values.begin(), values.end(), compare);

    return values[0];
}

#endif //MABE_TOURNAMENTSELECTOR_H
