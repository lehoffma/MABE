//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldTest__
#define __BasicMarkovBrainTemplate__WorldTest__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class TestWorld : public AbstractWorld {

public:

	static shared_ptr<ParameterLink<int>> modePL;
	static shared_ptr<ParameterLink<int>> numberOfOutputsPL;

	// The number of updates we have, got from params
	static shared_ptr<ParameterLink<int>> worldUpdates;

	int mode;
	int numberOfOutputs;

	// Interface param
	vector<int> field;

	TestWorld(shared_ptr<ParametersTable> _PT = nullptr);

	void makeField();

	virtual ~TestWorld() = default;
	virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;
	virtual int maxOrgsAllowed() override;
	virtual int minOrgsAllowed() override;
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldTest__) */
