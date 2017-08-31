
#include <iostream>
#include <fstream>
#include <sstream>

#include "SwarmWorld2.h"
#include "../../Organism/Organism.h"

shared_ptr<ParameterLink<int>> SwarmWorld2::gridXSizePL = Parameters::register_parameter("WORLD_SWARM2-gridX", 16, "size of grid X");
shared_ptr<ParameterLink<int>> SwarmWorld2::gridYSizePL = Parameters::register_parameter("WORLD_SWARM2-gridY", 10, "size of grid Y");
shared_ptr<ParameterLink<int>> SwarmWorld2::worldUpdatesPL = Parameters::register_parameter("WORLD_SWARM2-worldUpdates", 100, "amount of time a brain is tested");
shared_ptr<ParameterLink<double>> SwarmWorld2::nAgentsPL = Parameters::register_parameter("WORLD_SWARM2-nAgents", 1.0, "how many agents in a game in rate");
shared_ptr<ParameterLink<int>> SwarmWorld2::senseAgentsPL = Parameters::register_parameter("WORLD_SWARM2-senseAgents", 0, "1 if ants can sense");
shared_ptr<ParameterLink<string>> SwarmWorld2::senseSidesPL = Parameters::register_parameter("WORLD_SWARM2-senseSides", (string)"[1]", "1 if ants can sense");
shared_ptr<ParameterLink<int>> SwarmWorld2::resetOutputsPL = Parameters::register_parameter("WORLD_SWARM2-resetOutputs", 1, "1 if outputs should be reseted after one time step");
shared_ptr<ParameterLink<int>> SwarmWorld2::hasPenaltyPL = Parameters::register_parameter("WORLD_SWARM2-hasPenalty", 1, "1 if penalty when agents get hit");
shared_ptr<ParameterLink<double>> SwarmWorld2::penaltyPL = Parameters::register_parameter("WORLD_SWARM2-penalty", 0.075, "amount of penalty for hit");
shared_ptr<ParameterLink<int>> SwarmWorld2::waitForGoalPL = Parameters::register_parameter("WORLD_SWARM2-waitForGoal", 500, "timestep till the next goal is possible");

SwarmWorld2::SwarmWorld2(shared_ptr<ParametersTable> _PT) : AbstractWorld(_PT) {
    cout << "Using SwarmWorld2 \n";
    
    worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_SWARM2-worldUpdates");
    gridX = (PT == nullptr) ? gridXSizePL->lookup() : PT->lookupInt("WORLD_SWARM2-gridX");
    gridY = (PT == nullptr) ? gridYSizePL->lookup() : PT->lookupInt("WORLD_SWARM2-gridY");
    senseAgents = ((PT == nullptr) ? senseAgentsPL->lookup() : PT->lookupInt("WORLD_SWARM2-senseAgents")) == 1;
    resetOutputs = ((PT == nullptr) ? resetOutputsPL->lookup() : PT->lookupInt("WORLD_SWARM2-resetOutputs")) == 1;
    hasPenalty = ((PT == nullptr) ? hasPenaltyPL->lookup() : PT->lookupInt("WORLD_SWARM2-hasPenalty")) == 1;
    nAgents = ((PT == nullptr) ? nAgentsPL->lookup() : PT->lookupDouble("WORLD_SWARM2-nAgents"));
    convertCSVListToVector(((PT == nullptr) ? senseSidesPL->lookup() : PT->lookupString("WORLD_SWARM2-senseSides")), senseSides);
    penalty = (PT == nullptr) ? penaltyPL->lookup() : PT->lookupDouble("WORLD_SWARM2-penalty");
    waitForGoalI = (PT == nullptr) ? waitForGoalPL->lookup() : PT->lookupInt("WORLD_SWARM2-waitForGoal");
    

    generation = 0;

    cout << worldUpdates << " Updates\n";
    
    
    // columns to be added to ave file
    aveFileColumns.clear();
    aveFileColumns.push_back("score");
    std::remove("positions.csv");
    
    this->buildGrid();
    
}

void SwarmWorld2::buildGrid(){
    
    cout << "Build Map:\n";
    this->waterMap = levelThree();
    
    
    for(int i = 0; i < gridY; i++) {
        for(int j = 0; j < gridX; j++) {
            pair<int,int> loc = {j,i};
            if(isStart(loc) ) {
                startSlots.push_back(loc);
            }
        }
    }
    
}

