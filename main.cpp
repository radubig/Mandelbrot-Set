/** Controls
Num+ / Num- -> Zoom in/out
Scroll -> Zoom in/out
Z -> Auto Zoom in
Scroll + Hold S -> Change max_iterations
Q/E -> Add/Subtract max_iterations
R -> Reset
N -> Change Loaded Texture
F -> Trigger Freq Animation
G -> Trigger UV Animation
A -> Show FPS
L -> Print Stats
*/

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <iostream>
#include <windows.h>
#include <chrono>
#include <vector>
#include "utils.h"
#include "texture.h"

using namespace std;

//input functions
void key_callback(GLFWwindow* window, int, int, int, int);
void mouse_button_callback(GLFWwindow*, int, int, int);
void cursor_position_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void window_size_callback(GLFWwindow*, int, int);

unsigned int program;
bool isDragging = false;
bool isZooming = false;
bool freqChange = false;
bool UVChange = false;
double oldx, oldy;

bool showFPS = false;

const float UVoffsetCoef = 0.002f;
const float freqCoef = 1.001f;
int freqDir = 1;

int iter = 200;
double zoom = 100;
float freq = 30.0f;
float UVoffset = 0.0f;
double OffX, OffY;
int width=800,
    height=800;

int uniforms[100];
vector<uint32_t> TexVec;

enum UniformType{
    UTiter,
    UTzoom,
    UTscreenOffset,
    UTscreenSize,
    UTtex,
    UTfreq,
    UTUVoffset
};

int main()
{
    if(!glfwInit())
    {
        cout<<"GLFW not init\n";
        return -1;
    }

    GLFWwindow* window;
    window = glfwCreateWindow(width, height, "Mandelbrot", NULL, NULL);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwMakeContextCurrent(window);

    glfwSetErrorCallback([](int e, const char *s) { cerr << "[GLFW ERROR]: " << s << "\n"; });
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

    if(glewInit() != GLEW_OK)
    {
        cout<<"GLEW not ok\n";
        return -1;
    }

    float verticies[] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,

        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };

    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);

    program = CreateShader("vertex.glsl", "fragment.glsl");
    glUseProgram(program);

    LoadTexVectorVar(TexVec, 5, "pal.png", "pal2.png", "pal4.png", "pal5.png", "pal6.png");

    /// Special thanks to Alexandra Toma for some textures ideas
    /// and to Daniel Ghindea for the textures itself

    uniforms[UTiter] = glGetUniformLocation(program, "iter");
    uniforms[UTzoom] = glGetUniformLocation(program, "zoom");
    uniforms[UTscreenOffset] = glGetUniformLocation(program, "screenOffset");
    uniforms[UTscreenSize] = glGetUniformLocation(program, "screenSize");
    uniforms[UTtex] = glGetUniformLocation(program, "tex");
    uniforms[UTfreq] = glGetUniformLocation(program, "freq");
    uniforms[UTUVoffset] = glGetUniformLocation(program, "UVoffset");

    glUniform1i(uniforms[UTiter], iter);
    glUniform1d(uniforms[UTzoom], zoom);
    glUniform1f(uniforms[UTfreq], freq);
    glUniform1f(uniforms[UTUVoffset], UVoffset);
    glUniform2d(uniforms[UTscreenOffset], OffX, OffY);
    glUniform2d(uniforms[UTscreenSize], (double)width, (double)height);
    glUniform1i(uniforms[UTtex], 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, TexVec[0]);

    auto tbegin = chrono::high_resolution_clock::now();
    int framecount = 0;

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if(isZooming)
        {
            zoom *= 1.002;
            glUniform1d(uniforms[UTzoom], zoom);
        }

        if(freqChange)
        {
            if(freq > iter) freqDir = -1;
            else if(freq < 30.0f)
            {
                freqDir = 1;
                freq = 30.0f;
            }
            if(freqDir > 0) freq *= freqCoef;
            else freq /= freqCoef;
            glUniform1f(uniforms[UTfreq], freq);
        }

        if(UVChange)
        {
            UVoffset += UVoffsetCoef;
            if(UVoffset > 1.0f) UVoffset = 0.0f;
            glUniform1f(uniforms[UTUVoffset], UVoffset);
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
        framecount++;

        auto tend = chrono::high_resolution_clock::now();
        chrono::duration<double> diff = tend - tbegin;
        if(diff.count() >= 1)
        {
            if(showFPS) cout<<"[FPS]: "<<framecount<<"\n";
            framecount = 0;
            tbegin = chrono::high_resolution_clock::now();
        }
    }

    glfwTerminate();
    return 0;
}



//Callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
        zoom *= 2;
    else if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
        zoom /= 2;

    if(key == GLFW_KEY_E && action == GLFW_PRESS)
        iter += 50;
    else if(key == GLFW_KEY_Q && action == GLFW_PRESS)
        iter -= 50;

    if(key == GLFW_KEY_Z && action == GLFW_PRESS)
        isZooming = !isZooming;

    if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        zoom = 100;
        OffX = 0;
        OffY = 0;
        iter = 200;
        freq = 30.0f;
        UVoffset = 0.0f;
        isZooming = false;
        freqChange = false;
        UVChange = false;
        glUniform1f(uniforms[UTfreq], freq);
        glUniform1f(uniforms[UTUVoffset], UVoffset);
    }

    if(key == GLFW_KEY_F && action == GLFW_PRESS)
        freqChange = !freqChange;

    if(key == GLFW_KEY_G && action == GLFW_PRESS)
        UVChange = !UVChange;

    if(key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        static unsigned int index = 0;
        index++;
        if(index == TexVec.size()) index=0;
        glBindTexture(GL_TEXTURE_1D, TexVec[index]);
    }

    if(key == GLFW_KEY_A && action == GLFW_PRESS)
        showFPS = !showFPS;

    if(key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        cout<<"iter = "<<iter<<"\n";
        cout<<"zoom = "<<zoom<<"\n";
        cout<<"Offsets: x = "<<OffX<<"; y = "<<OffY<<"\n";
        cout<<"Frequency = "<<freq<<"\n";
        cout<<"UV Offset = "<<UVoffset<<endl;
    }

    glUniform1i(uniforms[UTiter], iter);
    glUniform1d(uniforms[UTzoom], zoom);
    glUniform2d(uniforms[UTscreenOffset], OffX, OffY);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        isDragging = true;
        glfwGetCursorPos(window, &oldx, &oldy);
    }
    else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        isDragging = false;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(!isDragging) return;
    OffX += (xpos - oldx) / zoom;
    OffY += (oldy - ypos) / zoom;
    oldx = xpos;
    oldy = ypos;
    glUniform2d(uniforms[UTscreenOffset], OffX, OffY);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        iter += int(yoffset) * 5;
    else
        zoom += yoffset * 10 * (zoom/100.0);

    glUniform1i(uniforms[UTiter], iter);
    glUniform1d(uniforms[UTzoom], zoom);
}

void window_size_callback(GLFWwindow* window, int w, int h)
{
    width = w;
    height = h;
    glUniform2d(uniforms[UTscreenSize], (double)width, (double)height);
    glViewport(0, 0, width, height);
}
