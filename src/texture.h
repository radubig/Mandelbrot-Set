#pragma once
#include <GL/glew.h>
#include <cstdarg>
#include <stb_image.h>
#include <vector>

uint32_t LoadPNG_1D(const char* path)
{
    int m_width, m_height, m_bpp;
    unsigned char* data;
    uint32_t texture;

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(path, &m_width, &m_height, &m_bpp, 4);

    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, m_width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texture;
}

void LoadTexVectorVar(std::vector<uint32_t>& v, int n, ...)
{
    v.clear();
    va_list args;
    va_start(args, n);
    for(int i=0; i<n; i++)
    {
        const char* path = va_arg(args, const char*);
        v.push_back(LoadPNG_1D(path));
    }
    va_end(args);
}
