#include "App.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include "shader.h"
#include "texture.h"

int App::s_fixedDeltaTime = 10; // milliseconds

App& App::getInstance()
{
    static App app;
    return app;
}

App::App()
    :m_window(nullptr), m_uniform_loc()
{

}

void App::run()
{
    if(!m_window)
        return;

    onCreate();

    std::thread fixedUpdateThread(&App::timing_thread, this);
    render();
    fixedUpdateThread.join();

    glfwTerminate();
    // Temporary line
    exit(0);
}

static void
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    if(type == GL_DEBUG_TYPE_ERROR)
        std::cerr << "GL ERROR: " << message << std::endl;
    else
        std::cout << "GL message: " << message << std::endl;
}

void App::initWindow()
{
    if(!glfwInit())
        throw std::runtime_error("[GLFW]: Could not initialize GLFW!");

    m_window = glfwCreateWindow(m_width, m_height, "Mandelbrot", nullptr, nullptr);
    glfwMakeContextCurrent(m_window);

    glfwSetErrorCallback([](int e, const char *s){std::cerr << "[GLFW ERROR]: " << s << "\n";});
    glfwSetKeyCallback(m_window, App::key_callback);
    glfwSetCursorPosCallback(m_window, App::cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, App::mouse_button_callback);
    glfwSetScrollCallback(m_window, App::scroll_callback);
    glfwSetWindowSizeCallback(m_window, App::window_size_callback);


    if(glewInit() != GLEW_OK)
        throw std::runtime_error("[GLEW]: Could not initialize GLEW!");

#ifdef MANDELBROT_DEBUG
    std::cout << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
#endif
}


// This function is called once before the main loop
void App::onCreate()
{
    // verticies that define the render area (consisting of two triangles)
    float verticies[] = {
            -1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f,  1.0f,

            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f,
    };

    // create vertex buffer and assign verticies
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    // set data in location 0 of vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);

    // compile shaders
    unsigned int VertexShaderID, FragmentShaderID;
    VertexShaderID   = Shader::CreateShader(Shader::vertex, "shaders/vertex.glsl");
    FragmentShaderID = Shader::CreateShader(Shader::fragment, "shaders/fragment.glsl");

    // link shaders
    unsigned int program = glCreateProgram();
    glAttachShader(program, VertexShaderID);
    glAttachShader(program, FragmentShaderID);
    glLinkProgram(program);
    glValidateProgram(program);
    glUseProgram(program);

    // Load 1D textures
    /// Special thanks to Alexandra Toma for some textures ideas
    /// and to Daniel Ghindea for creating the textures
    LoadTexVectorVar(m_textures, 5, "img/pal.png", "img/pal2.png", "img/pal4.png", "img/pal5.png", "img/pal6.png");

    // Retrieve uniform locations
    m_uniform_loc.iter = glGetUniformLocation(program, "iter");
    m_uniform_loc.zoom = glGetUniformLocation(program, "zoom");
    m_uniform_loc.screenOffset = glGetUniformLocation(program, "screenOffset");
    m_uniform_loc.screenSize = glGetUniformLocation(program, "screenSize");
    m_uniform_loc.tex = glGetUniformLocation(program, "tex");
    m_uniform_loc.freq = glGetUniformLocation(program, "freq");
    m_uniform_loc.UVoffset = glGetUniformLocation(program, "UVoffset");

    // Set initial uniform values
    glUniform1i(m_uniform_loc.iter, m_params.iter);
    glUniform1d(m_uniform_loc.zoom, m_params.zoom);
    glUniform1f(m_uniform_loc.freq, m_params.freq);
    glUniform1f(m_uniform_loc.UVoffset, m_params.UVoffset);
    glUniform2d(m_uniform_loc.screenOffset, m_params.OffX, m_params.OffY);
    glUniform2d(m_uniform_loc.screenSize, (double)m_width, (double)m_height);
    glUniform1i(m_uniform_loc.tex, 0);

    // Set active texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, m_textures[0]);
}

