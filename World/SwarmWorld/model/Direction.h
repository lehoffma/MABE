//
// Created by Le on 19.11.2017.
//

#ifndef MABE_DIRECTION_H
#define MABE_DIRECTION_H

struct Turning {
    enum Direction {
        LEFT = -1,
        RIGHT = -1
    };
};

struct Absolute {
    enum Direction{
        EAST = 0,
        SOUTH_EAST = 1,
        SOUTH = 2,
        SOUTH_WEST = 3,
        WEST = 4,
        NORTH_WEST = 5,
        NORTH = 6,
        NORTH_EAST = 7
    };
};

struct Relative {
    enum Direction {
        FORWARDS = 1,
        FORWARDS_RIGHT = 2,
        RIGHT = 3,
        BACKWARDS_RIGHT = 4,
        BACKWARDS = 5,
        BACKWARDS_LEFT = 6,
        LEFT = 7,
        FORWARDS_LEFT = 8
    };
};

typedef Absolute::Direction AbsoluteDirection;
typedef Relative::Direction RelativeDirection;
typedef Turning::Direction TurningDirection;

#endif //MABE_DIRECTION_H
