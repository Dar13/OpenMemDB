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

// C++ stdlib includes
#include <cstdlib>  // Used for lemon
#include <map>

// Project includes
#include "util/omdb_stdlib.h"

#include "sql/omdb_parser.h"
#include "sql/statements/expression.h"

#include "data/data_store.h"

#include "../source/sql/parser/parse.c"

static thread_local std::map<std::string, int> keywords;
static thread_local void* parser = nullptr;

/**
 *  \brief Attempts to parse the given SQL statement in the context of the 
 *  given Data Store object
 *
 *  \detail Failure cases are restricted to either memory allocation failure 
 *  or syntax error. ResultStatus::FAILURE implies syntax error and
 *  ResultStatus::FAILURE_OUT_MEMORY implies a memory allocation failure.
 */
ParseResult parse(std::string input, DataStore* data_store)
{
    if(parser == nullptr)
    {
        parser = ParseAlloc(malloc);

        if(parser == nullptr)
        {
            return ParseResult(ResultStatus::FAILURE_OUT_MEMORY, nullptr);
        }
    }

    StatementBuilder builder;
    builder.data_store = data_store;

    // Tokenize the input string
    std::vector<TokenPair> tokens = tokenize(input);

    /*
    for(auto token_pair = tokens.begin();
        token_pair != tokens.end() || token_pair->token_type != TK_ILLEGAL;
        token_pair++)
        */
    for(auto&& token_pair : tokens)
    {
        Parse(parser, token_pair.token_type, token_pair.token, &builder);
    }
    Parse(parser, 0, nullptr, &builder);

    // Clean up the tokens
    for(auto token_pair : tokens)
    {
        delete token_pair.token;
    }

    if(builder.statement == nullptr || !builder.valid)
    {
        printf("Parse failed!\n");
        return ParseResult(ResultStatus::FAILURE_SYNTAX, nullptr);
    }
    else
    {
        setupStatement(builder.statement, builder.expr, data_store);

        // Assume the expression tree was consumed, clean it up
        delete builder.expr;
        builder.expr = nullptr;

        return ParseResult(ResultStatus::SUCCESS, builder.statement);
    }

    // Shouldn't be hit unless some weird stuff happens
    return ParseResult(ResultStatus::FAILURE, nullptr);
}

void token_print(Token token)
{
    printf("Token: %s\n", token->text.c_str());
}

/**
 *  \brief Tokenizes the given string in the context of SQL syntax
 *
 *  \detail Memory allocation failure will abort the tokenization
 *  and return the current list of tokens. Further allocations should
 *  cause further processing of the statement to fail and those error
 *  messages should propagate up to the client.
 */
