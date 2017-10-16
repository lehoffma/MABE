

#ifndef __BasicMarkovBrainTemplate__WorldSwarm__
#define __BasicMarkovBrainTemplate__WorldSwarm__

#include "../AbstractWorld.h"

#include "../../Brain/MarkovBrain/MarkovBrain.h"
#include "model/WorldLogEntry.h"
#include "model/WorldLog.h"
#include "model/OrganismState.h"
#include "serialize/Serializer.h"
#include "serialize/SwarmWorldSerializer.h"

#include <cstdlib>
#include <thread>
#include <vector>

using namespace std;

class SwarmWorld : public AbstractWorld {

private:
    SwarmWorldSerializer serializer;

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
    virtual void initializeAgents(int organismCount, vector<vector<double>> &previousStates, vector<pair<int, int>> &location,
                                  vector<pair<int, int>> &oldLocation, vector<double> &score, vector<int> &facing,
                                  vector<double> &waitForGoal, vector<pair<int, int>> startSlots);

    /**
     *
     * @param visualize
     * @param organismCount
     * @param org
     * @param previousStates
     * @param worldLog
     */
    virtual void initializeEvaluation(int visualize, int organismCount,
                                      const shared_ptr<Organism> &org,
                                      vector<vector<double>> &previousStates,
                                      WorldLog &worldLog);

    /**
     *
     * @param scores - the scores of every indivual
     * @return an aggregation of the given list of scores (i.e. the average)
     */
    virtual double getScore(const std::vector<double> &scores);

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
    virtual vector<int> getInputs(std::pair<int,int> location, int facing, std::vector<int> senseSides,
                                  double** pheroMap, bool phero, bool senseAgents);

    const double DECAY_RATE = 0.9;

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

    /**
     * Parameter link describing how often the world should be updated in the evaluation
     */
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
    static shared_ptr<ParameterLink<int>> pheroPL;

    int **levelThree();

    /**
     *
     */
    int generation;
    bool senseAgents;
    bool resetOutputs;
    bool hasPenalty;
    bool phero;
    double nAgents;
    vector<int> senseSides;


    int gridX;
    int gridY;
    int **waterMap;
    int **bridgeMap;
    int **agentMap;
    double **pheroMap;
    int worldUpdates;
    double penalty;
    int waitForGoalI;

    pair<int, int> avgGoal;
    vector<pair<int, int>> startSlots;
    vector<pair<int, int>> location;
    vector<pair<int, int>> oldLocation;
    vector<double> score;
    vector<double> waitForGoal;
    vector<int> facing;

    SwarmWorld(shared_ptr<ParametersTable> _PT = nullptr);

    virtual ~SwarmWorld() = default;

    //virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0)override;
    virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;


    /**
     * Creates a vector of size <gridX,gridY> containing pairs of increasing values
     * @return
     */
    virtual vector<pair<int, int>> buildGrid();

    virtual int requiredInputs() override;

    virtual int requiredOutputs() override;

    void showMat(int **mat, int x, int y);

    void writeMap();

    bool isWater(pair<int, int> loc);

    bool isWall(pair<int, int> loc);

    bool isFloor(pair<int, int> loc);

    bool isAgent(pair<int, int> loc);

    /**
     * Checks whether the given pair is located inside the grid (> 0 and < gridX/gridY)
     * @param loc
     * @return
     */
    bool isValid(pair<int, int> loc);

    //int countAgent(pair<int,int> loc, int group);
    bool isGoal(pair<int, int> loc);

    bool isStart(pair<int, int> loc);

    pair<int, int> isGoalInSight(pair<int, int> loc, int facing);

    void move(int idx, pair<int, int> newloc, int dir);

    void decay();

    bool canMove(pair<int, int> locB);


    //void placeAgent(int idx, pair<int,int> oldloc, pair<int,int> loc, int group, shared_ptr<Organism2D> org);
    virtual pair<int, int> getRelativePosition(pair<int, int> loc, int facing, int direction);

    int distance(pair<int, int> a, pair<int, int> b);


    int **getTPM(shared_ptr<MarkovBrain> brain);

    vector<vector<int>> getCM(shared_ptr<MarkovBrain> brain);
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSwarm__) */
