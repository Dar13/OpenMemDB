/* TODO: File header */

#ifndef DATA_HASH_FUNCTOR_H
#define DATA_HASH_FUNCTOR_H

/**
 *  \brief An object that implements various helper functions needed by Tervel's
 *         hash map implementation.
 */
template<class Key, class Value>
struct TableHashFunctor
{
  Key hash(Key k)
  {
    // TODO: Implement an actual hash algorithm
    return k;
  }

  bool key_equals(Key a, Key b)
  {
    return (a == b);
  }
};


#endif
