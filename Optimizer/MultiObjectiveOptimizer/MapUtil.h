//
// Created by Le on 14.06.2018.
//

#ifndef MABE_MAPUTIL_H
#define MABE_MAPUTIL_H

#include <unordered_map>
#include <iostream>
#include <sstream>

namespace Map{
    template<typename Key, typename Value>
    inline std::string toString(const std::unordered_map<Key, Value>& map){
        std::stringstream ss{};
        for(auto const& entry: map){
            ss << entry.first << ": " << entry.second << "; ";
        }
        return ss.str();
    }

    template<typename Key, typename Value>
    inline Value get(const std::unordered_map<Key, Value>& map, Key key){
        auto iter = map.find(key);

        if(iter != map.end()){
            return iter->second;
        }

        std::cout << "could not find value for key '" << key << "' in map " << Map::toString(map);
        exit(1);
    };
}

#endif //MABE_MAPUTIL_H
