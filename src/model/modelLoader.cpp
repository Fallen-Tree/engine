#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <model.hpp>

Model* Model::loadFromFile(const char* filePath) {
    std::filesystem::path fileExtension = ((std::filesystem::path) filePath).extension();
    if (fileExtension != ".obj") {
        std::cout << "INCORRECT::FILE::FORMAT" << std::endl;
        return 0;
    }
    std::ifstream objFile;
    objFile.open(filePath);

    // Check if file opened correctly

    // zero point/normal/texture is used if it is not defined in .obj
    std::vector<float> modelPoints(3, 0);
    std::vector<float> modelNormals(3, 0);
    std::vector<float> modelTextures(2, 0);
    std::vector<float> modelVertices(0);
    int modelVerticeCount = 0;
    std::vector<unsigned> modelIndices(0);
    if (objFile.is_open()) {
        int line_num = 0;
        std::string line;
        while (objFile.good()) {
            std::getline(objFile, line);
            std::stringstream lineStream;
            lineStream << line;
            std::string key;
            lineStream >> key;
            if (key == "v") {
                // read vertice coordinates
                float x, y, z;
                lineStream >> x >> y >> z;
                modelPoints.push_back(x);
                modelPoints.push_back(y);
                modelPoints.push_back(z);
            } else if (key == "vn") {
                // read normal coordinates
                float x, y, z;
                lineStream >> x >> y >> z;
                modelNormals.push_back(x);
                modelNormals.push_back(y);
                modelNormals.push_back(z);
            } else if (key == "vt") {
                // read texture coordinates
                float x, y;
                lineStream >> x >> y;
                modelTextures.push_back(x);
                modelTextures.push_back(y);
            } else if (key == "f") {
                // add triangles
                std::vector<unsigned> pointsIndices(0);
                std::vector<unsigned> normalsIndices(0);
                std::vector<unsigned> texturesIndices(0);

                std::vector<unsigned> faceIndices;
                std::string verticeString;
                while (lineStream >> verticeString) {
                    verticeString += "/";  // adding endline character
                    std::string indexString = "";
                    int currentField = 0;
                    for (unsigned i = 0; i < verticeString.size(); ++i) {
                        if (verticeString[i] == '/') {
                            int index = 1;
                            if (indexString != "") {
                                index = stoi(indexString);
                            }
                            if (currentField == 0) {
                                pointsIndices.push_back(index);
                            } else if (currentField == 1) {
                                texturesIndices.push_back(index);
                            } else if (currentField == 2) {
                                normalsIndices.push_back(index);
                            } else {
                                // incorrect file
                            }
                            indexString = "";
                            currentField++;
                        } else {
                            indexString += verticeString[i];
                        }
                    }
                    if (normalsIndices.size() < pointsIndices.size()) {
                        normalsIndices.push_back(1);
                    }
                    if (texturesIndices.size() < pointsIndices.size()) {
                        texturesIndices.push_back(1);
                    }
                    faceIndices.push_back(modelVerticeCount);
                    modelVerticeCount++;
                }
                for (int i = 0; i < faceIndices.size(); ++i) {
                    modelVertices.push_back(modelPoints[3 * pointsIndices[i]]);
                    modelVertices.push_back(modelPoints[3 * pointsIndices[i] + 1]);
                    modelVertices.push_back(modelPoints[3 * pointsIndices[i] + 2]);
                    modelVertices.push_back(modelNormals[3 * normalsIndices[i]]);
                    modelVertices.push_back(modelNormals[3 * normalsIndices[i] + 1]);
                    modelVertices.push_back(modelNormals[3 * normalsIndices[i] + 2]);
                    // modelVertices.push_back(modelTextures[2 * texturesIndices[i]]);
                    // modelVertices.push_back(modelTextures[2 * texturesIndices[i] + 1]);
                }
                for (int i = 1; i < faceIndices.size() - 1; ++i) {
                    modelIndices.push_back(faceIndices[0]);
                    modelIndices.push_back(faceIndices[i]);
                    modelIndices.push_back(faceIndices[i + 1]);
                }
            } else {
                // not a valid key at the start of the string
            }
        }
        objFile.close();
    }
    Model* newModel = new Model(modelVertices, modelIndices);
    return newModel;
}
