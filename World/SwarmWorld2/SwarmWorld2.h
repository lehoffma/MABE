

#ifndef __BasicMarkovBrainTemplate__WorldSwarm2__
#define __BasicMarkovBrainTemplate__WorldSwarm2__

#include "../AbstractWorld.h"

#include "../../Brain/MarkovBrain/MarkovBrain.h"
#include "../SwarmWorld/SwarmWorld.h"

#include <cstdlib>
#include <thread>
#include <vector>

using namespace std;

class SwarmWorld2 : public SwarmWorld {

public:

    const int DIRECTIONS[8] = {1, 2, 3, 4, 5, 6, 7, 8}; // 1=e, 2=se, 3=s, 4=sw, 5=w, 6=nw, 7=n, 8=ne
    const int RELPOS[8][2] = {{1,  0},
                              {1,  -1},
                              {0,  -1},
                              {-1, -1},
                              {-1, 0},
                              {-1, 1},
                              {0,  1},
                              {1,  1}};

    static shared_ptr<ParameterLink<int>> worldUpdatesPL;
    static shared_ptr<ParameterLink<int>> gridXSizePL;
    static shared_ptr<ParameterLink<int>> gridYSizePL;
    static shared_ptr<ParameterLink<int>> hasPenaltyPL;
    static shared_ptr<ParameterLink<int>> senseAgentsPL;
    static shared_ptr<ParameterLink<int>> detectWaterPL;
    static shared_ptr<ParameterLink<double>> nAgentsPL;
    static shared_ptr<ParameterLink<string>> senseSidesPL;
    static shared_ptr<ParameterLink<int>> resetOutputsPL;
    static shared_ptr<ParameterLink<double>> penaltyPL;
    static shared_ptr<ParameterLink<int>> waitForGoalPL;

    int generation;
    bool senseAgents;
    bool resetOutputs;
    bool hasPenalty;
    double nAgents;
    double penalty;
    int waitForGoalI;
    vector<int> senseSides;


    int gridX;
    int gridY;
    int **waterMap;
    int **bridgeMap;
    int **agentMap;
    int worldUpdates;

    pair<int, int> avgGoal;
    vector<pair<int, int>> startSlots;
    vector<pair<int, int>> location;
    vector<pair<int, int>> oldLocation;
    vector<double> score;
    vector<double> waitForGoal;
    vector<int> facing;

    SwarmWorld2(shared_ptr<ParametersTable> _PT = nullptr);

    ~SwarmWorld2() override = default;

    virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;

    int requiredInputs() override;

    int requiredOutputs() override;

    pair<int, int> SwarmWorld2::getRelativePosition(pair<int, int> loc, int facing, int direction);
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSwarm2__) */
