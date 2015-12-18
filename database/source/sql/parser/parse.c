/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
**
** This version of "lempar.c" is modified, slightly, for use by SQLite.
** The only modifications are the addition of a couple of NEVER()
** macros to disable tests that are needed in the case of a general
** LALR(1) grammar but which are always false in the
** specific grammar used by SQLite.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 3 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
#include <sql/omdb_parser.h>
#line 16 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 65
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE Token
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  StatementBuilder* builder;
#define ParseARG_PDECL , StatementBuilder* builder
#define ParseARG_FETCH  StatementBuilder* builder = yypParser->builder
#define ParseARG_STORE yypParser->builder = builder
#define YYNSTATE             40
#define YYNRULE              51
#define YY_MAX_SHIFT         39
#define YY_MIN_SHIFTREDUCE   78
#define YY_MAX_SHIFTREDUCE   128
#define YY_MIN_REDUCE        129
#define YY_MAX_REDUCE        179
#define YY_ERROR_ACTION      180
#define YY_ACCEPT_ACTION     181
#define YY_NO_ACTION         182

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE

**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
#define YY_ACTTAB_COUNT (113)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   100,   25,   25,   26,   26,   26,   26,   81,   31,   27,
 /*    10 */    27,   92,   93,   25,   25,   26,   26,   26,   26,  125,
 /*    20 */   127,   27,   27,   92,   93,   25,   25,   26,   26,   26,
 /*    30 */    26,  181,   10,   80,    2,   91,   34,   89,    5,   20,
 /*    40 */    38,   37,  104,  105,   35,  116,   26,   26,   26,   26,
 /*    50 */    97,   97,   39,   84,   23,   21,   16,   13,  115,   91,
 /*    60 */   172,   88,    5,   36,   17,   29,   35,   95,   96,  129,
 /*    70 */    81,   79,    2,   14,  122,   98,  123,    8,   22,    9,
 /*    80 */   114,  112,  112,  118,   28,  124,   24,  102,  111,   15,
 /*    90 */    87,    6,  110,    4,   86,   15,  101,    1,   85,  117,
 /*   100 */    33,   11,  107,    7,    3,  120,   12,   32,   18,   30,
 /*   110 */    19,  103,   82,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     5,   18,   19,   20,   21,   22,   23,    1,   51,   14,
 /*    10 */    15,    7,    8,   18,   19,   20,   21,   22,   23,   62,
 /*    20 */    63,   14,   15,    7,    8,   18,   19,   20,   21,   22,
 /*    30 */    23,   34,   35,   36,   37,   42,   43,   44,   45,    9,
 /*    40 */    10,    2,   12,   13,   42,   29,   20,   21,   22,   23,
 /*    50 */     7,    8,   38,   39,   40,   42,   54,   42,   56,   42,
 /*    60 */     1,   44,   45,   24,   25,   46,   42,   48,   49,    0,
 /*    70 */     1,   36,   37,   58,   61,   50,   61,    6,   42,    6,
 /*    80 */    56,   26,   27,   42,    4,   57,   59,    7,   55,   30,
 /*    90 */     5,    6,   42,   53,   42,   30,   52,    4,   41,   57,
 /*   100 */    51,   47,   51,   32,   31,   60,   28,   51,    3,   51,
 /*   110 */     3,   11,    1,
};
#define YY_SHIFT_USE_DFLT (-18)
#define YY_SHIFT_COUNT (39)
#define YY_SHIFT_MIN   (-17)
#define YY_SHIFT_MAX   (111)
static const signed char yy_shift_ofst[] = {
 /*     0 */     6,    4,   39,    4,    4,   43,    4,  -18,    4,    4,
 /*    10 */    69,   30,   16,   59,   71,    4,   73,   55,    4,    4,
 /*    20 */    80,   65,   65,   93,  -18,  -18,  -18,  -18,  -18,  -18,
 /*    30 */    -5,    7,  -17,   26,   85,   78,  105,  107,  100,  111,
};
#define YY_REDUCE_USE_DFLT (-44)
#define YY_REDUCE_COUNT (29)
#define YY_REDUCE_MIN   (-43)
#define YY_REDUCE_MAX   (58)
static const signed char yy_reduce_ofst[] = {
 /*     0 */    -3,   -7,   14,   15,    2,   19,   17,  -43,   13,   24,
 /*    10 */    35,   25,   36,   28,   27,   41,   33,   40,   50,   52,
 /*    20 */    44,   28,   42,   57,   45,   49,   51,   56,   58,   54,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   134,  180,  180,  180,  180,  145,  180,  180,  180,  180,
 /*    10 */   134,  141,  180,  170,  180,  180,  180,  164,  180,  180,
 /*    20 */   160,  170,  170,  180,  177,  180,  180,  180,  180,  150,
 /*    30 */   180,  179,  157,  159,  180,  180,  180,  180,  180,  180,
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "SEMICOLON",     "CREATE",        "TABLE",       
  "LPAREN",        "RPAREN",        "COMMA",         "ID",          
  "STRING",        "DEFAULT",       "NOT",           "NULL",        
  "UNIQUE",        "AUTO_INCREMENT",  "OR",            "AND",         
  "BETWEEN",       "IN",            "NE",            "EQ",          
  "GT",            "LE",            "LT",            "GE",          
  "DROP",          "SELECT",        "DISTINCT",      "ALL",         
  "DOT",           "ASTERISK",      "AS",            "FROM",        
  "WHERE",         "error",         "input",         "cmd_list",    
  "end_cmd",       "explain",       "cmdx",          "cmd",         
  "create_table",  "create_table_args",  "name",          "column_list", 
  "column",        "column_id",     "type",          "column_args", 
  "typetoken",     "typename",      "column_constraints",  "expr",        
  "term",          "set_quantifier",  "select_columns",  "select_table",
  "select_column",  "as_clause",     "table_references",  "where_clause",
  "group_by_clause",  "table_reference",  "search_condition",  "comparison_predicate",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "input ::= cmd_list",
 /*   1 */ "cmd_list ::= cmd_list end_cmd",
 /*   2 */ "cmd_list ::= end_cmd",
 /*   3 */ "end_cmd ::= SEMICOLON",
 /*   4 */ "end_cmd ::= explain cmdx SEMICOLON",
 /*   5 */ "explain ::=",
 /*   6 */ "cmdx ::= cmd",
 /*   7 */ "cmd ::= create_table create_table_args",
 /*   8 */ "create_table ::= CREATE TABLE name",
 /*   9 */ "create_table_args ::= LPAREN column_list RPAREN",
 /*  10 */ "column_list ::= column_list COMMA column",
 /*  11 */ "column_list ::= column",
 /*  12 */ "column ::= column_id type column_args",
 /*  13 */ "column_id ::= name",
 /*  14 */ "name ::= ID",
 /*  15 */ "name ::= STRING",
 /*  16 */ "type ::=",
 /*  17 */ "type ::= typetoken",
 /*  18 */ "typetoken ::= typename",
 /*  19 */ "typename ::= ID|STRING",
 /*  20 */ "column_args ::= column_args column_constraints",
 /*  21 */ "column_args ::=",
 /*  22 */ "column_constraints ::= DEFAULT LPAREN expr RPAREN",
 /*  23 */ "column_constraints ::= DEFAULT term",
 /*  24 */ "column_constraints ::= DEFAULT ID",
 /*  25 */ "column_constraints ::= NOT NULL",
 /*  26 */ "column_constraints ::= UNIQUE",
 /*  27 */ "column_constraints ::= AUTO_INCREMENT",
 /*  28 */ "expr ::= expr AND|OR expr",
 /*  29 */ "expr ::= expr LT|GT|GE|LE expr",
 /*  30 */ "expr ::= expr EQ|NE expr",
 /*  31 */ "term ::=",
 /*  32 */ "cmd ::= DROP TABLE name",
 /*  33 */ "cmd ::= SELECT set_quantifier select_columns select_table",
 /*  34 */ "set_quantifier ::= DISTINCT|ALL",
 /*  35 */ "set_quantifier ::=",
 /*  36 */ "select_columns ::= select_columns COMMA select_column",
 /*  37 */ "select_columns ::= select_column",
 /*  38 */ "select_column ::= name DOT ASTERISK",
 /*  39 */ "select_column ::= name DOT name as_clause",
 /*  40 */ "as_clause ::= AS name",
 /*  41 */ "as_clause ::=",
 /*  42 */ "select_table ::= FROM table_references where_clause group_by_clause",
 /*  43 */ "select_table ::= FROM name",
 /*  44 */ "table_references ::= table_references COMMA table_reference",
 /*  45 */ "table_references ::= table_reference",
 /*  46 */ "table_reference ::= name as_clause",
 /*  47 */ "where_clause ::= WHERE search_condition",
 /*  48 */ "group_by_clause ::=",
 /*  49 */ "search_condition ::= comparison_predicate",
 /*  50 */ "comparison_predicate ::= expr",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(u64)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (u64)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  /* There is no mechanism by which the parser stack can be popped below
  ** empty in SQLite.  */
  assert( pParser->yyidx>=0 );
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  /* In SQLite, we never try to destroy a parser that was not successfully
  ** created in the first place. */
  if( NEVER(pParser==0) ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ) return yy_default[stateno];
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    int i;
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
      fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
      for(i=1; i<=yypParser->yyidx; i++)
        fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
      fprintf(yyTraceFILE,"\n");
    }else{
      fprintf(yyTraceFILE,"%sShift *\n",yyTracePrompt);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.  Return the number of errors.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 34, 1 },
  { 35, 2 },
  { 35, 1 },
  { 36, 1 },
  { 36, 3 },
  { 37, 0 },
  { 38, 1 },
  { 39, 2 },
  { 40, 3 },
  { 41, 3 },
  { 43, 3 },
  { 43, 1 },
  { 44, 3 },
  { 45, 1 },
  { 42, 1 },
  { 42, 1 },
  { 46, 0 },
  { 46, 1 },
  { 48, 1 },
  { 49, 1 },
  { 47, 2 },
  { 47, 0 },
  { 50, 4 },
  { 50, 2 },
  { 50, 2 },
  { 50, 2 },
  { 50, 1 },
  { 50, 1 },
  { 51, 3 },
  { 51, 3 },
  { 51, 3 },
  { 52, 0 },
  { 39, 3 },
  { 39, 4 },
  { 53, 1 },
  { 53, 0 },
  { 54, 3 },
  { 54, 1 },
  { 56, 3 },
  { 56, 4 },
  { 57, 2 },
  { 57, 0 },
  { 55, 4 },
  { 55, 2 },
  { 58, 3 },
  { 58, 1 },
  { 61, 2 },
  { 59, 2 },
  { 60, 0 },
  { 62, 1 },
  { 63, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s] -> state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[-yysize].stateno);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 3: /* end_cmd ::= SEMICOLON */
