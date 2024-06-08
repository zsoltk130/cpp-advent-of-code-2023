#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <map>
#include <algorithm>

struct Range 
{
    std::pair<long long int, long long int> sourceRange;
    std::pair<long long int, long long int> destinationRange;
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

// Function to find the index of the line containing a specific string
int findIndexOfLineContainingString(const std::vector<std::string>& data, const std::string& target)
{
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i] == target)
        {
            return i;
        }
    }
    return -1;
}

// Function to extract ranges from a section of the almanac
std::vector<Range> extractRangesFromSection(const std::vector<std::string>& almanac, const std::string& sectionHeader)
{
    int indexOfSectionHeader = findIndexOfLineContainingString(almanac, sectionHeader);
    std::vector<Range> ranges;
    std::regex numberPattern("\\d+");

    if (indexOfSectionHeader == -1)
    {
        std::cerr << "Error: Section header not found: " << sectionHeader << std::endl;
        return ranges;
    }

    int lineNumber = 1;
    while (true)
    {
        if (indexOfSectionHeader + lineNumber >= almanac.size())
        {
            break;
        }

        std::string line = almanac[indexOfSectionHeader + lineNumber];
        auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
        auto iterator_end = std::sregex_iterator();

        long long int destinationRangeStart = 0;
        long long int sourceRangeStart = 0;
        long long int rangeLength = 0;
        int column = 0;

        Range range;
        
        for (auto it = iterator_begin; it != iterator_end; ++it)
        {
            std::smatch match = *it;
            std::string currentNumber = match.str();

            try
            {
                if (column == 0)
                {
                    destinationRangeStart = std::stoll(currentNumber);
                }
                else if (column == 1)
                {
                    sourceRangeStart = std::stoll(currentNumber);
                }
                else if (column == 2)
                {
                    rangeLength = std::stoll(currentNumber);
                    range.sourceRange = { sourceRangeStart, sourceRangeStart + rangeLength };
                    range.destinationRange = { destinationRangeStart, destinationRangeStart + rangeLength };
                    ranges.push_back(range);
                    column = -1;
                }
            }
            catch (const std::out_of_range& e)
            {
                std::cerr << "Error: Number out of range in section " << sectionHeader << std::endl;
                return ranges;
            }
            column++;
        }

        if (line.empty() || indexOfSectionHeader + lineNumber == almanac.size() - 1)
        {
            break;
        }

        lineNumber++;
    }

    return ranges;
}

// Generalized function to map values from one range to another
std::vector<std::pair<long long int, long long int>> mapValues(const std::vector<std::string>& almanac, 
                                                                const std::vector<std::pair<long long int, long long int>>& previousMap, 
                                                                const std::string& sectionHeader) 
{
    std::vector<std::pair<long long int, long long int>> resultMap;
    std::vector<Range> ranges = extractRangesFromSection(almanac, sectionHeader);

    // Get values from second element of pair from previous map and store in sourceValue
    for (const auto& pair : previousMap)
    {
        long long int sourceValue = pair.second;
        bool found = false;

        // Map sourceValue to destValue by checking whether it is part of the ranges from the almanac
        for (const auto& range : ranges)
        {
            long long int sourceLower = range.sourceRange.first;
            long long int sourceHigher = range.sourceRange.second;
            long long int destLower = range.destinationRange.first;
            long long int destHigher = range.destinationRange.second;

            if (sourceLower <= sourceValue && sourceValue < sourceHigher)
            {
                long long int offset = destLower - sourceLower;
                long long int destValue = sourceValue + offset;

                if (destLower <= destValue && destValue < destHigher)
                {
                    resultMap.emplace_back(sourceValue, destValue);
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            resultMap.emplace_back(sourceValue, sourceValue);
        }
    }

    return resultMap;
}

// Function to map seeds to soil
std::vector<std::pair<long long int, long long int>> mapSeedToSoil(const std::vector<std::string>& almanac) 
{
    std::vector<std::pair<long long int, long long int>> seedToSoilMap;

    // Get seeds from the first line of almanac
    std::vector<long long int> seeds;
    std::string line = almanac.front();
    std::regex numberPattern("\\d+");

    auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
    auto iterator_end = std::sregex_iterator();

    for (auto it = iterator_begin; it != iterator_end; ++it)
    {
        std::smatch match = *it;
        std::string currentNumber = match.str();
        
        try
        {
            seeds.push_back(std::stoll(currentNumber));
        }
        catch (const std::out_of_range& e)
        {
            std::cerr << "Error: Seed number out of range" << std::endl;
        }
    }

    // Get ranges from under "seed-to-soil map:"
    std::vector<Range> ranges = extractRangesFromSection(almanac, "seed-to-soil map:");

    for (long long int seed : seeds)
    {
        bool found = false;
        
        for (const Range& range : ranges)
        {
            long long int seedLower = range.sourceRange.first;
            long long int seedHigher = range.sourceRange.second;
            long long int soilLower = range.destinationRange.first;
            long long int soilHigher = range.destinationRange.second;

            long long int seedSoilOffset = soilLower - seedLower;
            
            if (seedLower <= seed && seed < seedHigher) 
            {
                long long int soil = seed + seedSoilOffset;
                
                if (soilLower <= soil && soil < soilHigher) 
                {
                    seedToSoilMap.emplace_back(seed, soil);
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            seedToSoilMap.emplace_back(seed, seed);
        }
    }

    return seedToSoilMap;
}

void runPartOne(const std::string& filename)
{
    std::vector<std::string> almanac = readLinesFromFile(filename);
    
    if (almanac.empty())
    {
        std::cerr << "Error: Almanac is empty." << std::endl;
        return;
    }

    auto seedToSoilMaps = mapSeedToSoil(almanac);
    auto soilToFertilizerMaps = mapValues(almanac, seedToSoilMaps, "soil-to-fertilizer map:");
    auto fertilizerToWaterMaps = mapValues(almanac, soilToFertilizerMaps, "fertilizer-to-water map:");
    auto waterToLightMaps = mapValues(almanac, fertilizerToWaterMaps, "water-to-light map:");
    auto lightToTemperatureMaps = mapValues(almanac, waterToLightMaps, "light-to-temperature map:");
    auto temperatureToHumidityMaps = mapValues(almanac, lightToTemperatureMaps, "temperature-to-humidity map:");
    auto humidityToLocationMaps = mapValues(almanac, temperatureToHumidityMaps, "humidity-to-location map:");

    if (!humidityToLocationMaps.empty()) 
    {
        // Sort pairs by their second element in ascending order
        std::sort(humidityToLocationMaps.begin(), humidityToLocationMaps.end(), [](const std::pair<long long int, long long int>& a, const std::pair<long long int, long long int>& b) {
            return a.second < b.second;
            });

        std::cout << "Lowest location: " << humidityToLocationMaps.front().second << std::endl;
    }
    else {
        std::cerr << "Error: No humidity to location mappings found." << std::endl;
    }
}

int main() {
    std::string filename = "day5.txt";
    std::string testfile = "test.txt";
    
    runPartOne(filename);

    return 0;
}