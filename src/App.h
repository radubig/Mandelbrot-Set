#ifndef MANDELBROTSET_APP_H
#define MANDELBROTSET_APP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class App
{
public:
    static App& getInstance();
    ~App() = default;
    void initWindow();
    void run();

    //static callback functions
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void window_size_callback(GLFWwindow* window, int w, int h);

private:
    App();
    void onCreate();
    void onUpdate();
    void onFixedUpdate();
    void render();
    void timing_thread();
    void resetDefaultValues();

    static int s_fixedDeltaTime;

    GLFWwindow *m_window;
    std::vector<uint32_t> m_textures;

    struct {
        int iter, zoom, freq, tex, screenOffset, screenSize, UVoffset;
    }m_uniform_loc;

    int m_width = 800;
    int m_height = 800;
    int m_active_texture = 0;

    struct {
        int iter = 200;
        double zoom = 100;
        float freq = 30;
        float UVoffset = 0.0;
        double OffX = 0, OffY = 0;

        bool isZooming = false;
        bool freqChange = false;
        bool UVChange = false;
        bool isDragging = false;
    } m_params;

    //temporary
    double oldx = 0, oldy = 0;
    const float UVoffsetCoef = 0.002f;
    const float freqCoef = 1.001f;
    const float zoomCoef = 1.003f;
    int freqDir = 1;
};


#endif //MANDELBROTSET_APP_H
