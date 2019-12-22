#include "Game/Application.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::cout << "RUN" << std::endl;
    try
    {
        Application app;
        if (argc == 2 && argv[1] == std::string("test"))
        {
            app.runTest();
        }
        else
        {
            app.run();
        }

    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}