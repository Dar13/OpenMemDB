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

  if(builder.statement == nullptr)
  {
      printf("Failed to parse statement!\n");
  }
  else
  {
      printf("Statement parse was successful!\n");
  }

  switch(builder.type)
  {
      case SQLStatement::CREATE_TABLE:
      {
          CreateTableCommand* create = reinterpret_cast<CreateTableCommand*>(builder.statement);
      }
          break;
      case SQLStatement::DROP_TABLE:
      {
          DropTableCommand* drop = reinterpret_cast<DropTableCommand*>(builder.statement);
      }
          break;
      case SQLStatement::SELECT:
      {
          SelectQuery* query = reinterpret_cast<SelectQuery*>(builder.statement);
      }
          break;
      // TODO: The other statements
      case SQLStatement::UPDATE:
      case SQLStatement::INSERT_INTO:
      case SQLStatement::DELETE:
      default:
          break;
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
          case '.':
            pair.token = new std::string(".");
            pair.token_type = TK_DOT;
            break;
          case '*':
            pair.token = new std::string("*");
            pair.token_type = TK_ASTERISK;
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
  keywords["AS"] = TK_AS;
  keywords["WHERE"] = TK_WHERE;
  keywords["SELECT"] = TK_SELECT;
  keywords["FROM"] = TK_FROM;
  keywords["ALL"] = TK_ALL;
  keywords["DISTINCT"] = TK_DISTINCT;
  keywords["UNIQUE"] = TK_UNIQUE;
  keywords["AUTO_INCREMENT"] = TK_AUTO_INCREMENT;
  keywords["NOT"] = TK_NOT;
  keywords["NULL"] = TK_NULL;
  keywords["DEFAULT"] = TK_DEFAULT;
}
