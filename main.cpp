#include <iostream>
#include <fstream>

int main() {

    std::string line;

    std::ifstream my_file("test.txt");

    if (my_file.is_open())
    {
        while (std::getline(my_file, line))
        {
            std::cout << line << '\n';
        }
        my_file.close();
    }
    else
    {
        std::cerr << "oiwejifojewf\n";
    }


    return 0;
}
