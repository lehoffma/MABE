
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <utility>

#include "SwarmWorld.h"
#include "util/GridUtils.h"
#include "grid-initializers/GridInitializerFactory.h"
#include "level/SwarmLevel.h"
#include "level/move/PenaltyCollisionStrategy.h"
#include "util/DirectionUtils.h"
#include "scoring/OrganismScoringStrategyFactory.h"
#include "model/OrganismStateContainer.h"
#include "movement-penalties/StandingStillPenalty.h"
#include "movement-penalties/SpinningPenalty.h"
#include "SwarmWorldParameters.h"
#include "SwarmWorldVisualizer.h"

SwarmWorld::SwarmWorld(shared_ptr<ParametersTable> _PT) : AbstractWorld(std::move(_PT)) {
    cout << "Using SwarmWorld \n";

    SwarmWorldParameters::initSwarmWorld(*this);

    // columns to be added to ave file
    popFileColumns.clear();
    popFileColumns.emplace_back("score");
    popFileColumns.emplace_back("gatePassages");
    popFileColumns.emplace_back("collisions");
    std::remove("positions.csv");

    cout << "Build Map:\n";
    this->startSlots = this->buildGrid();
    this->serializer = *new SwarmWorldSerializer();

    cout << "Available slots: " << this->startSlots.size() << std::endl;
}


SwarmWorld::~SwarmWorld() {
    this->gridInitializer = nullptr;
    this->level = nullptr;
}


void SwarmWorld::evaluate(map<string, shared_ptr<Group>> &groups, int analyse, int visualize, int debug) {
    const auto population = groups["root::"]->population;

    auto resetPositions = resetPositionsInterval > 0 && Global::update % resetPositionsInterval == 0;
    for (auto &org: population) {
        //reset datamap values from previous generations
        org->dataMap.set("score", std::vector<double>{});
        org->dataMap.set("gatePassages", std::vector<double>{});
        org->dataMap.set("collisions", std::vector<double>{});
        org->dataMap.set("directionHistory", std::vector<int>{});
        if (resetPositions) {
            org->dataMap.set("xPos", std::vector<int>{});
            org->dataMap.set("yPos", std::vector<int>{});
        }
    }

    if (this->simulationMode == "heterogeneous") {
        evaluateGroup(population, visualize);
    } else {
        std::cout << "Invalid simulation mode value '" << this->simulationMode << "'." << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(1);
    }
}


void SwarmWorld::evaluateGroup(const std::vector<std::shared_ptr<Organism>> population,
                               int visualize) {
    unsigned long long int popSize = population.size();
    //how many slots are available
    auto maxAmountOfAgents = static_cast<int>(std::ceil(startSlots.size() * nAgents));

    //there are too few slots available for the whole population
    if (popSize > maxAmountOfAgents) {
        std::cout << "Too many Organisms! Please choose a different level or choose"
                  << " a popSize value that is lower or equal to the available"
                  << " slot size of " << maxAmountOfAgents << "." << std::endl;
        exit(1);
    }

    auto amountOfCopies = static_cast<int>(std::floor(maxAmountOfAgents / popSize));
    //reset agent positions etc.
    level->reset();

    //write all scores of each organisms and their average to the data file
    for (const auto &org: population) {
        org->dataMap.setOutputBehavior("score", DataMap::AVE | DataMap::LIST);
        org->dataMap.setOutputBehavior("collisions", DataMap::AVE | DataMap::LIST);
        org->dataMap.setOutputBehavior("movementPenalties", DataMap::AVE | DataMap::LIST);
        org->dataMap.setOutputBehavior("gatePassages", DataMap::AVE | DataMap::LIST);
    }

    const int maxIteration = visualize ? 1 : repeats;
    for (int iteration = 0; iteration < maxIteration; iteration++) {
        //information about the world (x,y,time)
        //todo use agent class internally at least
        WorldLog worldLog;
        //stores pointers to every organism/agent
        vector<std::shared_ptr<Agent>> agents;
        //stores every state and how often it occurred in the simulation, grouped by organism
        std::vector<OrganismStateContainer> organismStates(popSize);
        //stores the previous states of the given organism (by their ID)
        std::unordered_map<int, std::vector<double>> previousStates{};

        //initialize the log
        if (visualize) {
            worldLog.initialize(maxAmountOfAgents, worldUpdates);
        }

        //place agents
        std::vector<std::pair<int, int>> usedLocations;
        for (int i = 0; i < popSize; i++) {
            //reset input/output in case there are any leftover values
            population[i]->brain->resetBrain();
            this->initializeAgents(population[i], *this->gridInitializer, amountOfCopies,
                                   agents, this->startSlots, usedLocations);
        }
        //todo save the order the locations were used in
        this->serializer.withLocation(usedLocations, gridX, gridY);

        //for every world/simulation update
        for (int t = 0; t < worldUpdates; t++) {
            //for every organism
            for (auto &agent: agents) {
                moveAgent(agent, previousStates);
            }


            if (visualize) {
                SwarmWorldVisualizer::serializeCompleteWorldUpdate(population, worldLog, agents, organismStates,
                                                                   amountOfCopies, t);
            }
        }

        //calculate scores for every organism in the population
        this->addToDataMap(agents, population);

        if (visualize) {
            SwarmWorldVisualizer::serializeEndResult(population, worldLog, organismStates,
                                                     static_cast<int>(requiredInputs()), requiredOutputs, serializer);
        }
    }
}


