//
// Created by Le on 07.12.2017.
//

#include "SequenceScoringStrategy.h"

bool SequenceScoringStrategy::isValid(Level<Field> *level, const Field &field, const std::pair<int, int> &to) {
    const auto history = field.agent->getHistory();
    const auto historySize = history.size();

    if (historySize < this->sequence.size()) {
        return false;
    }
    //walk backwards through the history and search for the given sequence of fields
    for (size_t i = 0; i < this->sequence.size(); i++) {
        const auto historyEntry = history[historySize - 1 - i];
        if (!level->isFieldType(historyEntry, this->sequence[i])) {
            return false;
        }
    }
    return true;
}

void SequenceScoringStrategy::scoringSideEffect(Field &field) {
    //nothing
}

SequenceScoringStrategy::SequenceScoringStrategy(std::vector<FieldType> sequence) : sequence(std::move(sequence)) {

}
