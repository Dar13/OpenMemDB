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

#include "sql/common.h"
#include <map>
#include <cctype>

/*
 *  This table was taken from SQLite3, with the following comment:
 *  TODO: Copy over copyright notice as needed
 *
 * ----------------------------------------------------------------------------
 *  The following 256 byte lookup table is used to support SQLites built-in
 *  equivalents to the following standard library functions:
 *
 *    isspace()                        0x01
 *    isalpha()                        0x02
 *    isdigit()                        0x04
 *    isalnum()                        0x06
 *    isxdigit()                       0x08
 *    toupper()                        0x20
 *    SQLite identifier character      0x40
 *
 *  Bit 0x20 is set if the mapped character requires translation to upper
 *  case. i.e. if the character is a lower-case ASCII character.
 *  If x is a lower-case ASCII character, then its upper-case equivalent
 *  is (x - 0x20). Therefore toupper() can be implemented as:
 *
 *    (x & ~(map[x]&0x20))
 *
 *  Standard function tolower() is implemented using the sqlite3UpperToLower[]
 *  array. tolower() is used more often than toupper() by SQLite.
 *
 *  Bit 0x40 is set if the character non-alphanumeric and can be used in an 
 *  SQLite identifier.  Identifiers are alphanumerics, "_", "$", and any
 *  non-ASCII UTF character. Hence the test for whether or not a character is
 *  part of an identifier is 0x46.
 *
 *  SQLite's versions are identical to the standard versions assuming a
 *  locale of "C". They are implemented as macros in sqliteInt.h.
 * ----------------------------------------------------------------------------
 *  OpenMemDB has the following constants to make it easier to reason about the 
 *  above:
 *    SQL_NONE for characters not used by SQL
 *    SQL_SPACE for whitespace characters
 *    SQL_ALPHA for alphabet characters as defined by ASCII
 *    SQL_DIGIT for digits [0-9]
 *    SQL_ALNUM for the union of SQL_ALPHA and SQL_DIGIT
 *    SQL_XDIGIT for the hexadecimal digits [A-F][0-9][xX]
 *    SQL_IDENT for all characters that can be used in a SQL identifier
 *    SQL_SYMBOL for all symbolic characters used within SQL
 */

const unsigned char sql_ascii_map[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 00..07    ........ */
  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,  /* 08..0f    ........ */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 10..17    ........ */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 18..1f    ........ */
  0x01, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,  /* 20..27     !"#$%&' */
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,  /* 28..2f    ()*+,-./ */
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,  /* 30..37    01234567 */
  0x0c, 0x0c, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00,  /* 38..3f    89:;<=>? */
  0x00, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x02,  /* 40..47    @ABCDEFG */
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,  /* 48..4f    HIJKLMNO */
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,  /* 50..57    PQRSTUVW */
  0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x40,  /* 58..5f    XYZ[\]^_ */
  0x00, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x22,  /* 60..67    `abcdefg */
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,  /* 68..6f    hijklmno */
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,  /* 70..77    pqrstuvw */
  0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 78..7f    xyz{|}~. */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 80..87    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 88..8f    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 90..97    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 98..9f    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* a0..a7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* a8..af    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* b0..b7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* b8..bf    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* c0..c7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* c8..cf    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* d0..d7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* d8..df    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* e0..e7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* e8..ef    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* f0..f7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40   /* f8..ff    ........ */
};

int strnicmp(const char* left, const char* right, int n)
{
  const unsigned char *str_left, *str_right;
  if(left == nullptr)
  {
    return right ? -1 : 0;
  }
  else
  {
    if(right == nullptr)
    {
      return 1;
    }
  }

  str_left = reinterpret_cast<const unsigned char*>(left);
  str_right = reinterpret_cast<const unsigned char*>(right);

  while( (n--) > 0 && (*str_left) != 0 &&
         (tolower(*str_left) == tolower(*str_right)))
  {
    str_left++;
    str_right++;
  }

  return (n < 0) ? 0 : (tolower(*str_left) - tolower(*str_right));
}

