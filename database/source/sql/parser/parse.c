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
#line 22 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
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
#define YYNOCODE 83
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
#define YYNSTATE             55
#define YYNRULE              61
#define YY_MAX_SHIFT         54
#define YY_MIN_SHIFTREDUCE   105
#define YY_MAX_SHIFTREDUCE   165
#define YY_MIN_REDUCE        166
#define YY_MAX_REDUCE        226
#define YY_ERROR_ACTION      227
#define YY_ACCEPT_ACTION     228
#define YY_NO_ACTION         229

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
#define YY_ACTTAB_COUNT (141)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   119,  120,   51,   53,   39,  162,  133,   54,  111,   35,
 /*    10 */   163,  108,   40,   41,  228,   20,  107,    7,   25,  134,
 /*    20 */   134,  134,  134,  151,  153,  163,  163,  163,  163,  161,
 /*    30 */    50,   29,   53,   34,   26,  133,  118,  148,   16,   47,
 /*    40 */    46,   40,  130,   43,  118,   42,  116,   11,   53,   53,
 /*    50 */    53,  133,  133,  133,   49,  119,  120,  158,  131,  127,
 /*    60 */   157,   38,  165,  122,  123,    8,   28,  118,  140,  115,
 /*    70 */    11,    6,   52,  160,  129,  119,  120,  124,  124,  156,
 /*    80 */    33,   49,  141,  207,  118,  166,  108,  132,  106,    7,
 /*    90 */    32,  125,   17,  155,   18,  139,    1,  149,   45,  147,
 /*   100 */   143,   48,   37,  137,  137,  159,   21,   10,    5,    5,
 /*   110 */     2,    2,  114,   12,  135,  136,  113,   27,    1,   27,
 /*   120 */   164,  142,    3,    1,    9,  112,  145,   24,   14,   13,
 /*   130 */    23,    4,   44,   36,   15,   22,   30,   31,  128,   19,
 /*   140 */   109,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     7,    8,    2,   53,   79,   80,   56,   49,   50,   51,
 /*    10 */    11,    1,   62,   63,   45,   46,   47,   48,   53,   26,
 /*    20 */    27,   28,   29,   73,   74,   26,   27,   28,   29,   80,
 /*    30 */    30,   31,   53,   53,   69,   56,   53,   72,   38,   56,
 /*    40 */    40,   62,   63,   43,   53,   54,   55,   56,   53,   53,
 /*    50 */    53,   56,   56,   56,   53,    7,    8,   62,   62,   62,
 /*    60 */    77,   57,   53,   59,   60,    6,   65,   53,   67,   55,
 /*    70 */    56,    9,   10,   53,   12,    7,    8,    7,    8,   53,
 /*    80 */    53,   53,   34,    1,   53,    0,    1,   56,   47,   48,
 /*    90 */    81,   61,    6,   70,    6,   67,   37,   68,   78,   72,
 /*   100 */    53,   75,   70,   32,   33,    5,    6,   64,   19,   20,
 /*   110 */    13,   14,    5,    6,   53,   66,   53,   35,   37,   35,
 /*   120 */    70,   68,    4,   37,   36,   52,   71,   76,   36,    4,
 /*   130 */    58,   20,   42,   39,   41,   25,    3,    3,   11,   25,
 /*   140 */     1,
};
#define YY_SHIFT_USE_DFLT (-8)
#define YY_SHIFT_COUNT (54)
#define YY_SHIFT_MIN   (-7)
#define YY_SHIFT_MAX   (139)
static const short yy_shift_ofst[] = {
 /*     0 */    10,   -7,   -7,   68,   -7,   -7,   -7,    0,   68,   68,
 /*    10 */    68,   70,   68,   -1,   68,   68,   68,   68,   68,   68,
 /*    20 */    85,   -1,   48,   62,   59,   82,   86,   68,   88,   71,
 /*    30 */    68,   68,   81,   84,   84,  118,   -8,   -8,   -8,  100,
 /*    40 */    89,   97,  107,   92,  125,   90,   93,  111,   94,  110,
 /*    50 */   133,  134,  127,  114,  139,
};
#define YY_REDUCE_USE_DFLT (-76)
#define YY_REDUCE_COUNT (38)
#define YY_REDUCE_MIN   (-75)
#define YY_REDUCE_MAX   (73)
static const signed char yy_reduce_ofst[] = {
 /*     0 */   -31,  -50,  -21,   -9,   -5,   -4,   -3,  -42,  -17,  -35,
 /*    10 */     1,    4,   14,  -75,    9,   20,   26,   27,   28,   31,
 /*    20 */    41,  -51,  -20,   30,   23,   29,   32,   47,   49,   43,
 /*    30 */    61,   63,   50,   29,   53,   73,   51,   55,   72,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   171,  227,  227,  227,  227,  227,  227,  227,  227,  211,
 /*    10 */   227,  182,  227,  227,  227,  227,  227,  211,  227,  227,
 /*    20 */   171,  227,  227,  178,  227,  205,  227,  227,  227,  199,
 /*    30 */   227,  227,  227,  205,  205,  227,  227,  213,  187,  227,
 /*    40 */   227,  215,  227,  227,  227,  227,  227,  227,  227,  227,
 /*    50 */   227,  227,  227,  179,  227,
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
  "AUTO_INCREMENT",  "OR",            "AND",           "BETWEEN",     
  "IN",            "ISNULL",        "NOTNULL",       "NE",          
  "EQ",            "GT",            "LE",            "LT",          
  "GE",            "DOT",           "INTEGER",       "FLOAT",       
  "DATE",          "TIME",          "DROP",          "SELECT",      
  "DISTINCT",      "ALL",           "ASTERISK",      "AS",          
  "FROM",          "WHERE",         "UPDATE",        "SET",         
  "INSERT",        "INTO",          "VALUES",        "DELETE",      
  "error",         "input",         "cmd_list",      "end_cmd",     
  "explain",       "cmdx",          "cmd",           "create_table",
  "create_table_args",  "name",          "column_list",   "column",      
  "column_id",     "type",          "column_args",   "typetoken",   
  "typename",      "column_constraints",  "term",          "expr",        
  "set_quantifier",  "select_columns",  "select_table",  "select_column",
  "as_clause",     "table_references",  "where_clause",  "group_by_clause",
  "table_reference",  "search_condition",  "comparison_predicate",  "update_table",
  "update_expr_list",  "update_expr",   "insert_table",  "insert_values",
  "insert_term",   "delete_table",
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
 /*  22 */ "column_constraints ::= DEFAULT term",
 /*  23 */ "column_constraints ::= NOT NULL",
 /*  24 */ "column_constraints ::= AUTO_INCREMENT",
 /*  25 */ "expr ::= expr AND|OR expr",
 /*  26 */ "expr ::= term NE|EQ term",
 /*  27 */ "term ::= name DOT column_id",
 /*  28 */ "term ::= column_id",
 /*  29 */ "term ::= INTEGER|FLOAT|DATE|TIME",
 /*  30 */ "cmd ::= DROP TABLE name",
 /*  31 */ "cmd ::= SELECT set_quantifier select_columns select_table",
 /*  32 */ "set_quantifier ::= DISTINCT|ALL",
 /*  33 */ "set_quantifier ::=",
 /*  34 */ "select_columns ::= select_columns COMMA select_column",
 /*  35 */ "select_columns ::= select_column",
 /*  36 */ "select_column ::= name DOT ASTERISK",
 /*  37 */ "select_column ::= name DOT name as_clause",
 /*  38 */ "as_clause ::= AS name",
 /*  39 */ "as_clause ::=",
 /*  40 */ "select_table ::= FROM table_references where_clause group_by_clause",
 /*  41 */ "select_table ::= FROM name",
 /*  42 */ "table_references ::= table_references COMMA table_reference",
 /*  43 */ "table_references ::= table_reference",
 /*  44 */ "table_reference ::= name as_clause",
 /*  45 */ "table_reference ::=",
 /*  46 */ "where_clause ::= WHERE search_condition",
 /*  47 */ "group_by_clause ::=",
 /*  48 */ "search_condition ::= comparison_predicate",
 /*  49 */ "comparison_predicate ::= expr",
 /*  50 */ "cmd ::= UPDATE update_table SET update_expr_list where_clause",
 /*  51 */ "update_table ::= name",
 /*  52 */ "update_expr_list ::= update_expr_list COMMA update_expr",
 /*  53 */ "update_expr ::= column_id EQ term",
 /*  54 */ "cmd ::= INSERT INTO insert_table VALUES LPAREN insert_values RPAREN",
 /*  55 */ "insert_table ::= name",
 /*  56 */ "insert_values ::= insert_values COMMA insert_term",
 /*  57 */ "insert_values ::= insert_term",
 /*  58 */ "insert_term ::= INTEGER|FLOAT|DATE|TIME|NULL",
 /*  59 */ "cmd ::= DELETE FROM delete_table where_clause",
 /*  60 */ "delete_table ::= name",
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
  { 45, 1 },
  { 46, 2 },
  { 46, 1 },
  { 47, 1 },
  { 47, 3 },
  { 48, 0 },
  { 49, 1 },
  { 50, 2 },
  { 51, 3 },
  { 52, 3 },
  { 54, 3 },
  { 54, 1 },
  { 55, 3 },
  { 56, 1 },
  { 53, 1 },
  { 53, 1 },
  { 57, 0 },
  { 57, 1 },
  { 59, 1 },
  { 60, 1 },
  { 58, 2 },
  { 58, 0 },
  { 61, 2 },
  { 61, 2 },
  { 61, 1 },
  { 63, 3 },
  { 63, 3 },
  { 62, 3 },
  { 62, 1 },
  { 62, 1 },
  { 50, 3 },
  { 50, 4 },
  { 64, 1 },
  { 64, 0 },
  { 65, 3 },
  { 65, 1 },
  { 67, 3 },
  { 67, 4 },
  { 68, 2 },
  { 68, 0 },
  { 66, 4 },
  { 66, 2 },
  { 69, 3 },
  { 69, 1 },
  { 72, 2 },
  { 72, 0 },
  { 70, 2 },
  { 71, 0 },
  { 73, 1 },
  { 74, 1 },
  { 50, 5 },
  { 75, 1 },
  { 76, 3 },
  { 77, 3 },
  { 50, 7 },
  { 78, 1 },
  { 79, 3 },
  { 79, 1 },
  { 80, 1 },
  { 50, 4 },
  { 81, 1 },
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
      case 5: /* explain ::= */
      case 49: /* comparison_predicate ::= expr */ yytestcase(yyruleno==49);
#line 55 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ }
#line 866 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 8: /* create_table ::= CREATE TABLE name */
#line 63 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{builderStartCreateTable(builder, yymsp[0].minor.yy0);}
#line 871 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 12: /* column ::= column_id type column_args */
#line 71 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  (void)yygotominor.yy0;
  builderAddColumn(builder, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 879 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 13: /* column_id ::= name */
      case 14: /* name ::= ID */ yytestcase(yyruleno==14);
      case 15: /* name ::= STRING */ yytestcase(yyruleno==15);
      case 17: /* type ::= typetoken */ yytestcase(yyruleno==17);
      case 18: /* typetoken ::= typename */ yytestcase(yyruleno==18);
      case 19: /* typename ::= ID|STRING */ yytestcase(yyruleno==19);
#line 76 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 889 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 16: /* type ::= */
      case 39: /* as_clause ::= */ yytestcase(yyruleno==39);
#line 86 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = nullptr; }
#line 895 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 22: /* column_constraints ::= DEFAULT term */
#line 98 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  builderAddColumnConstraint(builder, SQLConstraintType::DEFAULT, yymsp[0].minor.yy0);
}
#line 902 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 23: /* column_constraints ::= NOT NULL */
#line 103 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  builderAddColumnConstraint(builder, SQLConstraintType::NOT_NULL);
}
#line 909 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 24: /* column_constraints ::= AUTO_INCREMENT */
#line 108 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  builderAddColumnConstraint(builder, SQLConstraintType::AUTO_INCREMENT);
}
#line 916 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 25: /* expr ::= expr AND|OR expr */
#line 119 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderStartNestedExpr(builder, yymsp[-1].minor.yy0);}
#line 921 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 26: /* expr ::= term NE|EQ term */
#line 120 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddValueExpr(builder, yymsp[-1].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 926 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 27: /* term ::= name DOT column_id */
#line 122 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
    yymsp[-2].minor.yy0->table_name = yymsp[-2].minor.yy0->text;
    yymsp[-2].minor.yy0->column_name = yymsp[0].minor.yy0->text;
    yymsp[-2].minor.yy0->text = yymsp[-2].minor.yy0->text + "." + yymsp[0].minor.yy0->text;
    //printf("Reference: %s.%s\n", yymsp[-2].minor.yy0->table_name.c_str(), yymsp[-2].minor.yy0->column_name.c_str());
    yymsp[-2].minor.yy0->is_column = true;
    yygotominor.yy0 = yymsp[-2].minor.yy0;
}
#line 938 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 28: /* term ::= column_id */
#line 130 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; /*printf("expression term\n");*/ }
#line 943 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 29: /* term ::= INTEGER|FLOAT|DATE|TIME */
#line 132 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
  //printf("Numeric term handling\n");
  yygotominor.yy0 = yymsp[0].minor.yy0;
}
#line 951 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 30: /* cmd ::= DROP TABLE name */
#line 139 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{builderStartDropTable(builder, yymsp[0].minor.yy0);}
#line 956 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 36: /* select_column ::= name DOT ASTERISK */
#line 151 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddSelectAllColumns(builder, yymsp[-2].minor.yy0); }
#line 961 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 37: /* select_column ::= name DOT name as_clause */
#line 152 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  builderAddQualifiedSelectColumn(builder, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 968 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 38: /* as_clause ::= AS name */
#line 156 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 973 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 41: /* select_table ::= FROM name */
      case 42: /* table_references ::= table_references COMMA table_reference */ yytestcase(yyruleno==42);
      case 43: /* table_references ::= table_reference */ yytestcase(yyruleno==43);
