/*
 *
 */

#ifndef SQL_CHARACTER_H
#define SQL_CHARACTER_H

// Project-specific includes
#include <sql/common.h>
#include <sql/types/exact_numeric.h>

template <typename T>
class SQLCharacter : public SQLNullable
{
public:
  SQLCharacter()
    : SQLNullable(true, true), m_string(nullptr), m_size(0)
  {}

  SQLCharacter(const SQLCharacter& other)
    : SQLNullable(false, other.IsNullable()), m_size(other.m_size)
  {
    // TODO: Error handling
    m_string = new T[m_size];
    memcpy(m_string, other.m_string, sizeof(T) * m_size);
  }

  SQLCharacter(T* orig, uint32_t size, bool nullable = true)
    : SQLNullable(false, nullable), m_size(size), m_string(nullptr)
  {
    // TODO: Error handling
    m_string = new T[size];
    memcpy(m_string, orig, sizeof(T) * size);
  }

  uint32_t m_size;
  T* m_string;

  SQLInteger length();

  void concat(const SQLCharacter<T>& other);
  static SQLCharacter<T> concat(const SQLCharacter<T>& lhs, const SQLCharacter<T>& rhs);

  SQLCharacter<T> substr(size_t start_pos, size_t len);
  static SQLCharacter<T> substr(const SQLCharacter<T>& str, size_t start_pos, size_t len);
};

typedef SQLCharacter<char> SQLCharacterASCII;

#endif
