//
// Created by Le on 15.10.2017.
//

#ifndef MABE_SERIALIZER_H
#define MABE_SERIALIZER_H

#include <fstream>
#include "sstream"
#include "vector"
#include "algorithm"
#include <functional>

class Serializer {
protected:
    std::vector<std::function<void(Serializer)>> serializers;


public:
    Serializer &withSerializer(const std::function<void(Serializer)> &serializer);

    Serializer &withFileSerializer(const std::string &outputDirectory,
                                           const std::string &fileName,
                                           const std::string &content);

    void serializeToFile(const std::string &outputDirectory,
                         const std::string &fileName,
                         const std::string &csvContent) const;

    virtual void serialize();
};


#endif //MABE_SERIALIZER_H
