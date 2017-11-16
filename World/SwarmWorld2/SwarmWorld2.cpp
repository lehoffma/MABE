
#include <iostream>
#include <fstream>
#include <sstream>

#include "SwarmWorld2.h"
#include "../../Organism/Organism.h"
#include "../SwarmWorld/util/GridUtils.h"

shared_ptr<ParameterLink<int>> SwarmWorld2::gridXSizePL = Parameters::register_parameter("WORLD_SWARM2-gridX", 16,
                                                                                         "size of grid X");
shared_ptr<ParameterLink<int>> SwarmWorld2::gridYSizePL = Parameters::register_parameter("WORLD_SWARM2-gridY", 10,
                                                                                         "size of grid Y");
shared_ptr<ParameterLink<int>> SwarmWorld2::worldUpdatesPL = Parameters::register_parameter("WORLD_SWARM2-worldUpdates",
                                                                                            100,
                                                                                            "amount of time a brain is tested");
shared_ptr<ParameterLink<double>> SwarmWorld2::nAgentsPL = Parameters::register_parameter("WORLD_SWARM2-nAgents", 1.0,
                                                                                          "how many agents in a game in rate");
shared_ptr<ParameterLink<int>> SwarmWorld2::senseAgentsPL = Parameters::register_parameter("WORLD_SWARM2-senseAgents",
                                                                                           0, "1 if ants can sense");
shared_ptr<ParameterLink<string>> SwarmWorld2::senseSidesPL = Parameters::register_parameter("WORLD_SWARM2-senseSides",
                                                                                             (string) "[1]",
                                                                                             "1 if ants can sense");
shared_ptr<ParameterLink<int>> SwarmWorld2::resetOutputsPL = Parameters::register_parameter("WORLD_SWARM2-resetOutputs",
                                                                                            1,
                                                                                            "1 if outputs should be reseted after one time step");
shared_ptr<ParameterLink<int>> SwarmWorld2::hasPenaltyPL = Parameters::register_parameter("WORLD_SWARM2-hasPenalty", 1,
                                                                                          "1 if penalty when agents get hit");
shared_ptr<ParameterLink<double>> SwarmWorld2::penaltyPL = Parameters::register_parameter("WORLD_SWARM2-penalty", 0.075,
                                                                                          "amount of penalty for hit");
shared_ptr<ParameterLink<int>> SwarmWorld2::waitForGoalPL = Parameters::register_parameter("WORLD_SWARM2-waitForGoal",
                                                                                           500,
                                                                                           "timestep till the next goal is possible");

SwarmWorld2::SwarmWorld2(shared_ptr<ParametersTable> _PT) : SwarmWorld(_PT) {
    cout << "Using SwarmWorld2 \n";

    worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_SWARM2-worldUpdates");
    gridX = (PT == nullptr) ? gridXSizePL->lookup() : PT->lookupInt("WORLD_SWARM2-gridX");
    gridY = (PT == nullptr) ? gridYSizePL->lookup() : PT->lookupInt("WORLD_SWARM2-gridY");
    senseAgents = ((PT == nullptr) ? senseAgentsPL->lookup() : PT->lookupInt("WORLD_SWARM2-senseAgents")) == 1;
    resetOutputs = ((PT == nullptr) ? resetOutputsPL->lookup() : PT->lookupInt("WORLD_SWARM2-resetOutputs")) == 1;
    hasPenalty = ((PT == nullptr) ? hasPenaltyPL->lookup() : PT->lookupInt("WORLD_SWARM2-hasPenalty")) == 1;
    nAgents = ((PT == nullptr) ? nAgentsPL->lookup() : PT->lookupDouble("WORLD_SWARM2-nAgents"));
    convertCSVListToVector(((PT == nullptr) ? senseSidesPL->lookup() : PT->lookupString("WORLD_SWARM2-senseSides")),
                           senseSides);
    penalty = (PT == nullptr) ? penaltyPL->lookup() : PT->lookupDouble("WORLD_SWARM2-penalty");
    waitForGoalI = (PT == nullptr) ? waitForGoalPL->lookup() : PT->lookupInt("WORLD_SWARM2-waitForGoal");


    generation = 0;

    cout << worldUpdates << " Updates\n";


    // columns to be added to ave file
    aveFileColumns.clear();
    aveFileColumns.push_back("score");
    std::remove("positions.csv");

    //todo remove
//    this->waterMap = this->levelThree();
    this->startSlots = this->buildGrid();
}

