
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

#include "SwarmWorld.h"
#include "util/GridUtils.h"
#include "grid-initializers/GridInitializerFactory.h"
#include "level/SwarmLevel.h"
#include "level/move/PenaltyCollisionStrategy.h"

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
    phero = ((PT == nullptr) ? pheroPL->lookup() : PT->lookupInt("WORLD_SWARM-phero")) == 1;
    waitForGoalInterval = (PT == nullptr) ? waitForGoalPL->lookup() : PT->lookupInt("WORLD_SWARM-waitForGoal");

    gridInitializer = GridInitializerFactory::getFromString(
            (PT == nullptr)
            ? gridInitializerPL->lookup()
            : PT->lookupString("WORLD_SWARM-gridInitializer")
    );

    //todo level factory + level parameter
    std::pair<int, int> dimensions(gridX, gridY);

    level = std::unique_ptr<Level<Field>>(new SwarmLevel(dimensions));
    if (hasPenalty) {
        level->setCollisionStrategy(
                std::unique_ptr<CollisionStrategy<Field>>(new PenaltyCollisionStrategy<Field>(penalty)));
    }
    level->loadFromFile("level.csv", ' ');

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
    cout << waitForGoalInterval << " Waitforgoal\n";


    // columns to be added to ave file
    aveFileColumns.clear();
    aveFileColumns.emplace_back("score");
    std::remove("positions.csv");

    cout << "Build Map:\n";
    this->startSlots = this->buildGrid();
    this->serializer = *new SwarmWorldSerializer();
}


SwarmWorld::~SwarmWorld() {
    this->gridInitializer = nullptr;
    this->level = nullptr;
}


vector<pair<int, int>> SwarmWorld::buildGrid() {
    vector<pair<int, int>> grid = vector<pair<int, int>>();

    for (int i = 0; i < gridY; i++) {
        for (int j = 0; j < gridX; j++) {
            pair<int, int> loc = {j, i};
            if (level->isFieldType(loc, START)) {
                grid.push_back(loc);
            }
        }
    }

    return grid;
}


void SwarmWorld::initializeAgents(GridInitializer &gridInitializer, int organismCount, vector<vector<double>> &previousStates,
                                  vector<std::shared_ptr<Agent>> &organismInfos, const vector<pair<int, int>> &startSlots,
                                  std::vector<std::pair<int, int>> &alreadyUsedLocations) {
    for (int index = 0; index < organismCount; index++) {
        std::shared_ptr<Agent> agent = std::make_shared<Agent>(pair<int, int>({-1, -1}),
                                                               0, 0, 1, waitForGoalInterval);
        std::pair<int, int> nextPosition = gridInitializer
                .getNextPosition(alreadyUsedLocations, startSlots);
        agent->setLocation(nextPosition);

        organismInfos.push_back(agent);
        alreadyUsedLocations.push_back(nextPosition);
        previousStates.emplace_back(vector<double>());

        if (level->get(nextPosition)) {
            level->get(nextPosition)->agent = agent;
        }
    }
}


void SwarmWorld::initializeEvaluation(int visualize, int organismCount,
                                      const shared_ptr<Organism> &org,
                                      vector<std::shared_ptr<Agent>>& organismInfos,
                                      vector<vector<double>> &previousStates,
                                      vector<vector<double>> &pheroMap,
                                      WorldLog &worldLog) {
    if (phero) {
        pheroMap = GridUtils::zerosVector<double>(gridX, gridY);
    }


    //initialize the log
    if (visualize) {
        worldLog.initialize(organismCount, worldUpdates);
    }


    org->brain->resetBrain();
    //place agents
    std::vector<std::pair<int, int>> usedLocations;
    this->initializeAgents(*this->gridInitializer, organismCount, previousStates, organismInfos, this->startSlots,
                           usedLocations);

    this->serializer.withLocation(usedLocations, gridX, gridY);
}


//void SwarmWorld::evaluate(map<string, shared_ptr<Group>> &groups, int analyse, int visualize, int debug) {
//    //this is the usual implementation
//    int popSize = groups["default"]->population.size();
//    for (int i = 0; i < popSize; i++) {
//        evaluateSolo(groups["default"]->population[i], analyse, visualize, debug);
//    }
//
//    //todo instead:
//    /*
//     *
//     *
//     *
//     *
//     *
//     *
//     */
//}


void SwarmWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    //todo MA: organismCount is responsible for how many copies are created
    auto organismCount = static_cast<int>(startSlots.size() * nAgents);
