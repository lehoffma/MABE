//
// Created by Le on 14.05.2018.
//

#ifndef MABE_SWARMWORLDPARAMETERS_H
#define MABE_SWARMWORLDPARAMETERS_H


#include "../../Utilities/Parameters.h"
#include "SwarmWorld.h"
#include "grid-initializers/GridInitializerFactory.h"
#include "scoring/OrganismScoringStrategyFactory.h"
#include "level/SwarmLevel.h"
#include "movement-penalties/SpinningPenalty.h"
#include "movement-penalties/StandingStillPenalty.h"
#include "level/move/PenaltyCollisionStrategy.h"

class SwarmWorldParameters {
public:
    /**
     * Parameter link describing how often the world should be updated in the evaluation
     */
    static shared_ptr<ParameterLink<int>> worldUpdatesPL;
    static shared_ptr<ParameterLink<int>> gridXSizePL;
    static shared_ptr<ParameterLink<int>> gridYSizePL;
    static shared_ptr<ParameterLink<int>> hasPenaltyPL;
    static shared_ptr<ParameterLink<double>> invalidMovePenaltyPL;
    static shared_ptr<ParameterLink<int>> senseAgentsPL;
    static shared_ptr<ParameterLink<int>> detectWaterPL;
    static shared_ptr<ParameterLink<double>> nAgentsPL;
    static shared_ptr<ParameterLink<string>> senseSidesPL;
    static shared_ptr<ParameterLink<int>> resetOutputsPL;
    static shared_ptr<ParameterLink<double>> penaltyPL;
    static shared_ptr<ParameterLink<int>> waitForGoalPL;
    static shared_ptr<ParameterLink<int>> pheroPL;
    static shared_ptr<ParameterLink<string>> gridInitializerPL;
    static shared_ptr<ParameterLink<string>> simulationModePL;
    static shared_ptr<ParameterLink<string>> scoringStrategyPL;
    static shared_ptr<ParameterLink<int>> repeatsPL;
    static shared_ptr<ParameterLink<int>> resetPositionsPL;

    static void initSwarmWorld(SwarmWorld &swarmWorld) {
        swarmWorld.worldUpdates = worldUpdatesPL->get(swarmWorld.PT);
        swarmWorld.worldUpdates = worldUpdatesPL->get(swarmWorld.PT);
        swarmWorld.gridX = gridXSizePL->get(swarmWorld.PT);
        swarmWorld.gridY = gridYSizePL->get(swarmWorld.PT);
        swarmWorld.senseAgents = senseAgentsPL->get(swarmWorld.PT) == 1;
        swarmWorld.resetOutputs = resetOutputsPL->get(swarmWorld.PT) == 1;
        swarmWorld.hasPenalty = hasPenaltyPL->get(swarmWorld.PT) == 1;
        swarmWorld.nAgents = nAgentsPL->get(swarmWorld.PT);
        convertCSVListToVector(senseSidesPL->get(swarmWorld.PT), swarmWorld.senseSides);

        swarmWorld.penalty = penaltyPL->get(swarmWorld.PT);
        swarmWorld.invalidMovePenalty = invalidMovePenaltyPL->get(swarmWorld.PT);
        swarmWorld.phero = pheroPL->get(swarmWorld.PT) == 1;
        swarmWorld.resetPositionsInterval = resetPositionsPL->get(swarmWorld.PT);
        swarmWorld.waitForGoalInterval = waitForGoalPL->get(swarmWorld.PT);
        swarmWorld.simulationMode = simulationModePL->get(swarmWorld.PT);

        swarmWorld.gridInitializer = GridInitializerFactory::getFromString(gridInitializerPL->get(swarmWorld.PT));
        swarmWorld.repeats = repeatsPL->get(swarmWorld.PT);

        swarmWorld.organismScoringStrategy = OrganismScoringStrategyFactory::getFromString(
                scoringStrategyPL->get(swarmWorld.PT));


        swarmWorld.movementPenaltyManager = std::unique_ptr<MovementPenaltyManager>(new MovementPenaltyManager());
        swarmWorld.movementPenaltyManager->add(
                        std::shared_ptr<MovementPenalty>(new StandingStillPenalty(swarmWorld.invalidMovePenalty)))
                .add(std::shared_ptr<MovementPenalty>(new SpinningPenalty(swarmWorld.invalidMovePenalty)));
//        swarmWorld.movementPenaltyManager->add(
//                std::shared_ptr<MovementPenalty>(new SpinningPenalty(swarmWorld.invalidMovePenalty)));

        //todo level factory + level parameter
        std::pair<int, int> dimensions(swarmWorld.gridX, swarmWorld.gridY);

        swarmWorld.level = std::unique_ptr<Level<Field>>(new SwarmLevel(swarmWorld.PT, dimensions));
        if (swarmWorld.hasPenalty) {
            swarmWorld.level->setCollisionStrategy(
                    std::shared_ptr<CollisionStrategy<Field>>(new PenaltyCollisionStrategy<Field>(swarmWorld.penalty)));
        }
        swarmWorld.level->loadFromFile("level.csv", ' ');
    }
};


