all: MABE

MABE: objectFiles/main.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_Parameters.o objectFiles/World_AbstractWorld.o objectFiles/Genome_AbstractGenome.o objectFiles/Brain_AbstractBrain.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Archivist_DefaultArchivist.o objectFiles/World_LoIWorld_LoIWorld.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Optimizer_GAOptimizer_GAOptimizer.o objectFiles/Optimizer_TournamentOptimizer_TournamentOptimizer.o objectFiles/Optimizer_Tournament2Optimizer_Tournament2Optimizer.o objectFiles/Brain_MarkovBrain_MarkovBrain.o objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o objectFiles/Brain_MarkovBrain_Gate_GPGate.o objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o objectFiles/Brain_MarkovBrain_Gate_VoidGate.o objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o objectFiles/Brain_HumanBrain_HumanBrain.o
	c++ -Wall -std=c++11 objectFiles/main.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_Parameters.o objectFiles/World_AbstractWorld.o objectFiles/Genome_AbstractGenome.o objectFiles/Brain_AbstractBrain.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Archivist_DefaultArchivist.o objectFiles/World_LoIWorld_LoIWorld.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Optimizer_GAOptimizer_GAOptimizer.o objectFiles/Optimizer_TournamentOptimizer_TournamentOptimizer.o objectFiles/Optimizer_Tournament2Optimizer_Tournament2Optimizer.o objectFiles/Brain_MarkovBrain_MarkovBrain.o objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o objectFiles/Brain_MarkovBrain_Gate_GPGate.o objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o objectFiles/Brain_MarkovBrain_Gate_VoidGate.o objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o objectFiles/Brain_HumanBrain_HumanBrain.o -o MABE

objectFiles/main.o:
	c++ -Wall -std=c++11 -c main.cpp -o objectFiles/main.o

objectFiles/Global.o:
	c++ -Wall -std=c++11 -c Global.cpp -o objectFiles/Global.o

objectFiles/Group_Group.o:
	c++ -Wall -std=c++11 -c Group/Group.cpp -o objectFiles/Group_Group.o

objectFiles/Organism_Organism.o:
	c++ -Wall -std=c++11 -c Organism/Organism.cpp -o objectFiles/Organism_Organism.o

objectFiles/Utilities_Data.o:
	c++ -Wall -std=c++11 -c Utilities/Data.cpp -o objectFiles/Utilities_Data.o

objectFiles/Utilities_Parameters.o:
	c++ -Wall -std=c++11 -c Utilities/Parameters.cpp -o objectFiles/Utilities_Parameters.o

objectFiles/World_AbstractWorld.o:
	c++ -Wall -std=c++11 -c World/AbstractWorld.cpp -o objectFiles/World_AbstractWorld.o

objectFiles/Genome_AbstractGenome.o:
	c++ -Wall -std=c++11 -c Genome/AbstractGenome.cpp -o objectFiles/Genome_AbstractGenome.o

objectFiles/Brain_AbstractBrain.o:
	c++ -Wall -std=c++11 -c Brain/AbstractBrain.cpp -o objectFiles/Brain_AbstractBrain.o

objectFiles/Optimizer_AbstractOptimizer.o:
	c++ -Wall -std=c++11 -c Optimizer/AbstractOptimizer.cpp -o objectFiles/Optimizer_AbstractOptimizer.o

objectFiles/Archivist_DefaultArchivist.o:
	c++ -Wall -std=c++11 -c Archivist/DefaultArchivist.cpp -o objectFiles/Archivist_DefaultArchivist.o

objectFiles/World_LoIWorld_LoIWorld.o:
	c++ -Wall -std=c++11 -c World/LoIWorld/LoIWorld.cpp -o objectFiles/World_LoIWorld_LoIWorld.o

objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o:
	c++ -Wall -std=c++11 -c Archivist/LODwAPArchivist/LODwAPArchivist.cpp -o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o

objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o:
	c++ -Wall -std=c++11 -c Archivist/SSwDArchivist/SSwDArchivist.cpp -o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o

objectFiles/Genome_CircularGenome_CircularGenome.o:
	c++ -Wall -std=c++11 -c Genome/CircularGenome/CircularGenome.cpp -o objectFiles/Genome_CircularGenome_CircularGenome.o

objectFiles/Genome_MultiGenome_MultiGenome.o:
	c++ -Wall -std=c++11 -c Genome/MultiGenome/MultiGenome.cpp -o objectFiles/Genome_MultiGenome_MultiGenome.o

objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o:
	c++ -Wall -std=c++11 -c Genome/MultiGenome/Chromosome/AbstractChromosome.cpp -o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o

objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o:
	c++ -Wall -std=c++11 -c Genome/MultiGenome/Chromosome/TemplatedChromosome.cpp -o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o

objectFiles/Optimizer_GAOptimizer_GAOptimizer.o:
	c++ -Wall -std=c++11 -c Optimizer/GAOptimizer/GAOptimizer.cpp -o objectFiles/Optimizer_GAOptimizer_GAOptimizer.o

objectFiles/Optimizer_TournamentOptimizer_TournamentOptimizer.o:
	c++ -Wall -std=c++11 -c Optimizer/TournamentOptimizer/TournamentOptimizer.cpp -o objectFiles/Optimizer_TournamentOptimizer_TournamentOptimizer.o

objectFiles/Optimizer_Tournament2Optimizer_Tournament2Optimizer.o:
	c++ -Wall -std=c++11 -c Optimizer/Tournament2Optimizer/Tournament2Optimizer.cpp -o objectFiles/Optimizer_Tournament2Optimizer_Tournament2Optimizer.o

objectFiles/Brain_MarkovBrain_MarkovBrain.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/MarkovBrain.cpp -o objectFiles/Brain_MarkovBrain_MarkovBrain.o

objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/AbstractGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o

objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/DeterministicGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o

objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/EpsilonGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o

objectFiles/Brain_MarkovBrain_Gate_GPGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/GPGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_GPGate.o

objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/NeuronGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o

objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/ProbabilisticGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o

objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/TritDeterministicGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o

objectFiles/Brain_MarkovBrain_Gate_VoidGate.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/Gate/VoidGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_VoidGate.o

objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/GateBuilder/GateBuilder.cpp -o objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o

objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o:
	c++ -Wall -std=c++11 -c Brain/MarkovBrain/GateListBuilder/GateListBuilder.cpp -o objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o

objectFiles/Brain_HumanBrain_HumanBrain.o:
	c++ -Wall -std=c++11 -c Brain/HumanBrain/HumanBrain.cpp -o objectFiles/Brain_HumanBrain_HumanBrain.o

clean:
	rm -r objectFiles MABE

cleanup:
	rm -r objectFiles
