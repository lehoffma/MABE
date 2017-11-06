//
// Created by Le on 15.10.2017.
//

#ifndef MABE_WORLDLOG_H
#define MABE_WORLDLOG_H


#include <vector>
#include <sstream>
#include "WorldLogEntry.h"

class WorldLog {
private:
    /**
     * Contains logged information about the simulated world
     * example:
     *
     *      i=0 (organism1) -> WorldLogEntry (t=0) -> {x: 0, y: 0, facing: "-1", score: 500}
     *                         WorldLogEntry (t=1) -> {x: 0, y: 1, facing: "0", score: 499},
     *                         ...
     *
     *      i=1 (organism2) -> WorldLogEntry (t=0) -> {x: 0, y: 0, facing: "-1", score: 500}
     *                         WorldLogEntry (t=1) -> {x: 0, y: 1, facing: "0", score: 499},
     *                         ...
     *
     */
    std::vector<std::vector<WorldLogEntry>> log;

public:
    WorldLog() = default;

    /**
     * Initializes the given world log vector with empty data
     * @param worldLog containing location, facing and score data about every organism of the simulation at every timestep
     *
     * example:
     *
     *      organism1 -> WorldLogEntry (t=0) -> {x: 0, y: 0, facing: "-1", score: 500}
     *                   WorldLogEntry (t=1) -> {x: 0, y: 1, facing: "0", score: 499}
     *
     * @param organismCount how many organisms are used in the simulation
     * @param worldUpdates how often the world will update in the simulation
     */
    WorldLog &initialize(int organismCount, int worldUpdates);

    /**
     *
     * example structure (adapted from dominik)
     *
     * x -> organism1 -> t = 0 -> 1
     *                   t = 1 -> 5
     *                   t = 2 -> 10
     *      organism2 -> t = 0 -> 2
     *                    ...
     *
     * y -> organism1 -> ...
     *
     * facing -> organism1 -> t = 0 -> ...
     *
     * score -> organism1 -> t = 0 -> ...
     *
     * @param stream
     * @return
     */
    std::ostream &serializeToStream(std::ostream &stream) const;


    std::string serializeToString() const;


    std::vector<WorldLogEntry>& operator[](int organismIndex){
        return this->log[organismIndex];
    }
};


#endif //MABE_WORLDLOG_H
