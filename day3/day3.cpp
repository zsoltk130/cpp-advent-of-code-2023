#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <regex>

struct Symbol
{
    char symbol;
    int posX;
    int posY;
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

// FOR DEBUG
void writeVectorToFile(const std::vector<std::string>& vec, const std::string& filename) {
    // Create an ofstream object to open the file
    std::ofstream outfile(filename);

    // Check if the file is open
    if (!outfile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Write each string in the vector to the file
    for (const auto& str : vec) {
        outfile << str << std::endl;
    }

    // Close the file
    outfile.close();

    // Inform the user that writing is complete
    std::cout << "Data written to file: " << filename << std::endl;
}

// Finds the whole number that is adjacent to a special character by looping all the way left until '.' or any other non digit is found
// then looping all the way to the right until '.' or any other non digit is found and record the number (from left to right)
int findCompleteNumber(const std::vector<std::string>& schematic, int posX, int posY)
{
    std::string currentLine = schematic[posY];
    char currentChar = currentLine[posX];

    std::string number;

    // Go left until '.' or special char
    while (true)
    {
        if (posX == 0)
        {
            break;
        }
        char previousChar = currentLine[posX - 1];
        if (!isdigit(previousChar))
        {
            break;
        }
        currentChar = currentLine[--posX];
    }

    // Go right and add each number char to the number string variable until '.' or special char
    while (true)
    {
        char nextChar = currentLine[posX + 1];
        number += currentChar;
        // schematic[posY][posX] = 'X'; // FOR DEBUG (replaces all "spent" numbers with X's in the vector to be printed and visually analysed)

        if (!isdigit(nextChar))
        {
            break;
        }
        currentChar = currentLine[++posX];
    }

    // Convert number string to int before returning it
    return stoi(number);
}

// Finds numbers which are adjacent to special characters
void findAdjacentNumbers(const std::vector<std::string> &schematic, int x, int y, std::vector<int>& numbers)
{
    // Adjacent number search logic:
    // 
    //  x0123456789
    //y
    //0  ....401...
    //1  ...*......
    //2  ..159.....
    //
    // 1. Find symbol xy position
    // 
    //  x0123456789
    //y
    //0  ....401...
    //1  ...X......
    //2  ..159.....
    //
    // 2. Search 1 character around symbol until digit found starting from top left
    // 
    //  x0123456789
    //y
    //0  ..X.401...
    //1  ...*......
    //2  ..159.....
    //
    //  x0123456789
    //y
    //0  ...X401...
    //1  ...*......
    //2  ..159.....
    // 
    //  x0123456789
    //y
    //0  ....X01...
    //1  ...*......
    //2  ..159.....
    // 
    // When digit found use findCompleteNumber -> go all the way to the left until '.' found, then go right one 
    // character at a time and record the number until the other '.' is found
    // 
    //  x0123456789
    //y
    //0  ....XXX...
    //1  ...*......
    //2  ..159.....
    // =>401
    //
    // Continue searching around the symbol one character at a time left to right
    // 
    //  x0123456789
    //y
    //0  ....401...
    //1  ..X*X.....
    //2  ..159.....
    // 
    // When digit found go all the way to the left one digit at a time, then right one digit at a time recording the whole number left to right
    // 
    //  x0123456789
    //y
    //0  ....401...
    //1  ...*......
    //2  ..XXX.....
    // =>159
    //
    // And continue to the next symbol xy position
    

    // Define adjacent area to symbol
    int xMin = x - 1;
    int xMax = x + 1;
    int yMin = y - 1;
    int yMax = y + 1;
        
    std::string currentLine = schematic[y];
    char currentChar = currentLine[x];

    // Flag is set when we are looping over a number that we've already found
    // Flag is reset when we pass over a '.' or any of the other special chars
    // This is for cases when the surrounding area around the symbol looks like (or similar):
    // 
    // 3.4
    // .#5
    // 46.
    bool sameNumber = false;

    for (y = yMin; y <= yMax; y++)
    {
        currentLine = schematic[y];
        sameNumber = false;
        for (x = xMin; x <= xMax; x++)
        {
            currentChar = currentLine[x];
            if (!isdigit(currentChar))
            {
                sameNumber = false;
            }

            if (isdigit(currentChar) && !sameNumber)
            {
                int num = findCompleteNumber(schematic, x, y);

                if (!sameNumber)
                {
                    numbers.push_back(num);
                    sameNumber = true;
                }
            }
        }
    }
}

// Loops through schematic line by line, left to right and stores the symbol, x and y position in a vector of structs when found
std::vector<Symbol> findSymbolPositions(const std::vector<std::string>& schematic)
{
    std::vector<Symbol> symbolPositions;
   
    int schematicXSize = schematic[0].size(); // Vector width
    int schematicYSize = schematic.size(); // Vector height

    std::set<char> symbols = { '*', '%', '#', '@', '+', '-', '=', '/', '&', '$' };

    for (int posY = 0; posY < schematicYSize; posY++) // Y-Axis
    {
        for (int posX = 0; posX < schematicXSize; posX++) // X-Axis
        {
            std::string row = schematic[posY];
            char symbol = row[posX];
            
            if (symbols.find(symbol) != symbols.end()) {
                Symbol s;
                s.symbol = symbol;
                s.posX = posX;
                s.posY = posY;
                
                symbolPositions.push_back(s);
            }
        }
    }
    return symbolPositions;
}

int findSum(const std::vector<int>& partNumbers)
{
    int total = 0;
    for (int number : partNumbers)
    {
        total += number;
    }
    return total;
}

int findProduct(const std::vector<int>& partNumbers)
{
    int total = 1;
    for (int partNumber : partNumbers)
    {
        total *= partNumber;
    }
    return total;
}

// Finds sum of all part numbers which are adjacent to a special character
int runPartOne(const std::string& filename)
{
    std::vector<int> partNumbers;

    int total = 0;

    std::vector<std::string> schematic = readLinesFromFile(filename);
    std::vector<Symbol> symbols = findSymbolPositions(schematic);
    for (auto& symbol : symbols)
    {
        int symbolPosX = symbol.posX;
        int symbolPosY = symbol.posY;
        char symbolChar = symbol.symbol;

        findAdjacentNumbers(schematic, symbolPosX, symbolPosY, partNumbers);
    }
    total = findSum(partNumbers);

    return total;
}

// Finds sum of products of 2 or more part numbers which are adjacent to a gear '*'
int runPartTwo(const std::string& filename)
{
    std::vector<int> gearNumbers;

    int total = 0;

    std::vector<std::string> schematic = readLinesFromFile(filename);
    std::vector<Symbol> symbols = findSymbolPositions(schematic);
    for (auto& symbol : symbols)
    {
        int symbolPosX = symbol.posX;
        int symbolPosY = symbol.posY;
        char symbolChar = symbol.symbol;
        gearNumbers.clear();

        if (symbolChar == '*')
        {
            findAdjacentNumbers(schematic, symbolPosX, symbolPosY, gearNumbers);

            if (gearNumbers.size() > 1)
            {
                total += findProduct(gearNumbers);
                gearNumbers.clear();
            }
        }
    }
    
    return total;
}

int main()
{
    std::string filename = "day3.txt";
    // std::string outfile = "aoc3debug.txt"; // FOR DEBUG

    int partOne = runPartOne(filename);
    int partTwo = runPartTwo(filename);

    // writeVectorToFile(schematic, outfile); // FOR DEBUG

    std::cout << "Part 1 Total: " << partOne << std::endl << "Part 2 Total: " << partTwo << std::endl;
	return 0;
}
