#include "App.h"
#include <iostream>
#include <stdexcept>
#include "shader.h"
#include "texture.h"

App& App::getInstance()
{
    static App app;
    return app;
}

App::App()
    :m_window(nullptr)
{

}

void App::run()
{
    if(!m_window)
        return;

    onCreate();

    while(!glfwWindowShouldClose(m_window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        onUpdate();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

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
        std::cerr << "GL ERROR (severity " << severity << "): " << message << std::endl;
    else
        std::cout << "GL message (severity " << severity << "): " << message << std::endl;
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
    m_uniforms["iter"] = glGetUniformLocation(program, "iter");
    m_uniforms["zoom"] = glGetUniformLocation(program, "zoom");
    m_uniforms["screenOffset"] = glGetUniformLocation(program, "screenOffset");
    m_uniforms["screenSize"] = glGetUniformLocation(program, "screenSize");
    m_uniforms["tex"] = glGetUniformLocation(program, "tex");
    m_uniforms["freq"] = glGetUniformLocation(program, "freq");
    m_uniforms["UVoffset"] = glGetUniformLocation(program, "UVoffset");

    // Set initial uniform values
    glUniform1i(m_uniforms["iter"], m_params.iter);
    glUniform1d(m_uniforms["zoom"], m_params.zoom);
    glUniform1f(m_uniforms["freq"], m_params.freq);
    glUniform1f(m_uniforms["UVoffset"], m_params.UVoffset);
    glUniform2d(m_uniforms["screenOffset"], m_params.OffX, m_params.OffY);
    glUniform2d(m_uniforms["screenSize"], (double)m_width, (double)m_height);
    glUniform1i(m_uniforms["tex"], 0);

    // Set active texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, m_textures[0]);
}


// This function is called once per frame
void App::onUpdate()
{
    if(m_params.isZooming)
    {
        m_params.zoom *= 1.002;
        glUniform1d(m_uniforms["zoom"], m_params.zoom);
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
        glUniform1f(m_uniforms["freq"], m_params.freq);
    }

    if(m_params.UVChange)
    {
        m_params.UVoffset += UVoffsetCoef;
        if(m_params.UVoffset > 1.0f) m_params.UVoffset = 0.0f;
        glUniform1f(m_uniforms["UVoffset"], m_params.UVoffset);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
}


// This function is called on a fixed clock
void App::onFixedUpdate()
{

}






void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    App& app = App::getInstance();

    if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
        app.m_params.zoom *= 2;
    else if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
        app.m_params.zoom /= 2;

    if(key == GLFW_KEY_E && action == GLFW_PRESS)
        app.m_params.iter += 50;
    else if(key == GLFW_KEY_Q && action == GLFW_PRESS)
        app.m_params.iter -= 50;

    if(key == GLFW_KEY_Z && action == GLFW_PRESS)
        app.m_params.isZooming = !app.m_params.isZooming;

    if(key == GLFW_KEY_R && action == GLFW_PRESS)
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
        glUniform1f(app.m_uniforms["freq"], app.m_params.freq);
        glUniform1f(app.m_uniforms["UVoffset"], app.m_params.UVoffset);
        glUniform1i(app.m_uniforms["iter"], app.m_params.iter);
        glUniform1d(app.m_uniforms["zoom"], app.m_params.zoom);
        glUniform2d(app.m_uniforms["screenOffset"], app.m_params.OffX, app.m_params.OffY);
    }

    if(key == GLFW_KEY_F && action == GLFW_PRESS)
        app.m_params.freqChange = !app.m_params.freqChange;

    if(key == GLFW_KEY_G && action == GLFW_PRESS)
        app.m_params.UVChange = !app.m_params.UVChange;

    if(key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        static unsigned int index = 0;
        index++;
        if(index == app.m_textures.size()) index=0;
        glBindTexture(GL_TEXTURE_1D, app.m_textures[index]);
    }

    //if(key == GLFW_KEY_A && action == GLFW_PRESS)
    //    showFPS = !showFPS;

    if(key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        std::cout<<"iter = "<<app.m_params.iter<<"\n";
        std::cout<<"zoom = "<<app.m_params.zoom<<"\n";
        std::cout<<"Offsets: x = "<<app.m_params.OffX<<"; y = "<<app.m_params.OffY<<"\n";
        std::cout<<"Frequency = "<<app.m_params.freq<<"\n";
        std::cout<<"UV Offset = "<<app.m_params.UVoffset<<std::endl;
    }

    /*if(key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        LoadCustomSettings(app.m_params.iter,
                           app.m_params.zoom,
                           app.m_params.OffX,
                           app.m_params.OffY,
                           app.m_params.freq,
                           app.m_params.UVoffset);
        glUniform1i(uniforms[UTiter], iter);
        glUniform1d(uniforms[UTzoom], zoom);
        glUniform2d(uniforms[UTscreenOffset], OffX, OffY);
        glUniform1f(uniforms[UTfreq], freq);
        glUniform1f(uniforms[UTUVoffset], UVoffset);
    }*/

    glUniform1i(app.m_uniforms["iter"], app.m_params.iter);
    glUniform1d(app.m_uniforms["zoom"], app.m_params.zoom);
    //glUniform2d(uniforms[UTscreenOffset], OffX, OffY);
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
    glUniform2d(app.m_uniforms["screenOffset"], app.m_params.OffX, app.m_params.OffY);
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    App& app = App::getInstance();
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        app.m_params.iter += int(yoffset) * 5;
    else
        app.m_params.zoom += yoffset * 10 * (app.m_params.zoom/100.0);

    glUniform1i(app.m_uniforms["iter"], app.m_params.iter);
    glUniform1d(app.m_uniforms["zoom"], app.m_params.zoom);
}

void App::window_size_callback(GLFWwindow* window, int w, int h)
{
    App& app = App::getInstance();
    app.m_width = w;
    app.m_height = h;
    glUniform2d(app.m_uniforms["screenSize"], (double)app.m_width, (double)app.m_height);
    glViewport(0, 0, app.m_width, app.m_height);
}
