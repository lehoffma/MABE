//
// LevelOfIntelligenceWorld.h
// BasicMarkovBrainTemplate
//
// Created by Alex Lambert 12/22/15
//g++ -std=c++11 -Wall -O3 *.cpp */*.cpp */*/*.cpp -o MB
//./MB makeConfigFile 1


#include "LoIWorld.h"



const int UpperWhite = 0;
const int Black = 3;
const int White = 1;
const int Clear = 0;
const int Floor = 0;
const int Wall = 3;
const int Button = 2;
const int ProgressUpdate = -10;
const int ProgressPopulation = 100;
const int BeginUpdate = 0;
const int EndUpdate = 799;
const int Oppsite = -1;
const double SmallScore1 = .005;
const double SmallScore2 = .001;
const double SmallScore3 = .003;
const double MedScore1 = .50;
const double MedScore2 = .70;
const double MedScore3 = .90;
const double MedScore4 = 2.00;
const double HighScore1 = 10000000;
const double HighScore2 = 10000000000;
const int Varint = 0;

/*
Outputs: 
First two bits 
(00) = Do nothing
(01) = Move Right
(10) = Move Left
(11) = Move Foward
third bit and fourth bit are commincation
Inputs:
First twelelve bits are phyical object in each direction and What coloe
Two Bits per direction 
First is person(1) or not(0)
Second is  button(2) or wall(3)
Then next two are Color Map
First is Clear(0) or White(1)
The next is Black(3) 
The direction are 
Front
Left 
Right
Last two bits 
Beep 1
Beep 2
*/




//int& LevelOfIntelligenceWorld::WorldY = Parameters::register_parameter("LevelOfIntelligenceWorld_WorldY", 2, "world Y size", "WORLD - Baseball");
//double& LevelOfIntelligenceWorld::HitCost = Parameters::register_parameter("LevelOfIntelligenceWorld_HitCost", 1.4, "cost to get hit by ball", "WORLD - Baseball");
shared_ptr<ParameterLink<int>> LoIWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_LOI-worldUpdates", 400, "amount of time a brain is tested");
shared_ptr<ParameterLink<int>> LoIWorld::HumanBrainPL = Parameters::register_parameter("WORLD_LOI-humanBrain", 0, "Testing with the Human brian");
//int& LevelOfIntelligenceWorld::TestAmount = Parameters::register_parameter("LevelOfIntelligenceWorld_TestAmount", 20, "Test updates total", "WORLD - Baseball");
//int& LevelOfIntelligenceWorld::TotalField = Parameters::register_parameter("LevelOfIntelligenceWorld_TotalField", 8, "How long the Field is", "WORLD - Baseball");
//int& LevelOfIntelligenceWorld::MaxDistance = Parameters::register_parameter("LevelOfIntelligenceWorld_MaxDistance", 35, "MaxDistance the ball can go this includes off the map", "WORLD - Baseball");
//int& LevelOfIntelligenceWorld::MinDistance = Parameters::register_parameter("LevelOfIntelligenceWorld_MinDistance", 20, "MinDistance the ball can go this includes off the map", "WORLD - Baseball");
//int& LevelOfIntelligenceWorld::BlackoutDistance = Parameters::register_parameter("LevelOfIntelligenceWorld_BlackoutDistance", 5, "Distance the ball can't be seen", "WORLD - Baseball");
//int& LevelOfIntelligenceWorld::MaxSwings = Parameters::register_parameter("LevelOfIntelligenceWorld_MaxSwings", 10, "How many attempts he has", "WORLD - Baseball");

LoIWorld::LoIWorld(shared_ptr<ParametersTable> _PT) :
	AbstractWorld(_PT)
{
	inputStatesCount = 5 + 5 + 5 + 1; //four for each side,front
	outputStatesCount = 2 + 2; //turn left right and communicate
	differnce = 20;
	// Direction  0
	//			 3 1
	//			  2
	X = { 0, 1, 0, -1 }; // 0 is North 1 is 
	Y = { 1, 0, -1, 0 };
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
	aveFileColumns.push_back("Hits");
	aveFileColumns.push_back("Misses");
	aveFileColumns.push_back("Beep1Org1Total");
	aveFileColumns.push_back("Beep1Org2Total");
	aveFileColumns.push_back("Beep2Org1Total");
	aveFileColumns.push_back("Beep2Org2Total");

	cout << "  World using following BrainSates:\n    Inputs: 0 to " << inputStatesCount - 1 << "\n    Outputs: " << inputStatesCount << " to " << inputStatesCount + outputStatesCount - 1 << "\n";
}




