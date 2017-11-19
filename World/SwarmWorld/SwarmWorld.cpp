
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

#include "SwarmWorld.h"
#include "util/GridUtils.h"
#include "grid-initializers/GridInitializerFactory.h"
#include "level/SwarmLevel.h"
#include "level/move/PenaltyCollisionStrategy.h"
#include "util/DirectionUtils.h"

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


shared_ptr<ParameterLink<string>> SwarmWorld::simulationModePL = Parameters::register_parameter(
        "WORLD_SWARM-simulationMode", string("heterogeneous"),
        "whether to use heterogeneous or homogeneous simulation");


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
    simulationMode = (PT == nullptr) ? simulationModePL->lookup() : PT->lookupString("WORLD_SWARM-simulationMode");

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


void SwarmWorld::evaluate(map<string, shared_ptr<Group>> &groups, int analyse, int visualize, int debug) {
    if (this->simulationMode == "homogeneous") {
        //this is the usual implementation
        int popSize = groups["default"]->population.size();
        for (int i = 0; i < popSize; i++) {
            evaluateSolo(groups["default"]->population[i], analyse, visualize, debug);
        }
    }
        /////
    else if (this->simulationMode == "heterogeneous") {
        //todo test
        //todo organisms are not moving at all anymore
        int popSize = groups["default"]->population.size();
        //how many slots are available
        auto maxAmountOfAgents = startSlots.size() * nAgents;
        //todo at the moment: fills slots until floored value is reached, those that are left will be left empty
        // 77.5 slots
        //popSize = 25
        // 77.5 / 25 = 3.1 => 3
        // 2.5 or 3 slots are still empty => fill with random?
        auto amountOfCopies = static_cast<int>(std::floor(maxAmountOfAgents / popSize));
        //reset agent positions etc.
        level->reset();

        //information about the world (x,y,time)
        //todo use agent class internally at least
        WorldLog worldLog;
        //stores pointers to every organism/agent
        vector<std::shared_ptr<Agent>> agents;
        //stores every state and how often it occurred in the simulation
        std::vector<OrganismState> organismStates;

        //the values of the nodes of the organisms at the previous iteration
        vector<vector<double>> pheroMap = GridUtils::zerosVector<double>(gridX, gridY);


        //initialize the log
        if (visualize) {
            worldLog.initialize(static_cast<int>(maxAmountOfAgents), worldUpdates);
        }

        //place agents
        std::vector<std::pair<int, int>> usedLocations;
        //for every organism: create N copies and put them into agents array
        for (int i = 0; i < popSize; i++) {
            groups["default"]->population[i]->brain->resetBrain();
            this->initializeAgents(groups["default"]->population[i], *this->gridInitializer, amountOfCopies,
                                   agents, this->startSlots, usedLocations);
        }
        this->serializer.withLocation(usedLocations, gridX, gridY);

        //for every world/simulation update
        for (int t = 0; t < worldUpdates; t++) {
            if (phero) {
                pheroMap = decay(pheroMap);
            }
            //for every organism
            for (auto &agent: agents) {
                int amountOfNodes = (int) dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes.size();
                simulateOnce(agent, pheroMap, amountOfNodes);
            }

            if (visualize) {
                int currentIndex = 0;
                for (const auto &org: groups["default"]->population) {
                    //todo test
                    for (int organismIdx = currentIndex; organismIdx < (currentIndex + amountOfCopies); organismIdx++) {
                        serializeWorldUpdate(org, worldLog, agents, organismStates, amountOfCopies, organismIdx, t);
                    }
                    currentIndex += amountOfCopies;
                }
            }
        }


        //calculate score for every organism in the population
        for (const auto &org: groups["default"]->population) {
            //iterate over every agent object that has the same ID as the organism and calculate the average of those
            double globalScore = 0;
            for (const auto &agent: agents) {
                if (org->ID == agent->getOrganism()->ID) {
                    globalScore += agent->getScore();
                }
            }
            globalScore /= amountOfCopies;

            org->dataMap.setOutputBehavior("score", DataMap::AVE | DataMap::LIST);
            org->dataMap.Append("score", globalScore);

            if (visualize) {
                serializeResult(org, worldLog, organismStates, globalScore);
            }
        }
    } else {
        std::cout << "Invalid simulation mode value '" << this->simulationMode << "'." << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(1);
    }
}


void SwarmWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    //reset agent positions etc.
    level->reset();

    //information about the world (x,y,time)
    //todo use agent class internally at least
    WorldLog worldLog;
    //stores pointers to every organism/agent
    vector<std::shared_ptr<Agent>> agents;
    //stores every state and how often it occurred in the simulation
    std::vector<OrganismState> organismStates;

    //the values of the nodes of the organisms at the previous iteration
    vector<vector<double>> pheroMap;

    auto copies = static_cast<int>(startSlots.size() * nAgents);
    this->initializeEvaluation(org, visualize, copies, agents, pheroMap, worldLog);

    org->brain->resetBrain();
    int amountOfNodes = (int) dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes.size();

    for (int t = 0; t < worldUpdates; t++) {
        if (phero) {
            pheroMap = decay(pheroMap);
        }
        for (auto &agent: agents) {
            simulateOnce(agent, pheroMap, amountOfNodes);
        }
        if (visualize) {
            for (int organismIdx = 0; organismIdx < copies; organismIdx++) {
                serializeWorldUpdate(org, worldLog, agents, organismStates, copies, organismIdx, t);
            }
        }
    }

    // CALCULATE SCORE
    double globalScore = this->getScore(agents);
    org->dataMap.setOutputBehavior("score", DataMap::AVE | DataMap::LIST);
    org->dataMap.Append("score", globalScore);

    if (visualize) {
        serializeResult(org, worldLog, organismStates, globalScore);
    }
}

void SwarmWorld::serializeResult(const shared_ptr<Organism> &org, const WorldLog &worldLog,
                                 vector<OrganismState> &organismStates, double globalScore) {
    // WRITE BEST BRAIN TPM/CM
    shared_ptr<MarkovBrain> mb = dynamic_pointer_cast<MarkovBrain>(org->brain->makeCopy());

    // WRITE STATES
    //sort states by amount
    sort(organismStates.begin(), organismStates.end(),
         [](OrganismState stateA, OrganismState stateB) -> int { return stateB.amount - stateA.amount; });


    serializer
            .with(organismStates)
            .with(worldLog)
            .with(globalScore)
            .withBrain(*mb, requiredInputs(), requiredOutputs())
            .serialize();
}

