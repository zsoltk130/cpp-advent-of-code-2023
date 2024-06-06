#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

std::vector<std::string> readLinesFromFile(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();
    return lines;
}

int main()
{
    std::string testfile = "testfile.txt";
    std::string input = "day5.txt";
    
    std::vector<std::string> almanac = readLinesFromFile(testfile);
    
    return 0;
}