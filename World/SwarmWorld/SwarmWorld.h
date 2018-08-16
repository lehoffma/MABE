

#ifndef __BasicMarkovBrainTemplate__WorldSwarm__
#define __BasicMarkovBrainTemplate__WorldSwarm__

#include "../AbstractWorld.h"

#include "../../Brain/MarkovBrain/MarkovBrain.h"
#include "model/WorldLogEntry.h"
#include "model/WorldLog.h"
#include "model/OrganismState.h"
#include "serialize/Serializer.h"
#include "serialize/SwarmWorldSerializer.h"
#include "grid-initializers/GridInitializer.h"
#include "model/Agent.h"
#include "level/Level.h"
#include "level/Field.h"
#include "scoring/OrganismScoringStrategy.h"
#include "model/OrganismStateContainer.h"
#include "movement-penalties/MovementPenaltyManager.h"

#include <cstdlib>
#include <thread>
#include <vector>

using namespace std;

class SwarmWorld : public AbstractWorld {

protected:

    /**
     *
     * @param organismCount
     * @param previousStates
     * @param location
     * @param oldLocation
     * @param score
     * @param facing
     * @param waitForGoal
     * @param startSlots
     */
    virtual void initializeAgents(const shared_ptr<Organism> &org, GridInitializer &gridInitializer, int organismCount,
                                  vector<std::shared_ptr<Agent>> &organismInfos,
                                  const vector<pair<int, int>> &startSlots,
                                  std::vector<std::pair<int, int>> &alreadyUsedLocations);

    /**
     *
     * @param location
     * @param facing
     * @param senseSides
     * @param pheroMap
     * @param phero
     * @param senseAgents
     * @return
     */
    virtual vector<int>
    getInputs(std::pair<int, int> location, AbsoluteDirection facing, std::vector<int> senseSides, bool senseAgents);

    const double DECAY_RATE = 0.9;

public:
    std::unique_ptr<SwarmWorldSerializer> serializer;
    std::unique_ptr<GridInitializer> gridInitializer;
    std::unique_ptr<Level<Field>> level;
    std::unique_ptr<OrganismScoringStrategy> organismScoringStrategy;
    std::unique_ptr<MovementPenaltyManager> movementPenaltyManager;

    unordered_map<string, unordered_set<string>> requiredGroups() override;

    /**
     *
     */
    bool senseAgents;
    bool resetOutputs;
    bool hasPenalty;
    bool phero;
    int resetPositionsInterval;
    double nAgents;
    vector<int> senseSides;

    std::string simulationMode;
    int repeats;
    double invalidMovePenalty;

    int gridX;
    int gridY;
    int worldUpdates;
    double penalty;
    int waitForGoalInterval;
    int _dataInterval;

    vector<pair<int, int>> startSlots;


    explicit SwarmWorld(shared_ptr<ParametersTable> _PT = nullptr);

    ~SwarmWorld() override;

    void evaluate(map<string, shared_ptr<Group>> &groups, int analyse = 0, int visualize = 0, int debug = 0) override;

    void evaluateGroup(const std::vector<std::shared_ptr<Organism>>& population, int visualize);


    /**
     * Creates a vector of size <gridX,gridY> containing pairs of increasing values
     * @return
     */
    virtual vector<pair<int, int>> buildGrid();

    unsigned long long int requiredInputs();

    int requiredOutputs = 2;


    int distance(pair<int, int> a, pair<int, int> b);


    void addToDataMap(const vector<shared_ptr<Agent>>& agents);

    void moveAgent(const shared_ptr<Agent> &agent);

};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSwarm__) */
