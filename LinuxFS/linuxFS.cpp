//build command: g++ -std=c++20 -o linuxFS linuxFS.cpp

#include <iostream>
#include <filesystem>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " dir_path partial_name" << std::endl;
        return 1;
    }

    std::filesystem::path dir_path = argv[1];
    std::string partial_name = argv[2];

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path))
    {
        if (entry.is_regular_file() && entry.path().filename().string().find(partial_name) != std::string::npos)
        {
            std::cout << entry.path() << std::endl;
        }
    }

    return 0;
}