//    auto organismCount = static_cast<int>(10);

    //reset agent positions etc.
    level->reset();

    //information about the world (x,y,time)
    //todo use organismInfo class internally at least
    WorldLog worldLog;
    //stores pointers to every organism/agent
    vector<std::shared_ptr<Agent>> organismInfos;
    //stores every state and how often it occurred in the simulation
    std::vector<OrganismState> organismStates;

    //the values of the nodes of the organisms at the previous iteration
    vector<vector<double>> previousStates;
    vector<vector<double>> pheroMap;

    this->initializeEvaluation(visualize, organismCount, org, organismInfos, previousStates, pheroMap, worldLog);

    int amountOfNodes = (int) dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes.size();

    for (int t = 0; t < worldUpdates; t++) {
        if (phero) {
            pheroMap = decay(pheroMap);
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
            vector<int> organismInputs = this->getInputs(organismInfos[organismIdx]->getLocation(),
                                                         organismInfos[organismIdx]->getFacing(),
                                                         senseSides,
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

            //todo more comments or at least use an enum or something
            int newDirection = 0;
            int direction = organismInfos[organismIdx]->getFacing();
            if (outputs[0] == 1 && outputs[1] == 0) {
                direction = (direction - 2) % 8;
                if (direction < 0) direction += 8;
            } else if (outputs[0] == 0 && outputs[1] == 1) {
                direction = (direction + 2) % 8;
                if (direction < 0) direction += 8;
            } else if (outputs[0] == 1 && outputs[1] == 1) {
                newDirection = 1;
            }

            organismInfos[organismIdx]->setFacing(direction);


            if (newDirection != 0) {
                pair<int, int> new_pos = level->getRelative(organismInfos[organismIdx]->getLocation(),
                                                            organismInfos[organismIdx]->getFacing(), newDirection);
                level->move(organismInfos[organismIdx]->getLocation(), new_pos);
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
                        .setX(organismInfos[orgIndex]->getLocation().first)
                        .setY(organismInfos[orgIndex]->getLocation().second)
                        .setFacing(std::to_string(organismInfos[orgIndex]->getFacing()))
                        .setScore(organismInfos[orgIndex]->getScore());


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


    double globalScore = this->getScore(organismInfos);
    org->dataMap.setOutputBehavior("score", DataMap::AVE | DataMap::LIST);
    org->dataMap.Append("score", globalScore);

    if (visualize) {
        // WRITE BEST BRAIN TPM/CM
        shared_ptr<MarkovBrain> mb = dynamic_pointer_cast<MarkovBrain>(org->brain->makeCopy());

        // WRITE STATES
        //sort states by amount
        std::sort(organismStates.begin(), organismStates.end(),
                  [](OrganismState stateA, OrganismState stateB) -> int { return stateB.amount - stateA.amount; });


        this->serializer
                .with(organismStates)
                .with(worldLog)
                .with(globalScore)
                .withBrain(*mb, requiredInputs(), requiredOutputs())
                .serialize();
    }

    generation++;

    level->reset();
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


int SwarmWorld::distance(pair<int, int> a, pair<int, int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

double SwarmWorld::getScore(const std::vector<std::shared_ptr<Agent>> &organismInfos) {
    double score = 0;
    for(const auto &agent: organismInfos){
        score += agent->getScore();
    }
    return score / organismInfos.size();
}

vector<int> SwarmWorld::getInputs(std::pair<int, int> location, int facing, std::vector<int> senseSides,
                                  std::vector<std::vector<double>> &pheroMap, bool phero, bool senseAgents) {
    std::vector<int> organismInputs;
    for (int senseSide : senseSides) {
        pair<int, int> loc = level->getRelative(location, facing, senseSide);
        organismInputs.push_back(
                level->getMoveValidityStrategy()->isValid(level.get(), std::pair<int, int>{-1, -1}, loc)
        );

        if (senseAgents) {
            int isAgent = 0;
            //the field is not out of bounds
            if (level->get(loc)) {
                //the field hosts an agent on it
                isAgent = !!level->get(loc)->agent;
            }

            organismInputs.push_back(isAgent);
        }
    }
    if (phero) {
        for (int i = 1; i <= 4; i++) {
            pair<int, int> loc = level->getRelative(location, facing, i);
            organismInputs.push_back(pheroMap[loc.first][loc.second] > 0.5);
        }

    }
    return organismInputs;
}

vector<vector<double>> SwarmWorld::decay(vector<vector<double>> &pheroMap) {
    for (auto &row : pheroMap) {
        for (auto &cell: row) {
            cell *= DECAY_RATE;
        }
    }
    return pheroMap;
}
