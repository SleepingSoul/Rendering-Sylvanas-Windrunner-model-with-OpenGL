/* copyright Tihran Katolikian 2018*/

#ifndef GL_IMAGE_HPP
#define GL_IMAGE_HPP

#include <cassert>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#include <glad/glad.h>
#include <GL/glfw3.h>

class GLTextureGenerator
{
public:
    GLTextureGenerator() = delete;
    static void setFlipVerticallyOnLoad(const bool f)
    {
        stbi_set_flip_vertically_on_load(f);
    }
    static unsigned generateTexture2D(const char *path)
    {
        // ------------------------------
        // creating, generating, binding ans setting-up the texture object
        unsigned texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // ------------------------------
        // loading image in memory using stbi_load function from stbi_image.h
        int width, height, chan_num;
        unsigned char *data = stbi_load(path, &width, &height,
                                        &chan_num, 0);
        
        GLenum format;
        switch (chan_num) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
        default:
            std::cout << "Cannot identify format. Perhaps, this format is not "
                         "supported by this library (for example, .tif will "
                         "cause this assertion)\n";
            assert(false);
        }
        
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                         format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
            std::cout << "Failed to load texture\n";
    
        // ------------------------------
        // deallocating image memory, because now it is already stored
        // in GPU memory
        stbi_image_free(data);

        return texture;
    }
    static unsigned generateTexture2D(const std::string &path, const GLenum color_model = 0)
    {
        // ------------------------------
        // creating, generating, binding ans setting-up the texture object
        unsigned texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // ------------------------------
        // loading image in memory using stbi_load function from stbi_image.h
        int width, height, chan_num;
        unsigned char *data = stbi_load(path.c_str(), &width, &height,
                                        &chan_num, 0);
        
        GLenum format;
        switch (chan_num) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            std::cout << "Cannot identify format. Perhaps, this format is not "
                         "supported by this library (for example, .tif will "
                         "cause this assertion)\n";
            assert(false);
        }
        
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                         format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
            std::cout << "Failed to load texture\n";
    
        // ------------------------------
        // deallocating image memory, because now it is already stored
        // in GPU memory
        stbi_image_free(data);

        return texture;
    }
};

#endif // GL_IMAGE_HPP
