//
// Created by Le on 15.10.2017.
//

#ifndef MABE_STRINGUTILS_H
#define MABE_STRINGUTILS_H

#include <vector>
#include <numeric>
#include "string"
#include "functional"

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


    template<typename T>
    static std::string join(const std::vector<T> &vector,
                            const std::string &delimiter,
                            const std::function<std::string(T)> toString);

};


template<typename T>
inline std::string StringUtils::join(const std::vector<T> &vector,
                                     const std::string &delimiter,
                                     const std::function<std::string(T)> toString) {
    if (vector.empty()) {
        return "";
    }

    return std::accumulate(vector.begin(), vector.end(), std::string(),
                           [delimiter, toString](const std::string &s, const T &piece) -> std::string {
                               return s + (s.length() > 0 ? delimiter : "") + toString(piece);
                           });
}

/**
 * helper method to join a list of strings together, divided by the given delimiter
 * @param vector
 * @param delimiter
 * @return
 */
template<>
inline std::string StringUtils::join(const std::vector<std::string> &vector, const std::string &delimiter) {
    return StringUtils::join<std::string>(vector, delimiter, [](std::string value) -> std::string {
        return value;
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
    return StringUtils::join<T>(vector, delimiter, std::to_string);
}

#endif //MABE_STRINGUTILS_H
