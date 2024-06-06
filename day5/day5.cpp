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

std::vector<int> extractSeedsFromAlmanac(std::vector<std::string>& almanac)
{
    std::vector<int> seeds;
    std::string line = almanac[0];
    std::regex numberPattern("\\d+");

    auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
    auto iterator_end = std::sregex_iterator();

    for (std::sregex_iterator it = iterator_begin; it != iterator_end; ++it) {
        std::smatch match = *it;
        std::string currentNumber = match.str();
        seeds.push_back(std::stoi(currentNumber));
    }

    return seeds;
}

int main()
{
    std::string testfile = "test.txt";
    std::string filename = "day5.txt";
    
    std::vector<std::string> almanac = readLinesFromFile(testfile);
    std::vector<int> seeds = extractSeedsFromAlmanac(almanac);
    
    return 0;
}