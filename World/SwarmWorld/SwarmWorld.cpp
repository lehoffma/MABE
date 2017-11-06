
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

#include "SwarmWorld.h"
#include "util/GridUtils.h"
#include "grid-initializers/GridInitializerFactory.h"

shared_ptr<ParameterLink<int>> SwarmWorld::gridXSizePL = Parameters::register_parameter("WORLD_SWARM-gridX", 16,
                                                                                        "size of grid X");
shared_ptr<ParameterLink<int>> SwarmWorld::gridYSizePL = Parameters::register_parameter("WORLD_SWARM-gridY", 10,
                                                                                        "size of grid Y");
shared_ptr<ParameterLink<int>> SwarmWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_SWARM-worldUpdates",
                                                                                           100,
                                                                                           "amount of time a brain is tested");
shared_ptr<ParameterLink<double>> SwarmWorld::nAgentsPL = Parameters::register_parameter("WORLD_SWARM-nAgents", 1.0,
                                                                                         "how many agents in a game in rate");
shared_ptr<ParameterLink<int>> SwarmWorld::senseAgentsPL = Parameters::register_parameter("WORLD_SWARM-senseAgents", 0,
                                                                                          "1 if ants can sense");
shared_ptr<ParameterLink<string>> SwarmWorld::senseSidesPL = Parameters::register_parameter("WORLD_SWARM-senseSides",
                                                                                            (string) "[1]",
                                                                                            "1 if ants can sense");
shared_ptr<ParameterLink<int>> SwarmWorld::resetOutputsPL = Parameters::register_parameter("WORLD_SWARM-resetOutputs",
                                                                                           0,
                                                                                           "1 if outputs should be reseted after one time step");
shared_ptr<ParameterLink<int>> SwarmWorld::pheroPL = Parameters::register_parameter("WORLD_SWARM-phero", 0,
                                                                                    "do it with pheromones sexy"); //lol
shared_ptr<ParameterLink<int>> SwarmWorld::hasPenaltyPL = Parameters::register_parameter("WORLD_SWARM-hasPenalty", 1,
                                                                                         "1 if penalty when agents get hit");
shared_ptr<ParameterLink<double>> SwarmWorld::penaltyPL = Parameters::register_parameter("WORLD_SWARM-penalty", 0.075,
                                                                                         "amount of penalty for hit");
shared_ptr<ParameterLink<int>> SwarmWorld::waitForGoalPL = Parameters::register_parameter("WORLD_SWARM-waitForGoal",
                                                                                          500,
                                                                                            "timestep till the next goal is possible");

shared_ptr<ParameterLink<string>> SwarmWorld::gridInitializerPL = Parameters::register_parameter(
        "WORLD_SWARM-gridInitializer", string("firstAvailable"), "which grid initializer function to use");