shared_ptr<ParameterLink<int>> SwarmWorldParameters::gridXSizePL = Parameters::register_parameter("WORLD_SWARM-gridX",
                                                                                                  16,
                                                                                                  "size of grid X");
shared_ptr<ParameterLink<int>> SwarmWorldParameters::gridYSizePL = Parameters::register_parameter("WORLD_SWARM-gridY",
                                                                                                  10,
                                                                                                  "size of grid Y");
shared_ptr<ParameterLink<int>> SwarmWorldParameters::worldUpdatesPL = Parameters::register_parameter(
        "WORLD_SWARM-worldUpdates",
        100,
        "amount of time a brain is tested");
shared_ptr<ParameterLink<double>> SwarmWorldParameters::nAgentsPL = Parameters::register_parameter(
        "WORLD_SWARM-nAgents", 1.0,
        "how many agents in a game in rate");
shared_ptr<ParameterLink<int>> SwarmWorldParameters::senseAgentsPL = Parameters::register_parameter(
        "WORLD_SWARM-senseAgents", 0,
        "1 if ants can sense");
shared_ptr<ParameterLink<string>> SwarmWorldParameters::senseSidesPL = Parameters::register_parameter(
        "WORLD_SWARM-senseSides",
        (string) "1",
        "1 if ants can sense");
shared_ptr<ParameterLink<int>> SwarmWorldParameters::resetOutputsPL = Parameters::register_parameter(
        "WORLD_SWARM-resetOutputs",
        0,
        "1 if outputs should be reseted after one time step");
shared_ptr<ParameterLink<int>> SwarmWorldParameters::pheroPL = Parameters::register_parameter("WORLD_SWARM-phero", 0,
                                                                                              "do it with pheromones sexy"); //lol
shared_ptr<ParameterLink<int>> SwarmWorldParameters::hasPenaltyPL = Parameters::register_parameter(
        "WORLD_SWARM-hasPenalty", 1,
        "1 if penalty when agents get hit");
shared_ptr<ParameterLink<int>> SwarmWorldParameters::resetPositionsPL = Parameters::register_parameter(
        "WORLD_SWARM-resetPositionsInterval", 1,
        "interval between each position reset. value == 0 means the positions won't be reset at all");
shared_ptr<ParameterLink<double>> SwarmWorldParameters::penaltyPL = Parameters::register_parameter(
        "WORLD_SWARM-penalty", 0.075,
        "amount of penalty for hit");
shared_ptr<ParameterLink<double>> SwarmWorldParameters::invalidMovePenaltyPL = Parameters::register_parameter(
        "WORLD_SWARM-invalidMovePenalty", 0.025,
        "amount of penalty for performing an invalid move (e.g. against a wall)");

shared_ptr<ParameterLink<int>> SwarmWorldParameters::waitForGoalPL = Parameters::register_parameter(
        "WORLD_SWARM-waitForGoal",
        500,
        "timestep till the next goal is possible");

shared_ptr<ParameterLink<int>> SwarmWorldParameters::repeatsPL = Parameters::register_parameter(
        "WORLD_SWARM-repeats",
        5,
        "how often the simulation is repeated in one generation");


shared_ptr<ParameterLink<string>> SwarmWorldParameters::gridInitializerPL = Parameters::register_parameter(
        "WORLD_SWARM-gridInitializer", string("firstAvailable"), "which grid initializer function to use");


shared_ptr<ParameterLink<string>> SwarmWorldParameters::simulationModePL = Parameters::register_parameter(
        "WORLD_SWARM-simulationMode", string("heterogeneous"),
        "whether to use heterogeneous or homogeneous simulation");

shared_ptr<ParameterLink<string>> SwarmWorldParameters::scoringStrategyPL = Parameters::register_parameter(
        "WORLD_SWARM-scoringStrategy", string("individual"), "how the individual's scores are calculated"
);


#endif //MABE_SWARMWORLDPARAMETERS_H
