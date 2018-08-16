//
// Created by Le on 03.11.2017.
//


#include <iostream>
#include "Serializer.h"

void Serializer::serializeToFile(const std::string &outputDirectory,
                                 const std::string &fileName,
                                 const std::string &csvContent) const {
    std::stringstream fileNameStream;
    fileNameStream << outputDirectory << "/" << fileName;
    std::string outputFile = fileNameStream.str();

    std::ofstream fileOutputStream;
    fileOutputStream.open(outputFile);

    fileOutputStream << csvContent;

    fileOutputStream.close();
}

void Serializer::serialize() {
    std::cout << "serializing size: " << this->serializers.size() << std::endl;
    std::for_each(this->serializers.begin(), this->serializers.end(),
                  [this](std::function<void(Serializer)> serializer) {
                      serializer(*this);
                  });
}

Serializer &Serializer::withSerializer(const std::function<void(Serializer)> &serializer) {
    this->serializers.emplace_back(serializer);
    return *this;
}

Serializer &Serializer::withFileSerializer(const std::string &outputDirectory,
                                           const std::string &fileName,
                                           const std::string &content) {
    this->serializers.emplace_back(
            [outputDirectory, fileName, content](Serializer serializer) {
                serializer.serializeToFile(outputDirectory, fileName, content);
            }
    );
    return *this;
}
