#include "Randomizer.h"

#include <string>
#include <random>

using namespace std;


// Generate a random string with max length len
// TODO: replace with random dictionary words
string Randomizer::generateString(const int length)
{
    char str[length];
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for(int i = 0; i < length; ++i)
    {
        str[i] = alphanum[generateInt((sizeof(alphanum) - 1))];
    }

    str[length] = 0;

    string ret(str);
    return ret;
}

int Randomizer::generateInt(const int max)
{
    // Seed
    random_device rd;

    // Initialize generator
    mt19937 gen(rd());

    // Make a random uniform distribution
    uniform_int_distribution<> numbers(0,max);

    return numbers(gen);

}