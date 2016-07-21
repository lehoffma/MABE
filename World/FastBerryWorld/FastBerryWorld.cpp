//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "FastBerryWorld.h"


shared_ptr<ParameterLink<int>> FastBerryWorld::worldSizePL = Parameters::register_parameter("WORLD_FAST_BERRY-worldSize", 8, "size of world");
shared_ptr<ParameterLink<double>> FastBerryWorld::TSKPL = Parameters::register_parameter("WORLD_FAST_BERRY-taskSwitchingCost", 1.4, "cost to switch foods");

FastBerryWorld::FastBerryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {
	worldSize = (PT == nullptr) ? worldSizePL->lookup() : PT->lookupInt("WORLD_FAST_BERRY-worldSize");
	TSK = (PT == nullptr) ? TSKPL->lookup() : PT->lookupDouble("WORLD_FAST_BERRY-taskSwitchingCost");

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void FastBerryWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {
	double score = 0.0;
	vector<vector<int>> grid;
	for (int row = 0; row < worldSize; row++) {
		vector<int> newRow;
		for (int col = 0; col < worldSize; col++) {
			newRow.push_back(Random::getInt(1, 2));
		}
		grid.push_back(newRow);
	}

	int OrgX = Random::getIndex(worldSize);
	int OrgY = Random::getIndex(worldSize);
	int OrgF = Random::getIndex(4);

	org->brain->resetBrain();

	int downRed;
	int downBlue;
	int forwardRed;
	int forwardBlue;

	int move1, move2, eat;
	int lastFood = 0;

	for (int worldUpdates = 0; worldUpdates < 100; worldUpdates++) {
		for (int row = 0; row < worldSize; row++) {
			for (int col = 0; col < worldSize; col++) {
				if (col == OrgX && row == OrgY) {
					//cout << "x ";
				}
				else {
					//cout << grid[row][col] << " ";
				}
			}
			//cout << endl;
		}

		// sense
		if (grid[OrgY][OrgX] == 0) {
			downRed = 0;
			downBlue = 0;
		} else if (grid[OrgY][OrgX] == 1) {
			downRed = 1;
			downBlue = 0;
		}
		else if (grid[OrgY][OrgX] == 2) {
			downRed = 0;
			downBlue = 1;
		}
		
		int FX;
		int FY;
		if (OrgF == 0) {
			FX = 0;
			FY = -1;
		}
		else if (OrgF == 1) {
			FX = 1;
			FY = 0;
		}
		else if (OrgF == 2) {
			FX = 0;
			FY = 1;
		}
		else {
			FX = -1;
			FY = 0;
		}

		int LX = loopMod(OrgX + FX,worldSize);
		int LY = loopMod(OrgY + FY, worldSize);

		if (grid[LX][LY] == 0) {
			forwardRed = 0;
			forwardBlue = 0;
		}
		else if (grid[LY][LX] == 1) {
			forwardRed = 1;
			forwardBlue = 0;
		}
		else if (grid[LY][LX] == 2) {
			forwardRed = 0;
			forwardBlue = 1;
		}

		org->brain->setInput(0, downRed);
		org->brain->setInput(1, downBlue);
		org->brain->setInput(2, forwardRed);
		org->brain->setInput(3, forwardBlue);

		org->brain->update();
		//update world

		move1 = org->brain->readOutput(0);
		move2 = org->brain->readOutput(1);
		eat = org->brain->readOutput(2);

		if (eat) {
			//cout << "eat" << endl;
			if (grid[OrgY][OrgX] != 0) {
				if (grid[OrgY][OrgX] != lastFood && grid[OrgY][OrgX] != 0) {
					score -= TSK;
					lastFood = grid[OrgY][OrgX];
				}
				score += 1;
				grid[OrgY][OrgX] = 0;
			}
		}
		else if (move1 && move2) {
			//cout << "move" << endl;
			if (grid[OrgY][OrgX] == 0) {
				grid[OrgY][OrgX] = Random::getInt(1, 2);
			}
			OrgY = LY;
			OrgX = LX;
		}
		else if (move1) {
			//cout << "left" << endl;
			OrgF = loopMod(OrgF - 1, 4);
		}
		else if (move2) {
			//cout << "right" << endl;
			OrgF = loopMod(OrgF + 1, 4);
		}
	}
	//org->brain->setInput(0, 1);  // give the brain a constant 1 (for wire brain)
	//for (int i = 0; i < org->brain->nrOutNodes; i++) {
	//	if (mode == 0) {
	//		score += Bit(org->brain->readOutput(i));
	//	} else {
	//		score += org->brain->readOutput(i);
	//	}
	//}
	if (score < 0.0) {
		score = 0.0;
	}
	org->score = score;
	org->dataMap.Append("allscore", score);
}

int FastBerryWorld::requiredInputs() {
	return 4;
}
int FastBerryWorld::requiredOutputs() {
	return 3;
}
int FastBerryWorld::maxOrgsAllowed() {
	return 1;
}
int FastBerryWorld::minOrgsAllowed() {
	return 1;
}

