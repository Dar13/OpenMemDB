/* TODO copyright header */

#ifndef UTIL_STDLIB_H
#define UTIL_STDLIB_H

#include <iterator>

// TODO: Document this
template <typename T>
bool safe_advance(T& itr, T end, size_t distance)
{
	while(itr != end && distance > 0)
	{
		itr++;
		distance--;
	}

	// If this returns true, then the iterator never reached end
	// Else it reached end early
	return (distance == 0);
}

#endif