std::vector<TokenPair> tokenize(std::string input)
{
  std::string::iterator start_token = input.end();
  std::string::iterator end_token = input.end();

  std::string::iterator itr = input.begin();

  std::vector<TokenPair> tokens;
  tokens.reserve(16);

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

      std::string token(itr, next);

      pair.token = new (std::nothrow) TokenData(token);
      if(pair.token == nullptr)
      {
          return tokens;
      }

      if(keywords.find(pair.token->text) != keywords.end())
      {
        pair.token_type = keywords[pair.token->text];

        ExpressionOperation op = getOperation(pair.token->text);
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
            std::string::iterator next = itr + 1;

            while(next != input.end() && (isSQLNumericChar(*next) || *next == 'x'))
            {
                next++;
            } 
            
            switch(*next)
            {
                case '.':
                    {
                        pair.token_type = TK_FLOAT;
                        auto test = next + 1;
                        if(test == input.end() || !isSQLNumericChar(*test))
                        {
                            // Not a float, is an integer followed by a dot
                            // TODO: Generate two tokens
                        }
                        else
                        {
                            while(next != input.end() && isSQLNumericChar(*next)) { next++; }
                            std::string token(itr, next);
                            float value = std::stof(token);
                            pair.token = new (std::nothrow) TokenData(token, value);
                        }
                    }
                    break; 
                case ':':
                    {
                        pair.token_type = TK_TIME;
                        pair.token = nullptr;
                        
                        // TODO
                    }
                    break;
                case '-': 
                    {
                        pair.token_type = TK_DATE; 
                        auto test = next;
                        if(safe_advance(test, input.end(), 5)) 
                        {
                            if(checkDateFormat(std::string(itr, test)))
                            {
                                // Make a token
                                pair.token_type = TK_DATE;
                                std::string token(itr, test);
                                pair.token = new (std::nothrow) TokenData(token);
                                pair.token->is_value = true;
                                TervelData data = { .value = 0 };
                                DateData date = { .value = 0 };

                                date.year = std::stoul(token.substr(0, 4), nullptr, 10);
                                date.month = std::stoul(token.substr(5, 2), nullptr, 10);
                                date.day = std::stoul(token.substr(8, 2), nullptr, 10); 
                                
                                data.data.value = date.value;

                                pair.token->value = data;
                            }
                            else
                            {
                                pair.token_type = TK_ILLEGAL;
                                pair.token = new (std::nothrow) TokenData();
                            }
                        }
                        else
                        {
                            pair.token_type = TK_ILLEGAL;
                            pair.token = new (std::nothrow) TokenData();
                        } 
                    }
                    break;
                default:
                    {
                        // End token
                        pair.token_type = TK_INTEGER;
                        std::string token(itr, next); 
                        
                        int64_t value = 0;
                        if(token.find('x') != std::string::npos)
                        {
                            value = std::stoll(token, nullptr, 16);
                        }
                        else
                        {
                            value = std::stoll(token, nullptr, 10);
                        }
                        
                        pair.token = new (std::nothrow) TokenData(token, value); 
                    }
                    break;
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
                  pair.token->text = std::string("(");
                  pair.token_type = TK_LPAREN;
                  break;
              case ')':
                  pair.token->text = std::string(")");
                  pair.token_type = TK_RPAREN;
                  break;
              case ';':
                  pair.token->text = std::string(";");
                  pair.token_type = TK_SEMICOLON;
                  break;
              case ',':
                  pair.token->text = std::string(",");
                  pair.token_type = TK_COMMA;
                  break;
              case '.':
                  pair.token->text = std::string(".");
                  pair.token_type = TK_DOT;
                  break;
              case '*':
                  pair.token->text = std::string("*");
                  pair.token_type = TK_ASTERISK;
                  break;
              case '=':
                  pair.token->text = std::string("=");
                  pair.token->is_operation = true;
                  pair.token->operation = static_cast<uint16_t>(ExpressionOperation::EQUALS);
                  pair.token_type = TK_EQ;
                  break;
              case '!':
                  if(*(itr + 1) == '=')
                  {
                      pair.token->text = std::string("!=");
                      pair.token_type = TK_NE;
                      
                      pair.token->is_operation = true;
                      pair.token->operation = static_cast<uint16_t>(ExpressionOperation::NOT_EQUALS);
                  }
                  else
                  {
                      pair.token->text = std::string("!");
                      pair.token_type = TK_NOT;
                  }
                  break;
              case '<':
                  if(*(itr + 1) == '=')
                  {
                      pair.token->text = std::string("<=");
                      pair.token_type = TK_LE;

                      pair.token->is_operation = true;
                      pair.token->operation = static_cast<uint16_t>(ExpressionOperation::LESSER_EQUALS);
                  }
                  else
                  {
                      pair.token->text = std::string("<");
                      pair.token_type = TK_LT;

                      pair.token->is_operation = true;
                      pair.token->operation = static_cast<uint16_t>(ExpressionOperation::LESSER);
                  }
                  break;
              case '>':
                  if(*(itr + 1) == '=')
                  {
                      pair.token->text = std::string(">=");
                      pair.token_type = TK_GE;

                      pair.token->is_operation = true;
                      pair.token->operation = static_cast<uint16_t>(ExpressionOperation::GREATER_EQUALS);
                  }
                  else
                  {
                      pair.token->text = std::string(">");
                      pair.token_type = TK_GT;

                      pair.token->is_operation = true;
                      pair.token->operation = static_cast<uint16_t>(ExpressionOperation::GREATER);
                  }
                  break;
              default:
                  pair.token->text = std::string("ILLEGAL");
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

/**
 *  \brief Sets up the token mappings used by the tokenizer and parser
 *
 *  \detail Each worker thread must call this function before calling \refer parse.
 */
void setupTokenMappings()
{
    // Update as tokens are added
    keywords["CREATE"] = TK_CREATE;
    keywords["DROP"] = TK_DROP;
    keywords["SELECT"] = TK_SELECT;
    keywords["UPDATE"] = TK_UPDATE;
    keywords["INSERT"] = TK_INSERT;
    keywords["INTO"] = TK_INTO;
    keywords["DELETE"] = TK_DELETE;

    keywords["TABLE"] = TK_TABLE;
    keywords["AS"] = TK_AS;
    keywords["WHERE"] = TK_WHERE;
    keywords["FROM"] = TK_FROM;
    keywords["ALL"] = TK_ALL;
    keywords["DISTINCT"] = TK_DISTINCT;
    //keywords["UNIQUE"] = TK_UNIQUE;
    keywords["AUTO_INCREMENT"] = TK_AUTO_INCREMENT;
    keywords["NOT"] = TK_NOT;
    keywords["NULL"] = TK_NULL;
    keywords["DEFAULT"] = TK_DEFAULT;
    keywords["AND"] = TK_AND;
    keywords["OR"] = TK_OR;
    keywords["VALUES"] = TK_VALUES;
    keywords["SET"] = TK_SET;
}
