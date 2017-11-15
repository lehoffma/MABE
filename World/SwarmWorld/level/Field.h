//
// Created by Le on 15.11.2017.
//

#ifndef MABE_FIELD_H
#define MABE_FIELD_H

#include <utility>
#include <bits/shared_ptr.h>

#include "../model/Agent.h"
#include "FieldType.h"

class Field {
public:
    std::shared_ptr<Agent> agent;
    FieldType fieldType;

    Field(std::shared_ptr<Agent> agent, FieldType fieldType) : agent(agent), fieldType(fieldType) {

    }
};

#endif //MABE_FIELD_H