void SwarmWorld2::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    int maxOrgs = startSlots.size() * nAgents;


    vector<vector<vector<string>>> worldLog;
    vector<vector<int>> states;
    vector<int> states_count;

    this->agentMap = GridUtils::zeros<int>(this->gridX, this->gridY);

    // INIT LOG
    if (visualize) {
        for (int i = 0; i < maxOrgs; i++) {
            worldLog.push_back(vector<vector<string>>());
            worldLog[i].push_back(vector<string>()); // X
            worldLog[i].push_back(vector<string>()); // Y
            worldLog[i].push_back(vector<string>()); // F
            worldLog[i].push_back(vector<string>()); // S
            for (int j = 0; j < worldUpdates; j++) {
                worldLog[i][0].push_back("-1");
                worldLog[i][1].push_back("-1");
                worldLog[i][2].push_back("1");
                worldLog[i][3].push_back("0");
            }

        }

    }
    // PLACE AGENTS
    org->brain->resetBrain();
    for (int idx = 0; idx < maxOrgs; idx++) {

        location.push_back({-1, -1});
        oldLocation.push_back({-1, -1});
        score.push_back(0);
        facing.push_back(1);
        waitForGoal.push_back(0);

        //todo remove
//        move(idx, startSlots[idx], 1);
    }

    int nNodes = dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes.size();

    for (int t = 0; t < worldUpdates; t++) {
        //cout << "\n";

        int stimulis = 0;
        vector<int> o_inputs = vector<int>();
        for (int idx = 0; idx < maxOrgs; idx++) {

            // RESET OUTPUTS TO ZERO, TO AVOID CONNECTIONS FROM OUTPUT TO HIDDEN/INPUT
            // @TODO make parameter
            org->brain->setOutput(idx + 0, 0);
            org->brain->setOutput(idx + 1, 0);
            dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[requiredInputs() + idx] = 0;
            dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[requiredInputs() + idx + 1] = 0;

            pair<int, int> cl = location[idx];
            for (int i = 0; i < senseSides.size(); i++) {
                //todo remove
//                pair<int, int> loc = getRelativePosition(location[idx], facing[idx], senseSides[i]);
//                o_inputs.push_back(canMove(loc));
//
//                if (senseAgents) o_inputs.push_back(isAgent(loc));
            }

        }
        //cout << "Inputs read: " <<  o_inputs.size();
        for (int j = 0; j < o_inputs.size(); j++) {
            //cout << j;
            dynamic_pointer_cast<MarkovBrain>(org->brain)->setInput(j, o_inputs[j]);
            stimulis += o_inputs[j];
        }
        //cout << "\n";


        // UPDATE BRAINS
        dynamic_pointer_cast<MarkovBrain>(org->brain)->update();
        vector<int> outputs;

        int idx = 0;
        for (int i = 0; i < 2 * maxOrgs; i++) {
            outputs.push_back(Bit(org->brain->readOutput(i)));

            if ((i + 1) % 2 == 0) {
                int f = facing[idx];
                int new_dir = 0;
                if (outputs[i - 1] == 1 && outputs[i] == 0) {
                    f = (f - 2) % 8;
                    if (f < 0) f += 8;
                } else if (outputs[i - 1] == 0 && outputs[i] == 1) {
                    f = (f + 2) % 8;
                    if (f < 0) f += 8;
                } else if (outputs[i - 1] == 1 && outputs[i] == 1) {
                    new_dir = 1;
                }
                facing[idx] = f;


                if (new_dir != 0) {
                    //todo remove
//                    pair<int, int> new_pos = getRelativePosition(location[idx], facing[idx], new_dir);
//                    if (canMove(new_pos)) {
//                        move(idx, new_pos, f);
//                    }
                }
                idx++;
            }
        }




        // TRACK POSITIONS
        if (visualize) {
            for (int i = 0; i < maxOrgs; i++) {
                worldLog[i][0][t] = (to_string(location[i].first));
                worldLog[i][1][t] = (to_string(location[i].second));
                worldLog[i][2][t] = (to_string(facing[i]));
                worldLog[i][3][t] = (to_string(score[i]));


                vector<int> state;
                for (int i = 0; i < nNodes; i++) {
                    state.push_back(dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[i] > 0 ? 1 : 0);
                }
                bool f = false;
                int f_idx = -1;
                for (int i = 0; i < states.size(); i++) {
                    f = true;
                    for (int j = 0; j < states[i].size(); j++) {
                        if (states[i][j] != state[j]) {
                            f = false;
                            break;
                        }
                    }
                    if (f) {
                        f_idx = i;
                        break;
                    }
                }
                if (f_idx != -1) {
                    states_count[f_idx]++;
                } else {
                    states.push_back(state);
                    states_count.push_back(1);
                }

            }


        }
    }

    // CALCULATE SCORE
    double globalscore = 0;
    double minscore = 292929;
    for (int i = 0; i < maxOrgs; i++) {
        globalscore += score[i];
        if (score[i] < minscore) {
            minscore = score[i];
        }
    }
    globalscore /= maxOrgs;
    org->dataMap.setOutputBehavior("score", DataMap::AVE | DataMap::LIST);
    //org->dataMap.Append("score", globalscore);
    org->dataMap.Append("score", globalscore);

    if (visualize) {
        // WRITE BEST BRAIN TPM/CM
        shared_ptr<MarkovBrain> mb = dynamic_pointer_cast<MarkovBrain>(org->brain->makeCopy());
        //todo remove
//        getTPM(mb);
//        getCM(mb);

        // WRITE STATES
        //SORT
        for (int i = 0; i < states.size(); i++) {
            for (int j = 0; j < states.size(); j++) {
                if (states_count[j] > states_count[i]) {
                    int c = states_count[j];
                    states_count[j] = states_count[i];
                    states_count[i] = c;
                    vector<int> s = states[j];
                    states[j] = states[i];
                    states[i] = s;
                }
            }
        }

        stringstream ssfile;
        ssfile << FileManager::outputDirectory << "/states.csv";
        string states_file = ssfile.str();

        ofstream sfile;
        sfile.open(states_file);

        for (int i = 0; i < states.size(); i++) {
            for (int j = 0; j < states[i].size(); j++) {
                sfile << states[i][j];
                if (j < states[i].size() - 1) sfile << ",";
            }
            sfile << "\n";
        }
        sfile.close();

        stringstream ss_statesfile;
        ss_statesfile << FileManager::outputDirectory << "/states_count.csv";
        states_file = ss_statesfile.str();

        sfile.open(states_file);

        for (int i = 0; i < states_count.size(); i++) {
            sfile << states_count[i] << "\n";
        }
        sfile.close();

        // WRITE POSITONS
        stringstream ss;
        ss << FileManager::outputDirectory << "/positions.csv";
        string pos_file = ss.str();

        ofstream map;
        map.open(pos_file);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < maxOrgs; j++) {
                stringstream val;
                val << '"';
                for (int k = 0; k < worldUpdates; k++) {
                    if (k + 1 >= worldUpdates) {
                        val << worldLog[j][i][k];
                    } else {
                        val << worldLog[j][i][k] << '|';

                    }
                }
                val << '"';

                if (j + 1 >= maxOrgs) {
                    map << val.str();
                } else {
                    map << val.str() << ',';

                }
            }
            map << "\n";
        }
        map.close();

    }

    generation++;

    // CLEAN UP
    for (int i = 0; i < gridX; ++i) {
        delete[] this->agentMap[i];
    }

    location.clear();
    oldLocation.clear();
    score.clear();
    waitForGoal.clear();
    facing.clear();

}

int SwarmWorld2::requiredInputs() {
    int maxOrgs = startSlots.size() * nAgents; //@ToDo: Not clean
    //cout << (senseSides.size() * (senseAgents?2:1)) * maxOrgs << " Inputs \n";
    return (senseSides.size() * (senseAgents ? 2 : 1)) * maxOrgs; // moving + bridge + goal + comm
    //return groupSize * 12;
}

int SwarmWorld2::requiredOutputs() {
    int maxOrgs = startSlots.size() * nAgents;
    return (2 * maxOrgs);
}


