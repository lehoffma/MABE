//
// BaseballWorld.h
// BasicMarkovBrainTemplate
//
// Created by Alex Lambert 12/21/15

#ifndef __BasicMarkovBrainTemplate__LoI__
#define __BasicMarkovBrainTemplate__LoI__

#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>


#include "../../Genome/AbstractGenome.h"
#include "../AbstractWorld.h"
#include "../../Analyze/Analyze.h"
#include "../../Brain/AbstractBrain.h"
#include "../../Utilities/Parameters.h"
#include "../../Utilities/Utilities.h"
#include "../../Utilities/Random.h"
#include "../../Global.h"
#include <assert.h>

#pragma once
class LoIWorld : public AbstractWorld
{
public:
	LoIWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~LoIWorld() = default;

	//static double& HitCost;
	//static int& TestAmount;
	////static int& WorldY;
	//static int& TotalField;
	//static int& MaxDistance;
	//static int& MinDistance;
	//static int& BlackoutDistance;
	//static int& MaxSwings;
	//static bool& clearOutputs;
	static shared_ptr<ParameterLink<int>> worldUpdatesPL;
	static shared_ptr<ParameterLink<int>> HumanBrainPL;

	//unsigned char facingDisplay[8] { 94, 47, 62, 92, 118, 47, 60, 92 };
	fstream myfile;
	fstream progress;
	int inputStatesCount = 0;
	int outputStatesCount = 0;
	int differnce = 1;
	int MinDist = 80;
	int BallSpeed = 1;
	int BallLocation = 1;
	int swings = 0;
	int hits = 0;
	int r1 = 0;
	int r2 = 0;
	int ButtonLeft=0;
	int ButtonRight = 0;
	double score = 1;
	int beep1sum[2] = { 0, 0 };
	int beep2sum[2] = { 0,0 };
	int Strikes = 0;
	bool first = true;
	int blackoutStrikes = 0;
	std::vector<int> X;
	std::vector<int> Y;
	vector<int> directionall;
	pair<int, int> loc;
	pair<int, int> currentLoc;
	vector<pair<int, int>> currentLocAll;
	vector<pair<int, int>> startLocAll;
	vector<int> PlayerChoice;
	vector<int> Correct;
	std::vector<std::vector<int>> Map;
	std::vector<std::vector<int>> ColorMap;
	void InitializeGrid(int time,int RoundNum,int VariNum, bool visualize);
	virtual int requiredInputs()  override; //data and color for all direction; beep bits; direciton
	virtual int requiredOutputs() override;
	virtual int maxOrgsAllowed() override;
	virtual int minOrgsAllowed() override;
	void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;
	int GetLocation(pair<int, int> loc, pair<int, int> directionpoint = make_pair(-2, -2)){
		if (directionpoint.first == -2){
			return Map[loc.first][loc.second];
		}
		else{
			return Map[loc.first + directionpoint.first][loc.second + directionpoint.second];
		}
	}
	int GetColorLocation(pair<int, int> loc, pair<int, int> directionpoint = make_pair(-2, -2)){
		if (directionpoint.first == -2){
			return ColorMap[loc.first][loc.second];
		}
		else{
			return ColorMap[loc.first + directionpoint.first][loc.second + directionpoint.second];
		}
	}
	pair<int, int> GetFront(int i){
		return make_pair(Y[directionall[i]], X[directionall[i]]);
	}
	pair<int, int> GetLeft(int i){
		return make_pair(Y[(directionall[i] + 1) & 3], X[(directionall[i] + 1)&3]);
	}
	pair<int, int> GetRight(int i){
		return make_pair(Y[(directionall[i] - 1)&3], X[(directionall[i] - 1)&3]);

	}

	void printmap(bool);
	void printmaptofile(bool);
	void printall();
private:
	double scoremod(double score, double constant);
};





#endif /* defined(__BasicMarkovBrainTemplate__LoI__) */