void
SwarmWorld::addToDataMap(vector<shared_ptr<Agent>> agents, const std::vector<std::shared_ptr<Organism>> population) {
    //calculate scores for every organism in the population
    std::unordered_map<int, std::vector<double>> scoreMap = organismScoringStrategy->getOrganismScores(agents);

    for (const auto &org: population) {
        for (const auto &score: scoreMap[org->ID]) {
            org->dataMap.append("score", score);
        }
    }

    std::unordered_map<int, std::vector<double>> collisionMap;
    for (const auto &agent: agents) {
        collisionMap[agent->getOrganism()->ID].push_back(agent->getCollisions());
    }
    for (const auto &org: population) {
        for (const auto &collisions: collisionMap[org->ID]) {
            org->dataMap.append("collisions", collisions);
        }
    }

    std::unordered_map<int, std::vector<double>> gatePassagesMap;
    for (const auto &agent: agents) {
        gatePassagesMap[agent->getOrganism()->ID].push_back(agent->getGatePassages());
    }
    for (const auto &org: population) {
        for (const auto &gatePassages: gatePassagesMap[org->ID]) {
            org->dataMap.append("gatePassages", gatePassages);
        }
    }

    std::unordered_map<int, std::vector<double>> movementPenaltyMap;
    for (const auto &agent: agents) {
        movementPenaltyMap[agent->getOrganism()->ID].push_back(agent->getMovementPenalties());
    }
    for (const auto &org: population) {
        for (const auto &movementPenalty: movementPenaltyMap[org->ID]) {
            org->dataMap.append("movementPenalties", movementPenalty);
        }
    }

    std::unordered_map<int, std::vector<std::pair<int, int>>> positionsMap;
    for (const auto &agent: agents) {
        positionsMap[agent->getOrganism()->ID].push_back(agent->getLocation());
    }
    for (const auto &org: population) {
        for (const auto &position: positionsMap[org->ID]) {
            org->dataMap.append("xPos", position.first);
            org->dataMap.append("yPos", position.second);
        }
    }
}


