#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <regex>

// Holds relevant data
struct GameData {
    int gameNumber = 0;
    int redCount = 0;
    int greenCount = 0;
    int blueCount = 0;
};

// Read txt line by line into vector of strings
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

// Using regex to pattern match all numbers in a string and add to vector of strings
GameData getGameDataFromLine(const std::string& str) {
    GameData data;

    // Regex pattern to find game number i.e.: "Game 50"
    std::regex gameNumberPattern("(Game (\\d+))");

    auto gameNumberPattern_begin = std::sregex_iterator(str.begin(), str.end(), gameNumberPattern);
    auto gameNumberPattern_end = std::sregex_iterator();

    // Regex pattern to find colour counts i.e.: "6 red"
    std::regex diceColourPattern("((\\d+) (red|green|blue))");

    auto diceColourPattern_begin = std::sregex_iterator(str.begin(), str.end(), diceColourPattern);
    auto diceColourPattern_end = std::sregex_iterator();

    // Iterate through found regex matches and compile game data using GameData struct
    for (std::sregex_iterator it = gameNumberPattern_begin; it != gameNumberPattern_end; ++it) {
        std::smatch match = *it;
        data.gameNumber = std::stoi(match[2].str());

        for (std::sregex_iterator it = diceColourPattern_begin; it != diceColourPattern_end; ++it) {
            std::smatch match = *it;
            std::string colour = match[3].str();

            if (colour == "red")
            {
                int reds = std::stoi(match[2].str());
                // If larger number is found, replace previous number
                if (data.redCount < reds)
                {
                    data.redCount = reds;
                }
            }
            else if (colour == "green")
            {
                int greens = std::stoi(match[2].str());
                if (data.greenCount < greens)
                {
                    data.greenCount = greens;
                }
            }
            else if (colour == "blue")
            {
                int blues = std::stoi(match[2].str());
                if (data.blueCount < blues)
                {
                    data.blueCount = blues;
                }
            }
        }
    }

    return data;
}

int runPartOne(const std::string& filename)
{
    std::vector<std::string> lines = readLinesFromFile(filename);
    int total = 0;
    for (auto line : lines)
    {
        GameData gameData = getGameDataFromLine(line);
        // If red, green and blue counts are all less or equal to 12, 13 and 14 respectively
        // Add game number to total
        if (gameData.redCount <= 12 && gameData.greenCount <= 13 && gameData.blueCount <= 14)
        {
            total += gameData.gameNumber;
        }
    }
    return total;
}

int runPartTwo(const std::string& filename)
{
    std::vector<std::string> lines = readLinesFromFile(filename);
    int total = 0;
    for (auto line : lines)
    {
        GameData gameData = getGameDataFromLine(line);
        int power = gameData.redCount * gameData.greenCount * gameData.blueCount;
        total += power;
    }
    return total;
}

int main()
{
    std::string filename = "day2.txt";
    int partOne = runPartOne(filename);
    int partTwo = runPartTwo(filename);
    std::cout << "Part 1 Total: " << partOne << std::endl << "Part 2 Total: " << partTwo << std::endl;
}
