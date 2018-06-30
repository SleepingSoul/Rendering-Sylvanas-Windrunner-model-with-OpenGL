#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gl_image.hpp"
#include "shader.hpp"
#include "mesh.hpp"

class Model 
{
public:
    //----------------------
    //constructor expects the filepath to
    // 3d model
    Model(const std::string &path, const bool gamma = false)
    :   gamma_correction(gamma)
    {
        loadModel(path);
    }
    ~Model() = default;

    void draw(Shader &shader) const
    {
        for (auto &mesh : meshes)
            mesh.draw(shader);
    }
private:
    //----------------------
    // stores all the textures loaded so far, optimization
    // to make sure textures aren't loaded more than once.
    std::vector <Texture> textures_loaded;
    std::vector <Mesh> meshes;
    std::string directory;
    bool gamma_correction;
    //----------------------
    // loads a model with supported ASSIMP extensions from file
    // and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string &path)
    {
        //----------------------
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
                                                 aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);
        //----------------------
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
           !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString()
                      << '\n';
            return;
        }
        //----------------------
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        //----------------------
        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    //----------------------
    // processes a node in a recursive fashion. Processes each
    // individual mesh located at the node and repeats this
    // process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene)
    {
        //----------------------
        // process each mesh located at the current node
        for(unsigned i = 0; i < node->mNumMeshes; ++i) {
            //----------------------
            // the node object only contains indices to index
            // the actual objects in the scene. 
            // the scene contains all the data, node is just
            //to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then
        // recursively process each of the children nodes
        for(unsigned i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        //----------------------
        // data to fill
        std::vector <Vertex> vertices;
        std:: vector<unsigned> indices;
        std::vector <Texture> textures;

        //----------------------
        // Walk through each of the mesh's vertices
        for(unsigned i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;
            
            vertex.position = {mesh->mVertices[i].x,
                               mesh->mVertices[i].y,
                               mesh->mVertices[i].z};
                               
            vertex.normal = {mesh->mNormals[i].x,
                             mesh->mNormals[i].y,
                             mesh->mNormals[i].z};

            if(mesh->mTextureCoords[0]) {
                vertex.texture_coords = {mesh->mTextureCoords[0][i].x,
                                         mesh->mTextureCoords[0][i].y};
            }
            else
                vertex.texture_coords = {0.0f, 0.0f};

            vertex.tangent = {mesh->mTangents[i].x,
                              mesh->mTangents[i].y,
                              mesh->mTangents[i].z};

            vertex.bitangent = {mesh->mBitangents[i].x,
                                mesh->mBitangents[i].y,
                                mesh->mBitangents[i].z};

            vertices.push_back(vertex);
        }
        
        //----------------------
        // now walk through each of the mesh's faces (a face is a mesh
        // its triangle) and retrieve the corresponding vertex indices.
        for(unsigned i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            //----------------------
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }
        
        //----------------------
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        //----------------------
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN
        // all this samplers are members of struct Material material.

        // 1. diffuse maps
        std::vector <Texture> diffuse_maps = loadMaterialTextures(material,
                                                                  aiTextureType_DIFFUSE,
                                                                  "texture_diffuse");
        textures.insert(textures.end(), diffuse_maps.cbegin(), diffuse_maps.cend());
        // 2. specular maps
        std::vector <Texture> specular_maps = loadMaterialTextures(material,
                                                                   aiTextureType_SPECULAR,
                                                                   "texture_specular");
        textures.insert(textures.end(), specular_maps.cbegin(), specular_maps.cend());
        // 3. normal maps
        std::vector <Texture> normal_maps = loadMaterialTextures(material,
                                                               aiTextureType_HEIGHT,
                                                               "texture_normal");
        textures.insert(textures.end(), normal_maps.cbegin(), normal_maps.cend());
        // 4. height maps
        std::vector <Texture> height_maps = loadMaterialTextures(material,
                                                                 aiTextureType_AMBIENT,
                                                                 "texture_height");
        textures.insert(textures.end(), height_maps.cbegin(), height_maps.cend());
        
        //----------------------
        // return a mesh object created from the extracted mesh data
        return Mesh(std::move(vertices), std::move(indices), std::move(textures));
    }

    // -----------------------
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector <Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string type_name)
    {
        std::vector <Texture> textures;
        for(unsigned i = 0; i < mat->GetTextureCount(type); ++i)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // -----------------------
            // check if texture was loaded before and if so, continue to next iteration:
            // skip loading a new texture
            bool skip = false;
            for(auto &texture : textures_loaded) {
                if(std::strcmp(texture.path.data(), str.C_Str()) == 0) {
                    textures.push_back(texture);
                    // -----------------------
                    // a texture with the same filepath has already been loaded,
                    // continue to next one. (optimization)
                    skip = true;
                    break;
                }
            }
            if(!skip) {
                // -----------------------
                // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = GLTextureGenerator::generateTexture2D(directory + '/' + str.C_Str());
                texture.setTypeByName(type_name);
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
};

#endif  // MODEL_HPP
