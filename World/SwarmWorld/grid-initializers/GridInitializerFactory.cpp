//
// Created by Le on 11.06.2018.
//

#include "GridInitializerFactory.h"

const std::map<std::string, GridInitializerType> GridInitializerFactory::enumMap = {
        {"firstAvailable",   firstAvailable},
        {"random",           random},
        {"chineseMilitary",  chineseMilitary},
        {"greatestDistance", greatestDistance}
};