SwarmWorld::SwarmWorld(shared_ptr<ParametersTable> _PT) : AbstractWorld(std::move(_PT)) {
    cout << "Using SwarmWorld \n";

    worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_SWARM-worldUpdates");
    gridX = (PT == nullptr) ? gridXSizePL->lookup() : PT->lookupInt("WORLD_SWARM-gridX");
    gridY = (PT == nullptr) ? gridYSizePL->lookup() : PT->lookupInt("WORLD_SWARM-gridY");
    senseAgents = ((PT == nullptr) ? senseAgentsPL->lookup() : PT->lookupInt("WORLD_SWARM-senseAgents")) == 1;
    resetOutputs = ((PT == nullptr) ? resetOutputsPL->lookup() : PT->lookupInt("WORLD_SWARM-resetOutputs")) == 1;
    hasPenalty = ((PT == nullptr) ? hasPenaltyPL->lookup() : PT->lookupInt("WORLD_SWARM-hasPenalty")) == 1;
    nAgents = ((PT == nullptr) ? nAgentsPL->lookup() : PT->lookupDouble("WORLD_SWARM-nAgents"));
    convertCSVListToVector(((PT == nullptr) ? senseSidesPL->lookup() : PT->lookupString("WORLD_SWARM-senseSides")),
                           senseSides);


    penalty = (PT == nullptr) ? penaltyPL->lookup() : PT->lookupDouble("WORLD_SWARM-penalty");
    phero = ((PT == nullptr) ? senseAgentsPL->lookup() : PT->lookupInt("WORLD_SWARM-phero")) == 1;
    waitForGoalI = (PT == nullptr) ? waitForGoalPL->lookup() : PT->lookupInt("WORLD_SWARM-waitForGoal");

    gridInitializer = GridInitializerFactory::getFromString(
            (PT == nullptr)
            ? gridInitializerPL->lookup()
            : PT->lookupString("WORLD_SWARM-gridInitializer")
    );

    generation = 0;

    cout << worldUpdates << " Updates\n";
    cout << gridX << " X\n";
    cout << gridY << " Y\n";
    cout << senseAgents << " Sensor Agents\n";
    cout << resetOutputs << " Reset Outputs\n";
    cout << hasPenalty << " Penalty set\n";
    cout << nAgents << " factor agents\n";
    cout << PT->lookupString("WORLD_SWARM-senseSides") << " SenseSides\n";
    cout << penalty << " Penalty\n";
    cout << waitForGoalI << " Waitforgoal\n";


    // columns to be added to ave file
    aveFileColumns.clear();
    aveFileColumns.emplace_back("score");
    std::remove("positions.csv");

    cout << "Build Map:\n";
    this->waterMap = this->levelThree();
    this->startSlots = this->buildGrid();
    this->serializer = *new SwarmWorldSerializer();
}

vector<pair<int, int>> SwarmWorld::buildGrid() {
    vector<pair<int, int>> grid = vector<pair<int, int>>();

    for (int i = 0; i < gridY; i++) {
        for (int j = 0; j < gridX; j++) {
            pair<int, int> loc = {j, i};
            if (isStart(loc)) {
                grid.push_back(loc);
            }
        }
    }

    return grid;
}


void SwarmWorld::initializeAgents(GridInitializer &gridInitializer, int organismCount,
                                  vector<vector<double>> &previousStates,
                                  vector<pair<int, int>> &location,
                                  vector<pair<int, int>> &oldLocation, vector<double> &score, vector<int> &facing,
                                  vector<double> &waitForGoal, const vector<pair<int, int>> &startSlots) {
    for (int index = 0; index < organismCount; index++) {
        location.emplace_back(pair<int, int>({-1, -1}));
        oldLocation.emplace_back(-1, -1);
        score.push_back(0);
        facing.push_back(1);
        waitForGoal.push_back(0);
        previousStates.emplace_back(vector<double>());

        std::pair<int, int> nextPosition = gridInitializer.getNextPosition(location, startSlots);
        move(index, nextPosition, 1);
    }
}


void SwarmWorld::initializeEvaluation(int visualize, int organismCount,
                                      const shared_ptr<Organism> &org,
                                      vector<vector<double>> &previousStates,
                                      WorldLog &worldLog) {
    if (phero) {
        this->pheroMap = GridUtils::zeros<double>(this->gridX, this->gridY);
    }
    this->agentMap = GridUtils::zeros<int>(this->gridX, this->gridY);


    //initialize the log
    if (visualize) {
        worldLog.initialize(organismCount, worldUpdates);
    }


    org->brain->resetBrain();
    //place agents
    this->initializeAgents(*this->gridInitializer, organismCount, previousStates, this->location, this->oldLocation,
                           this->score, this->facing, this->waitForGoal, this->startSlots);

    this->serializer.withLocation(this->location, gridX, gridY);
}

void SwarmWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    //todo MA: organismCount is responsible for how many copies are created
//    auto organismCount = static_cast<int>(startSlots.size() * nAgents);
    auto organismCount = static_cast<int>(10);

    //information about the world (x,y,time)
    //todo organismInfo class?
    WorldLog worldLog;
    //stores every state and how often it occurred in the simulation
    std::vector<OrganismState> organismStates;

    //the values of the nodes of the organisms at the previous iteration
    vector<vector<double>> previousStates;

    this->initializeEvaluation(visualize, organismCount, org, previousStates, worldLog);

    int amountOfNodes = (int) dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes.size();

    for (int t = 0; t < worldUpdates; t++) {
        if (phero) {
            decay();
        }

        for (int organismIdx = 0; organismIdx < organismCount; organismIdx++) {

            // SET SHARED BRAIN TO OLD STATE
            if (previousStates[organismIdx].size() == amountOfNodes) {
                dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes = previousStates[organismIdx];
            }

            // RESET OUTPUTS TO ZERO, TO AVOID CONNECTIONS FROM OUTPUT TO HIDDEN/INPUT
            if (resetOutputs) {
                org->brain->setOutput(0, 0);
                org->brain->setOutput(1, 0);
                dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[requiredInputs()] = 0;
                dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[requiredInputs() + 1] = 0;
            }


            //set inputs of brain
            vector<int> organismInputs = this->getInputs(location[organismIdx], facing[organismIdx], senseSides,
                                                         pheroMap, phero, senseAgents);
            for (int j = 0; j < organismInputs.size(); j++) {
                dynamic_pointer_cast<MarkovBrain>(org->brain)->setInput(j, organismInputs[j]);
            }


            // UPDATE BRAINS
            dynamic_pointer_cast<MarkovBrain>(org->brain)->update();
            vector<int> outputs;

            for (int i = 0; i < requiredOutputs(); i++) {
                outputs.push_back(Bit(org->brain->readOutput(i)));
            }

            int newDirection = 0;
            int direction = facing[organismIdx];
            if (outputs[0] == 1 && outputs[1] == 0) {
                direction = (direction - 2) % 8;
                if (direction < 0) direction += 8;
            } else if (outputs[0] == 0 && outputs[1] == 1) {
                direction = (direction + 2) % 8;
                if (direction < 0) direction += 8;
            } else if (outputs[0] == 1 && outputs[1] == 1) {
                newDirection = 1;
            }

            facing[organismIdx] = direction;


            if (newDirection != 0) {
                pair<int, int> new_pos = getRelativePosition(location[organismIdx], facing[organismIdx], newDirection);
                if (canMove(new_pos)) {
                    move(organismIdx, new_pos, direction);
                }
            }
            // SET SHARED BRAIN TO OLD STATE

            previousStates[organismIdx].clear();
            for (int i = 0; i < amountOfNodes; i++) {
                previousStates[organismIdx].push_back(
                        reinterpret_cast<int &&>(dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes[i]));
            }
        }


        if (visualize) {
            for (int orgIndex = 0; orgIndex < organismCount; orgIndex++) {
                //write organism's data into world log
                worldLog[orgIndex][t]
                        .setX(location[orgIndex].first)
                        .setY(location[orgIndex].second)
                        .setFacing(std::to_string(facing[orgIndex]))
                        .setScore(score[orgIndex]);


                //state of the current organism
                vector<double> nodes = dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes;
                vector<int> state(nodes.size());
                std::transform(nodes.begin(), nodes.end(), state.begin(),
                               [](double nodeValue) { return nodeValue > 0 ? 1 : 0; });

                //search for index of first state of states of other organisms we've added so far that is completely equal
                //to the state data of the organism we're currently analyzing
                int index;
                auto iterator = std::find_if(organismStates.begin(), organismStates.end(),
                                             [state](const OrganismState &entry) -> bool {
                                                 return entry.state == state;
                                             });
                //state is not part of the states => add them to the list
                if (iterator == organismStates.end()) {
                    organismStates.push_back(*new OrganismState(state, 1));
                }
                    //or just increase frequency count if already part of the list
                else {
                    index = std::distance(organismStates.begin(), iterator);
                    organismStates[index].amount++;
                }
            }
        }
    }

    // CALCULATE SCORE
    //todo MA: scoring function
    double globalScore = this->getScore(this->score);
    org->dataMap.setOutputBehavior("score", DataMap::AVE | DataMap::LIST);
    org->dataMap.Append("score", globalScore);

    if (visualize) {
        // WRITE BEST BRAIN TPM/CM
        shared_ptr<MarkovBrain> mb = dynamic_pointer_cast<MarkovBrain>(org->brain->makeCopy());
        getTPM(mb);
        getCM(mb);

        // WRITE STATES
        //sort states by amount
        std::sort(organismStates.begin(), organismStates.end(),
                  [](OrganismState stateA, OrganismState stateB) -> int { return stateB.amount - stateA.amount; });

        this->serializer
                .with(organismStates)
                .with(worldLog)
                .with(globalScore)
                .serialize();
    }

    generation++;

    //clean up of member variables
    for (int i = 0; i < gridX; ++i) {
        delete[] this->agentMap[i];
        if (phero) delete[] this->pheroMap[i];
    }

    location.clear();
    oldLocation.clear();
    score.clear();
    waitForGoal.clear();
    facing.clear();
    for (auto &oldState : previousStates) {
        oldState.clear();
    }
    previousStates.clear();
}