void SwarmWorld::moveAgent(const shared_ptr<Agent> &agent,
                           std::unordered_map<int, std::vector<double>> &previousStates) {
    auto ID = agent->getOrganism()->ID;


    // SET SHARED BRAIN TO OLD STATE, if it exists
    if (previousStates.count(ID) > 0) {
        dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes = previousStates[ID];
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
                                           senseSides, senseAgents);
    for (int j = 0; j < organismInputs.size(); j++) {
        agent->getOrganism()->brain->setInput(j, organismInputs[j]);
    }


    // UPDATE BRAINS
    agent->getOrganism()->brain->update();
    vector<int> outputs;

    for (int i = 0; i < requiredOutputs; i++) {
        outputs.push_back(Bit(agent->getOrganism()->brain->readOutput(i)));
    }

    //interpret the brain's output
    bool moveForwards = false;
    AbsoluteDirection facingDirection = agent->getFacing();
    if (outputs[0] == 1 && outputs[1] == 0) {
        //turn right by 90°
        facingDirection = DirectionUtils::turn(facingDirection, TurningDirection::RIGHT, 2);
        agent->addToDirectionHistory(RelativeDirection::RIGHT);
    } else if (outputs[0] == 0 && outputs[1] == 1) {
        //turn left by 90°
        facingDirection = DirectionUtils::turn(facingDirection, TurningDirection::LEFT, 2);
        agent->addToDirectionHistory(RelativeDirection::LEFT);
    } else if (outputs[0] == 1 && outputs[1] == 1) {
        agent->addToDirectionHistory(RelativeDirection::NONE);
    } else {
        //drive forwards
        moveForwards = true;
    }
    agent->setFacing(facingDirection);

    if (moveForwards) {
        pair<int, int> new_pos = DirectionUtils::getRelativePosition(agent->getLocation(), agent->getFacing(),
                                                                     RelativeDirection::FORWARDS);
        bool moveWasSuccessful = level->move(agent->getLocation(), new_pos);
        if (moveWasSuccessful) {
            agent->addToDirectionHistory(RelativeDirection::FORWARDS);
        } else {
            agent->addToDirectionHistory(RelativeDirection::NONE);
        }
    }

    //apply invalid move penalty if the agent didn't move
    // e.g. because the new pos. would be out of bounds or because it had to rotate
    double movementPenalty = movementPenaltyManager->getPenalty(agent);
    agent->setScore(agent->getScore() - movementPenalty);
    agent->setMovementPenalties(agent->getMovementPenalties() + movementPenalty);

    // SET SHARED BRAIN TO OLD STATE
    previousStates[ID].clear();
    auto amountOfNodes = dynamic_pointer_cast<MarkovBrain>(agent->getOrganism()->brain)->nodes.size();
    for (int i = 0; i < amountOfNodes; i++) {
        previousStates[ID].push_back(
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
        //reuse position if specified, otherwise initialize with gridInitializer
        std::pair<int, int> agentPosition{-1, -1};
        if (organismCount != 1 ||
            !org->dataMap.fieldExists("xPos") || !org->dataMap.fieldExists("yPos")
            || org->dataMap.getIntVector("xPos").empty()
            || org->dataMap.getIntVector("yPos").empty()) {
            std::pair<int, int> nextPosition = gridInitializer
                    .getNextPosition(alreadyUsedLocations, startSlots);
            agentPosition.first = nextPosition.first;
            agentPosition.second = nextPosition.second;
        } else {
            auto xPositions = org->dataMap.getIntVector("xPos");
            auto yPositions = org->dataMap.getIntVector("yPos");
            agentPosition.first = xPositions[xPositions.size() - 1];
            agentPosition.second = yPositions[yPositions.size() - 1];
        }

        agent->setLocation(agentPosition);

        organismInfos.push_back(agent);
        alreadyUsedLocations.push_back(agentPosition);

        if (level->get(agentPosition)) {
            level->get(agentPosition)->agent = agent;
        }
    }
}


unsigned long long int SwarmWorld::requiredInputs() {
    return (senseSides.size() * (senseAgents ? 2 : 1)) + (phero ? 4 : 0); // moving + bridge + goal + comm
}

int SwarmWorld::distance(pair<int, int> a, pair<int, int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

vector<int> SwarmWorld::getInputs(std::pair<int, int> location, AbsoluteDirection facing,
                                  std::vector<int> senseSides, bool senseAgents) {
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
    return organismInputs;
}

unordered_map<string, unordered_set<string>> SwarmWorld::requiredGroups() {
    // default requires a root group and a brain (in root namespace) and no genome
    return {{"root::", {"B:root::," + to_string(requiredInputs()) + "," + to_string(requiredOutputs)}}};
}
