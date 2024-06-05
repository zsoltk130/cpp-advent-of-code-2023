#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <algorithm>

struct Card
{
    int cardNumber;
    std::vector<int> winningNumbers;
    std::vector<int> ownedNumbers;
    int matches = 0;
    int instances = 1;
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

// Take input vector of strings containing lines with a card, sequence of owned and sequence of winning numbers
// and use regex to find all numbers and the character '|'.
//
// First number found is always the card number.
// Everything after card number and '|' are winning numbers.
// Everything after '|' and end of line are owned numbers.
//
// Separate card number, winning numbers and owned numbers using the Card struct.
// Add each card with separated card number, winning numbers and owned numbers back into a vector of cards.
std::vector<Card> findNumbers(const std::vector<std::string>& input)
{
    std::regex numberPattern("\\d+|[|]");
    std::vector<Card> cards;

    for (std::string line : input)
    {
        Card card;
        std::string currentLine = line;

        auto numberPattern_begin = std::sregex_iterator(line.begin(), line.end(), numberPattern);
        auto numberPattern_end = std::sregex_iterator();

        bool isCardNumber = true;
        bool isSeparator = false;

        for (std::sregex_iterator it = numberPattern_begin; it != numberPattern_end; ++it) {
            std::smatch match = *it;
            std::string currentNumber = match.str();
            
            if (isCardNumber) 
            { 
                card.cardNumber = std::stoi(currentNumber);
                isCardNumber = false;
                continue;
            }
            if (currentNumber == "|")
            {
                isSeparator = true;
                continue;
            }
            if (!isSeparator && !isCardNumber)
            {
                int num = std::stoi(currentNumber);
                card.winningNumbers.push_back(num);
            }
            if (isSeparator && !isCardNumber)
            {
                int num = std::stoi(currentNumber);
                card.ownedNumbers.push_back(num);
            }
        }
        cards.push_back(card);
    }
    return cards;
}

// For each card, loop through all owned numbers one by one and check if it's among the winning numbers.
//
// First match results in 1 point.
// Subsequent matches double the gained points.
int findPointsFromWinningNumbers(Card card)
{
    int points = 0;

    std::vector<int> winningNumbers = card.winningNumbers;
    std::vector<int> ownedNumbers = card.ownedNumbers;

    for (int ownedNumber : ownedNumbers)
    {
        if (std::find(winningNumbers.begin(), winningNumbers.end(), ownedNumber) != winningNumbers.end())
        {
            if (points == 0)
            {
                points = 1;
            }
            else
            {
                points *= 2;
            }
        }
    }

    return points;
}

// Loop through vector of cards, card by card, and call findPointsFromWinningNumbers() on each card
// to retrieve the winning points from that card.
//
// Add all points together into total and return.
int findTotalPointsFromCards(std::vector<Card>& cards)
{
    int total = 0;

    for (Card card : cards)
    {
        total += findPointsFromWinningNumbers(card);
    }

    return total;
}

int findMatchingNumbers(Card card)
{
    int matches = 0;

    std::vector<int> winningNumbers = card.winningNumbers;
    std::vector<int> ownedNumbers = card.ownedNumbers;

    for (int ownedNumber : ownedNumbers)
    {
        if (std::find(winningNumbers.begin(), winningNumbers.end(), ownedNumber) != winningNumbers.end())
        {
            matches++;
        }
    }

    return matches;
}

void findAllMatchingNumbers(std::vector<Card>& cards)
{
    for (Card& card : cards)
    {
        card.matches = findMatchingNumbers(card);
    }
}

void findCardInstances(Card card, std::vector<Card>& cards)
{
    int matches = card.matches;
    int nextCardNumber = card.cardNumber;
    int maxCardNumber = nextCardNumber + matches;
    int currentCardInstances = card.instances;
    
    for (; nextCardNumber < maxCardNumber; nextCardNumber++)
    {
        cards[nextCardNumber].instances += 1 * currentCardInstances;
    }

}

void findAllInstances(std::vector<Card>& cards)
{
    for (Card& card : cards)
    {
        int matches = card.matches;

        if (matches > 0)
        {
            findCardInstances(card, cards);
        }
    }
}

int countInstances(std::vector<Card>& cards)
{
    int total = 0;

    for (Card card : cards)
    {
        total += card.instances;
    }

    return total;
}

void runPartOne(std::vector<Card>& cards)
{
    int total = findTotalPointsFromCards(cards);
    std::cout << "Part 1 Total: " << total << std::endl;
}

void runPartTwo(std::vector<Card>& cards)
{
    findAllMatchingNumbers(cards);
    findAllInstances(cards);
    int total = countInstances(cards);
    std::cout << "Part 2 Total: " << total << std::endl;
}

int main()
{
    std::string filename = "day4.txt";
    std::vector<std::string> input = readLinesFromFile(filename);
    std::vector<Card> cards = findNumbers(input);
    
    runPartOne(cards);
    runPartTwo(cards);
	return 0;
}
