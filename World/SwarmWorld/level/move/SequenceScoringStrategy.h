//
// Created by Le on 07.12.2017.
//

#ifndef MABE_RUNTHROUGHGOALSTRATEGY_H
#define MABE_RUNTHROUGHGOALSTRATEGY_H


#include "ScoringStrategy.h"
#include "../Field.h"

class SequenceScoringStrategy : public ScoringStrategy<Field> {
private:
    std::vector<FieldType> sequence;

public:
    explicit SequenceScoringStrategy(std::vector<FieldType> sequence);

    bool isValid(Level<Field> *level, const std::shared_ptr<Field> &field, const std::pair<int, int> &to) override;

    void scoringSideEffect(std::shared_ptr<Field> &field) override;
};


#endif //MABE_RUNTHROUGHGOALSTRATEGY_H
