//
// Created by Le on 11.06.2018.
//



#include "SwarmWorldParameters.h"

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

