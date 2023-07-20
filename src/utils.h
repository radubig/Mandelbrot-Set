#pragma once
#include <GL/glew.h>
#include <fstream>
#include <sstream>

unsigned int CreateShader(const char* VertexShaderPath, const char* FragmentShaderPath)
{
    unsigned int VertexShaderID, FragmentShaderID;
    VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string VertexSource, FragmentSource;

    std::ifstream in;
    in.open(VertexShaderPath);
    if(in.is_open())
    {
        std::stringstream ss;
        ss << in.rdbuf();
        VertexSource = ss.str();
    }
    else
    {
        std::cerr << "File " << VertexShaderPath << " could not be opened!\n";
        return 0;
    }
    in.close();

    in.open(FragmentShaderPath);
    if(in.is_open())
    {
        std::stringstream ss;
        ss << in.rdbuf();
        FragmentSource = ss.str();
    }
    else
    {
        std::cerr << "File " << FragmentShaderPath << " could not be opened!\n";
        return 0;
    }
    in.close();

    const char* VertexPointer = VertexSource.c_str();
    const char* FragmentPointer = FragmentSource.c_str();

    glShaderSource(VertexShaderID, 1, &VertexPointer, nullptr);
    glShaderSource(FragmentShaderID, 1, &FragmentPointer, nullptr);
    int compileStatus;

    glCompileShader(VertexShaderID);
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        int logsize;
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &logsize);
        char info[logsize+5];
        glGetShaderInfoLog(VertexShaderID, logsize, NULL, info);
        std::cout<<"[Vertex Shader Error]:\n"<<info<<"\n";
    }

    glCompileShader(FragmentShaderID);
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        int logsize;
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &logsize);
        char info[logsize+5];
        glGetShaderInfoLog(FragmentShaderID, logsize, NULL, info);
        std::cout<<"[Fragment Shader Error]:\n"<<info<<"\n";
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, VertexShaderID);
    glAttachShader(program, FragmentShaderID);
    glLinkProgram(program);
    glValidateProgram(program);
    return program;
}

void LoadCustomSettings(int& iterations,
                        double& zoom,
                        double& OffX, double& OffY,
                        float& freq,
                        float& UVoffset)
{
    std::ifstream in("settings.txt");
    if(!in)
    {
        std::cerr << "[Error] Could not open \"settings.txt\"!" << std::endl;
        return;
    }

    in >> iterations;
    in >> zoom;
    in >> OffX >> OffY;
    in >> freq;
    in >> UVoffset;
}
