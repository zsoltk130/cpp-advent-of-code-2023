#include <iostream>
#include <string>
#include <vector>
#include <fstream>

struct Race
{
    std::vector<int> times;
    std::vector<int> distances;
};

// Function to read lines from a file
std::vector<std::string> readLinesFromFile(const std::string& filename)
{
    std::vector<std::string> lines;
    std::ifstream file(filename);

    if (!file)
    {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    file.close();
    return lines;
}

int main()
{
    std::string testfile = "test.txt";
    std::vector<std::string> lines = readLinesFromFile(testfile);

    return 0;
}