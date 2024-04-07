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

Model* Model::loadFromFile(std::string path, ShaderProgram *shader) {
    Model* newModel = loadFromFile(path);
    newModel->shader = shader;
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
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // process vertex positions, normals and texture coordinates
        Vertex vertex;
        SetVertexBoneDataToDefault(&vertex);
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.TexCoords = {0.f, 0.f};
        }
        vertices.push_back(vertex);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    Texture t = Texture("default.png");
    float shininess = 1.0f;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString s;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &s);
            t = Texture(std::string(s.C_Str()));

            if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
                aiString s;
                mat->GetTexture(aiTextureType_SPECULAR, 0, &s);
                t.loadImage(std::string(s.C_Str()));
            }
        }
        mat->Get(AI_MATKEY_SHININESS, shininess);
    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);
    return RenderMesh(vertices, indices, Material{shininess, t});
}

void Model::setMaterial(Material material) {
    for (RenderMesh& mesh : meshes) {
        mesh.setMaterial(material);
    }
}

Model *Model::fromMesh(Mesh *mesh, Material material) {
    Model *newModel = new Model();
    std::vector<RenderMesh> meshes;
    meshes.push_back(*(new RenderMesh(mesh, material)));
    newModel->meshes = meshes;
    return newModel;
}

Model *Model::fromMesh(Mesh *mesh, Material material, ShaderProgram *shader) {
    Model *newModel = new Model();
    std::vector<RenderMesh> meshes;
    meshes.push_back(*(new RenderMesh(mesh, material)));
    newModel->meshes = meshes;
    newModel->shader = shader;
    return newModel;
}
