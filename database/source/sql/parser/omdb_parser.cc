/* TODO: File header */

#include <cstdlib>
#include "sql/omdb_parser.h"

#include "../source/sql/parser/parse.c"

void parse()
{
  void* parser = ParseAlloc(malloc);

  Token token;
  int token_id = 0;

  std::string input = "CREATE TABLE users (test INTEGER, test2 INTEGER);";

  // CREATE TABLE users ( test INTEGER );
  Token tokens[8];
  tokens[0] = new std::string("CREATE");
  tokens[1] = new std::string("TABLE");
  tokens[2] = new std::string("users");
  tokens[3] = new std::string("(");
  tokens[4] = new std::string("test");
  tokens[5] = new std::string("INTEGER");
  tokens[6] = new std::string(")");
  tokens[7] = new std::string(";");

  int token_ids[8] = { TK_CREATE, TK_TABLE, TK_ID, TK_LPAREN, TK_ID,
                       TK_STRING, TK_RPAREN, TK_SEMICOLON};

  StatementBuilder* builder = new StatementBuilder();

  for(int i = 0; i < 8; i++)
  {
    Parse(parser, token_ids[i], tokens[i], builder);
  }
  Parse(parser, 0, token, builder);
  ParseFree(parser, free);

  CreateTableCommand* create_command = (CreateTableCommand*)builder->statement;
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
