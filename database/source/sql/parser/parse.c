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
#define YYNOCODE 84
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
#define YYNSTATE             57
#define YYNRULE              63
#define YY_MAX_SHIFT         56
#define YY_MIN_SHIFTREDUCE   109
#define YY_MAX_SHIFTREDUCE   171
#define YY_MIN_REDUCE        172
#define YY_MAX_REDUCE        234
#define YY_ERROR_ACTION      235
#define YY_ACCEPT_ACTION     236
#define YY_NO_ACTION         237

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
#define YY_ACTTAB_COUNT (156)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */     3,  167,   55,  123,  124,  139,   53,  123,  124,  112,
 /*    10 */   122,   55,  132,   49,  139,  236,   21,  111,    8,  122,
 /*    20 */    42,   43,  138,  140,  140,  140,  140,  140,  140,  140,
 /*    30 */   140,  157,  159,  169,  163,   52,   30,   55,    5,   54,
 /*    40 */   139,  134,  135,   17,  171,   48,  136,   43,   45,  169,
 /*    50 */   169,  169,  169,   55,  129,   55,  139,  122,  139,  119,
 /*    60 */    12,   55,   40,   43,  139,  164,  122,   44,  120,   12,
 /*    70 */    26,  137,   33,   51,   56,  115,   36,  123,  124,   39,
 /*    80 */   162,  126,  127,  131,  166,   29,   27,  146,   34,  154,
 /*    90 */   123,  124,    2,    2,  128,  128,   51,   41,  168,  172,
 /*   100 */   112,    9,   50,  215,   18,  147,   19,  153,   35,   47,
 /*   110 */   145,  110,    8,  143,  143,  165,   22,    2,    2,  118,
 /*   120 */    13,    7,    7,  161,  155,  149,  142,   38,  141,   11,
 /*   130 */   117,    1,  170,    1,  148,    4,    1,   10,   28,  116,
 /*   140 */    28,  151,   15,   25,   23,   14,    6,   37,   24,   46,
 /*   150 */    16,   31,   32,  133,   20,  113,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     4,   81,   54,    7,    8,   57,    2,    7,    8,    1,
 /*    10 */    54,   54,   64,   57,   57,   46,   47,   48,   49,   54,
 /*    20 */    63,   64,   57,   27,   28,   29,   30,   27,   28,   29,
 /*    30 */    30,   74,   75,   11,   78,   31,   32,   54,    9,   10,
 /*    40 */    57,   12,   13,   39,   54,   41,   63,   64,   44,   27,
 /*    50 */    28,   29,   30,   54,   62,   54,   57,   54,   57,   56,
 /*    60 */    57,   54,   63,   64,   57,   64,   54,   55,   56,   57,
 /*    70 */    54,   64,   82,   54,   50,   51,   52,    7,    8,   58,
 /*    80 */    54,   60,   61,    5,   54,   66,   70,   68,   54,   73,
 /*    90 */     7,    8,   14,   15,    7,    8,   54,   80,   81,    0,
 /*   100 */     1,    6,   76,    1,    6,   35,    6,   73,   54,   79,
 /*   110 */    68,   48,   49,   33,   34,    5,    6,   14,   15,    5,
 /*   120 */     6,   20,   21,   71,   69,   54,   67,   71,   54,   65,
 /*   130 */    54,   38,   71,   38,   69,    4,   38,   37,   36,   53,
 /*   140 */    36,   72,   37,   77,   59,    4,   21,   40,   26,   43,
 /*   150 */    42,    3,    3,   11,   26,    1,
};
#define YY_SHIFT_USE_DFLT (-5)
#define YY_SHIFT_COUNT (56)
#define YY_SHIFT_MIN   (-4)
#define YY_SHIFT_MAX   (154)
static const short yy_shift_ofst[] = {
 /*     0 */     8,    0,    0,    0,   83,   -4,    0,    0,    4,   83,
 /*    10 */    83,   83,   87,   83,   22,   83,   83,   83,   83,   83,
 /*    20 */    83,   99,   22,   29,   70,   95,  102,   98,   83,  100,
 /*    30 */    80,   83,   83,   93,  104,  104,  131,   -5,   -5,   -5,
 /*    40 */    78,  110,  103,  101,  114,  105,  141,  106,  108,  125,
 /*    50 */   107,  122,  148,  149,  142,  128,  154,
};
#define YY_REDUCE_USE_DFLT (-81)
#define YY_REDUCE_COUNT (39)
#define YY_REDUCE_MIN   (-80)
#define YY_REDUCE_MAX   (86)
static const signed char yy_reduce_ofst[] = {
 /*     0 */   -31,  -43,  -17,   -1,   12,  -52,    1,    7,   24,  -44,
 /*    10 */    16,   19,   21,    3,   17,  -10,   30,   26,   34,   42,
 /*    20 */   -35,   63,  -80,   -8,   54,   52,   55,   56,   71,   59,
 /*    30 */    64,   74,   76,   61,   55,   65,   86,   66,   69,   85,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   177,  235,  235,  235,  235,  235,  235,  235,  235,  235,
 /*    10 */   219,  235,  188,  235,  235,  235,  235,  235,  219,  235,
 /*    20 */   235,  177,  235,  184,  235,  235,  213,  235,  235,  235,
 /*    30 */   207,  235,  235,  235,  213,  213,  235,  235,  221,  193,
 /*    40 */   235,  235,  223,  235,  235,  235,  235,  235,  235,  235,
 /*    50 */   235,  235,  235,  235,  235,  185,  235,
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
  "BETWEEN",       "IN",            "ISNULL",        "NOTNULL",     
  "NE",            "EQ",            "GT",            "LE",          
  "LT",            "GE",            "DOT",           "INTEGER",     
  "FLOAT",         "DATE",          "TIME",          "DROP",        
  "SELECT",        "DISTINCT",      "ALL",           "ASTERISK",    
  "AS",            "FROM",          "WHERE",         "UPDATE",      
  "SET",           "INSERT",        "INTO",          "VALUES",      
  "DELETE",        "error",         "input",         "cmd_list",    
  "end_cmd",       "explain",       "cmdx",          "cmd",         
  "create_table",  "create_table_args",  "name",          "column_list", 
  "column",        "column_id",     "type",          "column_args", 
  "typetoken",     "typename",      "column_constraints",  "expr",        
  "term",          "set_quantifier",  "select_columns",  "select_table",
  "select_column",  "as_clause",     "table_references",  "where_clause",
  "group_by_clause",  "table_reference",  "search_condition",  "comparison_predicate",
  "update_table",  "update_expr_list",  "update_expr",   "insert_table",
  "insert_values",  "insert_term",   "delete_table",
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
 /*  24 */ "column_constraints ::= NOT NULL",
 /*  25 */ "column_constraints ::= UNIQUE",
 /*  26 */ "column_constraints ::= AUTO_INCREMENT",
 /*  27 */ "expr ::= expr AND|OR expr",
 /*  28 */ "expr ::= term NE|EQ term",
 /*  29 */ "term ::= name DOT column_id",
 /*  30 */ "term ::= column_id",
 /*  31 */ "term ::= INTEGER|FLOAT|DATE|TIME",
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
 /*  47 */ "table_reference ::=",
 /*  48 */ "where_clause ::= WHERE search_condition",
 /*  49 */ "group_by_clause ::=",
 /*  50 */ "search_condition ::= comparison_predicate",
 /*  51 */ "comparison_predicate ::= expr",
 /*  52 */ "cmd ::= UPDATE update_table SET update_expr_list where_clause",
 /*  53 */ "update_table ::= name",
 /*  54 */ "update_expr_list ::= update_expr_list COMMA update_expr",
 /*  55 */ "update_expr ::= column_id EQ term",
 /*  56 */ "cmd ::= INSERT INTO insert_table VALUES LPAREN insert_values RPAREN",
 /*  57 */ "insert_table ::= name",
 /*  58 */ "insert_values ::= insert_values COMMA insert_term",
 /*  59 */ "insert_values ::= insert_term",
 /*  60 */ "insert_term ::= INTEGER|FLOAT|DATE|TIME|NULL",
 /*  61 */ "cmd ::= DELETE FROM delete_table where_clause",
 /*  62 */ "delete_table ::= name",
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
  { 46, 1 },
  { 47, 2 },
  { 47, 1 },
  { 48, 1 },
  { 48, 3 },
  { 49, 0 },
  { 50, 1 },
  { 51, 2 },
  { 52, 3 },
  { 53, 3 },
  { 55, 3 },
  { 55, 1 },
  { 56, 3 },
  { 57, 1 },
  { 54, 1 },
  { 54, 1 },
  { 58, 0 },
  { 58, 1 },
  { 60, 1 },
  { 61, 1 },
  { 59, 2 },
  { 59, 0 },
  { 62, 4 },
  { 62, 2 },
  { 62, 2 },
  { 62, 1 },
  { 62, 1 },
  { 63, 3 },
  { 63, 3 },
  { 64, 3 },
  { 64, 1 },
  { 64, 1 },
  { 51, 3 },
  { 51, 4 },
  { 65, 1 },
  { 65, 0 },
  { 66, 3 },
  { 66, 1 },
  { 68, 3 },
  { 68, 4 },
  { 69, 2 },
  { 69, 0 },
  { 67, 4 },
  { 67, 2 },
  { 70, 3 },
  { 70, 1 },
  { 73, 2 },
  { 73, 0 },
  { 71, 2 },
  { 72, 0 },
  { 74, 1 },
  { 75, 1 },
  { 51, 5 },
  { 76, 1 },
  { 77, 3 },
  { 78, 3 },
  { 51, 7 },
  { 79, 1 },
  { 80, 3 },
  { 80, 1 },
  { 81, 1 },
  { 51, 4 },
  { 82, 1 },
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
#line 54 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("semicolon found\n");}
#line 871 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 4: /* end_cmd ::= explain cmdx SEMICOLON */
#line 55 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("command found and semicolon found\n");}
#line 876 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 5: /* explain ::= */
      case 51: /* comparison_predicate ::= expr */ yytestcase(yyruleno==51);
#line 57 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ }
#line 882 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 8: /* create_table ::= CREATE TABLE name */
#line 65 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{builderStartCreateTable(builder, yymsp[0].minor.yy0);}
#line 887 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 12: /* column ::= column_id type column_args */
#line 73 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  (void)yygotominor.yy0;

  builderAddColumn(builder, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 896 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 13: /* column_id ::= name */
      case 17: /* type ::= typetoken */ yytestcase(yyruleno==17);
      case 18: /* typetoken ::= typename */ yytestcase(yyruleno==18);
      case 19: /* typename ::= ID|STRING */ yytestcase(yyruleno==19);
#line 79 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 904 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 14: /* name ::= ID */
      case 15: /* name ::= STRING */ yytestcase(yyruleno==15);
#line 85 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0; token_print(yymsp[0].minor.yy0);}
#line 910 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 16: /* type ::= */
#line 89 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = nullptr; }
#line 915 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 22: /* column_constraints ::= DEFAULT LPAREN expr RPAREN */
#line 100 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{(void)yymsp[-1].minor.yy0; printf("col_const\n");}
#line 920 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 23: /* column_constraints ::= DEFAULT term */
#line 101 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{(void)yymsp[0].minor.yy0; printf("col_const\n");}
#line 925 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 24: /* column_constraints ::= NOT NULL */
#line 104 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{printf("Token: NOT NULL\n");}
#line 930 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 25: /* column_constraints ::= UNIQUE */
#line 105 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{printf("Token: UNIQUE\n");}
#line 935 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 26: /* column_constraints ::= AUTO_INCREMENT */
#line 106 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{printf("Token: AUTO_INCREMENT\n");}
#line 940 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 27: /* expr ::= expr AND|OR expr */
#line 115 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderStartNestedExpr(builder, yymsp[-1].minor.yy0);}
#line 945 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 28: /* expr ::= term NE|EQ term */
#line 116 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddValueExpr(builder, yymsp[-1].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 950 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 29: /* term ::= name DOT column_id */
#line 118 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
    yymsp[-2].minor.yy0->table_name = yymsp[-2].minor.yy0->text;
    yymsp[-2].minor.yy0->column_name = yymsp[0].minor.yy0->text;
    yymsp[-2].minor.yy0->text = yymsp[-2].minor.yy0->text + "." + yymsp[0].minor.yy0->text;
    printf("Reference: %s.%s\n", yymsp[-2].minor.yy0->table_name.c_str(), yymsp[-2].minor.yy0->column_name.c_str());
    yymsp[-2].minor.yy0->is_column = true;
    yygotominor.yy0 = yymsp[-2].minor.yy0;
}
#line 962 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 30: /* term ::= column_id */
#line 126 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; printf("expression term\n"); }
#line 967 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 31: /* term ::= INTEGER|FLOAT|DATE|TIME */
#line 128 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
  printf("Numeric term handling\n");
  yygotominor.yy0 = yymsp[0].minor.yy0;
}
#line 975 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 32: /* cmd ::= DROP TABLE name */
#line 135 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{builderStartDropTable(builder, yymsp[0].minor.yy0);}
#line 980 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 38: /* select_column ::= name DOT ASTERISK */
#line 147 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddSelectAllColumns(builder, yymsp[-2].minor.yy0); }
#line 985 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 39: /* select_column ::= name DOT name as_clause */
#line 148 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  builderAddQualifiedSelectColumn(builder, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 992 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 40: /* as_clause ::= AS name */
#line 152 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 997 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 41: /* as_clause ::= */
#line 153 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy0 = nullptr; printf("Empty AS\n"); }
#line 1002 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 43: /* select_table ::= FROM name */
      case 44: /* table_references ::= table_references COMMA table_reference */ yytestcase(yyruleno==44);
      case 45: /* table_references ::= table_reference */ yytestcase(yyruleno==45);
