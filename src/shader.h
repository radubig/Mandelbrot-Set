#ifndef MANDELBROTSET_SHADER_H
#define MANDELBROTSET_SHADER_H

class Shader
{
public:
    enum ShaderType
    {
        vertex = 0,
        fragment = 1
    };

    static unsigned int CreateShader(ShaderType shaderType, const char* shaderPath);
};

#endif //MANDELBROTSET_SHADER_H
