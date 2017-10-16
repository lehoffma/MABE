//
// Created by Le on 15.10.2017.
//

#ifndef MABE_STRINGUTILS_H
#define MABE_STRINGUTILS_H

#include <vector>
#include <numeric>
#include "string"

class StringUtils {
public:
    /**
     * helper method to join a list of strings together, divided by the given delimiter
     * @param vector
     * @param delimiter
     * @return
     */
    template<typename T>
    static std::string join(const std::vector<T> &vector, const std::string &delimiter);

};



/**
 * helper method to join a list of strings together, divided by the given delimiter
 * @param vector
 * @param delimiter
 * @return
 */
template<>
inline std::string StringUtils::join(const std::vector<std::string> &vector, const std::string &delimiter) {
    if(vector.empty()){
        return "";
    }

    return std::accumulate(vector.begin(), vector.end(), vector[0],
                           [delimiter](std::string &s, const std::string &piece) -> std::string{   //NOLINT
                               return s += delimiter + piece;
                           });
}



/**
 * helper method to join a list of strings together, divided by the given delimiter
 * @param vector
 * @param delimiter
 * @return
 */
template<typename T>
inline std::string StringUtils::join(const std::vector<T> &vector, const std::string &delimiter) {
    if(vector.empty()){
        return "";
    }

    return std::accumulate(vector.begin(), vector.end(), std::to_string(vector[0]),
                           [delimiter](std::string &s, const T &piece) -> std::string {   //NOLINT
                               return s += delimiter + std::to_string(piece);
                           });
}

#endif //MABE_STRINGUTILS_H
