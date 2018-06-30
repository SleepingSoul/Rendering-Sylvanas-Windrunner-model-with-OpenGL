// Copyright 2018 Tihran Katolikian
// here are the implementations of few
// important classes:
// @ Vertex class - stores all vertex data
// @ Texture class - stores all texture (map) data
// @ Mesh class - implements Mesh and provides options
// to store mesh logical data, maniputate it and render
// it

#ifndef MESH_HPP
#define MESH_HPP

#include <cstddef>
#include <cassert>
#include <string>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Texture
{
public:
    enum TexType {diffuseType, specularType, normalType, heightType};
    unsigned int id;
    TexType type;
    std::string path;
    std::string getTypeString() const;
    void setTypeByName(const std::string &stype);
};

std::string Texture::getTypeString() const
{
    switch (type) {
        case Texture::diffuseType:
        return "texture_diffuse";
        case Texture::specularType:
        return "texture_specular";
        case Texture::normalType:
        return "texture_normal";
        case Texture::heightType:
        return "texture_height";
        default:
        assert(false);
    }
}

void Texture::setTypeByName(const std::string &stype)
{
    if (stype == "texture_diffuse")
        type = diffuseType;
    else if (stype == "texture_specular")
        type = specularType;
    else if (stype == "texture_normal")
        type = normalType;
    else if (stype == "texture_height")
        type = heightType;
}

class Mesh
{
public:
    Mesh(const std::vector <Vertex> &init_vertices,
         const std::vector <unsigned> &init_indices,
         const std::vector <Texture> &init_textures)
    :   vertices(init_vertices),
        indices(init_indices),
        textures(init_textures)
    {
        // -----------------------
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    Mesh(std::vector <Vertex> &&init_vertices,
         std::vector <unsigned> &&init_indices,
         std::vector <Texture> &&init_textures)
    :   vertices(init_vertices),
        indices(init_indices),
        textures(init_textures)
    {
        // -----------------------
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    ~Mesh() = default;

    // render the mesh
    void draw(Shader &shader) const
    {
        // bind appropriate textures
        unsigned diffuse_num  = 1;
        unsigned specular_num = 1;
        unsigned normal_num   = 1;
        unsigned height_num   = 1;
        for(unsigned i = 0; i < textures.size(); ++i) {
            // -------------------------
            // active proper texture unit before binding
            glActiveTexture(GL_TEXTURE0 + i);
            // -------------------------
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].getTypeString();

            if (name == "texture_diffuse")
				number = std::to_string(diffuse_num++);
			else if (name == "texture_specular")
				number = std::to_string(specular_num++);
            else if (name == "texture_normal")
				number = std::to_string(normal_num++);
            else if (name == "texture_height")
			    number = std::to_string(height_num++);

            glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);   // is it needed?!!!!!!!
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    unsigned VBO;
    unsigned EBO;
    unsigned VAO;
    
    std::vector <Vertex> vertices;
    std::vector <unsigned> indices;
    std::vector <Texture> textures;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                     vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     indices.data(), GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast <void *>(0));
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast <void *>(offsetof(Vertex, normal)));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast <void *>(offsetof(Vertex, texture_coords)));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast <void *>(offsetof(Vertex, tangent)));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast <void *>(offsetof(Vertex, bitangent)));

        glBindVertexArray(0);
    }
};

#endif  // MESH_HPP
