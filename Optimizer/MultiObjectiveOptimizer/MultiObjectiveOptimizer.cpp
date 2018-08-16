//
// Created by Le on 18.02.2018.
//

#include "MultiObjectiveOptimizer.h"
#include "ObjectivePoint.h"
#include "MultiObjectiveUtils.h"
#include "ObjectiveUtil.h"
#include "../../World/SwarmWorld/util/StringUtils.h"
#include "MapUtil.h"
#include "../../World/SwarmWorld/serialize/OrganismSerializer.h"


shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::minimizeObjectivesPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-minObjectives", (string) "",
        "Comma-separated list of MTree values that will be minimized by the optimizer");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::maximizeObjectivesPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-maxObjectives", (string) "DM_AVE[score]",
        "Comma-separated list of MTree values that will be maximized by the optimizer");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::optimizerFileNamePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-optimizerFileName", (string) "optimizer.csv",
        "Name of the file the optimizer writes the meta information like fronts, front spread, hypervolume, etc. to");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::frontDataFileNamePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-frontDataFileName", (string) "front",
        "Name of the file the optimizer writes the organism data of best front (i.e. data files) to");
shared_ptr<ParameterLink<int>> MultiObjectiveOptimizer::serializationIntervalPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-serializationInterval", 100,
        "How often the meta information is serialized (i.e. every X generations)");


MultiObjectiveOptimizer::MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT) : AbstractOptimizer(_PT) {
    std::vector<std::string> minimizeCsv{};
    std::vector<std::string> maximizeCsv{};
    convertCSVListToVector(minimizeObjectivesPL->get(PT), minimizeCsv, ',');
    convertCSVListToVector(maximizeObjectivesPL->get(PT), maximizeCsv, ',');
    optimizerFileName = optimizerFileNamePL->get();
    frontDataFileName = frontDataFileNamePL->get();
    serializationInterval = serializationIntervalPL->get();
    for (const auto &minimize: minimizeCsv) {
        minimizedObjectives.push_back(stringToMTree(minimize));
    }
    for (const auto &maximize: maximizeCsv) {
        maximizedObjectives.push_back(stringToMTree(maximize));
    }
}

void MultiObjectiveOptimizer::initObjectiveMap(std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
    for (auto &minimizedObjective: minimizedObjectives) {
        objectiveMap[minimizedObjective] = true;
    }
    for (auto &maximizedObjective: maximizedObjectives) {
        objectiveMap[maximizedObjective] = false;
    }
}

std::string MultiObjectiveOptimizer::serializeObjectiveScores(
        std::vector<std::shared_ptr<Organism>> &population,
        std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
) {
    std::stringstream ss{};
    auto i = 0;
    for (auto &objectiveEntry: objectiveMap) {
        auto objective = objectiveEntry.first;
        auto minimize = objectiveEntry.second;
        double bestObjectiveValue = (minimize ? INFINITY : -INFINITY);
        double valueSum = 0;
        for (auto &org: population) {
            auto value = objective->eval(org->dataMap, PT)[0];
            if (minimize) {
                bestObjectiveValue = std::min(bestObjectiveValue, value);
            } else {
                bestObjectiveValue = std::max(bestObjectiveValue, value);
            }
            valueSum += value;
        }
        auto average = valueSum / population.size();
        ss << "best " << objective->getFormula() << ": " << bestObjectiveValue
           << " | avg " << objective->getFormula() << ": " << average;

        if (i < objectiveMap.size()) {
            ss << " || ";
        }
        i++;
    }

    return ss.str();
}

