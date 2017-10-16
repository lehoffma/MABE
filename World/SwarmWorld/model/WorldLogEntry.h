//
// Created by Le on 15.10.2017.
//

#ifndef MABE_WORLDLOGENTRY_H
#define MABE_WORLDLOGENTRY_H

#include <string>
#include <utility>

enum WorldLogEntryType{
    X = 0,
    Y = 1,
    FACING = 2,
    SCORE = 3
};

class WorldLogEntry {
public:
    int x;
    int y;
    std::string facing;
    double score;

    WorldLogEntry(int x, int y, std::string facing, double score) : x(x), y(y), facing(std::move(facing)), score(score) {

    }

    WorldLogEntry& setX(int x) {
        this->x = x;
        return *this;
    }

    WorldLogEntry& setY(int y) {
        this->y = y;
        return *this;
    }

    WorldLogEntry& setFacing(std::string facing) {
        this->facing = std::move(facing);
        return *this;
    }

    WorldLogEntry& setScore(double score) {
        this->score = score;
        return *this;
    }

    std::string getSerialized(WorldLogEntryType type) const{
        switch(type){
            case X:
                return std::to_string(this->x);
            case Y:
                return std::to_string(this->y);
            case FACING:
                return this->facing;
            case SCORE:
                return std::to_string(this->score);
            default:
                return "";
        }
    }
};


#endif //MABE_WORLDLOGENTRY_H