#line 35 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("semicolon found\n");}
#line 829 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 4: /* end_cmd ::= explain cmdx SEMICOLON */
#line 36 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("command found and semicolon found\n");}
#line 834 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 5: /* explain ::= */
#line 38 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("explain\n"); }
#line 839 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 8: /* create_table ::= CREATE TABLE name */
#line 46 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{builderStartCreateTable(builder, yymsp[0].minor.yy0);}
#line 844 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 12: /* column ::= column_id type column_args */
#line 53 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[-2].minor.yy0; builderAddColumnName(builder, yymsp[-2].minor.yy0);}
#line 849 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 13: /* column_id ::= name */
      case 18: /* typetoken ::= typename */ yytestcase(yyruleno==18);
#line 55 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 855 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 14: /* name ::= ID */
      case 15: /* name ::= STRING */ yytestcase(yyruleno==15);
#line 61 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0; token_print(yymsp[0].minor.yy0);}
#line 861 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 17: /* type ::= typetoken */
#line 66 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yymsp[0].minor.yy0;}
#line 866 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 19: /* typename ::= ID|STRING */
#line 71 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0; builderAddColumnType(builder, yymsp[0].minor.yy0);}
#line 871 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 22: /* column_constraints ::= DEFAULT LPAREN expr RPAREN */
#line 76 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yymsp[-1].minor.yy0; printf("col_const\n");}
#line 876 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 23: /* column_constraints ::= DEFAULT term */
      case 24: /* column_constraints ::= DEFAULT ID */ yytestcase(yyruleno==24);
