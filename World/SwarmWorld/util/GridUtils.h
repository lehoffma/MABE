//
// Created by Le on 15.10.2017.
//

#ifndef MABE_GRIDUTILS_H
#define MABE_GRIDUTILS_H


#include <vector>

class GridUtils {
public:
    template<typename T>
    static T **zeros(int x, int y) {
        T **grid = new T *[x];

        for (int i = 0; i < x; i++) {
            grid[i] = new T[y];
            for (int j = 0; j < y; j++) {
                grid[i][j] = static_cast<T>(0);
            }
        }

        return grid;
    }


    template<typename T>
    static std::unordered_map<int, std::unordered_map<int, T>> zerosMap(int x, int y) {
        std::unordered_map<int, std::unordered_map<int, T>> map{};

        for (int i = 0; i < x; i++) {
            map[i] = std::unordered_map<int, T>{};
            for (int j = 0; j < y; j++) {
                map[i][j] = static_cast<T>(0);
            }
        }

        return map;
    };


    template<typename T>
    static void assignZerosMap(
            std::unordered_map<int, std::unordered_map<int, std::shared_ptr<T>>>& map,
            int x,
            int y
    ) {
        for (int i = 0; i < x; i++) {
            map[i] = std::unordered_map<int, std::shared_ptr<T>>{};
            for (int j = 0; j < y; j++) {
                map[i][j] = std::make_shared<T>(static_cast<T>(0));
            }
        }
    };

    template<typename T>
    static std::vector<std::vector<T>> zerosVector(int x, int y) {
        std::vector<std::vector<T>> grid(static_cast<unsigned int>(x), std::vector<T>(y));

        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                grid[i][j] = static_cast<T>(0);
            }
        }

        return grid;
    }
};


#endif //MABE_GRIDUTILS_H