int SwarmWorld::requiredInputs() {
    return (senseSides.size() * (senseAgents ? 2 : 1)) + (phero ? 4 : 0); // moving + bridge + goal + comm
}

int SwarmWorld::requiredOutputs() {
    return (2);
}

void SwarmWorld::decay() {
    for (int i = 0; i < gridX; i++) {
        for (int j = 0; j < gridY; j++) {
            pheroMap[i][j] *= DECAY_RATE;
        }
    }
}


bool SwarmWorld::isValid(pair<int, int> loc) {
    if (loc.first < 0) return false;
    if (loc.second < 0) return false;
    if (loc.first >= gridX) return false;
    return loc.second < gridY;

}

bool SwarmWorld::isWater(pair<int, int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 2;
}

bool SwarmWorld::isGoal(pair<int, int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 4;
}

bool SwarmWorld::isFloor(pair<int, int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 1;
}

bool SwarmWorld::isStart(pair<int, int> loc) {
    return isValid(loc) && this->waterMap[loc.first][loc.second] == 3;
}

pair<int, int> SwarmWorld::getRelativePosition(pair<int, int> loc, int facing, int direction) {
    int dir = ((facing + direction - 1) % 8) - 1;
    if (dir == -1) dir = 7;
    int x = loc.first + SwarmWorld::RELPOS[dir][0];
    int y = loc.second + SwarmWorld::RELPOS[dir][1];
    return {x, y};
}

