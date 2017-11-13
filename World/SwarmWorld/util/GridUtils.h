//
// Created by Le on 15.10.2017.
//

#ifndef MABE_GRIDUTILS_H
#define MABE_GRIDUTILS_H


#include <vector>

class GridUtils {
public:
    template<typename T>
    static T** zeros(int x, int y){
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
    static std::vector<std::vector<T>> zerosVector(int x, int y){
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
