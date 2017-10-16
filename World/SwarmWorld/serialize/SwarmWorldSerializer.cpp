//
// Created by Le on 15.10.2017.
//

#include "SwarmWorldSerializer.h"

#include "../../../Utilities/Data.h"
#include "../util/StringUtils.h"

template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(std::vector<OrganismState> value) {
    this->organismStates = std::move(value);
    return *this;
}

template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(WorldLog value) {
    this->worldLog = std::move(value);
    return *this;
}

template<>
SwarmWorldSerializer &SwarmWorldSerializer::with(double value) {
    this->globalScore = value;
    return *this;
}

template<typename T>
SwarmWorldSerializer &SwarmWorldSerializer::with(T value) {
    return *this;
}

//todo: function that receives name of output file and serialized value or something
void SwarmWorldSerializer::serialize() {
    /**
     * Serialize states
     */
    std::stringstream ssfile;
    ssfile << FileManager::outputDirectory << "/states.csv";
    string states_file = ssfile.str();

    ofstream sfile;
    sfile.open(states_file);

    std::vector<std::string> states(organismStates.size());
    std::transform(organismStates.begin(), organismStates.end(), states.begin(),
                   [](OrganismState orgState) -> std::string { return StringUtils::join(orgState.state, ","); });

    sfile << StringUtils::join(states, "\n") << "\n";

    sfile.close();

    /**
     * Serialize states_count
     */
    stringstream ss_statesfile;
    ss_statesfile << FileManager::outputDirectory << "/states_count.csv";
    states_file = ss_statesfile.str();

    sfile.open(states_file);

    for (OrganismState state : organismStates) {
        sfile << state.amount << "\n";
    }
    sfile.close();

    /**
     * Serialize Positions/WorldLog
     */
    stringstream ss;
    ss << FileManager::outputDirectory << "/positions.csv";
    string pos_file = ss.str();

    ofstream map;
    map.open(pos_file);
    worldLog.serialize(map);
    map.close();

    /**
     * Serialize scores
     */
    stringstream scorefile_ss;
    scorefile_ss << FileManager::outputDirectory << "/score.csv";
    string scorefile = scorefile_ss.str();

    ofstream scorefile_of;
    scorefile_of.open(scorefile);

    scorefile_of << globalScore;

    scorefile_of.close();


}
