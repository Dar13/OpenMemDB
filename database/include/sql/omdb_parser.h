/* TODO: File Header */

#ifndef SQL_OMDB_PARSER_H
#define SQL_OMDB_PARSER_H

// C standard library includes
#include <cstdio>
#include <cstdint>
#include <cassert>

// C++ standard library includes
#include <string>

// Project includes
#include "parser/parse.h"
#include "statements/builder.h"
#include "statements/data_definition.h"
#include "statements/data_manipulation.h"

// Needed for the parser generated by Lemon
typedef uint64_t u64;

/**
 *  \brief The datatype passed into the parser from the tokenizer
 */
typedef SQLToken Token;

/* TODO: Come back and rework this whole type system. */
/**
 *  \brief All the information related to a token needed by the parser
 */
struct TokenPair
{
  //! The actual token
  Token token;

  //! The token type
  uint32_t token_type;
};

void setupTokenMappings();

void token_print(Token token);

std::vector<TokenPair> tokenize(std::string input);

struct Parse;

void parse(std::string input);

#define NEVER(X) 0

#endif