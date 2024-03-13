#include "model.hpp"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

#include "path_resolver.hpp"

Model* Model::loadFromFile(std::string path) {
    Model* newModel = new Model();
    std::string finalPath = GetResourcePath(Resource::MODEL, path);
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(finalPath, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger::Error("ERROR::ASSIMP::%s", import.GetErrorString());
        return 0;
    }
    newModel->processNode(scene->mRootNode, scene);
    return newModel;
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

RenderMesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<float> points;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // process vertex positions, normals and texture coordinates
        Vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        Vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        Vec2 texture;
        if (mesh->mTextureCoords[0]) {
            texture.x = mesh->mTextureCoords[0][i].x;
            texture.y = mesh->mTextureCoords[0][i].y;
        }
        points.push_back(position.x);
        points.push_back(position.y);
        points.push_back(position.z);
        points.push_back(normal.x);
        points.push_back(normal.y);
        points.push_back(normal.z);
        points.push_back(texture.x);
        points.push_back(texture.y);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process material
    Material material = Material{1.0, Texture("Cat_diffuse.png")};
    // does not work now
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        // convert material from assimp to our material
    }
    RenderMesh newMesh = RenderMesh(points, indices, material);
    return newMesh;
}