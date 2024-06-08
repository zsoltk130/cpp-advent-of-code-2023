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


int findIndexOfLineContainingString(const std::vector<std::string>& data, const std::string& target) {
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == target) {
            return i;
        }
    }
    return -1;
}

std::vector<Range> extractRangesFromSection(std::vector<std::string>& almanac, std::string sectionHeader)
{
    int indexOfSectionHeader = findIndexOfLineContainingString(almanac, sectionHeader);
    std::regex numberPattern("\\d+");

    Range range;
    std::vector<Range> ranges;

    int lineNumber = 1;

    while (true)
    {
        std::string line = almanac[indexOfSectionHeader + lineNumber];

        auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
        auto iterator_end = std::sregex_iterator();

        long long int destinationRangeStart = 0;
        long long int sourceRangeStart = 0;
        long long int rangeLength = 0;
        int column = 0;

         for (std::sregex_iterator it = iterator_begin; it != iterator_end; ++it) {
            std::smatch match = *it;
            std::string currentNumber = match.str();
            
            if (column == 0)
            {
                destinationRangeStart = std::stoll(currentNumber);
                column++;
            }
            else if (column == 1)
            {
                sourceRangeStart = std::stoll(currentNumber);
                column++;
            }
            else if (column == 2)
            {
                rangeLength = std::stoll(currentNumber);

                range.sourceRange.first = sourceRangeStart;
                range.sourceRange.second = sourceRangeStart + rangeLength;
                range.destinationRange.first = destinationRangeStart;
                range.destinationRange.second = destinationRangeStart + rangeLength;
                ranges.push_back(range);

                column = 0;
            }
        }

        if (line == "")
        {
            break;
        }

        if (line == almanac.back())
        {
            break;
        }
        
        lineNumber++;
    }
    
    return ranges;
}

std::vector<std::pair<long long int, long long int>> mapSeedToSoil(std::vector<std::string>& almanac)
{
    std::vector<std::pair<long long int, long long int>> seedToSoilMap; // To return
    
    // Get first line from almanac (seeds) and put each number into a vector of ints using regex
    std::vector<long long int> seeds;
    std::string line = almanac[0];
    std::regex numberPattern("\\d+");

    auto iterator_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
    auto iterator_end = std::sregex_iterator();

    for (std::sregex_iterator it = iterator_begin; it != iterator_end; ++it) {
        std::smatch match = *it;
        std::string currentNumber = match.str();
        seeds.push_back(std::stoll(currentNumber));
    }

    std::vector<Range> ranges = extractRangesFromSection(almanac, "seed-to-soil map:");

    for (long long int seed : seeds)
    {
        bool found = false;
        for (Range range : ranges)
        {
            long long int seedLower = range.sourceRange.first;
            long long int seedHigher = range.sourceRange.second;
            long long int soilLower = range.destinationRange.first;
            long long int soilHigher = range.destinationRange.second;
        
            long long int seedSoilOffset = soilLower - seedLower;
            long long int soil = 0;

            std::pair<long long int, long long int> seedToSoil;

            if (seedLower <= seed && seed < seedHigher)
            {
                soil = seed + seedSoilOffset;

                if (soilLower <= soil && soil < soilHigher)
                {
                    seedToSoil.first = seed;
                    seedToSoil.second = soil;
                    seedToSoilMap.push_back(seedToSoil);
                    found = true;
                }
            }
        }

        if (!found)
        {
            std::pair<long long int, long long int> seedToSoil;
            seedToSoil.first = seed;
            seedToSoil.second = seed;
            seedToSoilMap.push_back(seedToSoil);
        }
    }

    return seedToSoilMap;
}

std::vector<std::pair<long long int, long long int>> mapSoilToFertilizer(std::vector<std::string>& almanac, std::vector<std::pair<long long int, long long int>>& map)
{
    std::vector<std::pair<long long int, long long int>> soilToFertilizerMap; // To return
    
    std::vector<Range> ranges = extractRangesFromSection(almanac, "soil-to-fertilizer map:");

    std::vector<std::pair<long long int, long long int>> seedToSoilMap = map;

    for (std::pair<long long int, long long int> map : seedToSoilMap)
    {
        long long int soil = map.second;
        bool found = false;

        for (Range range : ranges)
        {
            long long int soilLower = range.sourceRange.first;
            long long int soilHigher = range.sourceRange.second;
            long long int fertilizerLower = range.destinationRange.first;
            long long int fertilizerHigher = range.destinationRange.second;

            long long int offset = fertilizerHigher - soilHigher;

            std::pair<long long int, long long int> soilToFertilizer;

            if (soilLower <= soil && soil < soilHigher)
            {
                long long int fertilizer = soil + offset;

                if (fertilizerLower <= fertilizer && fertilizer < fertilizerHigher)
                {
                    soilToFertilizer.first = soil;
                    soilToFertilizer.second = fertilizer;
                    soilToFertilizerMap.push_back(soilToFertilizer);
                    found = true;
                }
            }
        }
        
        if (!found)
        {
            std::pair<long long int, long long int> soilToFertilizer;
            soilToFertilizer.first = soil;
            soilToFertilizer.second = soil;
            soilToFertilizerMap.push_back(soilToFertilizer);
        }
    }

    return soilToFertilizerMap;
}