double LoIWorld::scoremod(double score, double constant) {
	score += constant;
	//score -= fmod(score, constant);
	return score;
};

void LoIWorld::InitializeGrid(int time, int RoundNum, int VariNum, bool visualize) {
	startLocAll.clear();
	currentLocAll.clear();
	Map.clear();
	Correct.clear();
	PlayerChoice.clear();

	{
		//std::vector < int>  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20 }; Map.push_back(a0);
		std::vector < int> a0 = { 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a0);
		std::vector < int> a1 = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a1);
		std::vector < int> a2 = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a2);
		std::vector < int> a3 = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a3);
		std::vector < int> a4 = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a4);
		std::vector < int> a5 = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a5);
		std::vector < int> a6 = { 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(a6);
		std::vector < int> a7 = { 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0 }; Map.push_back(a7);
		std::vector < int> a8 = { 0, 3, 3, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 3, 3, 0 }; Map.push_back(a8);
		std::vector < int> a9 = { 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3 }; Map.push_back(a9);
		std::vector < int> aA = { 3, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 2, 3 }; Map.push_back(aA);//10
		std::vector < int> aB = { 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 }; Map.push_back(aB);//11
		std::vector < int> aC = { 3, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 2, 3 }; Map.push_back(aC);//12
		std::vector < int> aD = { 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3 }; Map.push_back(aD);//13
		std::vector < int> aE = { 0, 3, 3, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 3, 3, 0 }; Map.push_back(aE);//14
		std::vector < int> aF = { 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0 }; Map.push_back(aF);//15
		std::vector < int> aG = { 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aG);//16
		std::vector < int> aH = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aH);//17
		std::vector < int> aI = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aI);//18
		std::vector < int> aJ = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aJ);//19
		std::vector < int> aK = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aK);//20
		std::vector < int> aL = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aL);//21
		std::vector < int> aM = { 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 }; Map.push_back(aM);//22
		//std::vector < int>  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20K }; Map.push_back(a0);
	}
	ColorMap.clear();

	{
		//std::vector < int>  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20K }; Map.push_back(a0);
		std::vector < int> a0 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a0);
		std::vector < int> a1 = { 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a1);
		std::vector < int> a2 = { 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a2);
		std::vector < int> a3 = { 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a3);
		std::vector < int> a4 = { 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a4);
		std::vector < int> a5 = { 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a5);
		std::vector < int> a6 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a6);
		std::vector < int> a7 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a7);
		std::vector < int> a8 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a8);
		std::vector < int> a9 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(a9);
		std::vector < int> aA = { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }; ColorMap.push_back(aA);
		std::vector < int> aB = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aB);
		std::vector < int> aC = { 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 }; ColorMap.push_back(aC);
		std::vector < int> aD = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aD);
		std::vector < int> aE = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aE);
		std::vector < int> aF = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aF);
		std::vector < int> aG = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aG);
		std::vector < int> aH = { 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aH);
		std::vector < int> aI = { 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aI);
		std::vector < int> aJ = { 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aJ);
		std::vector < int> aK = { 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aK);
		std::vector < int> aL = { 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aL);
		std::vector < int> aM = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; ColorMap.push_back(aM);
		//std::vector < int>  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20K }; Map.push_back(a0);
	}


	int direction;
	direction = Random::getIndex(4);  // direction the agent is facing //1 is black //5 is strating point
	if (HumanBrainPL->lookup())
	{
		direction = 2;
	}
	directionall.push_back(direction);
	direction = Random::getIndex(4);  // direction the agent is facing //1 is black //5 is strating point
	if (HumanBrainPL->lookup())
	{
		direction = 0;
	}
	directionall.push_back(direction);
	int startingcol = Random::getIndex(5);
	
	if (HumanBrainPL->lookup())
	{
		startingcol=2;
	}
	//int startingrow = Random::getIndex(2);
	//pair<int, int > intial;
	currentLoc = make_pair(21, startingcol + 8);
	//intial = make_pair(21, startingcol + 8);
	startLocAll.push_back(currentLoc);
	currentLocAll.push_back(currentLoc);
	currentLoc = make_pair(1, startingcol + 8); //Y then X
	startLocAll.push_back(currentLoc);
	currentLocAll.push_back(currentLoc);
	//ColorMap[10][1] = Black
	//ColorMap[12][1] = White
	//r1 = Black; P1 area
	//r2 = White; P2 area

	Map[currentLocAll[0].first][currentLocAll[0].second] = 1;
	Map[currentLocAll[1].first][currentLocAll[1].second] = 1;
	int Switches = 0;
	switch (RoundNum) {
		/*
		//N Srt BUT
		//0 WB BW
		//1 WB WB
		//2	BW BW
		//3 BW WB
		//4 BB WB
		//5 WW WB
		//6 BC WB
		//7 WC WB
		//8 BB BW
		//9 WW BW
		//10 BC BW
		//11 WC BW
		//12 CC BW
		//13 CC WB
		//14
		//15
		*/
	case 0:
	case 1:
	case 2:
	case 3:
		r1 = White;
		r2 = Black;
		Correct.push_back(r1);
		Correct.push_back(r2);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		r1 = Black;
		r2 = White;
		Correct.push_back(r1);
		Correct.push_back(r2);
		break;
	case 8:
	case 9:
	case 10:
	case 11:
		r1 = Black;
		r2 = Black;
		Correct.push_back(Oppsite);
		Correct.push_back(Oppsite);
		break;
	case 12:
	case 13:
	case 14:
	case 15:
		r1 = White;
		r2 = White;
		Correct.push_back(Oppsite);
		Correct.push_back(Oppsite);
		break;
	case 16:
	case 17:
	case 18:
	case 19:
		r1 = Clear;
		r2 = Clear;
		Correct.push_back(Oppsite);
		Correct.push_back(Oppsite);
		break;
	case 20:
	case 21:
	case 22:
	case 23:
		r1 = Black;
		r2 = Clear;
		Correct.push_back(Black);
		Correct.push_back(White);
		break;
	case 24:
	case 25:
	case 26:
	case 27:
		r1 = White;
		r2 = Clear;
		Correct.push_back(White);
		Correct.push_back(Black);
		break;
	case 28:
	case 29:
	case 30:
	case 31:
		r1 = Clear;
		r2 = White;
		Correct.push_back(Black);
		Correct.push_back(White);
		break;
	case 32:
	case 33:
	case 34:
	case 35:
		r1 = Black;
		r2 = Clear;
		Correct.push_back(Black);
		Correct.push_back(White);
		break;


	}
	Switches = RoundNum % 4;
	switch (Switches)
	{
	case 0:
		ColorMap[10][1] = Black;
		ColorMap[12][19] = Black;
		ColorMap[12][1] = White;
		ColorMap[10][19] = White;
		break;

	case 1:
		ColorMap[10][1] = Black;
		ColorMap[10][19] = Black;
		ColorMap[12][1] = White;
		ColorMap[12][19] = White;
		break;

	case 2:
		ColorMap[10][1] = White;
		ColorMap[12][19] = White;
		ColorMap[12][1] = Black;
		ColorMap[10][19] = Black;
		break;

	case 3:
		ColorMap[10][1] = White;
		ColorMap[10][19] = White;
		ColorMap[12][1] = Black;
		ColorMap[12][19] = Black;
		break;
	}
	//cout << "AllColors" << endl;
	//cout <<"ColorMap[10][1] "  <<ColorMap[10][1] << endl;
	//cout <<"ColorMap[12][19] "  <<ColorMap[12][19] << endl;
	//cout << "ColorMap[12][1] " << ColorMap[12][1] << endl;
	//cout << "ColorMap[10][19] " << ColorMap[10][19] << endl;
	//cout << "End" << endl;

	//Changes each side of the spawns to match the assgined color
	for (int i = 0; i < ColorMap.size(); i++)
	{
		for (int j = 0; j < ColorMap[0].size(); j++)
		{
			if (ColorMap[i][j] == 5) {
				ColorMap[i][j] = r1;
			}
			if (ColorMap[i][j] == 6) {
				ColorMap[i][j] = r2;
			}
		}
	}

	//Starts Sending to Progress only after set constants
	if (visualize && time == BeginUpdate && VariNum == Varint) {
		progress.open("progress.csv", std::ofstream::out | std::ofstream::trunc);
		progress << "y," << "x," << "d," << "t" << endl;
	}
	if (visualize && time <= EndUpdate)
	{
		cout << "BWOpen ";
		if (score < 0) {
			score = 0;
		}
		long actualscore = score / HighScore2;
		progress << -3 << "," << actualscore << "," << VariNum << "," << -3 << endl;
	}
	//This is what actually causes the progress to start 
	if (visualize && time <= EndUpdate)
	{
		for (int i = 0; i < ColorMap.size(); i++)
		{
			for (int j = 0; j < ColorMap[0].size(); j++)
			{
				if (ColorMap[i][j] == Black) {
					if (Map[i][j] == 2) {
						progress << i << "," << j << "," << 0 << "," << 2 << endl; //Black Button
					}
					else {
						progress << i << "," << j << "," << 0 << "," << 2 << endl; //Black Floor
					}
				}
				if (ColorMap[i][j] == White) {
					if (Map[i][j] == 2) {
						progress << i << "," << j << "," << 0 << "," << 3 << endl; //White Button
					}
					else {
						progress << i << "," << j << "," << 0 << "," << 3 << endl; //White Floor
					}
				}

			}


		}
	}
	if (visualize && time <= EndUpdate)
	{
		cout << "BWClose ";
		progress << -1 << "," << -1 << "," << -1 << "," << -1 << endl;
	}
	//Initalize Player Choices
	PlayerChoice.push_back(-2);
	PlayerChoice.push_back(-2);
	//	printmap(true);
}

