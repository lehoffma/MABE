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

#endif //MABE_SWARMWORLDPARAMETERS_H
