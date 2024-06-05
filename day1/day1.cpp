#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

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
std::vector<std::string> getNumbersFromString(const std::string& str) {
    std::vector<std::string> numbers;
    
    // Initially thought spelled out words were included
    // (?=(one|two|three|four|five|six|seven|eight|nine|\\d))
    std::regex pattern("\\d"); // Regex pattern to filter numbers 1-9 as well as in string form "one", "two", "three"...

    // Use sregex_iterator to find all matches
    auto str_begin = std::sregex_iterator(str.begin(), str.end(), pattern);
    auto str_end = std::sregex_iterator();

    // Iterate over all matches and collect them
    for (std::sregex_iterator i = str_begin; i != str_end; ++i) {
        std::smatch match = *i;
        numbers.push_back(match.str(0)); // Extract the first capturing group
    }

    return numbers;
}

// Convert vector of strings to vector of ints
std::vector<int> convertStringsToIntegers(const std::vector<std::string>& strVector) {
    std::vector<int> intVector;
    std::unordered_map<std::string, int> numberMap = {
        {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4},
        {"five", 5}, {"six", 6}, {"seven", 7}, {"eight", 8},
        {"nine", 9}, {"ten", 10}
    };

    for (const std::string& str : strVector) {
        if (!str.empty() && std::isdigit(str[0])) {
            // Convert numeric string to integer
            intVector.push_back(std::stoi(str));
        }
        else {
            // Convert spelled-out string to integer using the map
            auto it = numberMap.find(str);
            if (it != numberMap.end()) {
                intVector.push_back(it->second);
            }
            else {
                std::cerr << "Error: Invalid number string \"" << str << "\"." << std::endl;
            }
        }
    }

    return intVector;
}

// Get first and last element from vector to compose final number
int composeNumber(const std::vector<int>& intVector) {
    int result = intVector.front() * 10; // Multiply by 10 to make int the first number in a double digit number
        result += intVector.back();

    return result;
}

// Add and return total from all string lines found in text file
int getTotal(const std::vector<std::string>& lines) {
    int total = 0;

    for (const std::string& line : lines)
    {
        std::vector<std::string> strVect = getNumbersFromString(line);
        std::vector<int> intVect = convertStringsToIntegers(strVect);
        int result = composeNumber(intVect);
        total += result;
    }

    return total;
}

int main() {
    std::string filename = "day1.txt";
    std::vector<std::string> lines = readLinesFromFile(filename);
    int total = getTotal(lines);

    std::cout << "Total: " << total << std::endl;
    return 0;
}