std::vector<std::pair<long long int, long long int>> mapFertilizerToWater(std::vector<std::string>& almanac, std::vector<std::pair<long long int, long long int>>& map)
{
    std::vector<std::pair<long long int, long long int>> fertilizerToWaterMap; // To return

    std::vector<Range> ranges = extractRangesFromSection(almanac, "fertilizer-to-water map:");

    std::vector<std::pair<long long int, long long int>> soilToFertilizerMap = map;

    for (std::pair<long long int, long long int> map : soilToFertilizerMap)
    {
        long long int fertilizer = map.second;
        bool found = false;

        for (Range range : ranges)
        {
            long long int fertilizerLower = range.sourceRange.first;
            long long int fertilizerHigher = range.sourceRange.second;
            long long int waterLower = range.destinationRange.first;
            long long int waterHigher = range.destinationRange.second;

            long long int offset = waterHigher - fertilizerHigher;

            std::pair<long long int, long long int> fertilizerToWater;

            if (fertilizerLower <= fertilizer && fertilizer < fertilizerHigher)
            {
                long long int water = fertilizer + offset;

                if (waterLower <= water && water < waterHigher)
                {
                    fertilizerToWater.first = fertilizer;
                    fertilizerToWater.second = water;
                    fertilizerToWaterMap.push_back(fertilizerToWater);
                    found = true;
                }
            }
        }

        if (!found)
        {
            std::pair<long long int, long long int> fertilizerToWater;
            fertilizerToWater.first = fertilizer;
            fertilizerToWater.second = fertilizer;
            fertilizerToWaterMap.push_back(fertilizerToWater);
        }
    }

    return fertilizerToWaterMap;
}

std::vector<std::pair<long long int, long long int>> mapWaterToLight(std::vector<std::string>& almanac, std::vector<std::pair<long long int, long long int>>& map)
{
    std::vector<std::pair<long long int, long long int>> waterToLightMap; // To return

    std::vector<Range> ranges = extractRangesFromSection(almanac, "water-to-light map:");

    std::vector<std::pair<long long int, long long int>> fertilizerToWaterMap = map;

    for (std::pair<long long int, long long int> map : fertilizerToWaterMap)
    {
        long long int water = map.second;
        bool found = false;

        for (Range range : ranges)
        {
            long long int waterLower = range.sourceRange.first;
            long long int waterHigher = range.sourceRange.second;
            long long int lightLower = range.destinationRange.first;
            long long int lightHigher = range.destinationRange.second;

            long long int offset = lightHigher - waterHigher;

            std::pair<long long int, long long int> waterToLight;

            if (waterLower <= water && water < waterHigher)
            {
                long long int light = water + offset;

                if (lightLower <= light && light < lightHigher)
                {
                    waterToLight.first = water;
                    waterToLight.second = light;
                    waterToLightMap.push_back(waterToLight);
                    found = true;
                }
            }
        }

        if (!found)
        {
            std::pair<long long int, long long int> waterToLight;
            waterToLight.first = water;
            waterToLight.second = water;
            waterToLightMap.push_back(waterToLight);
        }
    }

    return waterToLightMap;
}

std::vector<std::pair<long long int, long long int>> mapLightToTemperature(std::vector<std::string>& almanac, std::vector<std::pair<long long int, long long int>>& map)
{
    std::vector<std::pair<long long int, long long int>> lightToTemperatureMap; // To return

    std::vector<Range> ranges = extractRangesFromSection(almanac, "light-to-temperature map:");

    std::vector<std::pair<long long int, long long int>> waterToLightMap = map;

    for (std::pair<long long int, long long int> map : waterToLightMap)
    {
        long long int light = map.second;
        bool found = false;

        for (Range range : ranges)
        {
            long long int lightLower = range.sourceRange.first;
            long long int lightHigher = range.sourceRange.second;
            long long int temperatureLower = range.destinationRange.first;
            long long int temperatureHigher = range.destinationRange.second;

            long long int offset = temperatureHigher - lightHigher;

            std::pair<long long int, long long int> lightToTemperature;

            if (lightLower <= light && light < lightHigher)
            {
                long long int temperature = light + offset;

                if (temperatureLower <= temperature && temperature < temperatureHigher)
                {
                    lightToTemperature.first = light;
                    lightToTemperature.second = temperature;
                    lightToTemperatureMap.push_back(lightToTemperature);
                    found = true;
                }
            }
        }

        if (!found)
        {
            std::pair<long long int, long long int> lightToTemperature;
            lightToTemperature.first = light;
            lightToTemperature.second = light;
            lightToTemperatureMap.push_back(lightToTemperature);
        }
    }

    return lightToTemperatureMap;
}