#line 77 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yymsp[0].minor.yy0; printf("col_const\n");}
#line 882 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 25: /* column_constraints ::= NOT NULL */
#line 80 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{printf("Token: NOT NULL\n");}
#line 887 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 26: /* column_constraints ::= UNIQUE */
#line 81 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{printf("Token: UNIQUE\n");}
#line 892 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 27: /* column_constraints ::= AUTO_INCREMENT */
#line 82 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{printf("Token: AUTO_INCREMENT\n");}
#line 897 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 28: /* expr ::= expr AND|OR expr */
      case 29: /* expr ::= expr LT|GT|GE|LE expr */ yytestcase(yyruleno==29);
      case 30: /* expr ::= expr EQ|NE expr */ yytestcase(yyruleno==30);
#line 91 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0; builderAddExpressions(builder, yymsp[-2].minor.yy0, yymsp[0].minor.yy0, yymsp[-1].minor.yy0); }
#line 904 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 31: /* term ::= */
#line 95 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0; printf("term\n");}
#line 909 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 32: /* cmd ::= DROP TABLE name */
#line 99 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{builderStartDropTable(builder, yymsp[0].minor.yy0);}
#line 914 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 38: /* select_column ::= name DOT ASTERISK */
#line 111 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddSelectAllColumns(builder, yymsp[-2].minor.yy0); }
#line 919 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 39: /* select_column ::= name DOT name as_clause */
#line 112 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  builderAddQualifiedSelectColumn(builder, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 926 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 40: /* as_clause ::= AS name */
#line 116 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 931 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 41: /* as_clause ::= */
#line 117 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = nullptr; }
#line 936 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 43: /* select_table ::= FROM name */
#line 120 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yymsp[0].minor.yy0; }
#line 941 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 46: /* table_reference ::= name as_clause */
#line 126 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yymsp[-1].minor.yy0; yymsp[0].minor.yy0;}
#line 946 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 50: /* comparison_predicate ::= expr */
#line 135 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddSelectPredicate(builder, yymsp[0].minor.yy0); }
#line 951 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      default:
      /* (0) input ::= cmd_list */ yytestcase(yyruleno==0);
      /* (1) cmd_list ::= cmd_list end_cmd */ yytestcase(yyruleno==1);
      /* (2) cmd_list ::= end_cmd */ yytestcase(yyruleno==2);
      /* (6) cmdx ::= cmd */ yytestcase(yyruleno==6);
      /* (7) cmd ::= create_table create_table_args */ yytestcase(yyruleno==7);
      /* (9) create_table_args ::= LPAREN column_list RPAREN */ yytestcase(yyruleno==9);
      /* (10) column_list ::= column_list COMMA column */ yytestcase(yyruleno==10);
      /* (11) column_list ::= column */ yytestcase(yyruleno==11);
      /* (16) type ::= */ yytestcase(yyruleno==16);
      /* (20) column_args ::= column_args column_constraints */ yytestcase(yyruleno==20);
      /* (21) column_args ::= */ yytestcase(yyruleno==21);
      /* (33) cmd ::= SELECT set_quantifier select_columns select_table */ yytestcase(yyruleno==33);
      /* (34) set_quantifier ::= DISTINCT|ALL */ yytestcase(yyruleno==34);
      /* (35) set_quantifier ::= */ yytestcase(yyruleno==35);
      /* (36) select_columns ::= select_columns COMMA select_column */ yytestcase(yyruleno==36);
      /* (37) select_columns ::= select_column */ yytestcase(yyruleno==37);
      /* (42) select_table ::= FROM table_references where_clause group_by_clause */ yytestcase(yyruleno==42);
      /* (44) table_references ::= table_references COMMA table_reference */ yytestcase(yyruleno==44);
      /* (45) table_references ::= table_reference */ yytestcase(yyruleno==45);
      /* (47) where_clause ::= WHERE search_condition */ yytestcase(yyruleno==47);
      /* (48) group_by_clause ::= */ yytestcase(yyruleno==48);
      /* (49) search_condition ::= comparison_predicate */ yytestcase(yyruleno==49);
        break;
  };
  assert( yyruleno>=0 && yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact <= YY_MAX_SHIFTREDUCE ){
    if( yyact>YY_MAX_SHIFT ) yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
    /* If the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
      yyTraceShift(yypParser, yyact);
    }else{
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YY_ACCEPT_ACTION );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
#line 21 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  printf("Parse failed!\n");
  builderClean(builder);
#line 1025 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 10 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  printf("Syntax Error!\n");
  int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
  for(int i = 0; i < n; ++i) {
    int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
    if (a < YYNSTATE + YYNRULE) {
      printf("possible token: %s\n", yyTokenName[i]);
    }
  }
#line 1050 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
#line 26 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  printf("Parse accepts input!\n");
#line 1072 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      if( yyact > YY_MAX_SHIFT ) yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sReturn\n",yyTracePrompt);
  }
#endif
  return;
}
