#pragma once

#include <GL/glew.h>
#include <vector>

class Texture
{
public:
    Texture();
    bool loadFromFile(const char* filePath);
    bool loadSkyboxFromFile(const std::vector<std::string>& faces);
    GLuint id() const;
    bool isLoaded() const;

private:
    GLuint m_id;           // Identifier in GPU memory
    int m_width;		   // Number of columns
    int m_height;          // Number of lines
    int m_channelCount;    // Number of channels : 3 for color images and 1 for grayscale images
    bool m_isLoaded;
};