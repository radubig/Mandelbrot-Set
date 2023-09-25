#include <GL/glew.h>
#include "shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

unsigned int Shader::CreateShader(Shader::ShaderType shaderType, const char* shaderPath)
{
    std::string shaderTypeDescriptor;
    unsigned int shaderID;
    switch(shaderType)
    {
        case vertex:
            shaderID = glCreateShader(GL_VERTEX_SHADER);
            shaderTypeDescriptor = "Vertex";
            break;

        case fragment:
            shaderID = glCreateShader(GL_FRAGMENT_SHADER);
            shaderTypeDescriptor = "Fragment";
            break;
    }

    std::string shaderSource;
    std::ifstream in(shaderPath);
    if(in.is_open())
    {
        std::stringstream ss;
        ss << in.rdbuf();
        shaderSource = ss.str();
    }
    else
    {
        std::cerr << "Warning: File " << shaderPath << " could not be opened!\n";
        return 0;
    }
    in.close();

    const char* shaderSourcePointer = shaderSource.c_str();
    glShaderSource(shaderID, 1, &shaderSourcePointer, nullptr);

    int compileStatus;
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        int logsize;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logsize);
        char info[logsize+5];
        glGetShaderInfoLog(shaderID, logsize, nullptr, info);
        std::cout << shaderTypeDescriptor << " Shader Error:\n" << info << "\n";
    }

    return shaderID;
}
