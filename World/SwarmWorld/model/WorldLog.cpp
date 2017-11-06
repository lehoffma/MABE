//
// Created by Le on 15.10.2017.
//

#include <algorithm>
#include <numeric>
#include <iterator>
#include "WorldLog.h"
#include "../util/StringUtils.h"

WorldLog &WorldLog::initialize(int organismCount, int worldUpdates) {
    //create a list of timed world log entries for every organism
    for (int i = 0; i < organismCount; i++) {
        //indices of the emplaced vector = timestep
        this->log.emplace_back(std::vector<WorldLogEntry>());

        //push back one world log entry for each world update we are going to need
        for (int j = 0; j < worldUpdates; j++) {
            this->log[i].emplace_back(*(new WorldLogEntry(-1, -1, "1", 0)));
        }

    }
    return *this;
}

std::ostream &WorldLog::serializeToStream(std::ostream &stream) const {
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
     */
    for (int type = X; type != SCORE; type++) {

        //contains a list of values for every timestep of every organism for the current type (i.e. X)
        std::vector<std::string> organismValues(this->log.size());

        std::transform(this->log.begin(), this->log.end(), organismValues.begin(),
                       [this, type](std::vector<WorldLogEntry> orgEntries) -> std::string {
                           //map from log entries to the desired values
                           std::vector<std::string> entryTypeValues(orgEntries.size());
                           std::transform(orgEntries.begin(), orgEntries.end(), entryTypeValues.begin(),
                                          [type](WorldLogEntry &entry) -> std::string {
                                              return entry.getSerialized(static_cast<WorldLogEntryType>(type));
                                          });
                           //and join them with "|" as delimiter
                           std::string result = StringUtils::join(entryTypeValues, "|");

                           //and then surround the whole list with "s
                           std::stringstream val;
                           val << '"' << result << '"';
                           return val.str();
                       });

        //join values of organisms with commas
        std::string serializedOrganismValues = StringUtils::join(organismValues, ",");
        stream << serializedOrganismValues;
        stream << "\n";
    }

    return stream;
}

std::string WorldLog::serializeToString() const {
    std::stringstream stringstream;
    this->serializeToStream(stringstream);
    return stringstream.str();
}

