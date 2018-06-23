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


    template<typename T, typename Func>
    static std::string join(const std::vector<T> &vector,
                            const std::string &delimiter,
                            const Func toString);

};


template<typename T, typename Func>
inline std::string StringUtils::join(const std::vector<T> &vector,
                                     const std::string &delimiter,
                                     const Func toString) {
    if (vector.empty()) {
        return "";
    }

    return std::accumulate(vector.begin() + 1, vector.end(), toString(*vector.begin()),
                           [delimiter, toString](std::string &s, const T &piece) -> std::string {
                               return s.append(delimiter).append(toString(piece));
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
    return StringUtils::join<T>(vector, delimiter, [](T value) -> std::string {
        return std::to_string(value);
    });
}

#endif //MABE_STRINGUTILS_H
