#include "textured_quad_demo.hpp"

#include <helios/macros.hpp>

#include <iostream>

void printMessage()
{
    std::cout << "Run a Sample" << std::endl;
    std::cout << "[1]: Textured Quad" << std::endl;
    std::cout << "[0]: Exit" << std::endl << std::endl;
}

int main() {
    
    try
    {
        i32 sample = -1;
        do
        {
            printMessage();
            std::cin >> sample;
            if (sample == 1)
            {
                textured_quad::run();
            }
            std::cout << std::endl;
        } while (sample != 0);
    }
    catch (...)
    {
        std::cout << "Error in Sample" << std::endl;
    }
    return 0;
}
