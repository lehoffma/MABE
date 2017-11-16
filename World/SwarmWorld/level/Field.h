//
// Created by Le on 15.11.2017.
//

#ifndef MABE_FIELD_H
#define MABE_FIELD_H

#include <utility>
#include <memory>

#include "../model/Agent.h"
#include "FieldType.h"

class Field {
public:
    std::shared_ptr<Agent> agent;
    FieldType fieldType;

    Field() : agent(nullptr), fieldType(WALL) {
    }

    Field(int type) {
        this->agent = nullptr;
        switch (type) {
            case 0:
                this->fieldType = WALL;
                break;
            case 1:
                this->fieldType = FLOOR;
                break;
            case 3:
                this->fieldType = START;
                break;
            case 4:
                this->fieldType = GOAL;
                break;
            default:
                this->fieldType = FLOOR;
        }
    }

    Field(std::shared_ptr<Agent> agent, FieldType fieldType) : agent(agent), fieldType(fieldType) {

    }
};

#endif //MABE_FIELD_H
