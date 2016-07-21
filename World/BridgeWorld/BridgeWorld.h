//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldBridge__
#define __BasicMarkovBrainTemplate__WorldBridge__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class BridgeWorld : public AbstractWorld {

public:

	static shared_ptr<ParameterLink<string>> visualizationFileNamePL;
	static shared_ptr<ParameterLink<double>> windRatePL;
	static shared_ptr<ParameterLink<int>> worldUpdatesMaxPL;
	static shared_ptr<ParameterLink<int>> whichMapPL;

	string visualizationFileName;
	double windRate;
	int worldUpdatesMax;
	int whichMap;

	int worldX;
	int worldY;

	BridgeWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~BridgeWorld() = default;
	virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;
	virtual int maxOrgsAllowed() override;
	virtual int minOrgsAllowed() override;

	void SaveWorldState(string fileName, vector<vector<int>> grid, int CX, int CY, int facing);

};

#endif /* defined(__BasicMarkovBrainTemplate__WorldBridge__) */