std::vector<std::pair<long long int, long long int>> mapTemperatureToHumidity(std::vector<std::string>& almanac, std::vector<std::pair<long long int, long long int>>& map)
{
    std::vector<std::pair<long long int, long long int>> temperatureToHumidityMap; // To return

    std::vector<Range> ranges = extractRangesFromSection(almanac, "temperature-to-humidity map:");

    std::vector<std::pair<long long int, long long int>> lightToTemperatureMap = map;

    for (std::pair<long long int, long long int> map : lightToTemperatureMap)
    {
        long long int temperature = map.second;
        bool found = false;

        for (Range range : ranges)
        {
            long long int temperatureLower = range.sourceRange.first;
            long long int temperatureHigher = range.sourceRange.second;
            long long int humidityLower = range.destinationRange.first;
            long long int humidityHigher = range.destinationRange.second;

            long long int offset = humidityHigher - temperatureHigher;

            std::pair<long long int, long long int> temperatureToHumidity;

            if (temperatureLower <= temperature && temperature < temperatureHigher)
            {
                long long int humidity = temperature + offset;

                if (humidityLower <= humidity && humidity < humidityHigher)
                {
                    temperatureToHumidity.first = temperature;
                    temperatureToHumidity.second = humidity;
                    temperatureToHumidityMap.push_back(temperatureToHumidity);
                    found = true;
                }
            }
        }

        if (!found)
        {
            std::pair<long long int, long long int> temperatureToHumidity;
            temperatureToHumidity.first = temperature;
            temperatureToHumidity.second = temperature;
            temperatureToHumidityMap.push_back(temperatureToHumidity);
        }
    }

    return temperatureToHumidityMap;
}

std::vector<std::pair<long long int, long long int>> mapHumidityToLocation(std::vector<std::string>& almanac, std::vector<std::pair<long long int, long long int>>& map)
{
    std::vector<std::pair<long long int, long long int>> humidityToLocationMap; // To return

    std::vector<Range> ranges = extractRangesFromSection(almanac, "humidity-to-location map:");

    std::vector<std::pair<long long int, long long int>> temperatureToHumidityMap = map;

    for (std::pair<long long int, long long int> map : temperatureToHumidityMap)
    {
        long long int humidity = map.second;
        bool found = false;

        for (Range range : ranges)
        {
            long long int humidityLower = range.sourceRange.first;
            long long int humidityHigher = range.sourceRange.second;
            long long int locationLower = range.destinationRange.first;
            long long int locationHigher = range.destinationRange.second;

            long long int offset = locationHigher - humidityHigher;

            std::pair<long long int, long long int> humidityToLocation;

            if (humidityLower <= humidity && humidity < humidityHigher)
            {
                long long int location = humidity + offset;

                if (locationLower <= location && location < locationHigher)
                {
                    humidityToLocation.first = humidity;
                    humidityToLocation.second = location;
                    humidityToLocationMap.push_back(humidityToLocation);
                    found = true;
                }
            }
        }

        if (!found)
        {
            std::pair<long long int, long long int> humidityToLocation;
            humidityToLocation.first = humidity;
            humidityToLocation.second = humidity;
            humidityToLocationMap.push_back(humidityToLocation);
        }
    }

    return humidityToLocationMap;
}

int main()
{
    std::string testfile = "test.txt";
    std::string filename = "day5.txt";
    std::vector<std::string> almanac = readLinesFromFile(filename);
    std::vector<std::pair<long long int, long long int>> seedToSoilMaps = mapSeedToSoil(almanac);
    std::vector<std::pair<long long int, long long int>> soilToFertilizerMaps = mapSoilToFertilizer(almanac, seedToSoilMaps);
    std::vector<std::pair<long long int, long long int>> fertilizerToWaterMaps = mapFertilizerToWater(almanac, soilToFertilizerMaps);
    std::vector<std::pair<long long int, long long int>> waterToLightMaps = mapWaterToLight(almanac, fertilizerToWaterMaps);
    std::vector<std::pair<long long int, long long int>> lightToTemperatureMaps = mapLightToTemperature(almanac, waterToLightMaps);
    std::vector<std::pair<long long int, long long int>> temperatureToHumidityMaps = mapTemperatureToHumidity(almanac, lightToTemperatureMaps);
    std::vector<std::pair<long long int, long long int>> humidityToLocationMaps = mapHumidityToLocation(almanac, temperatureToHumidityMaps);

    std::sort(humidityToLocationMaps.begin(), humidityToLocationMaps.end(), [](const std::pair<long long int, long long int>& a, const std::pair<long long int, long long int>& b) {
        return a.second < b.second;
        });

    std::cout << "Lowest location: " << humidityToLocationMaps[0].second << std::endl;

    return 0;
}