#include "render_system_demo.hpp"
#include "simple_directional_lighting_demo.hpp"
#include "simple_pbr_demo.hpp"
#include "textured_cube_demo.hpp"
#include "textured_quad_demo.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <helios/core/entrypoint.hpp>
#include <helios/core/engine_context.hpp>
#include <helios/macros.hpp>

#include <iostream>

void printMessage()
{
    std::cout << "Run a Sample" << std::endl;
    std::cout << "[1]: Textured Quad" << std::endl;
    std::cout << "[2]: Textured Cube" << std::endl;
    std::cout << "[3]: Simple Directional Lighting" << std::endl;
    std::cout << "[4]: Simple PBR Lighting" << std::endl;
    std::cout << "[5]: Render System" << std::endl;
    std::cout << "[0]: Exit" << std::endl << std::endl;
}

class DemoApplication final : public helios::Application
{
public:
    void initialize() override
    {
        try
        {
            i32 sample = -1;
            do
            {
                printMessage();
                std::cin >> sample;
                helios::EngineContext::instance().window().show();
                if (sample == 1)
                {
                    textured_quad::run();
                }
                else if (sample == 2)
                {
                    textured_cube::run();
                }
                else if (sample == 3)
                {
                    simple_directional_lighting::run();
                }
                else if (sample == 4)
                {
                    simple_pbr::run();
                }
                else if (sample == 5)
                {
                    render_system::run();
                }
                helios::EngineContext::instance().window().hide();
                std::cout << std::endl;
            } while (sample != 0);
        }
        catch (...)
        {
            std::cout << "Error in Sample" << std::endl;
        }
    }
};

DEFAULT_ENTRYPOINT(DemoApplication)
