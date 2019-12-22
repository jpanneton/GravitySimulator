
#include "Texture.h"
#include <fstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
    : m_id{}
    , m_width{}
    , m_height{}
    , m_channelCount{}
    , m_isLoaded{}
{
}

bool Texture::loadFromFile(const char* filePath)
{
    unsigned char* data = stbi_load(filePath, &m_width, &m_height, &m_channelCount, 0);

    if (data)
    {		
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        auto format = (m_channelCount == 4 ? GL_RGBA : (m_channelCount == 3 ? GL_RGB : GL_RED));
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return m_isLoaded = true;
    }

    return m_isLoaded = false;
}

bool Texture::loadSkyboxFromFile(const std::vector<std::string>& faces)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &m_width, &m_height, &m_channelCount, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            return m_isLoaded = false;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return m_isLoaded = true;
}


GLuint Texture::id() const
{
    return m_id;
}

bool Texture::isLoaded() const
{
    return m_isLoaded;
}