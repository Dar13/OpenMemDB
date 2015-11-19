/* TODO: File header */

#include <cstdlib>
#include <map>
#include "sql/omdb_parser.h"

#include "../source/sql/parser/parse.c"

static thread_local std::map<std::string, int> keywords;
static thread_local StatementBuilder builder;

void parse(std::string input)
{
  void* parser = ParseAlloc(malloc);

  Token token;
  int token_id = 0;

  // Tokenize the input string
  std::vector<TokenPair> tokens = tokenize(input);

  for(auto token_pair : tokens)
  {
    Parse(parser, token_pair.token_type, token_pair.token, &builder);
  }
  Parse(parser, 0, nullptr, &builder);
  ParseFree(parser, free);

  CreateTableCommand* create_command = (CreateTableCommand*)builder.statement;
  if(create_command == nullptr)
  {
    printf("Command not created\n");
  }
  else
  {
    printf("Parse successful\n");
  }
}

void token_print(Token token)
{
  printf("Token: %s\n", token->c_str());
}

std::vector<TokenPair> tokenize(std::string input)
{
  std::string::iterator start_token = input.end();
  std::string::iterator end_token = input.end();

  std::string::iterator itr = input.begin();

  std::vector<TokenPair> tokens;

  TokenPair pair;

  while(itr != input.end())
  {
    if(isSQLIdentifierChar(*itr))
    {
      // Continue through the string until whitespace or non-identifier char found
      std::string::iterator next = itr + 1;

      while(isSQLIdentifierChar(*next))
      {
        next++;
      }

      pair.token = new std::string(itr, next);
      if(keywords.find(*pair.token) != keywords.end())
      {
        pair.token_type = keywords[*pair.token];
      }
      else
      {
        pair.token_type = TK_ID;
      }

      itr = next;

      tokens.push_back(pair);

      continue;
    }
    else
    {
      if(isSQLSymbolChar(*itr))
      {
        // Finish out the symbol
        switch(*itr)
        {
          case '(':
            pair.token = new std::string("(");
            pair.token_type = TK_LPAREN;
            break;
          case ')':
            pair.token = new std::string(")");
            pair.token_type = TK_RPAREN;
            break;
          case ';':
            pair.token = new std::string(";");
            pair.token_type = TK_SEMICOLON;
            break;
          case ',':
            pair.token = new std::string(",");
            pair.token_type = TK_COMMA;
            break;
          default:
            pair.token = new std::string("ILLEGAL");
            pair.token_type = TK_ILLEGAL;
            break;
        }

        tokens.push_back(pair);
      }
    }
      
    itr++;
  }

  return tokens;
}

void setupTokenMappings()
{
  // TODO: Update this as necessary
  keywords["CREATE"] = TK_CREATE;
  keywords["TABLE"] = TK_TABLE;
}
