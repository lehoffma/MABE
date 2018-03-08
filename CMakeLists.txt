cmake_minimum_required(VERSION 3.8)
project(MABE)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        #        #        zupply stuff
        #        Utilities/zupply/src/zupply.hpp
        Utilities/zupply/src/zupply.cpp
        #        #        end zupply stuff


        Utilities/Data.cpp
        Utilities/Data.h
        Utilities/MTree.h
        Utilities/Parameters.cpp
        Utilities/Parameters.h
        Utilities/Random.h
        Utilities/Utilities.h
        Utilities/VectorNd.h
        Utilities/PointNd.h
        Utilities/SensorArcs.h
        Utilities/TurnTable.h
        Utilities/WorldUtilities.h
        Utilities/graycode.h
        Utilities/Loader.cpp
        Utilities/Loader.h

        Global.cpp
        Global.h

        Genome/CircularGenome/CircularGenome.cpp
        Genome/CircularGenome/CircularGenome.h
        Genome/MultiGenome/Chromosome/AbstractChromosome.cpp
        Genome/MultiGenome/Chromosome/AbstractChromosome.h
        Genome/MultiGenome/Chromosome/TemplatedChromosome.cpp
        Genome/MultiGenome/Chromosome/TemplatedChromosome.h
        Genome/MultiGenome/MultiGenome.cpp
        Genome/MultiGenome/MultiGenome.h
        Genome/AbstractGenome.cpp
        Genome/AbstractGenome.h

        Analyze/Analyze.cpp
        Analyze/Analyze.h
        Analyze/MutationAnalyze.cpp
        Analyze/MutationAnalyze.h
        Archivist/LODwAPArchivist/LODwAPArchivist.cpp
        Archivist/LODwAPArchivist/LODwAPArchivist.h
        Archivist/SSwDArchivist/SSwDArchivist.cpp
        Archivist/SSwDArchivist/SSwDArchivist.h
        Archivist/DefaultArchivist.cpp
        Archivist/DefaultArchivist.h
        Brain/ConstantValuesBrain/ConstantValuesBrain.cpp
        Brain/ConstantValuesBrain/ConstantValuesBrain.h
        Brain/HumanBrain/HumanBrain.cpp
        Brain/HumanBrain/HumanBrain.h
        Brain/IPDBrain/IPDBrain.cpp
        Brain/IPDBrain/IPDBrain.h
        Brain/MarkovBrain/Gate/AbstractGate.cpp
        Brain/MarkovBrain/Gate/AbstractGate.h
        Brain/MarkovBrain/Gate/DeterministicGate.cpp
        Brain/MarkovBrain/Gate/DeterministicGate.h
        Brain/MarkovBrain/Gate/EpsilonGate.cpp
        Brain/MarkovBrain/Gate/EpsilonGate.h
        Brain/MarkovBrain/Gate/GPGate.cpp
        Brain/MarkovBrain/Gate/GPGate.h
        Brain/MarkovBrain/Gate/NeuronGate.cpp
        Brain/MarkovBrain/Gate/NeuronGate.h
        Brain/MarkovBrain/Gate/ProbabilisticGate.cpp
        Brain/MarkovBrain/Gate/ProbabilisticGate.h
        Brain/MarkovBrain/Gate/TritDeterministicGate.cpp
        Brain/MarkovBrain/Gate/TritDeterministicGate.h
        Brain/MarkovBrain/Gate/VoidGate.cpp
        Brain/MarkovBrain/Gate/VoidGate.h
        Brain/MarkovBrain/Gate/FeedbackGate.h
        Brain/MarkovBrain/Gate/FeedbackGate.cpp
        Brain/GateBrain/GateBrain.cpp
        Brain/GateBrain/GateBrain.h
        Brain/MarkovBrain/GateBuilder/GateBuilder.cpp
        Brain/MarkovBrain/GateBuilder/GateBuilder.h
        Brain/MarkovBrain/GateListBuilder/GateListBuilder.cpp
        Brain/MarkovBrain/GateListBuilder/GateListBuilder.h
        Brain/MarkovBrain/MarkovBrain.cpp
        Brain/MarkovBrain/MarkovBrain.h
        Brain/CGPBrain/CGPBrain.cpp
        Brain/CGPBrain/CGPBrain.h
        Brain/GeneticProgramingBrain/GeneticProgramingBrain.cpp
        Brain/GeneticProgramingBrain/GeneticProgramingBrain.h
        Brain/LSTMBrain/LSTMBrain.cpp
        Brain/LSTMBrain/LSTMBrain.h
        Brain/WireBrain/WireBrain.cpp
        Brain/WireBrain/WireBrain.h
        Brain/AbstractBrain.cpp
        Brain/AbstractBrain.h
        Group/Group.cpp
        Group/Group.h
        Optimizer/SimpleOptimizer/SimpleOptimizer.cpp
        Optimizer/SimpleOptimizer/SimpleOptimizer.h
        Optimizer/AbstractOptimizer.cpp
        Optimizer/AbstractOptimizer.h
        Organism/Organism.cpp
        Organism/Organism.h
        World/BerryPlusWorld/BerryPlusWorld.cpp
        World/BerryPlusWorld/BerryPlusWorld.h
        World/BerryWorld/BerryWorld.cpp
        World/BerryWorld/BerryWorld.h
        World/IPDWorld/IPDWorld.cpp
        World/IPDWorld/IPDWorld.h
        World/NumeralClassifierWorld/NumeralClassifierWorld.cpp
        World/NumeralClassifierWorld/NumeralClassifierWorld.h
        World/SOFWorld/SOFWorld.cpp
        World/SOFWorld/SOFWorld.h
        World/MemoryWorld/MemoryWorld.cpp
        World/MemoryWorld/MemoryWorld.h
        World/SwarmWorld/SwarmWorld.cpp
        World/SwarmWorld/SwarmWorld.h
        World/TestWorld/TestWorld.cpp
        World/TestWorld/TestWorld.h
        World/AbstractWorld.cpp
        World/AbstractWorld.h


        main.cpp
        modules.h
        World/SwarmWorld/model/WorldLogEntry.h
        World/SwarmWorld/model/WorldLog.cpp
        World/SwarmWorld/model/WorldLog.h
        World/SwarmWorld/model/OrganismState.h
        World/SwarmWorld/util/StringUtils.h
        World/SwarmWorld/serialize/SwarmWorldSerializer.cpp
        World/SwarmWorld/serialize/SwarmWorldSerializer.h
        World/SwarmWorld/serialize/Serializer.h
        World/SwarmWorld/util/GridUtils.h
        World/SwarmWorld/grid-initializers/FirstAvailableInitializer.cpp
        World/SwarmWorld/grid-initializers/FirstAvailableInitializer.h
        World/SwarmWorld/grid-initializers/GridInitializer.h
        World/SwarmWorld/grid-initializers/GridInitializerFactory.h
        World/SwarmWorld/grid-initializers/RandomInitializer.cpp
        World/SwarmWorld/grid-initializers/RandomInitializer.h
        World/SwarmWorld/grid-initializers/ChineseMilitaryInitializer.cpp
        World/SwarmWorld/grid-initializers/ChineseMilitaryInitializer.h
        World/SwarmWorld/serialize/Serializer.cpp
        World/SwarmWorld/grid-initializers/GreatestDistanceInitializer.cpp
        World/SwarmWorld/grid-initializers/GreatestDistanceInitializer.h
        World/SwarmWorld/model/Agent.h
        World/SwarmWorld/level/Level.cpp
        World/SwarmWorld/level/Level.h
        World/SwarmWorld/level/FieldType.h
        World/SwarmWorld/level/Field.h
        World/SwarmWorld/level/SwarmLevel.cpp
        World/SwarmWorld/level/SwarmLevel.h
        World/SwarmWorld/level/move/MoveValidityStrategy.h
        World/SwarmWorld/level/move/WithinBoundsStrategy.cpp
        World/SwarmWorld/level/move/WithinBoundsStrategy.h
        World/SwarmWorld/level/move/ScoringStrategy.h
        World/SwarmWorld/level/move/DebouncedScoringStrategy.cpp
        World/SwarmWorld/level/move/DebouncedScoringStrategy.h
        World/SwarmWorld/level/move/CollisionStrategy.h
        World/SwarmWorld/level/move/PenaltyCollisionStrategy.cpp
        World/SwarmWorld/level/move/PenaltyCollisionStrategy.h
        World/SwarmWorld/model/Direction.h
        World/SwarmWorld/util/DirectionUtils.h
        World/SwarmWorld/scoring/OrganismScoringStrategy.h
        World/SwarmWorld/scoring/IndividualScoringStrategy.cpp
        World/SwarmWorld/scoring/IndividualScoringStrategy.h
        World/SwarmWorld/scoring/PopulationAverageScoringStrategy.cpp
        World/SwarmWorld/scoring/PopulationAverageScoringStrategy.h
        World/SwarmWorld/scoring/OrganismScoringStrategyFactory.h
        World/SwarmWorld/level/move/SequenceScoringStrategy.cpp
        World/SwarmWorld/level/move/SequenceScoringStrategy.h
        World/SwarmWorld/serialize/OrganismBrain.h World/SwarmWorld/model/OrganismStateContainer.h Optimizer/MultiObjectiveOptimizer/Nsga2Optimizer.cpp Optimizer/MultiObjectiveOptimizer/Nsga2Optimizer.h Optimizer/MultiObjectiveOptimizer/MultiObjectiveSolution.h Optimizer/MultiObjectiveOptimizer/TournamentSelector.h Optimizer/MultiObjectiveOptimizer/MultiObjectiveUtils.h Optimizer/MultiObjectiveOptimizer/Neighbourhood/NeighbourhoodOptimizer.cpp Optimizer/MultiObjectiveOptimizer/Neighbourhood/NeighbourhoodOptimizer.h Optimizer/MultiObjectiveOptimizer/MultiObjectiveOptimizer.cpp Optimizer/MultiObjectiveOptimizer/MultiObjectiveOptimizer.h Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/DistanceStrategy.h Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/EuclideanDistanceStrategy.cpp Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/EuclideanDistanceStrategy.h Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/ManhattanDistanceStrategy.cpp Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/ManhattanDistanceStrategy.h Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/ChebyshevDistanceStrategy.cpp Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/ChebyshevDistanceStrategy.h Optimizer/MultiObjectiveOptimizer/Neighbourhood/DistanceStrategy/DistanceStrategyFactory.h World/SwarmWorld/util/CompareUtils.h)

add_executable(MABE ${SOURCE_FILES})
target_link_libraries(MABE Threads::Threads)