#line 160 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ (void)yymsp[0].minor.yy0; }
#line 980 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 44: /* table_reference ::= name as_clause */
#line 166 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  (void)yygotominor.yy0;
  (void)yymsp[-1].minor.yy0; 
  (void)yymsp[0].minor.yy0; 
  //printf("Table reference: %s as %s\n", yymsp[-1].minor.yy0->text->c_str(), yymsp[0].minor.yy0->text->c_str());
}
#line 990 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 51: /* update_table ::= name */
#line 190 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  // TODO: update builder functions
}
#line 998 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 53: /* update_expr ::= column_id EQ term */
#line 196 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddUpdateExpr(builder, yymsp[-1].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1003 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 55: /* insert_table ::= name */
#line 203 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
  yygotominor.yy0 = yymsp[0].minor.yy0; 
  builderStartInsertCommand(builder);
  builderAddTableName(builder, yygotominor.yy0);
  //printf("Starting INSERT INTO statement parse\n");
}
#line 1013 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 58: /* insert_term ::= INTEGER|FLOAT|DATE|TIME|NULL */
#line 214 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  builderAddDataItem(builder, yymsp[0].minor.yy0);
}
#line 1021 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 60: /* delete_table ::= name */
#line 224 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  // TODO: Update builder functions
}
#line 1029 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      default:
      /* (0) input ::= cmd_list */ yytestcase(yyruleno==0);
      /* (1) cmd_list ::= cmd_list end_cmd */ yytestcase(yyruleno==1);
      /* (2) cmd_list ::= end_cmd */ yytestcase(yyruleno==2);
      /* (3) end_cmd ::= SEMICOLON */ yytestcase(yyruleno==3);
      /* (4) end_cmd ::= explain cmdx SEMICOLON */ yytestcase(yyruleno==4);
      /* (6) cmdx ::= cmd */ yytestcase(yyruleno==6);
      /* (7) cmd ::= create_table create_table_args */ yytestcase(yyruleno==7);
      /* (9) create_table_args ::= LPAREN column_list RPAREN */ yytestcase(yyruleno==9);
      /* (10) column_list ::= column_list COMMA column */ yytestcase(yyruleno==10);
      /* (11) column_list ::= column */ yytestcase(yyruleno==11);
      /* (20) column_args ::= column_args column_constraints */ yytestcase(yyruleno==20);
      /* (21) column_args ::= */ yytestcase(yyruleno==21);
      /* (31) cmd ::= SELECT set_quantifier select_columns select_table */ yytestcase(yyruleno==31);
      /* (32) set_quantifier ::= DISTINCT|ALL */ yytestcase(yyruleno==32);
      /* (33) set_quantifier ::= */ yytestcase(yyruleno==33);
      /* (34) select_columns ::= select_columns COMMA select_column */ yytestcase(yyruleno==34);
      /* (35) select_columns ::= select_column */ yytestcase(yyruleno==35);
      /* (40) select_table ::= FROM table_references where_clause group_by_clause */ yytestcase(yyruleno==40);
      /* (45) table_reference ::= */ yytestcase(yyruleno==45);
      /* (46) where_clause ::= WHERE search_condition */ yytestcase(yyruleno==46);
      /* (47) group_by_clause ::= */ yytestcase(yyruleno==47);
      /* (48) search_condition ::= comparison_predicate */ yytestcase(yyruleno==48);
      /* (50) cmd ::= UPDATE update_table SET update_expr_list where_clause */ yytestcase(yyruleno==50);
      /* (52) update_expr_list ::= update_expr_list COMMA update_expr */ yytestcase(yyruleno==52);
      /* (54) cmd ::= INSERT INTO insert_table VALUES LPAREN insert_values RPAREN */ yytestcase(yyruleno==54);
      /* (56) insert_values ::= insert_values COMMA insert_term */ yytestcase(yyruleno==56);
      /* (57) insert_values ::= insert_term */ yytestcase(yyruleno==57);
      /* (59) cmd ::= DELETE FROM delete_table where_clause */ yytestcase(yyruleno==59);
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
#line 39 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  builderClean(builder);
#line 1108 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
#line 29 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
  for(int i = 0; i < n; ++i) {
    int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
    if (a < YYNSTATE + YYNRULE) {
      //printf("possible token: %s\n", yyTokenName[i]);
    }
  }
#line 1132 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
#line 43 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  //printf("Parse accepts input!\n");
#line 1154 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