pair<int, int> SwarmWorld::isGoalInSight(pair<int, int> loc, int facing) {
    pair<int, int> ret;
    ret.first = 0;
    ret.second = 0;
    switch (facing) {
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


int SwarmWorld::distance(pair<int, int> a, pair<int, int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}


int **SwarmWorld::levelThree() {
    // simple path from left to right
    int **mat = GridUtils::zeros<int>(gridX, gridY);

    string fileName = "./level.csv";
    std::ifstream file(fileName);

    for (int row = 0; row < gridY; ++row) {
        std::string line;
        std::getline(file, line);
        if (!file.good())
            break;

        std::stringstream iss(line);

        for (int col = 0; col < gridX; ++col) {
            std::string val;
            std::getline(iss, val, ' ');
            if (!iss.good())
                break;

            std::stringstream converter(val);
            converter >> mat[col][row];
        }
    }


    return mat;
}

void SwarmWorld::showMat(int **mat, int x, int y) {
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
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

void SwarmWorld::writeMap() {

    ofstream map;
    std::remove("map.csv");
    map.open("map.csv", ios::app);
    for (int i = 0; i < gridY; i++) {
        for (int j = 0; j < gridX; j++) {
            if (j + 1 >= gridX) {
                map << waterMap[j][i];
            } else {
                map << waterMap[j][i] << ',';

            }
        }
        map << "\n";
    }
    map.close();
}

int **SwarmWorld::getTPM(shared_ptr<MarkovBrain> brain) {

    // EXPECT THAT HIDDEN NODES ARE IN THE END OF THE NODE LIST (VERIFIED)
    int n = brain->nrNodes;
    int n_states = pow(2, n);
    int **mat = GridUtils::zeros<int>(n, n_states);


    for (int i = 0; i < n_states; i++) {
        brain->resetBrain();

        auto *array = new int[32];
        for (int j = 0; j < 32; ++j) {  // assuming a 32 bit int
            array[j] = i & (1 << j) ? 1 : 0;
        }

        for (int j = 0; j < n; j++) {
            if (j < brain->inputValues.size()) {
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
        for (int j = 0; j < n; j++) {
            int val = brain->nodes[j];

            mat[j][i] = (val > 0 ? 1 : 0);
        }
    }

    ofstream map;
    stringstream ss;
    ss << FileManager::outputDirectory << "/tpm.csv";
    map.open(ss.str());
    for (int i = 0; i < n_states; i++) {
        for (int j = 0; j < n; j++) {
            if (j + 1 >= n) {
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


vector<vector<int>> SwarmWorld::getCM(shared_ptr<MarkovBrain> brain) {


    int n = brain->nrNodes;
    vector<vector<int>> mat = brain->getConnectivityMatrix();

    ofstream map;
    stringstream ss;
    ss << FileManager::outputDirectory << "/cm.csv";
    map.open(ss.str());
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            //cout << mat[i][j] << " ";
            int val = mat[i][j] > 0;
            // DO NOT ALLOW CONNECTIONS TO INPUTS OR FROM OUTPUTS TO SOMEWHERE
            if (j < requiredInputs()) val = 0;
            if (i >= requiredInputs() && i < requiredInputs() + requiredOutputs()) val = 0;

            if (j + 1 >= n) {
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


void SwarmWorld::move(int organismIndex, pair<int, int> newloc, int dir) {

    //todo MA: score
    waitForGoal[organismIndex]--;
    if (isGoal(newloc) && waitForGoal[organismIndex] <= 0) {
        score[organismIndex] += 1;
        waitForGoal[organismIndex] = waitForGoalI;
    }
    if (hasPenalty && isAgent(newloc)) {
        score[organismIndex] -= penalty;
    }
    oldLocation[organismIndex] = location[organismIndex];
    location[organismIndex] = newloc;

    agentMap[newloc.first][newloc.second] += 1;
    if (phero) {
        pheroMap[newloc.first][newloc.second] = 1;
    }
    if (oldLocation[organismIndex].first > 0 && oldLocation[organismIndex].second > 0) {
        agentMap[oldLocation[organismIndex].first][oldLocation[organismIndex].second] -= 1;
    }

}

bool SwarmWorld::isAgent(pair<int, int> loc) {
    if (!isValid(loc)) return false;

    return agentMap[loc.first][loc.second] > 0;
}


bool SwarmWorld::canMove(pair<int, int> locB) {
    return isValid(locB) && !isWall(locB);
}


bool SwarmWorld::isWall(pair<int, int> loc) {
    return isValid(loc) && waterMap[loc.first][loc.second] == 0;
}

double SwarmWorld::getScore(const std::vector<double> &scores) {
    return std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
}

vector<int> SwarmWorld::getInputs(std::pair<int, int> location, int facing, std::vector<int> senseSides,
                                  double **pheroMap, bool phero, bool senseAgents) {
    std::vector<int> organismInputs;
    for (int senseSide : senseSides) {
        pair<int, int> loc = getRelativePosition(location, facing, senseSide);
        organismInputs.push_back(canMove(loc));

        if (senseAgents) {
            organismInputs.push_back(isAgent(loc));
        }
    }
    if (phero) {
        for (int i = 1; i <= 4; i++) {
            pair<int, int> loc = getRelativePosition(location, facing, i);
            organismInputs.push_back(pheroMap[loc.first][loc.second] > 0.5);
        }

    }
    return organismInputs;
}
