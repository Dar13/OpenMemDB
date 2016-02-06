/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UTIL_STDLIB_H
#define UTIL_STDLIB_H

#include <iterator>

/**
 * \brief Implements a safe advancement of the given iterator over a specified distance
 *
 * \return True if the advance is safe, false if it fails.
 */
template <typename T>
bool safe_advance(T& itr, const T end, size_t distance)
{
    typedef typename std::iterator_traits<T>::iterator_category tag;
    static_assert(std::is_base_of<std::forward_iterator_tag, tag>::value,
            "Iterators passed to safe_advance must be forward iterators");

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
