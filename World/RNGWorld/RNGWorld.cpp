//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "RNGWorld.h"

shared_ptr<ParameterLink<int>> RNGWorld::bitstringLengthPL = Parameters::register_parameter("WORLD_RNG-bitstringLength", 500, "Length of bitstring agents must create as random as possible");
shared_ptr<ParameterLink<int>> RNGWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_RNG-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains)");
shared_ptr<ParameterLink<string>> RNGWorld::groupNamePL = Parameters::register_parameter("WORLD_RNG_NAMES-groupName", (string)"root", "name of group to be evaluated\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");
shared_ptr<ParameterLink<string>> RNGWorld::brainNamePL = Parameters::register_parameter("WORLD_RNG_NAMES-brainName", (string)"root", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

RNGWorld::RNGWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {
	bitstringLength = (PT == nullptr) ? bitstringLengthPL->lookup() : PT->lookupInt("WORLD_RNG-bitstringLength");
	evaluationsPerGeneration = (PT == nullptr) ? evaluationsPerGenerationPL->lookup() : PT->lookupInt("WORLD_RNG-evaluationsPerGeneration");

	groupName = (PT == nullptr) ? groupNamePL->lookup() : PT->lookupString("WORLD_RNG_NAMES-groupName");
	brainName = (PT == nullptr) ? brainNamePL->lookup() : PT->lookupString("WORLD_RNG_NAMES-brainName");

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void RNGWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	//cout << "in test world evalSolo" << endl;
	auto brain = org->brains[brainName];
	for (int evaluationID = 0; evaluationID < evaluationsPerGeneration; evaluationID++) {
		///
		//vector<int> phiBuffer,R0;
		brain->resetBrain();
		double score = 0.0;
		string S,C,D="";
		map<string,int> theBuffer;
		int L=2;
		theBuffer["0"]=0;
		theBuffer["1"]=1;
		S="0";
		for(int n=0;n<500;n++){
			brain->setInput(0, n&1);  // give the brain a constant 1 (for wire brain)
			int I=0;
			//for(int i=0;i<maxNodes;i++)
			/// actually, I'm not sure how we access the internal states: TODO look up or ask Cliff
			//TRY THIS? for (int i = 0; i < brain->nrOutputValues; i++) {
			//	I=(I<<1)+Bit(brain->readOutput(i));
			//R0.push_back(I);
			brain->update();
			I=0;
			//for(int i=0;i<maxNodes;i++)
			/// actually, I'm not sure how we access the internal states: TODO look up or ask Cliff
			//TRY THIS? for (int i = 0; i < brain->nrOutputValues; i++) {
			//	I=(I<<1)+Bit(brain->readOutput(i));
			//phiBuffer.push_back(I);
			if(Bit(brain->readOutput(0))==0)
				C="0";
			else
				C="1";
			D=D+C;
			if(theBuffer.find(S+C)!=theBuffer.end()){
				S.append(C);
			} else {
				theBuffer[S+C]=L;
				L++;
				S=C;
				score += 1.0;
			}
		}
		org->dataMap.Append("score", score);
		/// leftover from neurocorrelates work - might use later
		//agent->Phi = computeAtomicPhi(phiBuffer, maxNodes);
		//agent->R = 0.0f;
		//agent->extra=1.0;
		//agent->correct = L;
		//agent->incorrect = 1;
		//agent->Connectedness = 0.0f;
		//agent->Sparseness = 0.0f;
		//agent->PIs2s=predictiveInformation_sensors_to_sensors(R0,1,pi_time_delay);
		//agent->PIs2m=predictiveInformation_sensors_to_motors(R0,phiBuffer,1,1,pi_time_delay);
		//vector<vector<int>> paths = agent->getDistMap(agent->getBrainMap());
		//int longestPath(0);
		//int sumPath(0);
		//for (auto& row : paths) {
		//	for (int& pathLength: row) {
		//		if (pathLength > longestPath) longestPath = pathLength;
		//	}
		//}
		//agent->Diameter = longestPath;
		//agent->Connectedness = agent->gammaIndex();
		//agent->Sparseness = 1.0-agent->Connectedness;
	}
}