#line 156 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ (void)yymsp[0].minor.yy0; }
#line 1009 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 46: /* table_reference ::= name as_clause */
#line 162 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  printf("Non-empty table reference\n");
  (void)yygotominor.yy0;
  (void)yymsp[-1].minor.yy0; 
  (void)yymsp[0].minor.yy0; 
  //printf("Table reference: %s as %s\n", yymsp[-1].minor.yy0->text->c_str(), yymsp[0].minor.yy0->text->c_str());
}
#line 1020 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 47: /* table_reference ::= */
#line 170 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("Empty table reference\n"); }
#line 1025 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 48: /* where_clause ::= WHERE search_condition */
#line 173 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ printf("WHERE clause\n"); }
#line 1030 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 53: /* update_table ::= name */
#line 187 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  // TODO: update builder functions
}
#line 1038 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 55: /* update_expr ::= column_id EQ term */
#line 193 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ builderAddUpdateExpr(builder, yymsp[-1].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1043 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 57: /* insert_table ::= name */
#line 200 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
  yygotominor.yy0 = yymsp[0].minor.yy0; 
  builderStartInsertCommand(builder);
  builderAddTableName(builder, yygotominor.yy0);
  printf("Starting INSERT INTO statement parse\n");
}
#line 1053 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 60: /* insert_term ::= INTEGER|FLOAT|DATE|TIME|NULL */
#line 211 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  builderAddDataItem(builder, yymsp[0].minor.yy0);
}
#line 1061 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 62: /* delete_table ::= name */
#line 221 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  // TODO: Update builder functions
}
#line 1069 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
      /* (20) column_args ::= column_args column_constraints */ yytestcase(yyruleno==20);
      /* (21) column_args ::= */ yytestcase(yyruleno==21);
      /* (33) cmd ::= SELECT set_quantifier select_columns select_table */ yytestcase(yyruleno==33);
      /* (34) set_quantifier ::= DISTINCT|ALL */ yytestcase(yyruleno==34);
      /* (35) set_quantifier ::= */ yytestcase(yyruleno==35);
      /* (36) select_columns ::= select_columns COMMA select_column */ yytestcase(yyruleno==36);
      /* (37) select_columns ::= select_column */ yytestcase(yyruleno==37);
      /* (42) select_table ::= FROM table_references where_clause group_by_clause */ yytestcase(yyruleno==42);
      /* (49) group_by_clause ::= */ yytestcase(yyruleno==49);
      /* (50) search_condition ::= comparison_predicate */ yytestcase(yyruleno==50);
      /* (52) cmd ::= UPDATE update_table SET update_expr_list where_clause */ yytestcase(yyruleno==52);
      /* (54) update_expr_list ::= update_expr_list COMMA update_expr */ yytestcase(yyruleno==54);
      /* (56) cmd ::= INSERT INTO insert_table VALUES LPAREN insert_values RPAREN */ yytestcase(yyruleno==56);
      /* (58) insert_values ::= insert_values COMMA insert_term */ yytestcase(yyruleno==58);
      /* (59) insert_values ::= insert_term */ yytestcase(yyruleno==59);
      /* (61) cmd ::= DELETE FROM delete_table where_clause */ yytestcase(yyruleno==61);
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
#line 40 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  printf("Parse failed!\n");
  builderClean(builder);
#line 1145 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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

  printf("Syntax Error!\n");
  int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
  for(int i = 0; i < n; ++i) {
    int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
    if (a < YYNSTATE + YYNRULE) {
      printf("possible token: %s\n", yyTokenName[i]);
    }
  }
#line 1170 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
#line 45 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  printf("Parse accepts input!\n");
#line 1192 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
