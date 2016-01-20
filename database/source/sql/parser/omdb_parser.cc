/* TODO: File header */

#include <cstdlib>
#include <map>
#include "sql/omdb_parser.h"
#include "sql/statements/expression.h"

#include "data/data_store.h"

#include "../source/sql/parser/parse.c"

static thread_local std::map<std::string, int> keywords;

ParseResult parse(std::string input, DataStore* data_store)
{
    // TODO: Is there a way to re-use the parse tree?
    void* parser = ParseAlloc(malloc);

    StatementBuilder builder;

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

    if(builder.statement == nullptr || !builder.valid)
    {
        printf("Failed to parse statement!\n");
        return ParseResult(ResultStatus::ERROR_SYNTAX, nullptr);
    }
    else
    {
        setupStatement(builder.statement, builder.expr, data_store);

        printf("Statement parse was successful!\n");
        return ParseResult(ResultStatus::SUCCESS, builder.statement);
    }

    return ParseResult(ResultStatus::ERROR_SYNTAX, nullptr);
}

void token_print(Token token)
{
  //printf("Token: %s\n", token->c_str());
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
    if(isSQLAlphabeticChar(*itr))
    {
      // Continue through the string until whitespace or non-identifier char found
      std::string::iterator next = itr + 1;

      while(isSQLIdentifierChar(*next))
      {
        next++;
      }

      std::string* token = new (std::nothrow) std::string(itr, next);
      if(token == nullptr)
      {
          // TODO: Error handling
      }

      pair.token = new (std::nothrow) TokenData(token);
      if(pair.token == nullptr)
      {
          // TODO: Error handling
      }

      if(keywords.find(*pair.token->text) != keywords.end())
      {
        pair.token_type = keywords[*pair.token->text];

        ExpressionOperation op = getOperation(*pair.token->text);
        if(op != ExpressionOperation::NO_OP)
        {
            pair.token->is_operation = true;
            pair.token->operation = static_cast<uint16_t>(op);
        }
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
        if(isSQLNumericChar(*itr))
        {
            // TODO: Handle hexadecimal numbers
            std::string::iterator next = itr + 1;

            while(next != input.end() && (isSQLNumericChar(*next) || *next == '.'))
            {
                next++;
            }

            std::string* token = new (std::nothrow) std::string(itr, next);
            if(token == nullptr)
            {
                // TODO: Error handling
            }

            if(token->find('.') == std::string::npos)
            {
                pair.token_type = TK_INTEGER;
                int64_t value = std::stoll(*token, nullptr, 10);
                
                pair.token = new (std::nothrow) TokenData(token, value);
                if(pair.token == nullptr)
                {
                    // TODO: Error handling
                }
            }
            else
            {
                pair.token_type = TK_FLOAT;
                float value = std::stof(*token);

                pair.token = new (std::nothrow) TokenData(token, value);
                if(pair.token == nullptr)
                {
                    // TODO: Error handling
                }
            }

            itr = next;
            tokens.push_back(pair);

            continue;
        }
            
      if(isSQLSymbolChar(*itr))
      {
          pair.token = new TokenData();
        // Finish out the symbol
        switch(*itr)
        {
          case '(':
            pair.token->text = new std::string("(");
            pair.token_type = TK_LPAREN;
            break;
          case ')':
            pair.token->text = new std::string(")");
            pair.token_type = TK_RPAREN;
            break;
          case ';':
            pair.token->text = new std::string(";");
            pair.token_type = TK_SEMICOLON;
            break;
          case ',':
            pair.token->text = new std::string(",");
            pair.token_type = TK_COMMA;
            break;
          case '.':
            pair.token->text = new std::string(".");
            pair.token_type = TK_DOT;
            break;
          case '*':
            pair.token->text = new std::string("*");
            pair.token_type = TK_ASTERISK;
            break;
          case '=':
            pair.token->text = new std::string("=");
            pair.token->is_operation = true;
            pair.token->operation = static_cast<uint16_t>(ExpressionOperation::EQUALS);

            pair.token_type = TK_EQ;
            break;
          case '!':
            if(*(itr + 1) == '=')
            {
                pair.token->text = new std::string("!=");
                pair.token_type = TK_NE;

                pair.token->is_operation = true;
                pair.token->operation = static_cast<uint16_t>(ExpressionOperation::NOT_EQUALS);
            }
            else
            {
                pair.token->text = new std::string("!");
                pair.token_type = TK_NOT;
            }
            break;
          case '<':
            if(*(itr + 1) == '=')
            {
                pair.token->text = new std::string("<=");
                pair.token_type = TK_LE;

                pair.token->is_operation = true;
                pair.token->operation = static_cast<uint16_t>(ExpressionOperation::LESSER_EQUALS);
            }
            else
            {
                pair.token->text = new std::string("<");
                pair.token_type = TK_LT;

                pair.token->is_operation = true;
                pair.token->operation = static_cast<uint16_t>(ExpressionOperation::LESSER);
            }
            break;
          case '>':
            if(*(itr + 1) == '=')
            {
                pair.token->text = new std::string(">=");
                pair.token_type = TK_GE;

                pair.token->is_operation = true;
                pair.token->operation = static_cast<uint16_t>(ExpressionOperation::GREATER_EQUALS);
            }
            else
            {
                pair.token->text = new std::string(">");
                pair.token_type = TK_GT;

                pair.token->is_operation = true;
                pair.token->operation = static_cast<uint16_t>(ExpressionOperation::GREATER);
            }
            break;
          default:
            pair.token->text = new std::string("ILLEGAL");
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
  keywords["AND"] = TK_AND;
  keywords["OR"] = TK_OR;
}
