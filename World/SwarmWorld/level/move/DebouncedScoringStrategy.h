//
// Created by Le on 16.11.2017.
//

#ifndef MABE_DEBOUNCEDGOALSTRATEGY_H
#define MABE_DEBOUNCEDGOALSTRATEGY_H


#include "ScoringStrategy.h"
#include "../Field.h"

class DebouncedGoalStrategy : public ScoringStrategy<Field>{
public:
    bool isValid(Level<Field>* level, const std::shared_ptr<Field> &field, const std::pair<int, int>& to) override;

    void scoringSideEffect(std::shared_ptr<Field> &field) override;
};


#endif //MABE_DEBOUNCEDGOALSTRATEGY_H
