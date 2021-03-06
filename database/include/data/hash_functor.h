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

#ifndef DATA_HASH_FUNCTOR_H
#define DATA_HASH_FUNCTOR_H

#include <functional>

/**
 *  \brief An object that implements various helper functions needed by Tervel's
 *         hash map implementation.
 */
template<class Key, class Value>
struct TableHashFunctor
{
  Key hash(Key k)
  {
    // Using a proper hashing algorithm is useless
    // It's a waste of cycles due to the compare of most types being so expensive.
    // If Tervel used an integral-type internal storage of keys, this would be 
    // worthy of a hashing algorithm.
    return k;
  }

  bool key_equals(Key a, Key b)
  {
    return (a == b);
  }
};


#endif
