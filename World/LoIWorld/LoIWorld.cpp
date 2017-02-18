//
// LOI.h
// BasicMarkovBrainTemplate
//
// Created by Alex Lambert 12/22/15
//g++ -std=c++11 -Wall -O3 *.cpp */*.cpp */*/*.cpp -o MB
//./MB makeConfigFile 1


#include "LoIWorld.h"



const int UPPER_WHITE = 0;
const int BLACK = 3;
const int WHITE = 1;
const int CLEAR = 0;
const int FLOOR = 0;
const int WALL = 3;
const int BUTTON = 2;
const int BEGIN_UPDATE = 0;
const int END_UPDATE = 799;
const int OPPSITE = -1;
const double SMALLSCORE1 = .005;
const double SMALLSCORE2 = .001;
const double SMALLSCORE3 = .003;
const double MEDSCORE1 = .50;
const double MEDSCORE2 = .70;
const double MedScore3 = .90;
const double MedScore4 = 2.00;
const double HighScore1 = 10000000;
const double HighScore2 = 10000000000;
const int Varint = 0;

/*
New Fitness Formula:
(X)
------
((1+Y)/(1.5+C))

35 
Floating TODO

10 Replicates Due Date
Same Visualization
Record Per Round
For Each Variation
Save Answer to each 
[RoundNum]=[Org1Choice,Org2Choice]

Make Graphs for First Two Level of Intelliegence Due Date Two Weeks


X=Number of Correct Positions
If they are the same color +8
They can't get this half right since they have to be oppsite
If they are differnt colors one correct +1
If they are differnt colors both correct +2
Y=Wrong Position
If they are the same color +1
If they are differnt colors and they are both wrong +4
C=Comm Bonus
If the first Comm is 10 and they start at Red+.1
If the first Comm is 01 and they start at Blue+.1
If the first Comm is 11 and they start at Clear+.5
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




shared_ptr<ParameterLink<int>> LoIWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_LOI-worldUpdates", 400, "amount of time a brain is tested");
shared_ptr<ParameterLink<int>> LoIWorld::HumanBrainPL = Parameters::register_parameter("WORLD_LOI-humanBrain", 0, "Testing with the Human brian");
shared_ptr<ParameterLink<bool>> LoIWorld::scaffoldingPL = Parameters::register_parameter("WORLD_LOI-scaffolding", false, "Use Scaffolding");
shared_ptr<ParameterLink<bool>> LoIWorld::skipClearClearPL = Parameters::register_parameter("WORLD_LOI-skipClear", false, "Skip clear clear");
shared_ptr<ParameterLink<bool>> LoIWorld::steppingStonePL = Parameters::register_parameter("WORLD_LOI-steppingStone", false, "Do last 16 then do all of them at 20,000 update");

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
	directionall.clear();
	startLocAll.clear();

	#pragma region Clear Maps

	Map.clear();
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
	#pragma endregion 

	#pragma region Place Orgranisms
	int direction; 
	

	direction = 2;  //Orgranism 1 Direction
	if (HumanBrainPL->lookup())
	{
		direction = 2;
	} 
	directionall.push_back(direction);


	direction = 0;  //Orgranism 2 Direction	
	if (HumanBrainPL->lookup())
	{
		direction = 0;
	}
	directionall.push_back(direction);

	
	
	int startingcol;

	startingcol = Random::getIndex(5); //Where the Orgranisms start at
	if (HumanBrainPL->lookup())
	{
		startingcol=2;
	}


	//Place Charcters in map
	currentLoc = make_pair(21, startingcol + 8);
	startLocAll.push_back(currentLoc);
	currentLocAll.push_back(currentLoc);
	currentLoc = make_pair(1, startingcol + 8); 
	startLocAll.push_back(currentLoc);
	currentLocAll.push_back(currentLoc);


	//Place in Map Charcters
	Map[currentLocAll[0].first][currentLocAll[0].second] = 1;
	Map[currentLocAll[1].first][currentLocAll[1].second] = 1;
	
	#pragma endregion 

	#pragma region Determine Start Colors

	switch (RoundNum) {
	case 0:
	case 1:
	case 2:
	case 3:
		r1 = WHITE;
		r2 = BLACK;
		Correct.push_back(r1);
		Correct.push_back(r2);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		r1 = BLACK;
		r2 = WHITE;
		Correct.push_back(r1);
		Correct.push_back(r2);
		break;
	case 8:
	case 9:
	case 10:
	case 11:
		r1 = BLACK;
		r2 = BLACK;
		Correct.push_back(OPPSITE);
		Correct.push_back(OPPSITE);
		break;
	case 12:
	case 13:
	case 14:
	case 15:
		r1 = WHITE;
		r2 = WHITE;
		Correct.push_back(OPPSITE);
		Correct.push_back(OPPSITE);
		break;
	case 16:
	case 17:
	case 18:
	case 19:
		r1 = CLEAR;
		r2 = CLEAR;
		Correct.push_back(OPPSITE);
		Correct.push_back(OPPSITE);
		break;
	case 20:
	case 21:
	case 22:
	case 23:
		r1 = BLACK;
		r2 = CLEAR;
		Correct.push_back(BLACK);
		Correct.push_back(WHITE);
		break;
	case 24:
	case 25:
	case 26:
	case 27:
		r1 = WHITE;
		r2 = CLEAR;
		Correct.push_back(WHITE);
		Correct.push_back(BLACK);
		break;
	case 28:
	case 29:
	case 30:
	case 31:
		r1 = CLEAR;
		r2 = WHITE;
		Correct.push_back(BLACK);
		Correct.push_back(WHITE);
		break;
	case 32:
	case 33:
	case 34:
	case 35:
		r1 = CLEAR;
		r2 = BLACK;
		Correct.push_back(WHITE);
		Correct.push_back(BLACK);
		break;


	}

	#pragma endregion

	#pragma region Place Buttons
	int Switches = 0;
	Switches = RoundNum % 4; //Determines where the buttons are located
	switch (Switches)
	{
		case 0: 
		{
			ColorMap[10][1] = BLACK;
			ColorMap[12][19] = BLACK;
			ColorMap[12][1] = WHITE;
			ColorMap[10][19] = WHITE;
			break;
		}


		case 1: 
		{
			ColorMap[10][1] = BLACK;
			ColorMap[10][19] = BLACK;
			ColorMap[12][1] = WHITE;
			ColorMap[12][19] = WHITE;
			break;
		}
		case 2:
		{
			ColorMap[10][1] = WHITE;
			ColorMap[12][19] = WHITE;
			ColorMap[12][1] = BLACK;
			ColorMap[10][19] = BLACK;
			break;
		}
		case 3:
		{
			ColorMap[10][1] = WHITE;
			ColorMap[10][19] = WHITE;
			ColorMap[12][1] = BLACK;
			ColorMap[12][19] = BLACK;
			break;
		}
	}
	#pragma endregion 


	#pragma region Update Entrace Colors
	
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
	#pragma endregion


	#pragma region Visualize
	//Starts Sending to Progress only after set constants
	if (visualize && time == BEGIN_UPDATE && VariNum == Varint) {
		progress.open("progress.csv", std::ofstream::out | std::ofstream::trunc);
		progress << "y," << "x," << "d," << "t" << endl;
	}
	if (visualize && time <= END_UPDATE)
	{
		if (score < 0) {
			score = 0;
		}
		long actualscore = score / HighScore2;
		progress << -3 << "," << actualscore << "," << VariNum << "," << -3 << endl;
	}


	
	if (visualize && time <= END_UPDATE)
	{
		for (int i = 0; i < ColorMap.size(); i++)
		{
			for (int j = 0; j < ColorMap[0].size(); j++)
			{
				if (ColorMap[i][j] == BLACK) {
					if (Map[i][j] == 2) {
						progress << i << "," << j << "," << 0 << "," << 2 << endl; //Black Button
					}
					else {
						progress << i << "," << j << "," << 0 << "," << 2 << endl; //Black Floor
					}
				}
				if (ColorMap[i][j] == WHITE) {
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

	if (visualize && time <= END_UPDATE)
	{
		progress << -1 << "," << -1 << "," << -1 << "," << -1 << endl;
	}
	#pragma endregion Start Visualizing
	//Initalize Player Choices
	PlayerChoice.push_back(-2);
	PlayerChoice.push_back(-2);
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
	
	//Data Map Tracking
	score = 1.0; //Current Score
	failPoints = 0.0; //Fail Score not used for anything yet
	commPoints = 0.0; //Comm Score not used for anything yet
	int hits = 0; //How many times the exit is reached
	int misses = 0; //How many times the exit is not reached

	//World Paramters
	bool HumanBrain = (bool)HumanBrainPL->lookup();
	int worldUpdates = worldUpdatesPL->lookup();
	bool skipClearClear = skipClearClearPL->lookup();
	bool scaffolding = scaffoldingPL->lookup();
	bool steppingStone = steppingStonePL->lookup();
	//Input Tracking
	const int AGENT_SIZE = 2;
	vector<bool> Freeze; //Stops an orgranism from moving once it exis
	int here, leftFront, front, rightFront;  // store grid values relitive to organsism
	int newLocation;  //Keeps track of the an Orgranism locations
	int statesAssignmentCounter; // Keeps track of which input to insert
	bool roundOver = false; //To Stop Round once all orgranims reach the exit
	double AmountCorrect = 0;
	vector<double> RoundPoints(36,0.0);
	//Orgranism Copying
	vector<shared_ptr<Organism>> orgs;
	//Copies over Orgranism
	for (int i = 0; i < 2; i++) {
		shared_ptr<Organism> Copyorg = make_shared<Organism>(org, org->genome);
		orgs.push_back(Copyorg);
	}


	//Data map Beep amount initlization
	for (int i = 0; i < 2; ++i) {
		beep1sum[i] = 0;
		beep2sum[i] = 0;
	}
	
	//Performance in Each Enviroment Sepreatley
	//Vector of Rounds created
	vector<int> roundVariations; //Initialize to all 36 variations of the world
	if(skipClearClear)
	{ 
		if (!steppingStone || Global::update > 2)
		{
			for (int i = 0; i < 16; ++i)
			{
				roundVariations.push_back(i);
				cout << "NotBad" << endl;
			}
		}
		for (int i = 20; i < 36; ++i)
		{
			roundVariations.push_back(i);
		}
	}
	else
	{
		for (int i = 0; i < 36; ++i)
		{
			roundVariations.push_back(i);
		}
	}



	int variationAmount = roundVariations.size(); //How Many Rounds are there
	
	for (int u = 0; u < variationAmount; u++) 
	{
		#pragma region Round Start 
		roundOver = false; //Both Orgranism aren't frozen
		Freeze.clear();
		Freeze.push_back(false);
		Freeze.push_back(false);
		#pragma endregion Unfreeze
		
		
		#pragma region Round Start
		orgs[0]->brain->resetBrain();
		orgs[1]->brain->resetBrain();
		int beep[2] = { 0, 0 };
		int beep2[2] = { 0, 0 };
		here = 0, leftFront = 0, front = 0, rightFront = 0;  //Reset Directions
		statesAssignmentCounter = 0;
		#pragma endregion Brain Reset 

		#pragma region Round Start
		//Find Random Round are removes it from the list
		int Choose = Random::getIndex(variationAmount - u);
		if (HumanBrainPL->lookup())
		{
			Choose = 0;
		}
		int Choice = roundVariations[Choose];
		roundVariations.erase(roundVariations.begin() + Choose);
		#pragma endregion Choose Next Round 
		

		InitializeGrid(0, Choice, u, visualize); //Initlize World to current Round
		
		//Go through the Round
		for (int t = 0; t < worldUpdates; ++t)
		{ 
			#pragma region HumanBrain 
			if (HumanBrainPL->lookup())
			{
				//True means include color map
				printmap(true);
			}
			#pragma endregion

			#pragma region Gather Inputs
			vector<vector<int>> MapDataAll;
			for (int j = 0; j < AGENT_SIZE; j++) 
			{
				vector<int> MapData;
				MapData.push_back(GetLocation(currentLocAll[j], GetFront(j))); //Front
				MapData.push_back(GetLocation(currentLocAll[j], GetLeft(j))); //Left
				MapData.push_back(GetLocation(currentLocAll[j], GetRight(j))); //Right
				MapDataAll.push_back(MapData);
			}
		    
			vector<vector<int>> ColorDataAll;
			for (int j = 0; j < AGENT_SIZE; j++) 
			{
				vector<int> ColorData;
				ColorData.push_back(GetColorLocation(currentLocAll[j], GetFront(j))); //Front
				ColorData.push_back(GetColorLocation(currentLocAll[j], GetLeft(j))); //Left
				ColorData.push_back(GetColorLocation(currentLocAll[j], GetRight(j))); //Right
				ColorDataAll.push_back(ColorData);
			}
			#pragma endregion

			#pragma region Insert Inputs
			for (int j = 0; j < AGENT_SIZE; j++) 
			{
				statesAssignmentCounter = 0;  // get ready to start assigning inputs

				//Go through each Map Cordinate starting Front,Left, and then Right 
				for (int i = 0; i < MapDataAll[0].size(); ++i) 
				{  
					//Map Data Bits
					orgs[j]->brain->setInput(statesAssignmentCounter++, MapDataAll[j][i] & 1); //First Bit is Person(1) or Not(0)
					orgs[j]->brain->setInput(statesAssignmentCounter++, (MapDataAll[j][i] >> 1) & 1);//Second Bit is Button(2) or Wall(3)		
					//Color Dats Bits
					orgs[j]->brain->setInput(statesAssignmentCounter++, ColorDataAll[j][i] & 1); //First Bit is Clear(0) or White(1)
					orgs[j]->brain->setInput(statesAssignmentCounter++, (ColorDataAll[j][i] >> 1) & 1); //Second Bit is Black(3)
				}

				//Add Communication Bits
				orgs[j]->brain->setInput(statesAssignmentCounter++, beep[1 - j]);
				orgs[j]->brain->setInput(statesAssignmentCounter++, beep2[1 - j]);

				//Add Direction Bits
				orgs[j]->brain->setInput(statesAssignmentCounter++, directionall[j]& 1); //Direction Cord 1
				orgs[j]->brain->setInput(statesAssignmentCounter++, (directionall[j]>>1)& 1); //Direction Cord 2
			}
			#pragma endregion

			#pragma region Update Brain
			//Update Each Brain
			for (int j = 0; j < AGENT_SIZE; j++)
			{
				orgs[j]->brain->update();
			}
			#pragma endregion 

			#pragma region Update World
			//Preform the Outputs
			for (int j = 0; j < AGENT_SIZE; j++) 
			{
				//See if the current orgranism has found an exit
				if (Freeze[j] == false) 
				{
					#pragma region Constants
					const int  DONOTHING = 0;
					const int  TURNRIGHT = 1;
					const int  TURNLEFT = 2;
					const int  MOVE = 3;
					#pragma endregion Action Case
					
					int action = 0;

					//Which action should the orgranism take
					action = Bit(orgs[j]->brain->readOutput(0)) + (Bit(orgs[j]->brain->readOutput(1)) << 1);

					//Reads the first beep
					beep[j] = Bit(orgs[j]->brain->readOutput(2));

					//Reads the second beep
					beep2[j] = Bit(orgs[j]->brain->readOutput(3)); 

					
					//What happens during each update
					switch (action) 
					{
						case DONOTHING: 
						{ 
							score += SMALLSCORE1;
							break;
						}
						case TURNRIGHT: 
						{
							directionall[j] = (directionall[j] + 1) & 3;
							score += SMALLSCORE2;
							break;
						}
						case TURNLEFT:
						{
							directionall[j] = (directionall[j] - 1) & 3;
							score += SMALLSCORE2;
							break;
						}
						case MOVE: 
						{
							#pragma region Constants
							const int  EmptySpace = 0;
							const int  Exit = 2;
							#pragma endregion Move Case

							newLocation = GetLocation(currentLocAll[j], GetFront(j));

							Map[currentLocAll[j].first][currentLocAll[j].second] = 0;
							//Move into the space if it's empty
							if (newLocation == EmptySpace)
							{
								//:/ Should be a function
								currentLocAll[j] = make_pair(currentLocAll[j].first + Y[directionall[j]], currentLocAll[j].second + X[directionall[j]]);
								
								#pragma region Constants
								const int  EXIT_CHAMBER_Y = 0;
								const int  EXIT_CHAMBER_LEFT = 16;
								const int  EXIT_CHAMBER_RIGHT = 4;

								const int  ARENA_CHAMBER_X = 10;
								const int  ARENA_CHAMBER_DOWN = 14;
								const int  ARENA_CHAMBER_UP = 8;
								#pragma endregion Chamber Case

								//if an orgranism enters an exit chamber
								if (currentLocAll[j].first == EXIT_CHAMBER_Y && 
								   (currentLocAll[j].second == EXIT_CHAMBER_LEFT 
								||  currentLocAll[j].second == EXIT_CHAMBER_RIGHT))
								{ 

								
									#pragma region Visualize
									//Close the Chamber
									if (visualize  && t <= END_UPDATE)
									{
										progress << -2 << "," << -2 << "," << -2 << "," << -2 << endl;
									}
									#pragma endregion Chamber Close Start


									if (currentLocAll[j].second == EXIT_CHAMBER_LEFT)
									{
										#pragma region Visualize
										if (visualize  && t <= END_UPDATE)
										{
											progress << 11 << "," << 15 << "," << 0 << "," << 0 << endl;
										}
										#pragma endregion Chamber CloseLeft


										Map[11][15] = 3; //Change Cordinate to Wall
									}
									else if (currentLocAll[j].second == EXIT_CHAMBER_RIGHT) 
									{
										#pragma region Visualize
										if (visualize  && t <= END_UPDATE)
										{
											progress << 11 << "," << 5 << "," << 0 << "," << 0 << endl;
										}
										#pragma endregion Chamber Close Right


										Map[11][5] = 3; //Change Cordinate to Wall
									}

									#pragma region Visualize
									if (visualize && t <= END_UPDATE)
									{
										progress << -1 << "," << -1 << "," << 0 << "," << 0 << endl;
									}
									#pragma endregion Chamber CloseEnd
								}
							
							
							
								//if an orgranims enters main area close off where he came from
								else if (currentLocAll[j].second == ARENA_CHAMBER_X && 
										(currentLocAll[j].first == ARENA_CHAMBER_UP || 
										 currentLocAll[j].first == ARENA_CHAMBER_DOWN))
								{ 
									if (currentLocAll[j].first == ARENA_CHAMBER_DOWN)
									{
										Map[15][10] = 3;
									}
									if (currentLocAll[j].first == ARENA_CHAMBER_UP)
									{
										Map[7][10] = 3;
									}
								}

								if (currentLocAll[j].first >= 6 && currentLocAll[j].first <= 15)
								{
									score += MEDSCORE1;
								}
								if (currentLocAll[j].second <= 7 || currentLocAll[j].second >= 13) {
									score += MEDSCORE2;
								}
								if (currentLocAll[j].second <= 5 || currentLocAll[j].second >= 15) {
									score += MedScore3;
								}
								if (currentLocAll[j].second <= 2 || currentLocAll[j].second >= 18) {
									score += MedScore4;
								}
								score += SMALLSCORE1;
							}

							//staring at a button
							else if (newLocation == Exit)
							{ 
							
								currentLocAll[j] = make_pair(currentLocAll[j].first + Y[directionall[j]], currentLocAll[j].second + X[directionall[j]]);
							

								//Both Orgrainsim Have Left
								if (Freeze[1 - j] == true)
								{

									PlayerChoice[j] = GetColorLocation(currentLocAll[j]);

									//Both Organisms have picked the oppsite and weren't given clear instructions(8 Points)
									if (Correct[0] == OPPSITE && PlayerChoice[j] != PlayerChoice[1 - j]) 
									{
										//cout << "RoundNum: " << u << " Player1 " << PlayerChoice[j] << " Player2 " << PlayerChoice[1 - j] << endl;
										score += HighScore2 * 8; 
										AmountCorrect += 1;
										RoundPoints[Choice] += 1;
										hits++;
									}
								
									//Both Organisms have picked the same and weren't give clear instructions
									else if (Correct[0] == OPPSITE && PlayerChoice[j] == PlayerChoice[1 - j]) 
									{
										failPoints += 1;
									}

									//Both Organims have picked the incorrect choice and were give clear instructions
									else if (Correct[0] != OPPSITE && PlayerChoice[j] != Correct[j] && PlayerChoice[1 - j] != Correct[1 - j]) 
									{
										failPoints += 4;
										
									}

									//Both Organisms have picked the correct choice and were given clear instructions
									else if (Correct[0] != OPPSITE && PlayerChoice[j] == Correct[j])
									{
										score += HighScore2;
										hits++;
										AmountCorrect += .5;
										RoundPoints[Choice] += .5;

									}

									//Only the Second Organism picked the correct choice and were given clear instructions
									else if (Correct[0] != OPPSITE && PlayerChoice[j] != Correct[j]) 
									{
										//right choice for both
										//score += HighScore2;
										//hits++;
										//AmountCorrect += .5;
										//RoundPoints[u] += .5;
									}

									//Should be impossible now
									else 
									{
										cout << "Player Choice J: " << PlayerChoice[j] << endl;
										cout << "Other Player Choice: " << PlayerChoice[1 - j] << endl;
										cout << "Correct: " << Correct[0] << endl;
										cout << "Correct1: " << Correct[1] << endl;
										cout << "Oppsite = "<< OPPSITE << endl;
								
										assert(false);
									}


									roundOver = true;

									//Probably Pointless
									Freeze[j] = false;
									Freeze[1 - j] = false;
								}

								//First Organism Finished
								else
								{

									PlayerChoice[j] = GetColorLocation(currentLocAll[j]);

									//Got the choice correct and were given clear instructions
									if (Correct[0] != OPPSITE && PlayerChoice[j] == Correct[j]) 
									{
										score += HighScore2;
										AmountCorrect += .5;
										RoundPoints[Choice] += .5;

									}
								
									//Got the choice wrong and clear instructions were given 
									//or clear instructions were not given
									else
									{
										score += HighScore1;
									}

									Freeze[j] = true; //Stop the orgranim from moving
								}
							}


							break;
						}
					}

					//Update the Map Cordinates
					Map[currentLocAll[j].first][currentLocAll[j].second] = 1; 
				}
				#pragma region Visualize
				if (visualize && t <= worldUpdates)
				{
					if (j == AGENT_SIZE - 2)
					{
						progress << -4 << "," << -4 << "," << -4 << "," << -4 << endl;
					}
					progress << currentLocAll[j].first << "," << currentLocAll[j].second << "," << directionall[j] << "," << j + 4 << endl;
					if (j == AGENT_SIZE - 1)
						progress << -2 << "," << -2 << "," << -2 << "," << -2 << endl; //First BIT IS FITNESS
				}
				if (visualize  && t == worldUpdates && j == AGENT_SIZE - 1 && u == variationAmount)
				{
					progress.close();
				}
				#pragma endregion Inserting New Cordinates
			}
			#pragma endregion 

	
			#pragma region DataMap
			//Add up all communication
			for (int j = 0; j < AGENT_SIZE; ++j) 
			{
	
					beep1sum[1-j] += beep[j];
					beep2sum[1-j] += beep2[j];
			}
			#pragma endregion Communication
			
			//Check to see if both creatures have gone to an exit
			if (roundOver == true) 
			{
				break;
			}
		}
	}
	
	#pragma region DataMapandScaffolding
	org->dataMap.Append("allscore", AmountCorrect);
	int roundCountTemp=0;
	for (int i = 0; i < RoundPoints.size(); ++i)
	{
		roundCountTemp += RoundPoints[i];
		if (i % 4 && scaffolding == true)
		{
			if (roundCountTemp == 4)
			{
				AmountCorrect += 100;
			}
			roundCountTemp = 0;
		}

		if (i >= 10)
		{
			org->dataMap.Append("allRP" + to_string(i), RoundPoints[i]);
		}
		else
		{
			org->dataMap.Append("allRP0" + to_string(i), RoundPoints[i]);
		}
		
	}
	//org->dataMap.Append("allscore", failPoints);
	//org->dataMap.Append("allscore", commPoints);
	org->dataMap.Append("allHits", hits);
	org->dataMap.Append("allMisses", misses);
	org->dataMap.Append("allBeep1Org1Total", beep1sum[0]);
	org->dataMap.Append("allBeep1Org2Total", beep1sum[1]);
	org->dataMap.Append("allBeep2Org1Total", beep2sum[0]);
	org->dataMap.Append("allBeep2Org2Total", beep2sum[1]);
	#pragma endregion Totals


	//Set organism score
	org->score = AmountCorrect;

}

int LoIWorld::requiredInputs() {
	return 4 + 4 + 4+2+2;
}

int LoIWorld::requiredOutputs() {
	return 2 + 2;
}

int LoIWorld::maxOrgsAllowed() {
	return 2;
}

int LoIWorld::minOrgsAllowed() {
	return 1;
}