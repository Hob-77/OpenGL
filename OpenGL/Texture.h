#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>

class Texture
{
public:
    unsigned int ID;
    std::string type;
    std::string path;

    Texture(const std::string& path, GLenum format = GL_RGB, bool flip = true);

    void bind(unsigned int unit = 0) const;
    void unbind() const;

    ~Texture();
};

#endif