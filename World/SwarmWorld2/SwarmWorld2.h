

#ifndef __BasicMarkovBrainTemplate__WorldSwarm2__
#define __BasicMarkovBrainTemplate__WorldSwarm2__

#include "../AbstractWorld.h"

#include "../../Brain/MarkovBrain/MarkovBrain.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class SwarmWorld2 : public AbstractWorld {
    
public:
    
    const int DIRECTIONS[8] = {1, 2, 3, 4, 5, 6, 7, 8}; // 1=e, 2=se, 3=s, 4=sw, 5=w, 6=nw, 7=n, 8=ne
    const int RELPOS[8][2] = {{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
    
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
    
    int** levelThree();
    
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
    
    pair<int,int> avgGoal;
    vector<pair<int,int>> startSlots;
    vector<pair<int,int>> location;
    vector<pair<int,int>> oldLocation;
    vector<double> score;
    vector<double> waitForGoal;
    vector<int> facing;
    
    SwarmWorld2(shared_ptr<ParametersTable> _PT = nullptr);
    virtual ~SwarmWorld2() = default;
    
    //virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0)override;
    virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;
    
    virtual void buildGrid();
    
    virtual int requiredInputs() override;
    virtual int requiredOutputs() override;
    //virtual int maxOrgsAllowed();
    //virtual int minOrgsAllowed();
    
    int** zeros(int x, int y);
    void showMat(int** mat, int x, int y);
    void writeMap();
    
    bool isWater(pair<int,int> loc);
    bool inWater(pair<int,int> loc);
    bool isWall(pair<int,int> loc);
    bool isFloor(pair<int,int> loc);
    bool isAgent(pair<int,int> loc);
    bool isValid(pair<int,int> loc);
    //int countAgent(pair<int,int> loc, int group);
    bool isGoal(pair<int,int> loc);
    bool isStart(pair<int,int> loc);
    pair<int,int> isGoalInSight(pair<int,int>loc, int facing);
    
    void move(int idx, pair<int,int> newloc, int dir);
    bool canMove(pair<int,int> locB);
    
    
    //void placeAgent(int idx, pair<int,int> oldloc, pair<int,int> loc, int group, shared_ptr<Organism2D> org);
    pair<int,int> getRelativePosition(pair<int,int> loc, int facing, int direction);
    
    int distance(pair<int,int> a, pair<int,int> b);
    
    
    int ** getTPM(shared_ptr<MarkovBrain> brain);
    vector<vector<int>> getCM(shared_ptr<MarkovBrain> brain);
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSwarm2__) */