// This function is called once per frame
void App::onUpdate()
{
    // set all uniforms
    glUniform1i(m_uniform_loc.iter, m_params.iter);
    glUniform1d(m_uniform_loc.zoom, m_params.zoom);
    glUniform1f(m_uniform_loc.freq, m_params.freq);
    glUniform1f(m_uniform_loc.UVoffset, m_params.UVoffset);
    glUniform2d(m_uniform_loc.screenOffset, m_params.OffX, m_params.OffY);
    glUniform2d(m_uniform_loc.screenSize, (double)m_width, (double)m_height);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// This function is called on a fixed delta time
void App::onFixedUpdate()
{
    // Important: DO NOT CALL GL FUNCTIONS FROM HERE!!!
    if(m_params.isZooming)
    {
        m_params.zoom *= 1.003;
    }

    if(m_params.freqChange)
    {
        if(m_params.freq > m_params.iter) freqDir = -1;
        else if(m_params.freq < 30.0f)
        {
            freqDir = 1;
            m_params.freq = 30.0f;
        }
        if(freqDir > 0) m_params.freq *= freqCoef;
        else m_params.freq /= freqCoef;
    }

    if(m_params.UVChange)
    {
        m_params.UVoffset += UVoffsetCoef;
        if(m_params.UVoffset > 1.0f) m_params.UVoffset = 0.0f;
    }
}

void App::render()
{
    while(!glfwWindowShouldClose(m_window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        onUpdate();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void App::timing_thread()
{
    // No OpenGL calls should be made here.
    typedef std::chrono::time_point<std::chrono::steady_clock> TP;
    while(!glfwWindowShouldClose(m_window))
    {
        TP moment = std::chrono::steady_clock::now() + std::chrono::milliseconds(s_fixedDeltaTime);
        onFixedUpdate();
        std::this_thread::sleep_until(moment);
    }
}



void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_PRESS)
        return;

    App& app = App::getInstance();

    if(key == GLFW_KEY_KP_ADD)
        app.m_params.zoom *= 2;
    else if(key == GLFW_KEY_KP_SUBTRACT)
        app.m_params.zoom /= 2;
    else if(key == GLFW_KEY_E)
        app.m_params.iter += 50;
    else if(key == GLFW_KEY_Q)
        app.m_params.iter -= 50;
    else if(key == GLFW_KEY_Z)
        app.m_params.isZooming = !app.m_params.isZooming;
    else if(key == GLFW_KEY_R)
    {
        app.m_params.zoom = 100;
        app.m_params.OffX = 0;
        app.m_params.OffY = 0;
        app.m_params.iter = 200;
        app.m_params.freq = 30.0f;
        app.m_params.UVoffset = 0.0f;
        app.m_params.isZooming = false;
        app.m_params.freqChange = false;
        app.m_params.UVChange = false;
    }
    else if(key == GLFW_KEY_F)
        app.m_params.freqChange = !app.m_params.freqChange;
    else if(key == GLFW_KEY_G)
        app.m_params.UVChange = !app.m_params.UVChange;
    else if(key == GLFW_KEY_N)
    {
        static unsigned int index = 0;
        index++;
        if(index == app.m_textures.size()) index=0;
        glBindTexture(GL_TEXTURE_1D, app.m_textures[index]);
    }

    //else if(key == GLFW_KEY_A)
    //    showFPS = !showFPS;

    else if(key == GLFW_KEY_P)
    {
        std::cout<<"iter = "<<app.m_params.iter<<"\n";
        std::cout<<"zoom = "<<app.m_params.zoom<<"\n";
        std::cout<<"Offsets: x = "<<app.m_params.OffX<<"; y = "<<app.m_params.OffY<<"\n";
        std::cout<<"Frequency = "<<app.m_params.freq<<"\n";
        std::cout<<"UV Offset = "<<app.m_params.UVoffset<<std::endl;
    }

    /*else if(key == GLFW_KEY_L)
    {
        LoadCustomSettings(app.m_params.iter,
                           app.m_params.zoom,
                           app.m_params.OffX,
                           app.m_params.OffY,
                           app.m_params.freq,
                           app.m_params.UVoffset);
    }*/
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    App& app = App::getInstance();
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        app.m_params.isDragging = true;
        glfwGetCursorPos(window, &app.oldx, &app.oldy); //?
    }
    else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        app.m_params.isDragging = false;
}

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    App& app = App::getInstance();
    if(!app.m_params.isDragging) return;
    app.m_params.OffX += (xpos - app.oldx) / app.m_params.zoom;
    app.m_params.OffY += (app.oldy - ypos) / app.m_params.zoom;
    app.oldx = xpos;
    app.oldy = ypos;
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    App& app = App::getInstance();
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        app.m_params.iter += int(yoffset) * 5;
    else
        app.m_params.zoom += yoffset * 10 * (app.m_params.zoom/100.0);
}

void App::window_size_callback(GLFWwindow* window, int w, int h)
{
    App& app = App::getInstance();
    app.m_width = w;
    app.m_height = h;
    glViewport(0, 0, app.m_width, app.m_height);
}
