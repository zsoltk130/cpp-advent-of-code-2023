#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

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

std::vector<int> extractNumbersFromString(const std::string& string)
{
    std::vector<int> vec;
    std::regex numberPattern("\\d+");

    std::string line = string;
    auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
    auto iterator_end = std::sregex_iterator();

    for (auto it = iterator_begin; it != iterator_end; ++it)
    {
        std::smatch match = *it;
        std::string currentNumber = match.str();

        vec.push_back(std::stoi(currentNumber));
    }

    return vec;
}

Race populateStruct(const std::vector<std::string>& lines)
{
    Race race;
    race.times = extractNumbersFromString(lines[0]);
    race.distances = extractNumbersFromString(lines[1]);

    return race;
}

int main()
{
    std::string testfile = "test.txt";
    std::vector<std::string> lines = readLinesFromFile(testfile);
    Race race = populateStruct(lines);

    return 0;
}