void SwarmWorld2::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    int maxOrgs = startSlots.size() * nAgents;
    
    
    vector<vector<vector<string>>> worldLog;
    vector<vector<int>> states;
    vector<int> states_count;
    
    this->agentMap = SwarmWorld2::zeros(this->gridX, this->gridY);
    
    // INIT LOG
    if(visualize) {
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
        
        location.push_back({-1,-1});
        oldLocation.push_back({-1,-1});
        score.push_back(0);
        facing.push_back(1);
        waitForGoal.push_back(0);
        
        move(idx,startSlots[idx], 1);
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
            
            pair<int,int> cl = location[idx];
            for(int i=0; i < senseSides.size(); i++) {
                pair<int,int> loc = getRelativePosition(location[idx], facing[idx], senseSides[i]);
                o_inputs.push_back(canMove(loc));
                
                if(senseAgents) o_inputs.push_back(isAgent(loc));
            }

        }
        //cout << "Inputs read: " <<  o_inputs.size();
        for(int j = 0; j < o_inputs.size(); j++) {
            //cout << j;
            dynamic_pointer_cast<MarkovBrain>(org->brain)->setInput(j, o_inputs[j]);
            stimulis += o_inputs[j];
        }
        //cout << "\n";
            
        
        // UPDATE BRAINS
        dynamic_pointer_cast<MarkovBrain>(org->brain)->update();
        vector<int> outputs;
        
        int idx = 0;
        for(int i=0; i < 2 * maxOrgs; i++) {
            outputs.push_back(Bit(org->brain->readOutput(i)));
            
            if ((i+1) % 2 == 0) {
                int f = facing[idx];
                int new_dir = 0;
                if(outputs[i - 1] == 1 &&  outputs[i] == 0) {
                    f = (f - 2) % 8;
                    if (f < 0) f+=8;
                } else if (outputs[i -1] == 0 &&  outputs[i] == 1) {
                    f = (f + 2) % 8;
                    if (f < 0) f+=8;
                } else if (outputs[i -1 ] == 1 &&  outputs[i] == 1) {
                    new_dir = 1;
                }
                facing[idx] = f;
                
                
                if(new_dir != 0) {
                    pair<int,int> new_pos = getRelativePosition(location[idx], facing[idx], new_dir);
                    if(canMove(new_pos)) {
                        move(idx, new_pos, f);
                    }
                }
                idx ++;
            }
        }
        
        
        
        
        // TRACK POSITIONS
        if(visualize) {
            for(int i = 0; i < maxOrgs; i++) {
                worldLog[i][0][t] = (to_string(location[i].first));
                worldLog[i][1][t] = (to_string(location[i].second));
                worldLog[i][2][t] = (to_string(facing[i]));
                worldLog[i][3][t] = (to_string(score[i]));
                
                
               
                vector<int> state;
                for(int i = 0; i < nNodes; i++) {
                    state.push_back(dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[i]>0?1:0);
                }
                bool f = false;
                int f_idx = -1;
                for(int i = 0; i < states.size(); i++) {
                    f = true;
                    for(int j = 0; j < states[i].size(); j++) {
                        if(states[i][j] != state[j]) {
                            f = false;
                            break;
                        }
                    }
                    if(f) {
                        f_idx = i;
                        break;
                    }
                }
                if(f_idx != -1) {
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
        if(score[i] < minscore) {
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
        getTPM(mb);
        getCM(mb);
        
        // WRITE STATES
        //SORT
        for(int i = 0; i < states.size(); i++) {
            for(int j = 0; j < states.size(); j++) {
                if(states_count[j] > states_count[i]) {
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
        sfile.open (states_file);

        for(int i = 0; i < states.size(); i++) {
            for(int j = 0; j < states[i].size(); j++) {
                sfile << states[i][j];
                if(j<states[i].size()-1) sfile << ",";
            }
            sfile << "\n";
        }
        sfile.close();
        
        stringstream ss_statesfile;
        ss_statesfile << FileManager::outputDirectory << "/states_count.csv";
        states_file = ss_statesfile.str();
        
        sfile.open (states_file);
        
        for(int i = 0; i < states_count.size(); i++) {
            sfile << states_count[i] << "\n";
        }
        sfile.close();
        
        // WRITE POSITONS
        stringstream ss;
        ss << FileManager::outputDirectory << "/positions.csv";
        string pos_file = ss.str();
        
        ofstream map;
        map.open (pos_file);
        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < maxOrgs; j++) {
                stringstream val;
                val << '"';
                for(int k = 0; k<worldUpdates; k++) {
                    if (k+1 >= worldUpdates) {
                        val << worldLog[j][i][k];
                    } else {
                        val << worldLog[j][i][k] << '|';
                        
                    }
                }
                val << '"';
                
                if (j+1 >= maxOrgs) {
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
    for (int i = 0; i < gridX ; ++i){
        delete [] this->agentMap[i];
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
    return (senseSides.size() * (senseAgents?2:1)) * maxOrgs; // moving + bridge + goal + comm
    //return groupSize * 12;
}
int SwarmWorld2::requiredOutputs() {
    int maxOrgs = startSlots.size() * nAgents;
    return (2 * maxOrgs);
}

int** SwarmWorld2::zeros(int x, int y) {
    int** m = new int*[x];
    
    for (int i = 0; i < x; i++) {
        m[i] = new int[y];
        for(int j = 0; j < y; j++) {
            m[i][j] = 0;
        }
    }
    
    return m;
}

bool SwarmWorld2::isValid(pair<int,int> loc) {
    if (loc.first < 0) return false;
    if (loc.second < 0) return false;
    if (loc.first  >= gridX) return false;
    if (loc.second >= gridY) return false;
    return true;
    
}

bool SwarmWorld2::isWater(pair<int,int> loc) {
    if(!isValid(loc)) return false;
    if(this->waterMap[loc.first][loc.second] == 2) {
        return true;
    } else {
        return false;
    }
}
bool SwarmWorld2::isGoal(pair<int,int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 4;
}
bool SwarmWorld2::isFloor(pair<int,int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 1;
}
bool SwarmWorld2::isStart(pair<int,int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 3;
}

pair<int,int> SwarmWorld2::getRelativePosition(pair<int,int> loc, int facing, int direction) {
    int dir = ((facing + direction - 1) % 8) - 1;
    if (dir == -1) dir = 7;
    int x = loc.first + SwarmWorld2::RELPOS[dir][0];
    int y = loc.second + SwarmWorld2::RELPOS[dir][1];
    return {x, y};
}

pair<int,int> SwarmWorld2::isGoalInSight(pair<int,int>loc, int facing) {
    pair<int,int> ret;
    ret.first = 0;
    ret.second = 0;
    switch(facing) {
        case 1:
            if (loc.first < avgGoal.first && loc.second <= avgGoal.second) ret.first = 1;
            if (loc.first < avgGoal.first && loc.second >= avgGoal.second) ret.second = 1;
            break;
        case 3:
            if (loc.first <= avgGoal.first && loc.second > avgGoal.second) ret.first = 1;
            if (loc.first >= avgGoal.first && loc.second > avgGoal.second) ret.second = 1;
            break;
        case 5:
            if (loc.first > avgGoal.first && loc.second <= avgGoal.second) ret.first = 1;
            if (loc.first < avgGoal.first && loc.second >= avgGoal.second) ret.second = 1;
            break;
        case 7:
            if (loc.first >= avgGoal.first && loc.second > avgGoal.second) ret.first = 1;
            if (loc.first <= avgGoal.first && loc.second > avgGoal.second) ret.second = 1;
            break;
    }
    return ret;
}


int SwarmWorld2::distance(pair<int, int> a, pair<int,int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}


int** SwarmWorld2::levelThree() {
    // simple path from left to right
    int** mat = zeros(gridX, gridY);
    
    string fileName = "./level2.csv";
    std::ifstream file(fileName);
    
    for(int row = 0; row < gridY; ++row)
    {
        std::string line;
        std::getline(file, line);
        if ( !file.good() )
            break;
        
        std::stringstream iss(line);
        
        for (int col = 0; col < gridX; ++col)
        {
            std::string val;
            std::getline(iss, val, ' ');
            if ( !iss.good() )
                break;
            
            std::stringstream convertor(val);
            convertor >> mat[col][row];
        }
    }
    
    
    return mat;
}

void SwarmWorld2::showMat(int** mat, int x, int y) {
    for (int i = 0; i < y; i++) {
        for(int j = 0; j < x; j++) {
            if (mat[j][i] > -1) {
                
                cout << ' ' << mat[j][i] << ' ';
            } else {
                cout << mat[j][i] << ' ';
                
            }
        }
        cout << "\n";
    }
    cout << "\n";
}

void SwarmWorld2::writeMap() {
    
    ofstream map;
    std::remove("map.csv");
    map.open ("map.csv",ios::app);
    for (int i = 0; i < gridY; i++) {
        for(int j = 0; j < gridX; j++) {
            if (j+1 >= gridX) {
                map << waterMap[j][i];
            } else {
                map << waterMap[j][i] << ',';
                
            }
        }
        map << "\n";
    }
    map.close();
}

int ** SwarmWorld2::getTPM(shared_ptr<MarkovBrain> brain) {
    
    // EXPECT THAT HIDDEN NODES ARE IN THE END OF THE NODE LIST (VERIFIED)
    int n = brain->nrNodes;
    int n_states = pow(2,n);
    int** mat = zeros(n, n_states);
    
    
    for (int i = 0; i < n_states; i++) {
        brain->resetBrain();
        
        int* array = new int[32];
        for (int j = 0; j < 32; ++j) {  // assuming a 32 bit int
            array[j] = i & (1 << j) ? 1 : 0;
            //cout << (i & (1 << j) ? 1 : 0);
        }
        //cout << "\n";
        
        for(int j = 0; j < n; j++) {
            if(j < brain->inputValues.size()) {
                brain->inputValues[j] = array[j];
            //} else if (j>=brain->inputValues.size() && j < brain->inputValues.size() + 2) {
                // MAKE SURE THAT OUTPUTS WILL NOT CAUSE ANYTHING (PYPHI-STUFF)
            //    brain->nodes[j] = 0;
            } else {
                // HIDDEN NODES
                brain->nodes[j] = array[j];
            }
        }
        brain->update();
        for(int j = 0; j < n; j++) {
            int val = brain->nodes[j];
            
            mat[j][i] = (val>0?1:0);
        }
    }
    
    ofstream map;
    stringstream ss;
    ss << FileManager::outputDirectory << "/tpm.csv";
    map.open (ss.str());
    for (int i = 0; i < n_states; i++) {
        for(int j = 0; j < n; j++) {
            if (j+1 >= n) {
                map << mat[j][i];
            } else {
                map << mat[j][i] << ' ';
                
            }
        }
        map << "\n";
    }
    map.close();
    
    return mat;
}


vector<vector<int>> SwarmWorld2::getCM(shared_ptr<MarkovBrain> brain) {
    
    
    int n = brain->nrNodes;
    vector<vector<int>> mat = brain->getConnectivityMatrix();
    
    ofstream map;
    stringstream ss;
    ss << FileManager::outputDirectory << "/cm.csv";
    map.open (ss.str());
    for (int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            //cout << mat[i][j] << " ";
            int val = mat[i][j] > 0;
            // DO NOT ALLOW CONNECTIONS TO INPUTS OR FROM OUTPUTS TO SOMEWHERE
            if(j < requiredInputs()) val = 0;
            if(i >= requiredInputs() && i < requiredInputs() + requiredOutputs()) val = 0;
            
            if (j+1 >= n) {
                map << val;
            } else {
                map << val << ' ';
                
            }
        }
        //cout << "\n";
        map << "\n";
    }
    map.close();
    
    return mat;
}


void SwarmWorld2::move(int idx, pair<int,int> newloc, int dir) {
    
    waitForGoal[idx] --;
    if(isGoal(newloc) && waitForGoal[idx]<=0) {
        score[idx] +=1;
        waitForGoal[idx] = waitForGoalI;
    }
    if(hasPenalty && isAgent(newloc)) {
        score[idx] -=penalty;
    }
    oldLocation[idx] = location[idx];
    location[idx] = newloc;
    
    agentMap[newloc.first][newloc.second] += 1;
    if(oldLocation[idx].first > 0 && oldLocation[idx].second > 0) {
        agentMap[oldLocation[idx].first][oldLocation[idx].second] -= 1;
    }
    
}

bool SwarmWorld2::isAgent(pair<int,int> loc) {
    if(!isValid(loc)) return false;
    
    if(agentMap[loc.first][loc.second] > 0) {
        return true;
    } else {
        return false;
    };
}




bool SwarmWorld2::canMove(pair<int,int> locB) {
    bool move = true;
    if(!isValid(locB)) {
        move = false;
    } else if(isWall(locB)) {
        move = false;
    }
    return move;
}


bool SwarmWorld2::isWall(pair<int, int> loc) {
    return isValid(loc) && waterMap[loc.first][loc.second] == 0;
}