void SwarmWorld::serializeWorldUpdate(const shared_ptr<Organism> &org, WorldLog &worldLog,
                                      const vector<shared_ptr<Agent>> &agents,
                                      vector<OrganismState> &organismStates, int copies, int orgIndex, int t) const {
    //write organism's data into world log
    worldLog[orgIndex][t]
            .setX(agents[orgIndex]->getLocation().first)
            .setY(agents[orgIndex]->getLocation().second)
            .setFacing(to_string(agents[orgIndex]->getFacing()))
            .setScore(agents[orgIndex]->getScore());


    //state of the current organism
    vector<double> nodes = dynamic_pointer_cast<MarkovBrain>(org->brain)->nodes;
    vector<int> state(nodes.size());
    transform(nodes.begin(), nodes.end(), state.begin(),
              [](double nodeValue) { return nodeValue > 0 ? 1 : 0; });

    //search for index of first state of states of other organisms we've added so far that is completely equal
    //to the state data of the organism we're currently analyzing
    int index;
    auto iterator = find_if(organismStates.begin(), organismStates.end(),
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

void SwarmWorld::simulateOnce(const shared_ptr<Agent> &agent, vector<vector<double>> &pheroMap, int amountOfNodes) {
    // SET SHARED BRAIN TO OLD STATE
    if (agent->getPreviousState().size() == amountOfNodes) {
        dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes = agent->getPreviousState();
    }

    // RESET OUTPUTS TO ZERO, TO AVOID CONNECTIONS FROM OUTPUT TO HIDDEN/INPUT
    if (resetOutputs) {
        agent->getOrganism()->brain->setOutput(0, 0);
        agent->getOrganism()->brain->setOutput(1, 0);
        dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes[requiredInputs()] = 0;
        dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes[requiredInputs() + 1] = 0;
    }


    //set inputs of brain
    vector<int> organismInputs = getInputs(agent->getLocation(),
                                           agent->getFacing(),
                                           senseSides,
                                           pheroMap, phero, senseAgents);
    for (int j = 0; j < organismInputs.size(); j++) {
        dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->setInput(j, organismInputs[j]);
    }


    // UPDATE BRAINS
    //todo outputs are always [0,0]
    dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->update();
    vector<int> outputs;

    for (int i = 0; i < requiredOutputs(); i++) {
        outputs.push_back(Bit(agent->getOrganism()->brain->readOutput(i)));
    }

    //interpret the brain's output
    bool moveForwards = false;
    AbsoluteDirection facingDirection = agent->getFacing();
    if (outputs[0] == 1 && outputs[1] == 0) {
        //turn left by 90°
        facingDirection = DirectionUtils::turn(facingDirection, TurningDirection::LEFT, 2);
    } else if (outputs[0] == 0 && outputs[1] == 1) {
        //turn right by 90°
        facingDirection = DirectionUtils::turn(facingDirection, TurningDirection::RIGHT, 2);
    } else if (outputs[0] == 1 && outputs[1] == 1) {
        //drive forwards
        moveForwards = true;
    }
    agent->setFacing(facingDirection);

    if (moveForwards) {
        pair<int, int> new_pos = DirectionUtils::getRelativePosition(agent->getLocation(), agent->getFacing(),
                                                                     RelativeDirection::FORWARDS);
        level->move(agent->getLocation(), new_pos);
    }

    // SET SHARED BRAIN TO OLD STATE
    agent->getPreviousState().clear();
    for (int i = 0; i < amountOfNodes; i++) {
        agent->getPreviousState().push_back(
                reinterpret_cast<int &&>(dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes[i]));
    }
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


void SwarmWorld::initializeAgents(const shared_ptr<Organism> &org, GridInitializer &gridInitializer, int organismCount,
                                  vector<std::shared_ptr<Agent>> &organismInfos,
                                  const vector<pair<int, int>> &startSlots,
                                  std::vector<std::pair<int, int>> &alreadyUsedLocations) {
    for (int index = 0; index < organismCount; index++) {
        std::shared_ptr<Agent> agent = std::make_shared<Agent>(org, pair<int, int>({-1, -1}),
                                                               0, 0, AbsoluteDirection::EAST, waitForGoalInterval);
        std::pair<int, int> nextPosition = gridInitializer
                .getNextPosition(alreadyUsedLocations, startSlots);
        agent->setLocation(nextPosition);

        organismInfos.push_back(agent);
        alreadyUsedLocations.push_back(nextPosition);

        if (level->get(nextPosition)) {
            level->get(nextPosition)->agent = agent;
        }
    }
}


void SwarmWorld::initializeEvaluation(const shared_ptr<Organism> &org, int visualize, int organismCount,
                                      vector<std::shared_ptr<Agent>> &organismInfos,
                                      vector<vector<double>> &pheroMap,
                                      WorldLog &worldLog) {
    if (phero) {
        pheroMap = GridUtils::zerosVector<double>(gridX, gridY);
    }


    //initialize the log
    if (visualize) {
        worldLog.initialize(organismCount, worldUpdates);
    }

    //place agents
    std::vector<std::pair<int, int>> usedLocations;
    this->initializeAgents(org, *this->gridInitializer, organismCount, organismInfos, this->startSlots,
                           usedLocations);

    this->serializer.withLocation(usedLocations, gridX, gridY);
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
    for (const auto &agent: organismInfos) {
        score += agent->getScore();
    }
    return score / organismInfos.size();
}

vector<int> SwarmWorld::getInputs(std::pair<int, int> location, AbsoluteDirection facing,
                                  std::vector<int> senseSides,
                                  std::vector<std::vector<double>> &pheroMap, bool phero, bool senseAgents) {
    std::vector<int> organismInputs;
    for (int senseSide : senseSides) {
        pair<int, int> loc = DirectionUtils::getRelativePosition(location, facing,
                                                                 static_cast<RelativeDirection>(senseSide));

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
            pair<int, int> loc = DirectionUtils::getRelativePosition(location, facing,
                                                                     static_cast<RelativeDirection>(i));
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