void MultiObjectiveOptimizer::serializeToFile(const std::vector<std::shared_ptr<Organism>> &population,
                                              const vector<vector<int>> &frontIndices,
                                              const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
    DataMap dataMap{};

    //hv, fronts, frontSize, frontSpread, frontUniformity
    auto hyperVolume = getHyperVolume(population, frontIndices[0], objectiveMap);
    dataMap.set("hyperVolume", hyperVolume);
    dataMap.setOutputBehavior("hyperVolume", DataMap::FIRST);

    std::vector<std::string> frontValues{frontIndices.size()};
    std::transform(frontIndices.begin(), frontIndices.end(), frontValues.begin(),
                   [&population](const std::vector<int> front) -> std::string {
                       std::vector<std::string> ids{front.size()};
                       std::transform(front.begin(), front.end(), ids.begin(),
                                      [&population](const int index) -> std::string {
                                          auto organismId = population[index]->ID;
                                          auto collisions = population[index]->dataMap.getAverage("collisions");
                                          auto gatePassages = population[index]->dataMap.getAverage("gatePassages");
                                          auto movementPenalties = population[index]->dataMap.getAverage(
                                                  "movementPenalties");

                                          std::stringstream ss{};
                                          ss << organismId << ";" << collisions << ";" << gatePassages << ";"
                                             << movementPenalties;
                                          return ss.str();
                                      });

                       return StringUtils::join(ids, "|");
                   });
    dataMap.set("frontValues", frontValues);


    std::vector<std::string> stringFronts{frontIndices.size()};
    std::transform(frontIndices.begin(), frontIndices.end(), stringFronts.begin(),
                   [&population](const std::vector<int> front) -> std::string {
                       std::vector<std::string> ids{front.size()};
                       std::transform(front.begin(), front.end(), ids.begin(),
                                      [&population](const int index) -> std::string {
                                          return std::to_string(population[index]->ID);
                                      });

                       return StringUtils::join(ids, "|");
                   });
    dataMap.set("fronts", stringFronts);

    std::vector<int> frontSizes{};
    for (auto const &front: frontIndices) {
        frontSizes.emplace_back(front.size());
    }
    dataMap.set("frontSize", frontSizes);

    dataMap.set("frontSpread", std::vector<double>{});
    dataMap.set("frontUniformity", std::vector<double>{});
    dataMap.setOutputBehavior("frontSpread", DataMap::LIST);
    dataMap.setOutputBehavior("frontUniformity", DataMap::LIST);
    for (auto const &front: frontIndices) {
        auto spread = frontSpread(population, front, objectiveMap);
        auto uniformity = frontUniformity(population, front, objectiveMap);

        dataMap.append("frontSpread", spread);
        dataMap.append("frontUniformity", uniformity);
    }

    dataMap.set("update", Global::update);

    std::cout << std::endl << serializeDataMap(dataMap);
    dataMap.writeToFile(optimizerFileName);


    //fill front files
    auto bestFront = frontIndices[0];
    auto fileName = std::string().append(frontDataFileName).append("_").append(std::to_string(Global::update)).append(
            ".csv");

    //todo remove, only works with swarm world
    auto senseAgents = SwarmWorldParameters::senseAgentsPL->get() == 1;
    std::vector<int> senseSides{};
    convertCSVListToVector(SwarmWorldParameters::senseSidesPL->get(), senseSides);
    int requiredInputs = static_cast<int>(senseSides.size() * (senseAgents ? 2 : 1));
    int requiredOutputs = 2;
    auto resetOutputs = SwarmWorldParameters::resetOutputsPL->get() == 1;

    for (const auto &index: bestFront) {
        bool addBrainData = !population[index]->dataMap.fieldExists("TPM");
        if (addBrainData) {
            OrganismSerializer::addBrainToDataMap(population[index], resetOutputs, requiredInputs, requiredOutputs);
        }

        population[index]->dataMap.writeToFile(fileName);

        if (addBrainData) {
            population[index]->dataMap.clear("TPM");
            population[index]->dataMap.clear("CM");
        }
    }
    FileManager::closeFile(fileName);
}

double MultiObjectiveOptimizer::getHyperVolume(const std::vector<std::shared_ptr<Organism>> &population,
                                               const vector<int> &optimalFrontIndices,
                                               const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {

    const std::vector<ObjectivePoint> &points = MultiObjective::frontToPoints(population, optimalFrontIndices, PT,
                                                                              objectiveMap);
    const ObjectivePoint refPoint = MultiObjective::getRefPoint(objectiveMap);

    return MultiObjective::HyperVolume::lebMeasure(
            points,
            refPoint,
            objectiveMap
    );
}

double MultiObjectiveOptimizer::frontSpread(const std::vector<std::shared_ptr<Organism>> &population,
                                            const vector<int> &frontIndices,
                                            const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
    if (frontIndices.size() == 1) {
        return 0;
    }

    const std::vector<ObjectivePoint> &points = MultiObjective::frontToPoints(population, frontIndices, PT,
                                                                              objectiveMap);

    //calculate (upper left, lower right or lower left, upper right) corners from points
    auto distance = 0;
    for (auto const &objectiveEntry : objectiveMap) {
        auto objective = objectiveEntry.first->getFormula();
        auto min = INFINITY;
        auto max = -INFINITY;

        for (auto const &point: points) {
            auto value = Map::get(point, objective);
            if (value < min) {
                min = value;
            }
            if (value > max) {
                max = value;
            }
        }

        //calculate distance between corner points
        distance += std::pow(max - min, 2);
    }

    return std::sqrt(distance);
}

double MultiObjectiveOptimizer::frontUniformity(const std::vector<std::shared_ptr<Organism>> &population,
                                                const vector<int> &frontIndices,
                                                const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
    if (frontIndices.size() == 1) {
        return 0;
    }

    const std::vector<ObjectivePoint> &points = MultiObjective::frontToPoints(population, frontIndices, PT,
                                                                              objectiveMap);

    std::vector<std::shared_ptr<MultiObjectiveSolution>> solutions{};
    for (auto &organism: population) {
        solutions.push_back(make_shared<MultiObjectiveSolution>(organism, 0, 0));
    }

    //reassign crowding distance of edge cases because we can't add up infinity properly
    //the boundary points are assigned a crowding distance of twice the distance to the nearest neighbor
    MultiObjective::NsgaII::crowdingDistanceAssignment(solutions, frontIndices, PT, objectiveMap, false);


    auto crowdingDistanceAvg = 0;
    for (const auto &index: frontIndices) {
        crowdingDistanceAvg += solutions[index]->crowdingDistance;
    }
    crowdingDistanceAvg /= frontIndices.size();

    auto uniformity = 0;
    for (const auto &index: frontIndices) {
        uniformity += std::abs(solutions[index]->crowdingDistance - crowdingDistanceAvg);
    }
    uniformity /= frontIndices.size();

    return uniformity;
}

std::string MultiObjectiveOptimizer::serializeDataMap(DataMap &dataMap) {
    auto keys = dataMap.getKeys();
    std::vector<std::string> entries{keys.size()};
    std::transform(keys.begin(), keys.end(), entries.begin(), [&dataMap](std::string key) -> std::string {
        return key + ": " + dataMap.getStringOfVector(key);
    });

    return StringUtils::join(entries, " | ");
}



















