//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "BridgeWorld.h"

shared_ptr<ParameterLink<string>> BridgeWorld::visualizationFileNamePL = Parameters::register_parameter("VISUALIZATION_MODE_WORLD_BRIDGE-visualizationFileName", (string)"worldUpdatesFile.txt", "in visualize mode, visualization data will be written to this file.");
shared_ptr<ParameterLink<double>> BridgeWorld::windRatePL = Parameters::register_parameter("WORLD_BRIDGE-windRate", 0.1, "% of time wind will blow");
shared_ptr<ParameterLink<int>> BridgeWorld::worldUpdatesMaxPL = Parameters::register_parameter("WORLD_BRIDGE-worldUpdatesMax", 100, "max world updates per evaluation (death or success may result in fewer updates)");
shared_ptr<ParameterLink<int>> BridgeWorld::whichMapPL = Parameters::register_parameter("WORLD_BRIDGE-whichMap", -1, "which map should be used during evaluation. (-1 use all maps. -2 use all maps every evaluation.)");

BridgeWorld::BridgeWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	visualizationFileName = (PT == nullptr) ? visualizationFileNamePL->lookup() : PT->lookupString("VISUALIZATION_MODE_WORLD_BRIDGE-visualizationFileName");
	windRate = (PT == nullptr) ? windRatePL->lookup() : PT->lookupDouble("WORLD_BRIDGE-windRate");
	worldUpdatesMax = (PT == nullptr) ? worldUpdatesMaxPL->lookup() : PT->lookupInt("WORLD_BRIDGE-worldUpdatesMax");
	whichMap = (PT == nullptr) ? whichMapPL->lookup() : PT->lookupInt("WORLD_BRIDGE-whichMap");

	worldX = 15;
	worldY = 15;

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
	aveFileColumns.push_back("drowned");
	aveFileColumns.push_back("endX");
	aveFileColumns.push_back("endY");
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void BridgeWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {
	double score = 0.0;
	//worldX = 15;
	//worldY = 15;
	vector<vector<vector<int>>> grids;
	grids.push_back({
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 }
	});

	grids.push_back({
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,1,1,1,1,1,0,0,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,0,0,1,1,1,1,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 }
	});

	grids.push_back({
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,1,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,1,1,1,0,0,0,0,1,1,1,1 },
		{ 0,0,0,1,1,1,1,1,0,0,0,1,1,1,1 },
		{ 0,0,1,1,1,1,1,1,1,0,0,1,1,1,1 },
		{ 0,1,1,1,1,1,0,1,1,1,0,1,1,1,1 },
		{ 1,1,1,1,1,0,0,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1 },
		{ 1,1,1,0,0,0,0,0,0,1,1,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 }
	});

	grids.push_back({
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 1,1,1,0,0,0,0,0,0,1,1,1,1,1,1 },
		{ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,0,0,1,1,1,1,1,1,1,1 },
		{ 0,1,1,1,1,0,1,1,1,1,0,1,1,1,1 },
		{ 0,0,1,1,1,1,1,1,1,0,0,1,1,1,1 },
		{ 0,0,0,1,1,1,1,1,0,0,0,1,1,1,1 },
		{ 0,0,0,0,1,1,1,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,1,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 }
	});

	grids.push_back({
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 1,1,0,0,0,1,1,0,0,0,0,1,1,1,1 },
		{ 1,1,1,0,1,1,1,1,0,0,0,1,1,1,1 },
		{ 1,1,1,0,1,1,1,1,1,0,0,1,1,1,1 },
		{ 1,1,1,0,1,1,0,1,1,1,0,1,1,1,1 },
		{ 1,1,1,1,1,1,0,1,1,1,1,1,1,1,1 },
		{ 0,0,1,1,1,1,0,0,1,1,1,1,1,1,1 },
		{ 0,0,0,0,1,0,0,0,0,1,1,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 }
	});

	vector<vector<vector<int>>> testGrids;

	if (whichMap > 0) {
		if (whichMap > (int)grids.size() - 1) {
			cout << "  in BridgeWorld :: whichMap is set to " << whichMap << ". This is greater then the size of the grids list... please set a lower value." << endl;
			exit(1);
		}
		testGrids.push_back(grids[whichMap]);
	}
	else if (whichMap == -1) { // pick random
		testGrids.push_back(grids[Random::getIndex((int)grids.size())]);
	}
	else if (whichMap == -2) { // all maps
		testGrids = grids;
	}

	org->brain->resetBrain();

	int move1, move2;

	int worldUpdate = 0;
	bool done;
	for (auto grid : testGrids) {
		int OrgX = 2; // 2 from the left edge
		int OrgY = 7; // in the middle of the bridge
		int OrgF = 1; // facing the other side

		if (visualize) {
			SaveWorldState(visualizationFileName, grid, OrgX, OrgY, OrgF);
		}
		done = false;
		while (!done) {
			// wind
			if (Random::getDouble(1) < windRate) {
				int d = Random::getIndex(2);
				if (d == 0) { // up;
					if (debug) cout << "u";
					OrgY = loopMod(OrgY - 1, worldY);
				}
				else {
					if (debug) cout << "d";
					OrgY = loopMod(OrgY + 1, worldY);
				}
			}
			for (int row = 0; row < worldY; row++) {
				for (int col = 0; col < worldX; col++) {
					if (col == OrgX && row == OrgY) {
						if (debug) {
							if (OrgF == 0) cout << "^ ";
							if (OrgF == 1) cout << "> ";
							if (OrgF == 2) cout << "v ";
							if (OrgF == 3) cout << "< ";
						}
					}
					else {
						if (debug) cout << grid[row][col] << " ";
					}
				}
				if (debug) cout << endl;
			}
			if (debug) cout << endl;

			// sense
			for (int RF = -1; RF < 2; RF++) {
				int TF = loopMod(OrgF + RF, 4);
				if (TF == 0) {
					org->brain->setInput(RF + 1, grid[loopMod(OrgY - 1, worldY)][OrgX]);
				}
				else if (TF == 1) {
					org->brain->setInput(RF + 1, grid[OrgY][min(OrgX + 1, worldX - 1)]);
				}
				else if (TF == 2) {
					org->brain->setInput(RF + 1, grid[loopMod(OrgY + 1, worldY)][OrgX]);
				}
				else if (TF == 3) {
					org->brain->setInput(RF + 1, grid[OrgY][max(OrgX - 1, 0)]);
				}
			}

			org->brain->update();
			//update world

			move1 = Bit((int)org->brain->readOutput(0));
			move2 = Bit((int)org->brain->readOutput(1));

			if (move1 && move2) {
				if (debug) cout << "move" << endl;
				if (OrgF == 0) {
					OrgY = loopMod(OrgY - 1, worldY);
				}
				else if (OrgF == 1) {
					OrgX = min(OrgX + 1, worldX - 1);
				}
				else if (OrgF == 2) {
					OrgY = loopMod(OrgY + 1, worldY);
				}
				else if (OrgF == 3) {
					OrgX = max(OrgX - 1, 0);
				}
			}
			else if (move1) {
				if (debug) cout << "left" << endl;
				OrgF = loopMod(OrgF - 1, 4);
			}
			else if (move2) {
				if (debug) cout << "right" << endl;
				OrgF = loopMod(OrgF + 1, 4);
			}
			if (worldUpdate++ > worldUpdatesMax || OrgX >= worldX - 1) {
				done = true;
				org->dataMap.Append("alldrowned", 0);
				score += OrgX;
			}

			if (grid[OrgY][OrgX] == 0) {
				done = true;
				org->dataMap.Append("alldrowned", 1);
				score += 0.0;
			}

			if (visualize) {
				SaveWorldState(visualizationFileName, grid, OrgX, OrgY, OrgF);
			}
		}

		if (visualize) {
			for (int i = 0; i < 10; i++) {
				SaveWorldState(visualizationFileName, grid, OrgX, OrgY, OrgF);
			}
		}

		if (debug) cout << endl;

		org->dataMap.Append("allendX", OrgX);
		org->dataMap.Append("allendY", OrgY);

		if (score < 0.0) {
			score = 0.0;
		}
	}
	org->score = score;
	org->dataMap.Append("allscore", score);
}

int BridgeWorld::requiredInputs() {
	return 4;
}
int BridgeWorld::requiredOutputs() {
	return 3;
}
int BridgeWorld::maxOrgsAllowed() {
	return 1;
}
int BridgeWorld::minOrgsAllowed() {
	return 1;
}

void BridgeWorld::SaveWorldState(string fileName, vector<vector<int>> grid, int CX, int CY, int facing) {
	string stateNow = "";

	for (auto row : grid) {
		for (auto v : row) {
			stateNow += to_string(v) + ",";
		}
		stateNow += "\n";
	}

	stateNow += "-\n";

	stateNow += to_string(CX) + "\n";
	stateNow += to_string(CY) + "\n";
	stateNow += to_string(facing) + "\n";
	stateNow += "-";
	
	FileManager::writeToFile(fileName, stateNow, to_string(worldX) + ',' + to_string(worldY));  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
}


