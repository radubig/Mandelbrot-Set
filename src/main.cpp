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
(TODO) Show FPS
(TODO) Load custom settings
*/

#include "App.h"
#include <iostream>

using namespace std;

int main()
{
    try
    {
        auto& app = App::getInstance();
        app.initWindow();
        app.run();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Error " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
