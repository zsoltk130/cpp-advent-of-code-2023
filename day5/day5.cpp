#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <map>
#include <algorithm>

struct RangeMap
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
std::vector<RangeMap> extractRangesFromSection(const std::vector<std::string>& almanac, const std::string& sectionHeader)
{
    int indexOfSectionHeader = findIndexOfLineContainingString(almanac, sectionHeader);
    std::vector<RangeMap> ranges;
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

        RangeMap rm;
        
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
                    rm.sourceRange = { sourceRangeStart, sourceRangeStart + rangeLength };
                    rm.destinationRange = { destinationRangeStart, destinationRangeStart + rangeLength };
                    ranges.push_back(rm);
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
    std::vector<RangeMap> ranges = extractRangesFromSection(almanac, sectionHeader);

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

std::vector<std::pair<long long int, long long int>> mapRanges(const std::vector<std::string>& almanac,
    const std::vector<std::pair<long long int, long long int>>& sourceRanges,
    const std::string& sectionHeader)
{
    std::vector<std::pair<long long int, long long int>> destinationRanges;
    std::vector<RangeMap> rangeMaps = extractRangesFromSection(almanac, sectionHeader);

    for (const std::pair<long long int, long long int>& range : sourceRanges)
    {
        long long int sourceRangeLower = range.first;
        long long int sourceRangeHigher = range.second;

        bool isMapped = false;

        for (const RangeMap& rangeMap : rangeMaps)
        {
            long long int rangeMapSourceRangeLower = rangeMap.sourceRange.first;
            long long int rangeMapSourceRangeHigher = rangeMap.sourceRange.second;

            long long int offset = rangeMap.destinationRange.first - rangeMap.sourceRange.first;
            
            // When both sourceRangeLower and sourceRange higher fit within the rangemap
            if (rangeMapSourceRangeLower <= sourceRangeLower && sourceRangeHigher < rangeMapSourceRangeHigher)
            {
                long long int destinationRangeLower = sourceRangeLower + offset;
                long long int destinationRangeHigher = sourceRangeHigher + offset;
                
                std::pair<long long int, long long int> destinationRange;

                destinationRange.first = destinationRangeLower;
                destinationRange.second = destinationRangeHigher;
                
                destinationRanges.push_back(destinationRange);

                isMapped = true;
            }
            // When sourceRangeHigher overshoots the rangemap but sourceRangeLower is still within the rangemap
            else if (((rangeMapSourceRangeLower <= sourceRangeLower) && (sourceRangeLower < rangeMapSourceRangeHigher)) && sourceRangeHigher >= rangeMapSourceRangeHigher)
            {
                std::pair<long long int, long long int> mappedRange;
                std::pair<long long int, long long int> remainder;

                long long int destinationRangeLower = sourceRangeLower + offset;
                long long int destinationRangeHigher = (rangeMapSourceRangeHigher - 1) + offset;

                mappedRange.first = destinationRangeLower;
                mappedRange.second = destinationRangeHigher;

                destinationRanges.push_back(mappedRange);

                destinationRangeLower = rangeMapSourceRangeHigher;
                destinationRangeHigher = sourceRangeHigher;

                remainder.first = destinationRangeLower;
                remainder.second = destinationRangeHigher;


                destinationRanges.push_back(remainder);

                isMapped = true;
            }
            // When sourceRangeLower undershoots the rangemap but sourceRangeHigher is still within the rangemap
            else if (rangeMapSourceRangeLower >= sourceRangeLower && ((rangeMapSourceRangeLower < sourceRangeHigher) && (sourceRangeHigher < rangeMapSourceRangeHigher)))
            {
                std::pair<long long int, long long int> mappedRange;
                std::pair<long long int, long long int> remainder;

                long long int destinationRangeLower = rangeMapSourceRangeLower + offset;
                long long int destinationRangeHigher = sourceRangeHigher + offset;

                mappedRange.first = destinationRangeLower;
                mappedRange.second = destinationRangeHigher;

                destinationRanges.push_back(mappedRange);

                destinationRangeLower = sourceRangeLower;
                destinationRangeHigher = rangeMapSourceRangeLower - 1;

                remainder.first = destinationRangeLower;
                remainder.second = destinationRangeHigher;

                destinationRanges.push_back(remainder);

                isMapped = true;
            }
            // When sourceRangeLower is lower than rangeMapSourceRangeLower and sourceRangeHigher is higher than rangeMapSourceRangeHigher
            else if (rangeMapSourceRangeLower >= sourceRangeLower && sourceRangeHigher > rangeMapSourceRangeHigher)
            {
                std::pair<long long int, long long int> mappedRange;
                std::pair<long long int, long long int> remainder;

                long long int destinationRangeLower = rangeMapSourceRangeLower + offset;
                long long int destinationRangeHigher = (rangeMapSourceRangeHigher - 1) + offset;

                mappedRange.first = destinationRangeLower;
                mappedRange.second = destinationRangeHigher;

                destinationRanges.push_back(mappedRange);

                destinationRangeLower = sourceRangeLower;
                destinationRangeHigher = rangeMapSourceRangeLower - 1;

                remainder.first = destinationRangeLower;
                remainder.second = destinationRangeHigher;

                destinationRanges.push_back(remainder);

                destinationRangeLower = rangeMapSourceRangeHigher;
                destinationRangeHigher = sourceRangeHigher;

                remainder.first = destinationRangeLower;
                remainder.second = destinationRangeHigher;

                destinationRanges.push_back(remainder);

                isMapped = true;
            }
        }
        if (!isMapped)
        {
            std::pair<long long int, long long int> mappedRange;

            long long int destinationRangeLower = sourceRangeLower;
            long long int destinationRangeHigher = sourceRangeHigher;

            mappedRange.first = destinationRangeLower;
            mappedRange.second = destinationRangeHigher;

            destinationRanges.push_back(mappedRange);
        }
    }
    
    return destinationRanges;
}

std::vector<std::pair<long long int, long long int>> getSeedRanges(const std::vector<std::string>& almanac)
{
    // Get seeds from the first line of almanac
    std::vector<std::pair<long long int, long long int>> seedRanges;
    std::pair<long long int, long long int> seedRange;

    std::string line = almanac.front();
    std::regex numberPattern("\\d+");

    auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
    auto iterator_end = std::sregex_iterator();

    bool isFirst = true;

    for (auto it = iterator_begin; it != iterator_end; ++it)
    {
        std::smatch match = *it;
        std::string currentNumber = match.str();

        try
        {
            if (isFirst)
            {
                seedRange.first = std::stoll(currentNumber);
                isFirst = false;
            }
            else
            {
                seedRange.second = seedRange.first + std::stoll(currentNumber);
                isFirst = true;
                seedRanges.push_back(seedRange);
            }
        }
        catch (const std::out_of_range& e)
        {
            std::cerr << "Error: Seed number out of range" << std::endl;
        }
    }

    return seedRanges;
}

// Function to map seed to soil
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
    std::vector<RangeMap> ranges = extractRangesFromSection(almanac, "seed-to-soil map:");

    for (long long int seed : seeds)
    {
        bool found = false;
        
        for (const RangeMap& rangeMap : ranges)
        {
            long long int seedLower = rangeMap.sourceRange.first;
            long long int seedHigher = rangeMap.sourceRange.second;
            long long int soilLower = rangeMap.destinationRange.first;
            long long int soilHigher = rangeMap.destinationRange.second;

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

void runPartTwo(const std::string& filename)
{
    std::vector<std::string> almanac = readLinesFromFile(filename);

    if (almanac.empty())
    {
        std::cerr << "Error: Almanac is empty." << std::endl;
        return;
    }

    std::vector<std::pair<long long int, long long int>> seedRanges = getSeedRanges(almanac);
    std::vector<std::pair<long long int, long long int>> soilRanges = mapRanges(almanac, seedRanges, "seed-to-soil map:");
    std::vector<std::pair<long long int, long long int>> fertilizerRanges = mapRanges(almanac, soilRanges, "soil-to-fertilizer map:");
    std::vector<std::pair<long long int, long long int>> waterRanges = mapRanges(almanac, fertilizerRanges, "fertilizer-to-water map:");
    std::vector<std::pair<long long int, long long int>> lightRanges = mapRanges(almanac, waterRanges, "water-to-light map:");
    std::vector<std::pair<long long int, long long int>> temperatureRanges = mapRanges(almanac, lightRanges, "light-to-temperature map:");
    std::vector<std::pair<long long int, long long int>> humidityRanges = mapRanges(almanac, temperatureRanges, "temperature-to-humidity map:");
    std::vector<std::pair<long long int, long long int>> locationRanges = mapRanges(almanac, humidityRanges, "humidity-to-location map:");

    if (!locationRanges.empty())
    {
        // Sort pairs by their second element in ascending order
        std::sort(locationRanges.begin(), locationRanges.end(), [](const std::pair<long long int, long long int>& a, const std::pair<long long int, long long int>& b) {
            return a.first < b.first;
            });

        std::cout << "Lowest location: " << locationRanges.front().first << std::endl;
    }
    else {
        std::cerr << "Error: No humidity to location mappings found." << std::endl;
    }
    return;
}

int main() {
    std::string filename = "day5.txt";
    std::string testfile = "test.txt";
    
    //runPartOne(filename);
    runPartTwo(filename);

    return 0;
}