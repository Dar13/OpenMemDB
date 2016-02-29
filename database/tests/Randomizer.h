#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <string>

using namespace std;


class Randomizer
{
	public:
		static int generateInt(const int max);
		static string generateString(const int length);
};


#endif // RANDOMIZER_H