void LoIWorld::printmap(bool iscolor = false) {
	if (iscolor) {
		cout << "color:" << endl;
		for (auto row : ColorMap) {
			for (auto col : row) {
				cout << col << " ";
			}
			cout << endl;
		}
	}
	cout << "Map:" << endl;
	for (auto row : Map) {
		for (auto col : row) {
			cout << col << " ";
		}
		cout << endl;
	}
	cout << "Direction:" << endl;
	for (auto direction : directionall) {
		cout << "Direction: "<< direction<< endl;
	}
	cout << "Fitness:" << endl;
	cout << score << endl;
}

void LoIWorld::printmaptofile(bool iscolor = false) {
	myfile << "Map:" << endl;
	for (auto row : Map) {
		for (auto col : row) {
			myfile << col << " ";
		}
		myfile << endl;
	}
	if (iscolor) {
		myfile << "color:" << endl;
		for (auto row : ColorMap) {
			for (auto col : row) {
				myfile << col << " ";
			}
			myfile << endl;
		}
	}
}

void LoIWorld::printall() {
	printmap(true);

}

void LoIWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {
	score = 1.0;
	int Hits = 0;
	int Misses = 0;
	//InitializeGrid(0,0);
	vector<shared_ptr<Organism>> orgs;
	int AgentSize = 2;
	vector<bool> Freeze;
	vector<int> stateCollector;
	int here, leftFront, front, rightFront;  // store grid values relitive to organsism

	//cout << "problem" << endl;
	int statesAssignmentCounter;  // this world can has number of brainState inputs set by parameter. This counter is used while assigning inputs
	for (int i = 0; i < 2; i++) {
		shared_ptr<Organism> Copyorg = make_shared<Organism>(org, org->genome);
		orgs.push_back(Copyorg);
	}

	for (int i = 0; i < 2; i++) {
		beep1sum[i] = 0;
		beep2sum[i] = 0;
	}
	//orgs[1]->brain->resetBrain();
	int output0;
	int output1;
	bool RoundOver = false;
	vector<int> Level2; 
	for (int i = 0; i < 36; i++) {
		Level2.push_back(i);
	}
	vector<int> Level3O{ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11 };
	int Variations = Level2.size();
	for (int u = 0; u < Variations; u++) {

		output1 = 0;  // store outputs from bra
		stateCollector.clear();
		here = 0, leftFront = 0, front = 0, rightFront = 0;  // store grid values relitive to organsism

		int statesAssignmentCounter = 0;
		output0 = 0;
		Freeze.clear();
		Freeze.push_back(false);
		Freeze.push_back(false);
		int Choose = Random::getIndex(Variations - u);
		if (HumanBrainPL->lookup())
		{
			Choose = 0;
		}
		int Choice = Level2[Choose];
		RoundOver = false;
		orgs[0]->brain->resetBrain();
		orgs[1]->brain->resetBrain();
		int beep[2] = { 0, 0 };
		int beep2[2] = { 0, 0 };
		Level2.erase(Level2.begin() + Choose);
		InitializeGrid(0, Choice, u, visualize);
		//cout << Varieties.size() << Choose << endl;
		for (int t = 0; t < worldUpdatesPL->lookup(); t++)
		{  //run agent for "worldUpdates" brain updates
			if (HumanBrainPL->lookup())
			{
				printmap(true);
			}
			vector<vector<int>> MapDataAll;
			for (int j = 0; j < AgentSize; j++) 
			{
				vector<int> MapData;
				here = GetLocation(currentLocAll[j]); MapData.push_back(here);
				front = GetLocation(currentLocAll[j], GetFront(j)); MapData.push_back(front);
				leftFront = GetLocation(currentLocAll[j], GetLeft(j)); MapData.push_back(leftFront);
				rightFront = GetLocation(currentLocAll[j], GetRight(j)); MapData.push_back(rightFront);
				MapDataAll.push_back(MapData);
			}
			vector<vector<int>> ColorDataAll;
			for (int j = 0; j < AgentSize; j++) 
			{
					vector<int> ColorData;
					here = GetColorLocation(currentLocAll[j]); ColorData.push_back(here);
					front = GetColorLocation(currentLocAll[j], GetFront(j)); ColorData.push_back(front);
					leftFront = GetColorLocation(currentLocAll[j], GetLeft(j)); ColorData.push_back(leftFront);
					rightFront = GetColorLocation(currentLocAll[j], GetRight(j)); ColorData.push_back(rightFront);
					ColorDataAll.push_back(ColorData);
			}
			for (int j = 0; j < AgentSize; j++) 
			{
				statesAssignmentCounter = 0;  // get ready to start assigning inputs
				for (int i = 1; i < MapDataAll[0].size(); i++) 
				{  // fill first states with food values at front location
					orgs[j]->brain->setInput(statesAssignmentCounter++, MapDataAll[j][i] & 1);
					orgs[j]->brain->setInput(statesAssignmentCounter++, (MapDataAll[j][i] >> 1) & 1);
					orgs[j]->brain->setInput(statesAssignmentCounter++, ColorDataAll[j][i] & 1);
					orgs[j]->brain->setInput(statesAssignmentCounter++, (ColorDataAll[j][i] >> 1) & 1);
				}
				orgs[j]->brain->setInput(statesAssignmentCounter++, beep[1 - j]);
				orgs[j]->brain->setInput(statesAssignmentCounter++, beep2[1 - j]);
			}
			for (int j = 0; j < AgentSize; j++)
			{
				orgs[j]->brain->update();
			}
			//printmap(true);
			for (int j = 0; j < AgentSize; j++) {
				if (Freeze[j] == false) 
				{
					  // just run the update!
					int action = 0;
					action = Bit(orgs[j]->brain->readOutput(0)) + (Bit(orgs[j]->brain->readOutput(1)) << 1);// +(Bit(orgs[j]->brain->readOutput(16)) << 2)
					beep[j] = Bit(orgs[j]->brain->readOutput(2)); //read first beep
					beep2[j] = Bit(orgs[j]->brain->readOutput(3)); //read second beep
					Map[currentLocAll[j].first][currentLocAll[j].second] = 0;
					switch (action) 
					{
					case 0: //Do nothing
						score += SmallScore1;
						break;
					case 1: //turn to the right
						directionall[j] = (directionall[j] + 1) & 3; currentLocAll[j];
						score += SmallScore2;
						break;
					case 2: //turn to the left
						directionall[j] = (directionall[j] - 1) & 3;
						score += SmallScore2;
						break;
					case 3: //move
						int newLoc = GetLocation(currentLocAll[j], GetFront(j));
						if (newLoc == 0) 
						{
							currentLocAll[j] = make_pair(currentLocAll[j].first + Y[directionall[j]], currentLocAll[j].second + X[directionall[j]]);
							if (currentLocAll[j].first == 11 && (currentLocAll[j].second == 16 || currentLocAll[j].second == 4))
							{ //close the chamber
								if (visualize  && t <= EndUpdate)
								{
									cout << "BWOpen ";
									progress << -2 << "," << -2 << "," << -2 << "," << -2 << endl;
								}
								if (currentLocAll[j].second == 16) 
								{
									if (visualize  && t <= EndUpdate)
									{
										cout << "BWOpen ";
										progress << 11 << "," << 15 << "," << 0 << "," << 0 << endl;
									}
									Map[11][15] = 3;
								}
								if (currentLocAll[j].second == 4) {
									if (visualize  && t <= EndUpdate)
									{
										cout << "BWOpen ";
										progress << 11 << "," << 5 << "," << 0 << "," << 0 << endl;
									}
									Map[11][5] = 3;
								}
								if (visualize && t <= EndUpdate)
								{
									cout << "BWOpen ";
									progress << -1 << "," << -1 << "," << 0 << "," << 0 << endl;
								}
							}
							else if (currentLocAll[j].second == 10 && (currentLocAll[j].first == 14 || currentLocAll[j].first == 8))
							{ //close the chamber
								if (currentLocAll[j].first == 14) 
								{
									Map[15][10] = 3;
								}
								if (currentLocAll[j].first == 8) 
								{
									Map[7][10] = 3;
								}
							}

							if (currentLocAll[j].first >= 6 && currentLocAll[j].first <= 15) 
							{
								score += MedScore1;
							}
							if (currentLocAll[j].second <= 7 || currentLocAll[j].second >= 13) {
								score += MedScore2;
							}
							if (currentLocAll[j].second <= 5 || currentLocAll[j].second >= 15) {
								score += MedScore3;
							}
							if (currentLocAll[j].second <= 2 || currentLocAll[j].second >= 18) {
								score += MedScore4;
							}
							score += SmallScore1;
						}
						else if (newLoc == 2) 
						{ //staring at a button
							currentLocAll[j] = make_pair(currentLocAll[j].first + Y[directionall[j]], currentLocAll[j].second + X[directionall[j]]);
							if (Freeze[1 - j] == true) 
							{
								PlayerChoice[j] = GetColorLocation(currentLocAll[j]);
								if (Correct[0] == Oppsite && PlayerChoice[j] != PlayerChoice[j - 1]) 
								{
									score += HighScore2 * 8;
									Hits++;
								}
								else if (Correct[0] != Oppsite && PlayerChoice[j] == Correct[j]) 
								{
									//right choice for both
									score += HighScore2;
									Hits++;
								}
								else 
								{
									score += HighScore1;
									Misses++;
									//cout << "wrong" << endl;
								}
								RoundOver = true;
								//InitializeGrid(t);
								Freeze[j] = false;
								Freeze[1 - j] = false;
							}
							else 
							{

								PlayerChoice[j] = GetColorLocation(currentLocAll[j]);
								if (Correct[0] != Oppsite && PlayerChoice[j] == Correct[j]) 
								{
									score += HighScore2;
								}
								else 
								{
									score += HighScore1;
								}

								Freeze[j] = true;
							}
						}
						else //Did nothing
						{
							//score += -1;
						}
						break;
					}
					Map[currentLocAll[j].first][currentLocAll[j].second] = 1;
				}
				if (true) 
				{
					if (visualize && t <= EndUpdate)
					{
						//printmap();
						if (j == AgentSize - 2)
						{
							progress << -4 << "," << -4 << "," << -4 << "," << -4 << endl;
						}
						cout << "update " << Global::update << " popNum" << 0 << " t " << t << "Fitness" << score;
						progress << currentLocAll[j].first << "," << currentLocAll[j].second << "," << directionall[j] << "," << j + 4 << endl;
						if (j == AgentSize - 1)
							progress << -2 << "," << -2 << "," << -2 << "," << -2 << endl; //First BIT IS FITNESS
					}
					if (visualize  && t == EndUpdate && j == AgentSize - 1 && u == Variations)
					{
						progress.close();
					}
				}
			}
			for (int j = 0; j < AgentSize; j++) 
			{
				if (Global::update != -10) 
				{
					beep1sum[j - 1] += beep[j];
					beep2sum[j - 1] += beep2[j];
				}
			}
			if (RoundOver == true) {
				break;
			}
		}
	}
	//cout << score << endl;
	if (Global::update != -10) 
	{
		org->dataMap.Append("allscore", score);
		org->dataMap.Append("allHits", Hits);
		org->dataMap.Append("allMisses", Misses);
		org->dataMap.Append("allBeep1Org1Total", beep1sum[0]);
		org->dataMap.Append("allBeep1Org2Total", beep1sum[1]);
		org->dataMap.Append("allBeep2Org1Total", beep2sum[0]);
		org->dataMap.Append("allBeep2Org2Total", beep2sum[1]);
	}
	org->score = score;
}
int LoIWorld::requiredInputs() {
	return 4 + 4 + 4+2;
}
int LoIWorld::requiredOutputs() {
	return 2 + 2;
}
int LoIWorld::maxOrgsAllowed() {
	return 1;
}
int LoIWorld::minOrgsAllowed() {
	return 1;
}