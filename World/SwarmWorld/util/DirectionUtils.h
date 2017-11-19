//
// Created by Le on 19.11.2017.
//

#ifndef MABE_DIRECTIONUTILS_H
#define MABE_DIRECTIONUTILS_H

#include <utility>
#include <map>
#include <unordered_map>
#include "../model/Direction.h"

typedef std::unordered_map<RelativeDirection, std::pair<TurningDirection, int>> RelativeDirectionLookupMap;


class DirectionUtils {
private:
    static int RELPOS[8][2];

    static RelativeDirectionLookupMap relativeDirectionToTurnDirection;
public:
    /**
     *
     * @param direction
     * @param turningDirection
     * @param howManySteps how many 45° steps we want to turn
     * @return
     */
    static AbsoluteDirection turn(AbsoluteDirection direction, TurningDirection turningDirection, int howManySteps);

    /**
     *
     * @param facingDirection
     * @param direction
     * @return
     */
    static AbsoluteDirection getRelativeDirection(AbsoluteDirection facingDirection,
                                                  RelativeDirection direction);

    /**
     *
     * @param location
     * @param direction
     * @return
     */
    static std::pair<int, int> getRelativePosition(const std::pair<int, int> &location,
                                                   AbsoluteDirection facingDirection,
                                                   RelativeDirection direction);
};

inline AbsoluteDirection DirectionUtils::turn(AbsoluteDirection direction,
                                              TurningDirection turningDirection,
                                              int howManySteps) {

    int _direction = static_cast<int>(direction);
    int _turningDirection = static_cast<int>(turningDirection);

    return static_cast<AbsoluteDirection>(((_direction + _turningDirection * (howManySteps % 8)) + 8) % 8);
}

inline std::pair<int, int> DirectionUtils::getRelativePosition(const std::pair<int, int> &location,
                                                               AbsoluteDirection facingDirection,
                                                               RelativeDirection direction) {
    AbsoluteDirection convertedDirection = getRelativeDirection(facingDirection, direction);
    return {
            location.first + RELPOS[convertedDirection][0],
            location.second + RELPOS[convertedDirection][1]
    };
}

inline AbsoluteDirection DirectionUtils::getRelativeDirection(AbsoluteDirection facingDirection,
                                                              RelativeDirection direction) {
    TurningDirection turningDirection;
    int howManySteps;
    std::tie(turningDirection, howManySteps) = relativeDirectionToTurnDirection.at(direction);

    return turn(facingDirection, turningDirection, howManySteps);
}


RelativeDirectionLookupMap DirectionUtils::relativeDirectionToTurnDirection = {
        {RelativeDirection::FORWARDS,        {TurningDirection::RIGHT, 0}},
        {RelativeDirection::FORWARDS_RIGHT,  {TurningDirection::RIGHT, 1}},
        {RelativeDirection::RIGHT,           {TurningDirection::RIGHT, 2}},
        {RelativeDirection::BACKWARDS_RIGHT, {TurningDirection::RIGHT, 3}},
        {RelativeDirection::BACKWARDS,       {TurningDirection::RIGHT, 4}},
        {RelativeDirection::BACKWARDS_LEFT,  {TurningDirection::LEFT,  3}},
        {RelativeDirection::LEFT,            {TurningDirection::LEFT,  2}},
        {RelativeDirection::FORWARDS_LEFT,   {TurningDirection::LEFT,  1}},
};


int DirectionUtils::RELPOS[8][2] = {{1,  0},  //EAST
                                    {1,  -1}, //SOUTH-EAST
                                    {0,  -1}, //SOUTH
                                    {-1, -1}, //SOUTH-WEST
                                    {-1, 0},  //WEST
                                    {-1, 1},  //NORTH-WEST
                                    {0,  1},  //NORTH
                                    {1,  1}}; //NORTH-EAST

#endif //MABE_DIRECTIONUTILS_H
