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
#line 49 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

#include "sqliteInt.h"

/*
** Disable all error recovery processing in the parser push-down
** automaton.
*/
#define YYNOERRORRECOVERY 1

/*
** Make yytestcase() the same as testcase()
*/
#define yytestcase(X) testcase(X)

/*
** An instance of this structure holds information about the
** LIMIT clause of a SELECT statement.
*/
struct LimitVal {
  Expr *pLimit;    /* The LIMIT expression.  NULL if there is no limit */
  Expr *pOffset;   /* The OFFSET expression.  NULL if there is none */
};

/*
** An instance of this structure is used to store the LIKE,
** GLOB, NOT LIKE, and NOT GLOB operators.
*/
struct LikeOp {
  Token eOperator;  /* "like" or "glob" or "regexp" */
  int bNot;         /* True if the NOT keyword is present */
};

/*
** An instance of the following structure describes the event of a
** TRIGGER.  "a" is the event type, one of TK_UPDATE, TK_INSERT,
** TK_DELETE, or TK_INSTEAD.  If the event is of the form
**
**      UPDATE ON (a,b,c)
**
** Then the "b" IdList records the list "a,b,c".
*/
struct TrigEvent { int a; IdList * b; };

/*
** An instance of this structure holds the ATTACH key and the key type.
*/
struct AttachKey { int type;  Token key; };

#line 413 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /*
  ** For a compound SELECT statement, make sure p->pPrior->pNext==p for
  ** all elements in the list.  And make sure list length does not exceed
  ** SQLITE_LIMIT_COMPOUND_SELECT.
  */
  static void parserDoubleLinkSelect(Parse *pParse, Select *p){
    if( p->pPrior ){
      Select *pNext = 0, *pLoop;
      int mxSelect, cnt = 0;
      for(pLoop=p; pLoop; pNext=pLoop, pLoop=pLoop->pPrior, cnt++){
        pLoop->pNext = pNext;
        pLoop->selFlags |= SF_Compound;
      }
      if( (p->selFlags & SF_MultiValue)==0 && 
        (mxSelect = pParse->db->aLimit[SQLITE_LIMIT_COMPOUND_SELECT])>0 &&
        cnt>mxSelect
      ){
        sqlite3ErrorMsg(pParse, "too many terms in compound SELECT");
      }
    }
  }
#line 827 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /* This is a utility routine used to set the ExprSpan.zStart and
  ** ExprSpan.zEnd values of pOut so that the span covers the complete
  ** range of text beginning with pStart and going to the end of pEnd.
  */
  static void spanSet(ExprSpan *pOut, Token *pStart, Token *pEnd){
    pOut->zStart = pStart->z;
    pOut->zEnd = &pEnd->z[pEnd->n];
  }

  /* Construct a new Expr object from a single identifier.  Use the
  ** new Expr to populate pOut.  Set the span of pOut to be the identifier
  ** that created the expression.
  */
  static void spanExpr(ExprSpan *pOut, Parse *pParse, int op, Token *pValue){
    pOut->pExpr = sqlite3PExpr(pParse, op, 0, 0, pValue);
    pOut->zStart = pValue->z;
    pOut->zEnd = &pValue->z[pValue->n];
  }
#line 917 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /* This routine constructs a binary expression node out of two ExprSpan
  ** objects and uses the result to populate a new ExprSpan object.
  */
  static void spanBinaryExpr(
    ExprSpan *pOut,     /* Write the result here */
    Parse *pParse,      /* The parsing context.  Errors accumulate here */
    int op,             /* The binary operation */
    ExprSpan *pLeft,    /* The left operand */
    ExprSpan *pRight    /* The right operand */
  ){
    pOut->pExpr = sqlite3PExpr(pParse, op, pLeft->pExpr, pRight->pExpr, 0);
    pOut->zStart = pLeft->zStart;
    pOut->zEnd = pRight->zEnd;
  }
#line 971 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /* Construct an expression node for a unary postfix operator
  */
  static void spanUnaryPostfix(
    ExprSpan *pOut,        /* Write the new expression node here */
    Parse *pParse,         /* Parsing context to record errors */
    int op,                /* The operator */
    ExprSpan *pOperand,    /* The operand */
    Token *pPostOp         /* The operand token for setting the span */
  ){
    pOut->pExpr = sqlite3PExpr(pParse, op, pOperand->pExpr, 0, 0);
    pOut->zStart = pOperand->zStart;
    pOut->zEnd = &pPostOp->z[pPostOp->n];
  }                           
#line 990 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /* A routine to convert a binary TK_IS or TK_ISNOT expression into a
  ** unary TK_ISNULL or TK_NOTNULL expression. */
  static void binaryToUnaryIfNull(Parse *pParse, Expr *pY, Expr *pA, int op){
    sqlite3 *db = pParse->db;
    if( pY && pA && pY->op==TK_NULL ){
      pA->op = (u8)op;
      sqlite3ExprDelete(db, pA->pRight);
      pA->pRight = 0;
    }
  }
#line 1018 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /* Construct an expression node for a unary prefix operator
  */
  static void spanUnaryPrefix(
    ExprSpan *pOut,        /* Write the new expression node here */
    Parse *pParse,         /* Parsing context to record errors */
    int op,                /* The operator */
    ExprSpan *pOperand,    /* The operand */
    Token *pPreOp         /* The operand token for setting the span */
  ){
    pOut->pExpr = sqlite3PExpr(pParse, op, pOperand->pExpr, 0, 0);
    pOut->zStart = pPreOp->z;
    pOut->zEnd = pOperand->zEnd;
  }
#line 1247 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  /* Add a single new term to an ExprList that is used to store a
  ** list of identifiers.  Report an error if the ID list contains
  ** a COLLATE clause or an ASC or DESC keyword, except ignore the
  ** error while parsing a legacy schema.
  */
  static ExprList *parserAddExprIdListTerm(
    Parse *pParse,
    ExprList *pPrior,
    Token *pIdToken,
    int hasCollate,
    int sortOrder
  ){
    ExprList *p = sqlite3ExprListAppend(pParse, pPrior, 0);
    if( (hasCollate || sortOrder!=SQLITE_SO_UNDEFINED)
        && pParse->db->init.busy==0
    ){
      sqlite3ErrorMsg(pParse, "syntax error after column name \"%.*s\"",
                         pIdToken->n, pIdToken->z);
    }
    sqlite3ExprListSetName(pParse, p, pIdToken, 1);
    return p;
  }
#line 188 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
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
**    sqlite3ParserTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is sqlite3ParserTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    sqlite3ParserARG_SDECL     A static variable declaration for the %extra_argument
**    sqlite3ParserARG_PDECL     A parameter declaration for the %extra_argument
**    sqlite3ParserARG_STORE     Code to store %extra_argument into yypParser
**    sqlite3ParserARG_FETCH     Code to extract %extra_argument from yypParser
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
#define YYNOCODE 254
#define YYACTIONTYPE unsigned short int
#define YYWILDCARD 70
#define sqlite3ParserTOKENTYPE Token
typedef union {
  int yyinit;
  sqlite3ParserTOKENTYPE yy0;
  Select* yy3;
  ExprList* yy14;
  With* yy59;
  SrcList* yy65;
  struct LikeOp yy96;
  Expr* yy132;
  u8 yy186;
  int yy328;
  ExprSpan yy346;
  struct TrigEvent yy378;
  u16 yy381;
  IdList* yy408;
  struct {int value; int mask;} yy429;
  TriggerStep* yy473;
  struct LimitVal yy476;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define sqlite3ParserARG_SDECL Parse *pParse;
#define sqlite3ParserARG_PDECL ,Parse *pParse
#define sqlite3ParserARG_FETCH Parse *pParse = yypParser->pParse
#define sqlite3ParserARG_STORE yypParser->pParse = pParse
#define YYFALLBACK 1
#define YYNSTATE             436
#define YYNRULE              328
#define YY_MAX_SHIFT         435
#define YY_MIN_SHIFTREDUCE   649
#define YY_MAX_SHIFTREDUCE   976
#define YY_MIN_REDUCE        977
#define YY_MAX_REDUCE        1304
#define YY_ERROR_ACTION      1305
#define YY_ACCEPT_ACTION     1306
#define YY_NO_ACTION         1307

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
#define YY_ACTTAB_COUNT (1501)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   311, 1306,  145,  651,    2,  192,  652,  338,  780,   92,
 /*    10 */    92,   92,   92,   85,   90,   90,   90,   90,   89,   89,
 /*    20 */    88,   88,   88,   87,  335,   88,   88,   88,   87,  335,
 /*    30 */   327,  856,  856,   92,   92,   92,   92,  776,   90,   90,
 /*    40 */    90,   90,   89,   89,   88,   88,   88,   87,  335,   86,
 /*    50 */    83,  166,   93,   94,   84,  868,  871,  860,  860,   91,
 /*    60 */    91,   92,   92,   92,   92,  335,   90,   90,   90,   90,
 /*    70 */    89,   89,   88,   88,   88,   87,  335,  311,  780,   90,
 /*    80 */    90,   90,   90,   89,   89,   88,   88,   88,   87,  335,
 /*    90 */   123,  808,  689,  689,  689,  689,  112,  230,  430,  257,
 /*   100 */   809,  698,  430,   86,   83,  166,  324,   55,  856,  856,
 /*   110 */   201,  158,  276,  387,  271,  386,  188,  689,  689,  828,
 /*   120 */   833,   49,  944,  269,  833,   49,  123,   87,  335,   93,
 /*   130 */    94,   84,  868,  871,  860,  860,   91,   91,   92,   92,
 /*   140 */    92,   92,  342,   90,   90,   90,   90,   89,   89,   88,
 /*   150 */    88,   88,   87,  335,  311,  328,  333,  332,  701,  408,
 /*   160 */   394,   69,  690,  691,  690,  691,  715,  910,  251,  354,
 /*   170 */   250,  698,  704,  430,  908,  430,  909,   89,   89,   88,
 /*   180 */    88,   88,   87,  335,  391,  856,  856,  690,  691,  183,
 /*   190 */    95,  340,  384,  381,  380,  833,   31,  833,   49,  912,
 /*   200 */   912,  333,  332,  379,  123,  311,   93,   94,   84,  868,
 /*   210 */   871,  860,  860,   91,   91,   92,   92,   92,   92,  114,
 /*   220 */    90,   90,   90,   90,   89,   89,   88,   88,   88,   87,
 /*   230 */   335,  430,  408,  399,  435,  657,  856,  856,  346,   57,
 /*   240 */   232,  828,  109,   20,  912,  912,  231,  393,  937,  760,
 /*   250 */    97,  751,  752,  833,   49,  708,  708,   93,   94,   84,
 /*   260 */   868,  871,  860,  860,   91,   91,   92,   92,   92,   92,
 /*   270 */   707,   90,   90,   90,   90,   89,   89,   88,   88,   88,
 /*   280 */    87,  335,  311,  114,   22,  706,  688,   58,  408,  390,
 /*   290 */   251,  349,  240,  749,  752,  689,  689,  847,  685,  115,
 /*   300 */    21,  231,  393,  689,  689,  697,  183,  355,  430,  384,
 /*   310 */   381,  380,  192,  856,  856,  780,  123,  160,  159,  223,
 /*   320 */   379,  738,   25,  315,  362,  841,  143,  689,  689,  835,
 /*   330 */   833,   48,  339,  937,   93,   94,   84,  868,  871,  860,
 /*   340 */   860,   91,   91,   92,   92,   92,   92,  914,   90,   90,
 /*   350 */    90,   90,   89,   89,   88,   88,   88,   87,  335,  311,
 /*   360 */   840,  840,  840,  266,  430,  690,  691,  778,  114, 1300,
 /*   370 */  1300,  430,    1,  690,  691,  697,  688,  689,  689,  689,
 /*   380 */   689,  689,  689,  287,  298,  780,  833,   10,  686,  115,
 /*   390 */   856,  856,  355,  833,   10,  828,  366,  690,  691,  363,
 /*   400 */   321,   76,  123,   74,   23,  737,  807,  323,  356,  353,
 /*   410 */   847,   93,   94,   84,  868,  871,  860,  860,   91,   91,
 /*   420 */    92,   92,   92,   92,  940,   90,   90,   90,   90,   89,
 /*   430 */    89,   88,   88,   88,   87,  335,  311,  806,  841,  429,
 /*   440 */   713,  941,  835,  430,  251,  354,  250,  690,  691,  690,
 /*   450 */   691,  690,  691,   86,   83,  166,   24,  942,  151,  753,
 /*   460 */   285,  907,  403,  907,  164,  833,   10,  856,  856,  965,
 /*   470 */   306,  754,  679,  840,  840,  840,  795,  216,  794,  222,
 /*   480 */   906,  344,  906,  904,   86,   83,  166,  286,   93,   94,
 /*   490 */    84,  868,  871,  860,  860,   91,   91,   92,   92,   92,
 /*   500 */    92,  430,   90,   90,   90,   90,   89,   89,   88,   88,
 /*   510 */    88,   87,  335,  311,  430,  724,  352,  705,  427,  699,
 /*   520 */   700,  376,  210,  833,   49,  793,  397,  857,  857,  940,
 /*   530 */   213,  762,  727,  334,  699,  700,  833,   10,   86,   83,
 /*   540 */   166,  345,  396,  902,  856,  856,  941,  385,  833,    9,
 /*   550 */   406,  869,  872,  187,  890,  728,  347,  398,  404,  977,
 /*   560 */   652,  338,  942,  954,  413,   93,   94,   84,  868,  871,
 /*   570 */   860,  860,   91,   91,   92,   92,   92,   92,  861,   90,
 /*   580 */    90,   90,   90,   89,   89,   88,   88,   88,   87,  335,
 /*   590 */   311, 1219,  114,  430,  834,  430,    5,  165,  192,  688,
 /*   600 */   832,  780,  430,  723,  430,  234,  325,  189,  163,  316,
 /*   610 */   356,  955,  115,  235,  269,  833,   35,  833,   36,  747,
 /*   620 */   720,  856,  856,  793,  833,   12,  833,   27,  745,  174,
 /*   630 */   968, 1290,  968, 1291, 1290,  310, 1291,  693,  317,  245,
 /*   640 */   264,  311,   93,   94,   84,  868,  871,  860,  860,   91,
 /*   650 */    91,   92,   92,   92,   92,  832,   90,   90,   90,   90,
 /*   660 */    89,   89,   88,   88,   88,   87,  335,  430,  320,  213,
 /*   670 */   762,  780,  856,  856,  920,  920,  369,  257,  966,  220,
 /*   680 */   966,  396,  663,  664,  665,  242,  259,  244,  262,  833,
 /*   690 */    37,  650,    2,   93,   94,   84,  868,  871,  860,  860,
 /*   700 */    91,   91,   92,   92,   92,   92,  430,   90,   90,   90,
 /*   710 */    90,   89,   89,   88,   88,   88,   87,  335,  311,  430,
 /*   720 */   239,  430,  917,  368,  430,  238,  916,  793,  833,   38,
 /*   730 */   430,  825,  430,   66,  430,  392,  430,  766,  766,  430,
 /*   740 */   367,  833,   39,  833,   28,  430,  833,   29,   68,  856,
 /*   750 */   856,  900,  833,   40,  833,   41,  833,   42,  833,   11,
 /*   760 */    72,  833,   43,  243,  305,  970,  114,  833,   99,  961,
 /*   770 */    93,   94,   84,  868,  871,  860,  860,   91,   91,   92,
 /*   780 */    92,   92,   92,  430,   90,   90,   90,   90,   89,   89,
 /*   790 */    88,   88,   88,   87,  335,  311,  430,  361,  430,  165,
 /*   800 */   147,  430,  186,  185,  184,  833,   44,  430,  289,  430,
 /*   810 */   246,  430,  971,  430,  212,  163,  430,  357,  833,   45,
 /*   820 */   833,   32,  932,  833,   46,  793,  856,  856,  718,  833,
 /*   830 */    47,  833,   33,  833,  117,  833,  118,   75,  833,  119,
 /*   840 */   288,  305,  967,  214,  935,  322,  311,   93,   94,   84,
 /*   850 */   868,  871,  860,  860,   91,   91,   92,   92,   92,   92,
 /*   860 */   430,   90,   90,   90,   90,   89,   89,   88,   88,   88,
 /*   870 */    87,  335,  430,  832,  426,  317,  288,  856,  856,  114,
 /*   880 */   763,  257,  833,   53,  930,  219,  364,  257,  257,  971,
 /*   890 */   361,  396,  257,  257,  833,   34,  257,  311,   93,   94,
 /*   900 */    84,  868,  871,  860,  860,   91,   91,   92,   92,   92,
 /*   910 */    92,  430,   90,   90,   90,   90,   89,   89,   88,   88,
 /*   920 */    88,   87,  335,  430,  217,  318,  124,  253,  856,  856,
 /*   930 */   218,  943,  257,  833,  100,  898,  759,  774,  361,  755,
 /*   940 */   423,  329,  758, 1017,  289,  833,   50,  682,  311,   93,
 /*   950 */    82,   84,  868,  871,  860,  860,   91,   91,   92,   92,
 /*   960 */    92,   92,  430,   90,   90,   90,   90,   89,   89,   88,
 /*   970 */    88,   88,   87,  335,  430,  256,  419,  114,  249,  856,
 /*   980 */   856,  331,  114,  400,  833,  101,  359,  187, 1064,  726,
 /*   990 */   725,  739,  401,  416,  420,  360,  833,  102,  424,  311,
 /*  1000 */   258,   94,   84,  868,  871,  860,  860,   91,   91,   92,
 /*  1010 */    92,   92,   92,  430,   90,   90,   90,   90,   89,   89,
 /*  1020 */    88,   88,   88,   87,  335,  430,  221,  261,  114,  114,
 /*  1030 */   856,  856,  808,  114,  156,  833,   98,  772,  733,  734,
 /*  1040 */   275,  809,  771,  316,  263,  265,  960,  833,  116,  307,
 /*  1050 */   741,  274,  722,   84,  868,  871,  860,  860,   91,   91,
 /*  1060 */    92,   92,   92,   92,  430,   90,   90,   90,   90,   89,
 /*  1070 */    89,   88,   88,   88,   87,  335,   80,  425,  830,    3,
 /*  1080 */  1214,  191,  430,  721,  336,  336,  833,  113,  252,   80,
 /*  1090 */   425,   68,    3,  913,  913,  428,  270,  336,  336,  430,
 /*  1100 */   377,  784,  430,  197,  833,  106,  430,  716,  428,  430,
 /*  1110 */   267,  430,  897,   68,  414,  430,  769,  409,  430,   71,
 /*  1120 */   430,  833,  105,  123,  833,  103,  847,  414,  833,   49,
 /*  1130 */   843,  833,  104,  833,   52,  800,  123,  833,   54,  847,
 /*  1140 */   833,   51,  833,   26,  831,  802,   77,   78,  191,  389,
 /*  1150 */   430,  372,  114,   79,  432,  431,  911,  911,  835,   77,
 /*  1160 */    78,  779,  893,  408,  410,  197,   79,  432,  431,  791,
 /*  1170 */   226,  835,  833,   30,  772,   80,  425,  716,    3,  771,
 /*  1180 */   411,  412,  897,  336,  336,  290,  291,  839,  703,  840,
 /*  1190 */   840,  840,  842,   19,  428,  695,  684,  672,  111,  671,
 /*  1200 */   843,  673,  840,  840,  840,  842,   19,  207,  661,  278,
 /*  1210 */   148,  304,  280,  414,  282,    6,  822,  348,  248,  241,
 /*  1220 */   358,  934,  720,   80,  425,  847,    3,  161,  382,  273,
 /*  1230 */   284,  336,  336,  415,  296,  958,  895,  894,  157,  674,
 /*  1240 */   107,  194,  428,  948,  135,   77,   78,  777,  953,  951,
 /*  1250 */    56,  319,   79,  432,  431,  121,   66,  835,   59,  128,
 /*  1260 */   146,  414,  350,  130,  351,  819,  131,  132,  133,  375,
 /*  1270 */   173,  149,  138,  847,  936,  365,  178,   70,  425,  827,
 /*  1280 */     3,  889,   62,  371,  915,  336,  336,  792,  840,  840,
 /*  1290 */   840,  842,   19,   77,   78,  208,  428,  144,  179,  373,
 /*  1300 */    79,  432,  431,  255,  180,  835,  260,  675,  181,  308,
 /*  1310 */   388,  744,  326,  743,  742,  414,  731,  718,  712,  402,
 /*  1320 */   309,  711,  788,   65,  277,  272,  789,  847,  730,  710,
 /*  1330 */   709,  279,  193,  787,  281,  876,  840,  840,  840,  842,
 /*  1340 */    19,  786,  283,   73,  418,  330,  422,   77,   78,  227,
 /*  1350 */    96,  407,   67,  405,   79,  432,  431,  292,  228,  835,
 /*  1360 */   215,  202,  229,  293,  767,  303,  302,  301,  204,  299,
 /*  1370 */   294,  295,  676,    7,  681,  433,  669,  206,  110,  224,
 /*  1380 */   203,  205,  434,  667,  666,  658,  120,  168,  656,  237,
 /*  1390 */   840,  840,  840,  842,   19,  337,  155,  233,  236,  341,
 /*  1400 */   167,  905,  108,  313,  903,  826,  314,  125,  126,  127,
 /*  1410 */   129,  170,  247,  756,  172,  928,  134,  136,  171,   60,
 /*  1420 */    61,  123,  169,  137,  175,  933,  176,  927,    8,   13,
 /*  1430 */   177,  254,  191,  918,  139,  370,  924,  140,  678,  150,
 /*  1440 */   374,  274,  182,  378,  141,  122,   63,   14,  383,  729,
 /*  1450 */   268,   15,   64,  225,  846,  845,  874,   16,  765,  770,
 /*  1460 */     4,  162,  209,  395,  211,  142,  878,  796,  801,  312,
 /*  1470 */   190,   71,   68,  875,  873,  939,  199,  938,   17,  195,
 /*  1480 */    18,  196,  417,  975,  152,  653,  976,  198,  153,  421,
 /*  1490 */   877,  154,  200,  844,  696,   81,  343,  297, 1019, 1018,
 /*  1500 */   300,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    19,  144,  145,  146,  147,   24,    1,    2,   27,   80,
 /*    10 */    81,   82,   83,   84,   85,   86,   87,   88,   89,   90,
 /*    20 */    91,   92,   93,   94,   95,   91,   92,   93,   94,   95,
 /*    30 */    19,   50,   51,   80,   81,   82,   83,  212,   85,   86,
 /*    40 */    87,   88,   89,   90,   91,   92,   93,   94,   95,  224,
 /*    50 */   225,  226,   71,   72,   73,   74,   75,   76,   77,   78,
 /*    60 */    79,   80,   81,   82,   83,   95,   85,   86,   87,   88,
 /*    70 */    89,   90,   91,   92,   93,   94,   95,   19,   97,   85,
 /*    80 */    86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
 /*    90 */    66,   33,   27,   28,   27,   28,   22,  201,  152,  152,
 /*   100 */    42,   27,  152,  224,  225,  226,   95,  211,   50,   51,
 /*   110 */    99,  100,  101,  102,  103,  104,  105,   27,   28,   59,
 /*   120 */   174,  175,  243,  112,  174,  175,   66,   94,   95,   71,
 /*   130 */    72,   73,   74,   75,   76,   77,   78,   79,   80,   81,
 /*   140 */    82,   83,  195,   85,   86,   87,   88,   89,   90,   91,
 /*   150 */    92,   93,   94,   95,   19,  209,   89,   90,  173,  209,
 /*   160 */   210,   26,   97,   98,   97,   98,  181,  100,  108,  109,
 /*   170 */   110,   97,  174,  152,  107,  152,  109,   89,   90,   91,
 /*   180 */    92,   93,   94,   95,  163,   50,   51,   97,   98,   99,
 /*   190 */    55,  244,  102,  103,  104,  174,  175,  174,  175,  132,
 /*   200 */   133,   89,   90,  113,   66,   19,   71,   72,   73,   74,
 /*   210 */    75,   76,   77,   78,   79,   80,   81,   82,   83,  198,
 /*   220 */    85,   86,   87,   88,   89,   90,   91,   92,   93,   94,
 /*   230 */    95,  152,  209,  210,  148,  149,   50,   51,  100,   53,
 /*   240 */   154,   59,  156,   22,  132,  133,  119,  120,  163,  163,
 /*   250 */    22,  192,  193,  174,  175,   27,   28,   71,   72,   73,
 /*   260 */    74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
 /*   270 */   174,   85,   86,   87,   88,   89,   90,   91,   92,   93,
 /*   280 */    94,   95,   19,  198,  198,  174,  152,   24,  209,  210,
 /*   290 */   108,  109,  110,  192,  193,   27,   28,   69,  164,  165,
 /*   300 */    79,  119,  120,   27,   28,   27,   99,  222,  152,  102,
 /*   310 */   103,  104,   24,   50,   51,   27,   66,   89,   90,  185,
 /*   320 */   113,  187,   22,  157,  239,   97,   58,   27,   28,  101,
 /*   330 */   174,  175,  246,  163,   71,   72,   73,   74,   75,   76,
 /*   340 */    77,   78,   79,   80,   81,   82,   83,   11,   85,   86,
 /*   350 */    87,   88,   89,   90,   91,   92,   93,   94,   95,   19,
 /*   360 */   132,  133,  134,   23,  152,   97,   98,   91,  198,  119,
 /*   370 */   120,  152,   22,   97,   98,   97,  152,   27,   28,   27,
 /*   380 */    28,   27,   28,  227,  160,   97,  174,  175,  164,  165,
 /*   390 */    50,   51,  222,  174,  175,   59,  230,   97,   98,  233,
 /*   400 */   188,  137,   66,  139,  234,  187,  177,  188,  152,  239,
 /*   410 */    69,   71,   72,   73,   74,   75,   76,   77,   78,   79,
 /*   420 */    80,   81,   82,   83,   12,   85,   86,   87,   88,   89,
 /*   430 */    90,   91,   92,   93,   94,   95,   19,  177,   97,  152,
 /*   440 */    23,   29,  101,  152,  108,  109,  110,   97,   98,   97,
 /*   450 */    98,   97,   98,  224,  225,  226,   22,   45,   24,   47,
 /*   460 */   152,  152,  152,  152,  152,  174,  175,   50,   51,  249,
 /*   470 */   250,   59,   21,  132,  133,  134,  124,  221,  124,  188,
 /*   480 */   171,  172,  171,  172,  224,  225,  226,  152,   71,   72,
 /*   490 */    73,   74,   75,   76,   77,   78,   79,   80,   81,   82,
 /*   500 */    83,  152,   85,   86,   87,   88,   89,   90,   91,   92,
 /*   510 */    93,   94,   95,   19,  152,  183,   65,   23,  170,  171,
 /*   520 */   172,   19,   23,  174,  175,   26,  152,   50,   51,   12,
 /*   530 */   196,  197,   37,  170,  171,  172,  174,  175,  224,  225,
 /*   540 */   226,  232,  208,  232,   50,   51,   29,   52,  174,  175,
 /*   550 */   188,   74,   75,   51,  103,   60,  222,  163,  209,    0,
 /*   560 */     1,    2,   45,  152,   47,   71,   72,   73,   74,   75,
 /*   570 */    76,   77,   78,   79,   80,   81,   82,   83,  101,   85,
 /*   580 */    86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
 /*   590 */    19,  140,  198,  152,   23,  152,   22,   98,   24,  152,
 /*   600 */   152,   27,  152,  183,  152,  152,  111,  213,  214,  107,
 /*   610 */   152,  164,  165,  152,  112,  174,  175,  174,  175,  181,
 /*   620 */   182,   50,   51,  124,  174,  175,  174,  175,  190,   26,
 /*   630 */    22,   23,   22,   23,   26,  166,   26,  168,  169,   16,
 /*   640 */    16,   19,   71,   72,   73,   74,   75,   76,   77,   78,
 /*   650 */    79,   80,   81,   82,   83,  152,   85,   86,   87,   88,
 /*   660 */    89,   90,   91,   92,   93,   94,   95,  152,  220,  196,
 /*   670 */   197,   97,   50,   51,  108,  109,  110,  152,   70,  221,
 /*   680 */    70,  208,    7,    8,    9,   62,   62,   64,   64,  174,
 /*   690 */   175,  146,  147,   71,   72,   73,   74,   75,   76,   77,
 /*   700 */    78,   79,   80,   81,   82,   83,  152,   85,   86,   87,
 /*   710 */    88,   89,   90,   91,   92,   93,   94,   95,   19,  152,
 /*   720 */   195,  152,   31,  220,  152,  152,   35,   26,  174,  175,
 /*   730 */   152,  163,  152,  130,  152,  115,  152,  117,  118,  152,
 /*   740 */    49,  174,  175,  174,  175,  152,  174,  175,   26,   50,
 /*   750 */    51,  152,  174,  175,  174,  175,  174,  175,  174,  175,
 /*   760 */   138,  174,  175,  140,   22,   23,  198,  174,  175,  152,
 /*   770 */    71,   72,   73,   74,   75,   76,   77,   78,   79,   80,
 /*   780 */    81,   82,   83,  152,   85,   86,   87,   88,   89,   90,
 /*   790 */    91,   92,   93,   94,   95,   19,  152,  152,  152,   98,
 /*   800 */    24,  152,  108,  109,  110,  174,  175,  152,  152,  152,
 /*   810 */   152,  152,   70,  152,  213,  214,  152,  152,  174,  175,
 /*   820 */   174,  175,  152,  174,  175,  124,   50,   51,  106,  174,
 /*   830 */   175,  174,  175,  174,  175,  174,  175,  138,  174,  175,
 /*   840 */   152,   22,   23,   22,  163,  189,   19,   71,   72,   73,
 /*   850 */    74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
 /*   860 */   152,   85,   86,   87,   88,   89,   90,   91,   92,   93,
 /*   870 */    94,   95,  152,  152,  168,  169,  152,   50,   51,  198,
 /*   880 */   197,  152,  174,  175,  152,  240,  152,  152,  152,   70,
 /*   890 */   152,  208,  152,  152,  174,  175,  152,   19,   71,   72,
 /*   900 */    73,   74,   75,   76,   77,   78,   79,   80,   81,   82,
 /*   910 */    83,  152,   85,   86,   87,   88,   89,   90,   91,   92,
 /*   920 */    93,   94,   95,  152,  195,  247,  248,  152,   50,   51,
 /*   930 */   195,  195,  152,  174,  175,  195,  195,   26,  152,  195,
 /*   940 */   252,  220,  163,  122,  152,  174,  175,  163,   19,   71,
 /*   950 */    72,   73,   74,   75,   76,   77,   78,   79,   80,   81,
 /*   960 */    82,   83,  152,   85,   86,   87,   88,   89,   90,   91,
 /*   970 */    92,   93,   94,   95,  152,  195,  252,  198,  240,   50,
 /*   980 */    51,  189,  198,   19,  174,  175,   19,   51,   23,  100,
 /*   990 */   101,   26,   28,  163,  163,   28,  174,  175,  163,   19,
 /*  1000 */   152,   72,   73,   74,   75,   76,   77,   78,   79,   80,
 /*  1010 */    81,   82,   83,  152,   85,   86,   87,   88,   89,   90,
 /*  1020 */    91,   92,   93,   94,   95,  152,  240,  152,  198,  198,
 /*  1030 */    50,   51,   33,  198,  123,  174,  175,  116,    7,    8,
 /*  1040 */   101,   42,  121,  107,  152,  152,   23,  174,  175,   26,
 /*  1050 */   152,  112,  183,   73,   74,   75,   76,   77,   78,   79,
 /*  1060 */    80,   81,   82,   83,  152,   85,   86,   87,   88,   89,
 /*  1070 */    90,   91,   92,   93,   94,   95,   19,   20,   23,   22,
 /*  1080 */    23,   26,  152,  152,   27,   28,  174,  175,   23,   19,
 /*  1090 */    20,   26,   22,  132,  133,   38,  152,   27,   28,  152,
 /*  1100 */    23,  215,  152,   26,  174,  175,  152,   27,   38,  152,
 /*  1110 */    23,  152,   27,   26,   57,  152,   23,  163,  152,   26,
 /*  1120 */   152,  174,  175,   66,  174,  175,   69,   57,  174,  175,
 /*  1130 */    27,  174,  175,  174,  175,  152,   66,  174,  175,   69,
 /*  1140 */   174,  175,  174,  175,  152,   23,   89,   90,   26,   91,
 /*  1150 */   152,  236,  198,   96,   97,   98,  132,  133,  101,   89,
 /*  1160 */    90,  152,   23,  209,  210,   26,   96,   97,   98,  152,
 /*  1170 */   212,  101,  174,  175,  116,   19,   20,   97,   22,  121,
 /*  1180 */   152,  193,   97,   27,   28,  152,  152,  152,  152,  132,
 /*  1190 */   133,  134,  135,  136,   38,   23,  152,  152,   26,  152,
 /*  1200 */    97,  152,  132,  133,  134,  135,  136,  235,  152,  212,
 /*  1210 */   199,  150,  212,   57,  212,  200,  203,  216,  241,  216,
 /*  1220 */   241,  203,  182,   19,   20,   69,   22,  186,  178,  177,
 /*  1230 */   216,   27,   28,  229,  202,   39,  177,  177,  200,  155,
 /*  1240 */   245,  122,   38,   41,   22,   89,   90,   91,  159,  159,
 /*  1250 */   242,  159,   96,   97,   98,   71,  130,  101,  242,  191,
 /*  1260 */   223,   57,   18,  194,  159,  203,  194,  194,  194,   18,
 /*  1270 */   158,  223,  191,   69,  203,  159,  158,   19,   20,  191,
 /*  1280 */    22,  203,  137,   46,  238,   27,   28,  159,  132,  133,
 /*  1290 */   134,  135,  136,   89,   90,  159,   38,   22,  158,  179,
 /*  1300 */    96,   97,   98,  237,  158,  101,  159,  159,  158,  179,
 /*  1310 */   107,  176,   48,  176,  176,   57,  184,  106,  176,  125,
 /*  1320 */   179,  178,  218,  107,  217,  176,  218,   69,  184,  176,
 /*  1330 */   176,  217,  159,  218,  217,  159,  132,  133,  134,  135,
 /*  1340 */   136,  218,  217,  137,  179,   95,  179,   89,   90,  228,
 /*  1350 */   129,  126,  128,  127,   96,   97,   98,  206,  231,  101,
 /*  1360 */     5,   25,  231,  205,  207,   10,   11,   12,   13,   14,
 /*  1370 */   204,  203,   17,   26,  162,  161,   13,    6,  180,  180,
 /*  1380 */   153,  153,  151,  151,  151,  151,  167,   32,    4,   34,
 /*  1390 */   132,  133,  134,  135,  136,    3,   22,  142,   43,   68,
 /*  1400 */    15,   23,   16,  251,   23,  120,  251,  248,  131,  111,
 /*  1410 */   123,   56,   16,   20,  125,    1,  123,  131,   63,   79,
 /*  1420 */    79,   66,   67,  111,   36,   28,  122,    1,    5,   22,
 /*  1430 */   107,  140,   26,   54,   54,   44,   61,  107,   20,   24,
 /*  1440 */    19,  112,  105,   53,   22,   40,   22,   22,   53,   30,
 /*  1450 */    23,   22,   22,   53,   23,   23,   23,   22,  116,   23,
 /*  1460 */    22,  122,   23,   26,   23,   22,   11,  124,   28,  114,
 /*  1470 */    36,   26,   26,   23,   23,   23,  122,   23,   36,   26,
 /*  1480 */    36,   22,   24,   23,   22,    1,   23,   26,   22,   24,
 /*  1490 */    23,   22,  122,   23,   23,   22,  141,   23,  122,  122,
 /*  1500 */    15,
};
#define YY_SHIFT_USE_DFLT (-72)
#define YY_SHIFT_COUNT (435)
#define YY_SHIFT_MIN   (-71)
#define YY_SHIFT_MAX   (1485)
static const short yy_shift_ofst[] = {
 /*     0 */     5, 1057, 1355, 1070, 1204, 1204, 1204,   90,   60,  -19,
 /*    10 */    58,   58,  186, 1204, 1204, 1204, 1204, 1204, 1204, 1204,
 /*    20 */    67,   67,  182,  336,   65,  250,  135,  263,  340,  417,
 /*    30 */   494,  571,  622,  699,  776,  827,  827,  827,  827,  827,
 /*    40 */   827,  827,  827,  827,  827,  827,  827,  827,  827,  827,
 /*    50 */   878,  827,  929,  980,  980, 1156, 1204, 1204, 1204, 1204,
 /*    60 */  1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204,
 /*    70 */  1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204,
 /*    80 */  1204, 1204, 1204, 1204, 1258, 1204, 1204, 1204, 1204, 1204,
 /*    90 */  1204, 1204, 1204, 1204, 1204, 1204, 1204, 1204,  -71,  -47,
 /*   100 */   -47,  -47,  -47,  -47,   -6,   88,  -66,   65,   65,  451,
 /*   110 */   502,  112,  112,   33,  127,  278,  -30,  -72,  -72,  -72,
 /*   120 */    11,  412,  412,  268,  608,  610,   65,   65,   65,   65,
 /*   130 */    65,   65,   65,   65,   65,   65,   65,   65,   65,   65,
 /*   140 */    65,   65,   65,   65,   65,  559,  138,  278,  127,   24,
 /*   150 */    24,   24,   24,   24,   24,  -72,  -72,  -72,  228,  341,
 /*   160 */   341,  207,  276,  300,  352,  354,  350,   65,   65,   65,
 /*   170 */    65,   65,   65,   65,   65,   65,   65,   65,   65,   65,
 /*   180 */    65,   65,   65,   65,  495,  495,  495,   65,   65,  499,
 /*   190 */    65,   65,   65,  574,   65,   65,  517,   65,   65,   65,
 /*   200 */    65,   65,   65,   65,   65,   65,   65,  566,  691,  288,
 /*   210 */   288,  288,  701,  620, 1058,  675,  603,  964,  964,  967,
 /*   220 */   603,  967,  722,  965,  936,  999,  964,  264,  999,  999,
 /*   230 */   911,  921,  434, 1196, 1119, 1119, 1202, 1202, 1119, 1222,
 /*   240 */  1184, 1126, 1244, 1244, 1244, 1244, 1119, 1251, 1126, 1222,
 /*   250 */  1184, 1184, 1126, 1119, 1251, 1145, 1237, 1119, 1119, 1251,
 /*   260 */  1275, 1119, 1251, 1119, 1251, 1275, 1203, 1203, 1203, 1264,
 /*   270 */  1275, 1203, 1211, 1203, 1264, 1203, 1203, 1194, 1216, 1194,
 /*   280 */  1216, 1194, 1216, 1194, 1216, 1119, 1119, 1206, 1275, 1250,
 /*   290 */  1250, 1275, 1221, 1225, 1224, 1226, 1126, 1336, 1347, 1363,
 /*   300 */  1363, 1371, 1371, 1371, 1371,  -72,  -72,  -72,  -72,  -72,
 /*   310 */   -72,  477,  623,  742,  819,  624,  694,   74, 1023,  221,
 /*   320 */  1055, 1065, 1077, 1087, 1080,  889, 1031,  939, 1093, 1122,
 /*   330 */  1085, 1139,  961, 1024, 1172, 1103,  821, 1384, 1392, 1374,
 /*   340 */  1255, 1385, 1331, 1386, 1378, 1381, 1285, 1277, 1298, 1287,
 /*   350 */  1393, 1289, 1396, 1414, 1293, 1286, 1340, 1341, 1312, 1397,
 /*   360 */  1388, 1304, 1426, 1423, 1407, 1323, 1291, 1379, 1406, 1380,
 /*   370 */  1375, 1391, 1330, 1415, 1418, 1421, 1329, 1337, 1422, 1390,
 /*   380 */  1424, 1425, 1427, 1429, 1395, 1419, 1430, 1400, 1405, 1431,
 /*   390 */  1432, 1433, 1342, 1435, 1436, 1438, 1437, 1339, 1439, 1441,
 /*   400 */  1440, 1434, 1443, 1343, 1445, 1442, 1446, 1444, 1445, 1450,
 /*   410 */  1451, 1452, 1453, 1454, 1459, 1455, 1460, 1462, 1458, 1461,
 /*   420 */  1463, 1466, 1465, 1461, 1467, 1469, 1470, 1471, 1473, 1354,
 /*   430 */  1370, 1376, 1377, 1474, 1485, 1484,
};
#define YY_REDUCE_USE_DFLT (-176)
#define YY_REDUCE_COUNT (310)
#define YY_REDUCE_MIN   (-175)
#define YY_REDUCE_MAX   (1234)
static const short yy_reduce_ofst[] = {
 /*     0 */  -143,  954,   86,   21,  -50,   23,   79,  134,  170, -175,
 /*    10 */   229,  260, -121,  212,  219,  291,  -54,  349,  362,  156,
 /*    20 */   309,  311,  334,   85,  224,  394,  314,  314,  314,  314,
 /*    30 */   314,  314,  314,  314,  314,  314,  314,  314,  314,  314,
 /*    40 */   314,  314,  314,  314,  314,  314,  314,  314,  314,  314,
 /*    50 */   314,  314,  314,  314,  314,  374,  441,  443,  450,  452,
 /*    60 */   515,  554,  567,  569,  572,  578,  580,  582,  584,  587,
 /*    70 */   593,  631,  644,  646,  649,  655,  657,  659,  661,  664,
 /*    80 */   708,  720,  759,  771,  810,  822,  861,  873,  912,  930,
 /*    90 */   947,  950,  957,  959,  963,  966,  968,  998,  314,  314,
 /*   100 */   314,  314,  314,  314,  314,  314,  314,  447,  -53,  166,
 /*   110 */   438,  348,  363,  314,  473,  469,  314,  314,  314,  314,
 /*   120 */   -15,   59,  101,  688,  220,  220,  525,  256,  729,  735,
 /*   130 */   736,  740,  741,  744,  645,  448,  738,  458,  786,  503,
 /*   140 */   780,  656,  721,  724,  792,  545,  568,  706,  683,  681,
 /*   150 */   779,  784,  830,  831,  835,  678,  601, -104,   -2,   96,
 /*   160 */   111,  218,  287,  308,  310,  312,  335,  411,  453,  461,
 /*   170 */   573,  599,  617,  658,  665,  670,  732,  734,  775,  848,
 /*   180 */   875,  892,  893,  898,  332,  420,  869,  931,  944,  886,
 /*   190 */   983,  992, 1009,  958, 1017, 1028,  988, 1033, 1034, 1035,
 /*   200 */   287, 1036, 1044, 1045, 1047, 1049, 1056,  915,  972,  997,
 /*   210 */  1000, 1002,  886, 1011, 1015, 1061, 1013, 1001, 1003,  977,
 /*   220 */  1018,  979, 1050, 1041, 1040, 1052, 1014, 1004, 1059, 1060,
 /*   230 */  1032, 1038, 1084,  995, 1089, 1090, 1008, 1016, 1092, 1037,
 /*   240 */  1068, 1062, 1069, 1072, 1073, 1074, 1105, 1112, 1071, 1048,
 /*   250 */  1081, 1088, 1078, 1116, 1118, 1046, 1066, 1128, 1136, 1140,
 /*   260 */  1120, 1147, 1146, 1148, 1150, 1130, 1135, 1137, 1138, 1132,
 /*   270 */  1141, 1142, 1143, 1149, 1144, 1153, 1154, 1104, 1107, 1108,
 /*   280 */  1114, 1115, 1117, 1123, 1125, 1173, 1176, 1121, 1165, 1127,
 /*   290 */  1131, 1167, 1157, 1151, 1158, 1166, 1168, 1212, 1214, 1227,
 /*   300 */  1228, 1231, 1232, 1233, 1234, 1152, 1155, 1159, 1198, 1199,
 /*   310 */  1219,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   982, 1300, 1300, 1300, 1214, 1214, 1214, 1305, 1300, 1109,
 /*    10 */  1138, 1138, 1274, 1305, 1305, 1305, 1305, 1305, 1305, 1212,
 /*    20 */  1305, 1305, 1305, 1300, 1305, 1113, 1144, 1305, 1305, 1305,
 /*    30 */  1305, 1305, 1305, 1305, 1305, 1273, 1275, 1152, 1151, 1254,
 /*    40 */  1125, 1149, 1142, 1146, 1215, 1208, 1209, 1207, 1211, 1216,
 /*    50 */  1305, 1145, 1177, 1192, 1176, 1305, 1305, 1305, 1305, 1305,
 /*    60 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*    70 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*    80 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*    90 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1186, 1191,
 /*   100 */  1198, 1190, 1187, 1179, 1178, 1180, 1181, 1305, 1305, 1008,
 /*   110 */  1074, 1305, 1305, 1182, 1305, 1020, 1183, 1195, 1194, 1193,
 /*   120 */  1015, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   130 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   140 */  1305, 1305, 1305, 1305, 1305,  982, 1300, 1305, 1305, 1300,
 /*   150 */  1300, 1300, 1300, 1300, 1300, 1292, 1113, 1103, 1305, 1305,
 /*   160 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1280, 1278,
 /*   170 */  1305, 1227, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   180 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   190 */  1305, 1305, 1305, 1109, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   200 */  1305, 1305, 1305, 1305, 1305, 1305,  988, 1305, 1247, 1109,
 /*   210 */  1109, 1109, 1111, 1089, 1101,  990, 1148, 1127, 1127, 1259,
 /*   220 */  1148, 1259, 1045, 1068, 1042, 1138, 1127, 1210, 1138, 1138,
 /*   230 */  1110, 1101, 1305, 1285, 1118, 1118, 1277, 1277, 1118, 1157,
 /*   240 */  1078, 1148, 1085, 1085, 1085, 1085, 1118, 1005, 1148, 1157,
 /*   250 */  1078, 1078, 1148, 1118, 1005, 1253, 1251, 1118, 1118, 1005,
 /*   260 */  1220, 1118, 1005, 1118, 1005, 1220, 1076, 1076, 1076, 1060,
 /*   270 */  1220, 1076, 1045, 1076, 1060, 1076, 1076, 1131, 1126, 1131,
 /*   280 */  1126, 1131, 1126, 1131, 1126, 1118, 1118, 1305, 1220, 1224,
 /*   290 */  1224, 1220, 1143, 1132, 1141, 1139, 1148, 1011, 1063,  998,
 /*   300 */   998,  987,  987,  987,  987, 1297, 1297, 1292, 1047, 1047,
 /*   310 */  1030, 1305, 1305, 1305, 1305, 1305, 1305, 1022, 1305, 1229,
 /*   320 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   330 */  1305, 1305, 1305, 1305, 1305, 1305, 1164, 1305,  983, 1287,
 /*   340 */  1305, 1305, 1284, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   350 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   360 */  1305, 1257, 1305, 1305, 1305, 1305, 1305, 1305, 1250, 1249,
 /*   370 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   380 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305,
 /*   390 */  1305, 1305, 1092, 1305, 1305, 1305, 1096, 1305, 1305, 1305,
 /*   400 */  1305, 1305, 1305, 1305, 1140, 1305, 1133, 1305, 1213, 1305,
 /*   410 */  1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1305, 1302,
 /*   420 */  1305, 1305, 1305, 1301, 1305, 1305, 1305, 1305, 1305, 1166,
 /*   430 */  1305, 1165, 1169, 1305,  996, 1305,
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
    0,  /*          $ => nothing */
    0,  /*       SEMI => nothing */
   27,  /*    EXPLAIN => ID */
   27,  /*      QUERY => ID */
   27,  /*       PLAN => ID */
   27,  /*      BEGIN => ID */
    0,  /* TRANSACTION => nothing */
   27,  /*   DEFERRED => ID */
   27,  /*  IMMEDIATE => ID */
   27,  /*  EXCLUSIVE => ID */
    0,  /*     COMMIT => nothing */
   27,  /*        END => ID */
   27,  /*   ROLLBACK => ID */
   27,  /*  SAVEPOINT => ID */
   27,  /*    RELEASE => ID */
    0,  /*         TO => nothing */
    0,  /*      TABLE => nothing */
    0,  /*     CREATE => nothing */
   27,  /*         IF => ID */
    0,  /*        NOT => nothing */
    0,  /*     EXISTS => nothing */
   27,  /*       TEMP => ID */
    0,  /*         LP => nothing */
    0,  /*         RP => nothing */
    0,  /*         AS => nothing */
   27,  /*    WITHOUT => ID */
    0,  /*      COMMA => nothing */
    0,  /*         ID => nothing */
    0,  /*    INDEXED => nothing */
   27,  /*      ABORT => ID */
   27,  /*     ACTION => ID */
   27,  /*      AFTER => ID */
   27,  /*    ANALYZE => ID */
   27,  /*        ASC => ID */
   27,  /*     ATTACH => ID */
   27,  /*     BEFORE => ID */
   27,  /*         BY => ID */
   27,  /*    CASCADE => ID */
   27,  /*       CAST => ID */
   27,  /*   COLUMNKW => ID */
   27,  /*   CONFLICT => ID */
   27,  /*   DATABASE => ID */
   27,  /*       DESC => ID */
   27,  /*     DETACH => ID */
   27,  /*       EACH => ID */
   27,  /*       FAIL => ID */
   27,  /*        FOR => ID */
   27,  /*     IGNORE => ID */
   27,  /*  INITIALLY => ID */
   27,  /*    INSTEAD => ID */
   27,  /*    LIKE_KW => ID */
   27,  /*      MATCH => ID */
   27,  /*         NO => ID */
   27,  /*        KEY => ID */
   27,  /*         OF => ID */
   27,  /*     OFFSET => ID */
   27,  /*     PRAGMA => ID */
   27,  /*      RAISE => ID */
   27,  /*  RECURSIVE => ID */
   27,  /*    REPLACE => ID */
   27,  /*   RESTRICT => ID */
   27,  /*        ROW => ID */
   27,  /*    TRIGGER => ID */
   27,  /*     VACUUM => ID */
   27,  /*       VIEW => ID */
   27,  /*    VIRTUAL => ID */
   27,  /*       WITH => ID */
   27,  /*    REINDEX => ID */
   27,  /*     RENAME => ID */
   27,  /*   CTIME_KW => ID */
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
  sqlite3ParserARG_SDECL                /* A place to hold %extra_argument */
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
void sqlite3ParserTrace(FILE *TraceFILE, char *zTracePrompt){
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
  "$",             "SEMI",          "EXPLAIN",       "QUERY",       
  "PLAN",          "BEGIN",         "TRANSACTION",   "DEFERRED",    
  "IMMEDIATE",     "EXCLUSIVE",     "COMMIT",        "END",         
  "ROLLBACK",      "SAVEPOINT",     "RELEASE",       "TO",          
  "TABLE",         "CREATE",        "IF",            "NOT",         
  "EXISTS",        "TEMP",          "LP",            "RP",          
  "AS",            "WITHOUT",       "COMMA",         "ID",          
  "INDEXED",       "ABORT",         "ACTION",        "AFTER",       
  "ANALYZE",       "ASC",           "ATTACH",        "BEFORE",      
  "BY",            "CASCADE",       "CAST",          "COLUMNKW",    
  "CONFLICT",      "DATABASE",      "DESC",          "DETACH",      
  "EACH",          "FAIL",          "FOR",           "IGNORE",      
  "INITIALLY",     "INSTEAD",       "LIKE_KW",       "MATCH",       
  "NO",            "KEY",           "OF",            "OFFSET",      
  "PRAGMA",        "RAISE",         "RECURSIVE",     "REPLACE",     
  "RESTRICT",      "ROW",           "TRIGGER",       "VACUUM",      
  "VIEW",          "VIRTUAL",       "WITH",          "REINDEX",     
  "RENAME",        "CTIME_KW",      "ANY",           "OR",          
  "AND",           "IS",            "BETWEEN",       "IN",          
  "ISNULL",        "NOTNULL",       "NE",            "EQ",          
  "GT",            "LE",            "LT",            "GE",          
  "ESCAPE",        "BITAND",        "BITOR",         "LSHIFT",      
  "RSHIFT",        "PLUS",          "MINUS",         "STAR",        
  "SLASH",         "REM",           "CONCAT",        "COLLATE",     
  "BITNOT",        "STRING",        "JOIN_KW",       "CONSTRAINT",  
  "DEFAULT",       "NULL",          "PRIMARY",       "UNIQUE",      
  "CHECK",         "REFERENCES",    "AUTOINCR",      "ON",          
  "INSERT",        "DELETE",        "UPDATE",        "SET",         
  "DEFERRABLE",    "FOREIGN",       "DROP",          "UNION",       
  "ALL",           "EXCEPT",        "INTERSECT",     "SELECT",      
  "VALUES",        "DISTINCT",      "DOT",           "FROM",        
  "JOIN",          "USING",         "ORDER",         "GROUP",       
  "HAVING",        "LIMIT",         "WHERE",         "INTO",        
  "INTEGER",       "FLOAT",         "BLOB",          "VARIABLE",    
  "CASE",          "WHEN",          "THEN",          "ELSE",        
  "INDEX",         "ALTER",         "ADD",           "error",       
  "input",         "cmdlist",       "ecmd",          "explain",     
  "cmdx",          "cmd",           "transtype",     "trans_opt",   
  "nm",            "savepoint_opt",  "create_table",  "create_table_args",
  "createkw",      "temp",          "ifnotexists",   "dbnm",        
  "columnlist",    "conslist_opt",  "table_options",  "select",      
  "column",        "columnid",      "type",          "carglist",    
  "typetoken",     "typename",      "signed",        "plus_num",    
  "minus_num",     "ccons",         "term",          "expr",        
  "onconf",        "sortorder",     "autoinc",       "eidlist_opt", 
  "refargs",       "defer_subclause",  "refarg",        "refact",      
  "init_deferred_pred_opt",  "conslist",      "tconscomma",    "tcons",       
  "sortlist",      "eidlist",       "defer_subclause_opt",  "orconf",      
  "resolvetype",   "raisetype",     "ifexists",      "fullname",    
  "selectnowith",  "oneselect",     "with",          "multiselect_op",
  "distinct",      "selcollist",    "from",          "where_opt",   
  "groupby_opt",   "having_opt",    "orderby_opt",   "limit_opt",   
  "values",        "nexprlist",     "exprlist",      "sclp",        
  "as",            "seltablist",    "stl_prefix",    "joinop",      
  "indexed_opt",   "on_opt",        "using_opt",     "joinop2",     
  "idlist",        "setlist",       "insert_cmd",    "idlist_opt",  
  "likeop",        "between_op",    "in_op",         "case_operand",
  "case_exprlist",  "case_else",     "uniqueflag",    "collate",     
  "nmnum",         "trigger_decl",  "trigger_cmd_list",  "trigger_time",
  "trigger_event",  "foreach_clause",  "when_clause",   "trigger_cmd", 
  "trnm",          "tridxby",       "database_kw_opt",  "key_opt",     
  "add_column_fullname",  "kwcolumn_opt",  "create_vtab",   "vtabarglist", 
  "vtabarg",       "vtabargtoken",  "lp",            "anylist",     
  "wqlist",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "input ::= cmdlist",
 /*   1 */ "cmdlist ::= cmdlist ecmd",
 /*   2 */ "cmdlist ::= ecmd",
 /*   3 */ "ecmd ::= SEMI",
 /*   4 */ "ecmd ::= explain cmdx SEMI",
 /*   5 */ "explain ::=",
 /*   6 */ "explain ::= EXPLAIN",
 /*   7 */ "explain ::= EXPLAIN QUERY PLAN",
 /*   8 */ "cmdx ::= cmd",
 /*   9 */ "cmd ::= BEGIN transtype trans_opt",
 /*  10 */ "trans_opt ::=",
 /*  11 */ "trans_opt ::= TRANSACTION",
 /*  12 */ "trans_opt ::= TRANSACTION nm",
 /*  13 */ "transtype ::=",
 /*  14 */ "transtype ::= DEFERRED",
 /*  15 */ "transtype ::= IMMEDIATE",
 /*  16 */ "transtype ::= EXCLUSIVE",
 /*  17 */ "cmd ::= COMMIT trans_opt",
 /*  18 */ "cmd ::= END trans_opt",
 /*  19 */ "cmd ::= ROLLBACK trans_opt",
 /*  20 */ "savepoint_opt ::= SAVEPOINT",
 /*  21 */ "savepoint_opt ::=",
 /*  22 */ "cmd ::= SAVEPOINT nm",
 /*  23 */ "cmd ::= RELEASE savepoint_opt nm",
 /*  24 */ "cmd ::= ROLLBACK trans_opt TO savepoint_opt nm",
 /*  25 */ "cmd ::= create_table create_table_args",
 /*  26 */ "create_table ::= createkw temp TABLE ifnotexists nm dbnm",
 /*  27 */ "createkw ::= CREATE",
 /*  28 */ "ifnotexists ::=",
 /*  29 */ "ifnotexists ::= IF NOT EXISTS",
 /*  30 */ "temp ::= TEMP",
 /*  31 */ "temp ::=",
 /*  32 */ "create_table_args ::= LP columnlist conslist_opt RP table_options",
 /*  33 */ "create_table_args ::= AS select",
 /*  34 */ "table_options ::=",
 /*  35 */ "table_options ::= WITHOUT nm",
 /*  36 */ "columnlist ::= columnlist COMMA column",
 /*  37 */ "columnlist ::= column",
 /*  38 */ "column ::= columnid type carglist",
 /*  39 */ "columnid ::= nm",
 /*  40 */ "nm ::= ID|INDEXED",
 /*  41 */ "nm ::= STRING",
 /*  42 */ "nm ::= JOIN_KW",
 /*  43 */ "type ::=",
 /*  44 */ "type ::= typetoken",
 /*  45 */ "typetoken ::= typename",
 /*  46 */ "typetoken ::= typename LP signed RP",
 /*  47 */ "typetoken ::= typename LP signed COMMA signed RP",
 /*  48 */ "typename ::= ID|STRING",
 /*  49 */ "typename ::= typename ID|STRING",
 /*  50 */ "signed ::= plus_num",
 /*  51 */ "signed ::= minus_num",
 /*  52 */ "carglist ::= carglist ccons",
 /*  53 */ "carglist ::=",
 /*  54 */ "ccons ::= CONSTRAINT nm",
 /*  55 */ "ccons ::= DEFAULT term",
 /*  56 */ "ccons ::= DEFAULT LP expr RP",
 /*  57 */ "ccons ::= DEFAULT PLUS term",
 /*  58 */ "ccons ::= DEFAULT MINUS term",
 /*  59 */ "ccons ::= DEFAULT ID|INDEXED",
 /*  60 */ "ccons ::= NULL onconf",
 /*  61 */ "ccons ::= NOT NULL onconf",
 /*  62 */ "ccons ::= PRIMARY KEY sortorder onconf autoinc",
 /*  63 */ "ccons ::= UNIQUE onconf",
 /*  64 */ "ccons ::= CHECK LP expr RP",
 /*  65 */ "ccons ::= REFERENCES nm eidlist_opt refargs",
 /*  66 */ "ccons ::= defer_subclause",
 /*  67 */ "ccons ::= COLLATE ID|STRING",
 /*  68 */ "autoinc ::=",
 /*  69 */ "autoinc ::= AUTOINCR",
 /*  70 */ "refargs ::=",
 /*  71 */ "refargs ::= refargs refarg",
 /*  72 */ "refarg ::= MATCH nm",
 /*  73 */ "refarg ::= ON INSERT refact",
 /*  74 */ "refarg ::= ON DELETE refact",
 /*  75 */ "refarg ::= ON UPDATE refact",
 /*  76 */ "refact ::= SET NULL",
 /*  77 */ "refact ::= SET DEFAULT",
 /*  78 */ "refact ::= CASCADE",
 /*  79 */ "refact ::= RESTRICT",
 /*  80 */ "refact ::= NO ACTION",
 /*  81 */ "defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt",
 /*  82 */ "defer_subclause ::= DEFERRABLE init_deferred_pred_opt",
 /*  83 */ "init_deferred_pred_opt ::=",
 /*  84 */ "init_deferred_pred_opt ::= INITIALLY DEFERRED",
 /*  85 */ "init_deferred_pred_opt ::= INITIALLY IMMEDIATE",
 /*  86 */ "conslist_opt ::=",
 /*  87 */ "conslist_opt ::= COMMA conslist",
 /*  88 */ "conslist ::= conslist tconscomma tcons",
 /*  89 */ "conslist ::= tcons",
 /*  90 */ "tconscomma ::= COMMA",
 /*  91 */ "tconscomma ::=",
 /*  92 */ "tcons ::= CONSTRAINT nm",
 /*  93 */ "tcons ::= PRIMARY KEY LP sortlist autoinc RP onconf",
 /*  94 */ "tcons ::= UNIQUE LP sortlist RP onconf",
 /*  95 */ "tcons ::= CHECK LP expr RP onconf",
 /*  96 */ "tcons ::= FOREIGN KEY LP eidlist RP REFERENCES nm eidlist_opt refargs defer_subclause_opt",
 /*  97 */ "defer_subclause_opt ::=",
 /*  98 */ "defer_subclause_opt ::= defer_subclause",
 /*  99 */ "onconf ::=",
 /* 100 */ "onconf ::= ON CONFLICT resolvetype",
 /* 101 */ "orconf ::=",
 /* 102 */ "orconf ::= OR resolvetype",
 /* 103 */ "resolvetype ::= raisetype",
 /* 104 */ "resolvetype ::= IGNORE",
 /* 105 */ "resolvetype ::= REPLACE",
 /* 106 */ "cmd ::= DROP TABLE ifexists fullname",
 /* 107 */ "ifexists ::= IF EXISTS",
 /* 108 */ "ifexists ::=",
 /* 109 */ "cmd ::= createkw temp VIEW ifnotexists nm dbnm eidlist_opt AS select",
 /* 110 */ "cmd ::= DROP VIEW ifexists fullname",
 /* 111 */ "cmd ::= select",
 /* 112 */ "select ::= with selectnowith",
 /* 113 */ "selectnowith ::= oneselect",
 /* 114 */ "selectnowith ::= selectnowith multiselect_op oneselect",
 /* 115 */ "multiselect_op ::= UNION",
 /* 116 */ "multiselect_op ::= UNION ALL",
 /* 117 */ "multiselect_op ::= EXCEPT|INTERSECT",
 /* 118 */ "oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt orderby_opt limit_opt",
 /* 119 */ "oneselect ::= values",
 /* 120 */ "values ::= VALUES LP nexprlist RP",
 /* 121 */ "values ::= values COMMA LP exprlist RP",
 /* 122 */ "distinct ::= DISTINCT",
 /* 123 */ "distinct ::= ALL",
 /* 124 */ "distinct ::=",
 /* 125 */ "sclp ::= selcollist COMMA",
 /* 126 */ "sclp ::=",
 /* 127 */ "selcollist ::= sclp expr as",
 /* 128 */ "selcollist ::= sclp STAR",
 /* 129 */ "selcollist ::= sclp nm DOT STAR",
 /* 130 */ "as ::= AS nm",
 /* 131 */ "as ::= ID|STRING",
 /* 132 */ "as ::=",
 /* 133 */ "from ::=",
 /* 134 */ "from ::= FROM seltablist",
 /* 135 */ "stl_prefix ::= seltablist joinop",
 /* 136 */ "stl_prefix ::=",
 /* 137 */ "seltablist ::= stl_prefix nm dbnm as indexed_opt on_opt using_opt",
 /* 138 */ "seltablist ::= stl_prefix nm dbnm LP exprlist RP as on_opt using_opt",
 /* 139 */ "seltablist ::= stl_prefix LP select RP as on_opt using_opt",
 /* 140 */ "seltablist ::= stl_prefix LP seltablist RP as on_opt using_opt",
 /* 141 */ "dbnm ::=",
 /* 142 */ "dbnm ::= DOT nm",
 /* 143 */ "fullname ::= nm dbnm",
 /* 144 */ "joinop ::= COMMA|JOIN",
 /* 145 */ "joinop ::= JOIN_KW JOIN",
 /* 146 */ "joinop ::= JOIN_KW nm JOIN",
 /* 147 */ "joinop ::= JOIN_KW nm nm JOIN",
 /* 148 */ "on_opt ::= ON expr",
 /* 149 */ "on_opt ::=",
 /* 150 */ "indexed_opt ::=",
 /* 151 */ "indexed_opt ::= INDEXED BY nm",
 /* 152 */ "indexed_opt ::= NOT INDEXED",
 /* 153 */ "using_opt ::= USING LP idlist RP",
 /* 154 */ "using_opt ::=",
 /* 155 */ "orderby_opt ::=",
 /* 156 */ "orderby_opt ::= ORDER BY sortlist",
 /* 157 */ "sortlist ::= sortlist COMMA expr sortorder",
 /* 158 */ "sortlist ::= expr sortorder",
 /* 159 */ "sortorder ::= ASC",
 /* 160 */ "sortorder ::= DESC",
 /* 161 */ "sortorder ::=",
 /* 162 */ "groupby_opt ::=",
 /* 163 */ "groupby_opt ::= GROUP BY nexprlist",
 /* 164 */ "having_opt ::=",
 /* 165 */ "having_opt ::= HAVING expr",
 /* 166 */ "limit_opt ::=",
 /* 167 */ "limit_opt ::= LIMIT expr",
 /* 168 */ "limit_opt ::= LIMIT expr OFFSET expr",
 /* 169 */ "limit_opt ::= LIMIT expr COMMA expr",
 /* 170 */ "cmd ::= with DELETE FROM fullname indexed_opt where_opt",
 /* 171 */ "where_opt ::=",
 /* 172 */ "where_opt ::= WHERE expr",
 /* 173 */ "cmd ::= with UPDATE orconf fullname indexed_opt SET setlist where_opt",
 /* 174 */ "setlist ::= setlist COMMA nm EQ expr",
 /* 175 */ "setlist ::= nm EQ expr",
 /* 176 */ "cmd ::= with insert_cmd INTO fullname idlist_opt select",
 /* 177 */ "cmd ::= with insert_cmd INTO fullname idlist_opt DEFAULT VALUES",
 /* 178 */ "insert_cmd ::= INSERT orconf",
 /* 179 */ "insert_cmd ::= REPLACE",
 /* 180 */ "idlist_opt ::=",
 /* 181 */ "idlist_opt ::= LP idlist RP",
 /* 182 */ "idlist ::= idlist COMMA nm",
 /* 183 */ "idlist ::= nm",
 /* 184 */ "expr ::= term",
 /* 185 */ "expr ::= LP expr RP",
 /* 186 */ "term ::= NULL",
 /* 187 */ "expr ::= ID|INDEXED",
 /* 188 */ "expr ::= JOIN_KW",
 /* 189 */ "expr ::= nm DOT nm",
 /* 190 */ "expr ::= nm DOT nm DOT nm",
 /* 191 */ "term ::= INTEGER|FLOAT|BLOB",
 /* 192 */ "term ::= STRING",
 /* 193 */ "expr ::= VARIABLE",
 /* 194 */ "expr ::= expr COLLATE ID|STRING",
 /* 195 */ "expr ::= CAST LP expr AS typetoken RP",
 /* 196 */ "expr ::= ID|INDEXED LP distinct exprlist RP",
 /* 197 */ "expr ::= ID|INDEXED LP STAR RP",
 /* 198 */ "term ::= CTIME_KW",
 /* 199 */ "expr ::= expr AND expr",
 /* 200 */ "expr ::= expr OR expr",
 /* 201 */ "expr ::= expr LT|GT|GE|LE expr",
 /* 202 */ "expr ::= expr EQ|NE expr",
 /* 203 */ "expr ::= expr BITAND|BITOR|LSHIFT|RSHIFT expr",
 /* 204 */ "expr ::= expr PLUS|MINUS expr",
 /* 205 */ "expr ::= expr STAR|SLASH|REM expr",
 /* 206 */ "expr ::= expr CONCAT expr",
 /* 207 */ "likeop ::= LIKE_KW|MATCH",
 /* 208 */ "likeop ::= NOT LIKE_KW|MATCH",
 /* 209 */ "expr ::= expr likeop expr",
 /* 210 */ "expr ::= expr likeop expr ESCAPE expr",
 /* 211 */ "expr ::= expr ISNULL|NOTNULL",
 /* 212 */ "expr ::= expr NOT NULL",
 /* 213 */ "expr ::= expr IS expr",
 /* 214 */ "expr ::= expr IS NOT expr",
 /* 215 */ "expr ::= NOT expr",
 /* 216 */ "expr ::= BITNOT expr",
 /* 217 */ "expr ::= MINUS expr",
 /* 218 */ "expr ::= PLUS expr",
 /* 219 */ "between_op ::= BETWEEN",
 /* 220 */ "between_op ::= NOT BETWEEN",
 /* 221 */ "expr ::= expr between_op expr AND expr",
 /* 222 */ "in_op ::= IN",
 /* 223 */ "in_op ::= NOT IN",
 /* 224 */ "expr ::= expr in_op LP exprlist RP",
 /* 225 */ "expr ::= LP select RP",
 /* 226 */ "expr ::= expr in_op LP select RP",
 /* 227 */ "expr ::= expr in_op nm dbnm",
 /* 228 */ "expr ::= EXISTS LP select RP",
 /* 229 */ "expr ::= CASE case_operand case_exprlist case_else END",
 /* 230 */ "case_exprlist ::= case_exprlist WHEN expr THEN expr",
 /* 231 */ "case_exprlist ::= WHEN expr THEN expr",
 /* 232 */ "case_else ::= ELSE expr",
 /* 233 */ "case_else ::=",
 /* 234 */ "case_operand ::= expr",
 /* 235 */ "case_operand ::=",
 /* 236 */ "exprlist ::= nexprlist",
 /* 237 */ "exprlist ::=",
 /* 238 */ "nexprlist ::= nexprlist COMMA expr",
 /* 239 */ "nexprlist ::= expr",
 /* 240 */ "cmd ::= createkw uniqueflag INDEX ifnotexists nm dbnm ON nm LP sortlist RP where_opt",
 /* 241 */ "uniqueflag ::= UNIQUE",
 /* 242 */ "uniqueflag ::=",
 /* 243 */ "eidlist_opt ::=",
 /* 244 */ "eidlist_opt ::= LP eidlist RP",
 /* 245 */ "eidlist ::= eidlist COMMA nm collate sortorder",
 /* 246 */ "eidlist ::= nm collate sortorder",
 /* 247 */ "collate ::=",
 /* 248 */ "collate ::= COLLATE ID|STRING",
 /* 249 */ "cmd ::= DROP INDEX ifexists fullname",
 /* 250 */ "cmd ::= VACUUM",
 /* 251 */ "cmd ::= VACUUM nm",
 /* 252 */ "cmd ::= PRAGMA nm dbnm",
 /* 253 */ "cmd ::= PRAGMA nm dbnm EQ nmnum",
 /* 254 */ "cmd ::= PRAGMA nm dbnm LP nmnum RP",
 /* 255 */ "cmd ::= PRAGMA nm dbnm EQ minus_num",
 /* 256 */ "cmd ::= PRAGMA nm dbnm LP minus_num RP",
 /* 257 */ "nmnum ::= plus_num",
 /* 258 */ "nmnum ::= nm",
 /* 259 */ "nmnum ::= ON",
 /* 260 */ "nmnum ::= DELETE",
 /* 261 */ "nmnum ::= DEFAULT",
 /* 262 */ "plus_num ::= PLUS INTEGER|FLOAT",
 /* 263 */ "plus_num ::= INTEGER|FLOAT",
 /* 264 */ "minus_num ::= MINUS INTEGER|FLOAT",
 /* 265 */ "cmd ::= createkw trigger_decl BEGIN trigger_cmd_list END",
 /* 266 */ "trigger_decl ::= temp TRIGGER ifnotexists nm dbnm trigger_time trigger_event ON fullname foreach_clause when_clause",
 /* 267 */ "trigger_time ::= BEFORE",
 /* 268 */ "trigger_time ::= AFTER",
 /* 269 */ "trigger_time ::= INSTEAD OF",
 /* 270 */ "trigger_time ::=",
 /* 271 */ "trigger_event ::= DELETE|INSERT",
 /* 272 */ "trigger_event ::= UPDATE",
 /* 273 */ "trigger_event ::= UPDATE OF idlist",
 /* 274 */ "foreach_clause ::=",
 /* 275 */ "foreach_clause ::= FOR EACH ROW",
 /* 276 */ "when_clause ::=",
 /* 277 */ "when_clause ::= WHEN expr",
 /* 278 */ "trigger_cmd_list ::= trigger_cmd_list trigger_cmd SEMI",
 /* 279 */ "trigger_cmd_list ::= trigger_cmd SEMI",
 /* 280 */ "trnm ::= nm",
 /* 281 */ "trnm ::= nm DOT nm",
 /* 282 */ "tridxby ::=",
 /* 283 */ "tridxby ::= INDEXED BY nm",
 /* 284 */ "tridxby ::= NOT INDEXED",
 /* 285 */ "trigger_cmd ::= UPDATE orconf trnm tridxby SET setlist where_opt",
 /* 286 */ "trigger_cmd ::= insert_cmd INTO trnm idlist_opt select",
 /* 287 */ "trigger_cmd ::= DELETE FROM trnm tridxby where_opt",
 /* 288 */ "trigger_cmd ::= select",
 /* 289 */ "expr ::= RAISE LP IGNORE RP",
 /* 290 */ "expr ::= RAISE LP raisetype COMMA nm RP",
 /* 291 */ "raisetype ::= ROLLBACK",
 /* 292 */ "raisetype ::= ABORT",
 /* 293 */ "raisetype ::= FAIL",
 /* 294 */ "cmd ::= DROP TRIGGER ifexists fullname",
 /* 295 */ "cmd ::= ATTACH database_kw_opt expr AS expr key_opt",
 /* 296 */ "cmd ::= DETACH database_kw_opt expr",
 /* 297 */ "key_opt ::=",
 /* 298 */ "key_opt ::= KEY expr",
 /* 299 */ "database_kw_opt ::= DATABASE",
 /* 300 */ "database_kw_opt ::=",
 /* 301 */ "cmd ::= REINDEX",
 /* 302 */ "cmd ::= REINDEX nm dbnm",
 /* 303 */ "cmd ::= ANALYZE",
 /* 304 */ "cmd ::= ANALYZE nm dbnm",
 /* 305 */ "cmd ::= ALTER TABLE fullname RENAME TO nm",
 /* 306 */ "cmd ::= ALTER TABLE add_column_fullname ADD kwcolumn_opt column",
 /* 307 */ "add_column_fullname ::= fullname",
 /* 308 */ "kwcolumn_opt ::=",
 /* 309 */ "kwcolumn_opt ::= COLUMNKW",
 /* 310 */ "cmd ::= create_vtab",
 /* 311 */ "cmd ::= create_vtab LP vtabarglist RP",
 /* 312 */ "create_vtab ::= createkw VIRTUAL TABLE ifnotexists nm dbnm USING nm",
 /* 313 */ "vtabarglist ::= vtabarg",
 /* 314 */ "vtabarglist ::= vtabarglist COMMA vtabarg",
 /* 315 */ "vtabarg ::=",
 /* 316 */ "vtabarg ::= vtabarg vtabargtoken",
 /* 317 */ "vtabargtoken ::= ANY",
 /* 318 */ "vtabargtoken ::= lp anylist RP",
 /* 319 */ "lp ::= LP",
 /* 320 */ "anylist ::=",
 /* 321 */ "anylist ::= anylist LP anylist RP",
 /* 322 */ "anylist ::= anylist ANY",
 /* 323 */ "with ::=",
 /* 324 */ "with ::= WITH wqlist",
 /* 325 */ "with ::= WITH RECURSIVE wqlist",
 /* 326 */ "wqlist ::= nm eidlist_opt AS LP select RP",
 /* 327 */ "wqlist ::= wqlist COMMA nm eidlist_opt AS LP select RP",
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
** to sqlite3Parser and sqlite3ParserFree.
*/
void *sqlite3ParserAlloc(void *(*mallocProc)(u64)){
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
  sqlite3ParserARG_FETCH;
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
    case 163: /* select */
    case 196: /* selectnowith */
    case 197: /* oneselect */
    case 208: /* values */
{
#line 407 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3SelectDelete(pParse->db, (yypminor->yy3));
#line 1446 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 174: /* term */
    case 175: /* expr */
{
#line 825 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3ExprDelete(pParse->db, (yypminor->yy346).pExpr);
#line 1454 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 179: /* eidlist_opt */
    case 188: /* sortlist */
    case 189: /* eidlist */
    case 201: /* selcollist */
    case 204: /* groupby_opt */
    case 206: /* orderby_opt */
    case 209: /* nexprlist */
    case 210: /* exprlist */
    case 211: /* sclp */
    case 221: /* setlist */
    case 228: /* case_exprlist */
{
#line 1245 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3ExprListDelete(pParse->db, (yypminor->yy14));
#line 1471 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 195: /* fullname */
    case 202: /* from */
    case 213: /* seltablist */
    case 214: /* stl_prefix */
{
#line 638 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3SrcListDelete(pParse->db, (yypminor->yy65));
#line 1481 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 198: /* with */
    case 252: /* wqlist */
{
#line 1523 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3WithDelete(pParse->db, (yypminor->yy59));
#line 1489 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 203: /* where_opt */
    case 205: /* having_opt */
    case 217: /* on_opt */
    case 227: /* case_operand */
    case 229: /* case_else */
    case 238: /* when_clause */
    case 243: /* key_opt */
{
#line 752 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3ExprDelete(pParse->db, (yypminor->yy132));
#line 1502 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 218: /* using_opt */
    case 220: /* idlist */
    case 223: /* idlist_opt */
{
#line 670 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3IdListDelete(pParse->db, (yypminor->yy408));
#line 1511 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 234: /* trigger_cmd_list */
    case 239: /* trigger_cmd */
{
#line 1359 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3DeleteTriggerStep(pParse->db, (yypminor->yy473));
#line 1519 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
    case 236: /* trigger_event */
{
#line 1345 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
sqlite3IdListDelete(pParse->db, (yypminor->yy378).b);
#line 1526 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
}
      break;
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
**       obtained from sqlite3ParserAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void sqlite3ParserFree(
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
int sqlite3ParserStackPeak(void *p){
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
   sqlite3ParserARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
#line 37 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  UNUSED_PARAMETER(yypMinor); /* Silence some compiler warnings */
  sqlite3ErrorMsg(pParse, "parser stack overflow");
#line 1716 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
   sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument var */
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
  { 144, 1 },
  { 145, 2 },
  { 145, 1 },
  { 146, 1 },
  { 146, 3 },
  { 147, 0 },
  { 147, 1 },
  { 147, 3 },
  { 148, 1 },
  { 149, 3 },
  { 151, 0 },
  { 151, 1 },
  { 151, 2 },
  { 150, 0 },
  { 150, 1 },
  { 150, 1 },
  { 150, 1 },
  { 149, 2 },
  { 149, 2 },
  { 149, 2 },
  { 153, 1 },
  { 153, 0 },
  { 149, 2 },
  { 149, 3 },
  { 149, 5 },
  { 149, 2 },
  { 154, 6 },
  { 156, 1 },
  { 158, 0 },
  { 158, 3 },
  { 157, 1 },
  { 157, 0 },
  { 155, 5 },
  { 155, 2 },
  { 162, 0 },
  { 162, 2 },
  { 160, 3 },
  { 160, 1 },
  { 164, 3 },
  { 165, 1 },
  { 152, 1 },
  { 152, 1 },
  { 152, 1 },
  { 166, 0 },
  { 166, 1 },
  { 168, 1 },
  { 168, 4 },
  { 168, 6 },
  { 169, 1 },
  { 169, 2 },
  { 170, 1 },
  { 170, 1 },
  { 167, 2 },
  { 167, 0 },
  { 173, 2 },
  { 173, 2 },
  { 173, 4 },
  { 173, 3 },
  { 173, 3 },
  { 173, 2 },
  { 173, 2 },
  { 173, 3 },
  { 173, 5 },
  { 173, 2 },
  { 173, 4 },
  { 173, 4 },
  { 173, 1 },
  { 173, 2 },
  { 178, 0 },
  { 178, 1 },
  { 180, 0 },
  { 180, 2 },
  { 182, 2 },
  { 182, 3 },
  { 182, 3 },
  { 182, 3 },
  { 183, 2 },
  { 183, 2 },
  { 183, 1 },
  { 183, 1 },
  { 183, 2 },
  { 181, 3 },
  { 181, 2 },
  { 184, 0 },
  { 184, 2 },
  { 184, 2 },
  { 161, 0 },
  { 161, 2 },
  { 185, 3 },
  { 185, 1 },
  { 186, 1 },
  { 186, 0 },
  { 187, 2 },
  { 187, 7 },
  { 187, 5 },
  { 187, 5 },
  { 187, 10 },
  { 190, 0 },
  { 190, 1 },
  { 176, 0 },
  { 176, 3 },
  { 191, 0 },
  { 191, 2 },
  { 192, 1 },
  { 192, 1 },
  { 192, 1 },
  { 149, 4 },
  { 194, 2 },
  { 194, 0 },
  { 149, 9 },
  { 149, 4 },
  { 149, 1 },
  { 163, 2 },
  { 196, 1 },
  { 196, 3 },
  { 199, 1 },
  { 199, 2 },
  { 199, 1 },
  { 197, 9 },
  { 197, 1 },
  { 208, 4 },
  { 208, 5 },
  { 200, 1 },
  { 200, 1 },
  { 200, 0 },
  { 211, 2 },
  { 211, 0 },
  { 201, 3 },
  { 201, 2 },
  { 201, 4 },
  { 212, 2 },
  { 212, 1 },
  { 212, 0 },
  { 202, 0 },
  { 202, 2 },
  { 214, 2 },
  { 214, 0 },
  { 213, 7 },
  { 213, 9 },
  { 213, 7 },
  { 213, 7 },
  { 159, 0 },
  { 159, 2 },
  { 195, 2 },
  { 215, 1 },
  { 215, 2 },
  { 215, 3 },
  { 215, 4 },
  { 217, 2 },
  { 217, 0 },
  { 216, 0 },
  { 216, 3 },
  { 216, 2 },
  { 218, 4 },
  { 218, 0 },
  { 206, 0 },
  { 206, 3 },
  { 188, 4 },
  { 188, 2 },
  { 177, 1 },
  { 177, 1 },
  { 177, 0 },
  { 204, 0 },
  { 204, 3 },
  { 205, 0 },
  { 205, 2 },
  { 207, 0 },
  { 207, 2 },
  { 207, 4 },
  { 207, 4 },
  { 149, 6 },
  { 203, 0 },
  { 203, 2 },
  { 149, 8 },
  { 221, 5 },
  { 221, 3 },
  { 149, 6 },
  { 149, 7 },
  { 222, 2 },
  { 222, 1 },
  { 223, 0 },
  { 223, 3 },
  { 220, 3 },
  { 220, 1 },
  { 175, 1 },
  { 175, 3 },
  { 174, 1 },
  { 175, 1 },
  { 175, 1 },
  { 175, 3 },
  { 175, 5 },
  { 174, 1 },
  { 174, 1 },
  { 175, 1 },
  { 175, 3 },
  { 175, 6 },
  { 175, 5 },
  { 175, 4 },
  { 174, 1 },
  { 175, 3 },
  { 175, 3 },
  { 175, 3 },
  { 175, 3 },
  { 175, 3 },
  { 175, 3 },
  { 175, 3 },
  { 175, 3 },
  { 224, 1 },
  { 224, 2 },
  { 175, 3 },
  { 175, 5 },
  { 175, 2 },
  { 175, 3 },
  { 175, 3 },
  { 175, 4 },
  { 175, 2 },
  { 175, 2 },
  { 175, 2 },
  { 175, 2 },
  { 225, 1 },
  { 225, 2 },
  { 175, 5 },
  { 226, 1 },
  { 226, 2 },
  { 175, 5 },
  { 175, 3 },
  { 175, 5 },
  { 175, 4 },
  { 175, 4 },
  { 175, 5 },
  { 228, 5 },
  { 228, 4 },
  { 229, 2 },
  { 229, 0 },
  { 227, 1 },
  { 227, 0 },
  { 210, 1 },
  { 210, 0 },
  { 209, 3 },
  { 209, 1 },
  { 149, 12 },
  { 230, 1 },
  { 230, 0 },
  { 179, 0 },
  { 179, 3 },
  { 189, 5 },
  { 189, 3 },
  { 231, 0 },
  { 231, 2 },
  { 149, 4 },
  { 149, 1 },
  { 149, 2 },
  { 149, 3 },
  { 149, 5 },
  { 149, 6 },
  { 149, 5 },
  { 149, 6 },
  { 232, 1 },
  { 232, 1 },
  { 232, 1 },
  { 232, 1 },
  { 232, 1 },
  { 171, 2 },
  { 171, 1 },
  { 172, 2 },
  { 149, 5 },
  { 233, 11 },
  { 235, 1 },
  { 235, 1 },
  { 235, 2 },
  { 235, 0 },
  { 236, 1 },
  { 236, 1 },
  { 236, 3 },
  { 237, 0 },
  { 237, 3 },
  { 238, 0 },
  { 238, 2 },
  { 234, 3 },
  { 234, 2 },
  { 240, 1 },
  { 240, 3 },
  { 241, 0 },
  { 241, 3 },
  { 241, 2 },
  { 239, 7 },
  { 239, 5 },
  { 239, 5 },
  { 239, 1 },
  { 175, 4 },
  { 175, 6 },
  { 193, 1 },
  { 193, 1 },
  { 193, 1 },
  { 149, 4 },
  { 149, 6 },
  { 149, 3 },
  { 243, 0 },
  { 243, 2 },
  { 242, 1 },
  { 242, 0 },
  { 149, 1 },
  { 149, 3 },
  { 149, 1 },
  { 149, 3 },
  { 149, 6 },
  { 149, 6 },
  { 244, 1 },
  { 245, 0 },
  { 245, 1 },
  { 149, 1 },
  { 149, 4 },
  { 246, 8 },
  { 247, 1 },
  { 247, 3 },
  { 248, 0 },
  { 248, 2 },
  { 249, 1 },
  { 249, 3 },
  { 250, 1 },
  { 251, 0 },
  { 251, 4 },
  { 251, 2 },
  { 198, 0 },
  { 198, 2 },
  { 198, 3 },
  { 252, 6 },
  { 252, 8 },
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
  sqlite3ParserARG_FETCH;
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
#line 105 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ sqlite3BeginParse(pParse, 0); }
#line 2172 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 6: /* explain ::= EXPLAIN */
#line 107 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ sqlite3BeginParse(pParse, 1); }
#line 2177 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 7: /* explain ::= EXPLAIN QUERY PLAN */
#line 108 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ sqlite3BeginParse(pParse, 2); }
#line 2182 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 8: /* cmdx ::= cmd */
#line 110 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ sqlite3FinishCoding(pParse); }
#line 2187 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 9: /* cmd ::= BEGIN transtype trans_opt */
#line 115 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3BeginTransaction(pParse, yymsp[-1].minor.yy328);}
#line 2192 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 13: /* transtype ::= */
#line 120 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = TK_DEFERRED;}
#line 2197 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 14: /* transtype ::= DEFERRED */
      case 15: /* transtype ::= IMMEDIATE */ yytestcase(yyruleno==15);
      case 16: /* transtype ::= EXCLUSIVE */ yytestcase(yyruleno==16);
      case 115: /* multiselect_op ::= UNION */ yytestcase(yyruleno==115);
      case 117: /* multiselect_op ::= EXCEPT|INTERSECT */ yytestcase(yyruleno==117);
#line 121 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = yymsp[0].major;}
#line 2206 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 17: /* cmd ::= COMMIT trans_opt */
      case 18: /* cmd ::= END trans_opt */ yytestcase(yyruleno==18);
#line 124 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3CommitTransaction(pParse);}
#line 2212 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 19: /* cmd ::= ROLLBACK trans_opt */
#line 126 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3RollbackTransaction(pParse);}
#line 2217 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 22: /* cmd ::= SAVEPOINT nm */
#line 130 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3Savepoint(pParse, SAVEPOINT_BEGIN, &yymsp[0].minor.yy0);
}
#line 2224 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 23: /* cmd ::= RELEASE savepoint_opt nm */
#line 133 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3Savepoint(pParse, SAVEPOINT_RELEASE, &yymsp[0].minor.yy0);
}
#line 2231 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 24: /* cmd ::= ROLLBACK trans_opt TO savepoint_opt nm */
#line 136 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3Savepoint(pParse, SAVEPOINT_ROLLBACK, &yymsp[0].minor.yy0);
}
#line 2238 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 26: /* create_table ::= createkw temp TABLE ifnotexists nm dbnm */
#line 143 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
   sqlite3StartTable(pParse,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0,yymsp[-4].minor.yy328,0,0,yymsp[-2].minor.yy328);
}
#line 2245 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 27: /* createkw ::= CREATE */
#line 146 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  pParse->db->lookaside.bEnabled = 0;
  yygotominor.yy0 = yymsp[0].minor.yy0;
}
#line 2253 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 28: /* ifnotexists ::= */
      case 31: /* temp ::= */ yytestcase(yyruleno==31);
      case 68: /* autoinc ::= */ yytestcase(yyruleno==68);
      case 81: /* defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt */ yytestcase(yyruleno==81);
      case 83: /* init_deferred_pred_opt ::= */ yytestcase(yyruleno==83);
      case 85: /* init_deferred_pred_opt ::= INITIALLY IMMEDIATE */ yytestcase(yyruleno==85);
      case 97: /* defer_subclause_opt ::= */ yytestcase(yyruleno==97);
      case 108: /* ifexists ::= */ yytestcase(yyruleno==108);
      case 219: /* between_op ::= BETWEEN */ yytestcase(yyruleno==219);
      case 222: /* in_op ::= IN */ yytestcase(yyruleno==222);
      case 247: /* collate ::= */ yytestcase(yyruleno==247);
#line 151 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = 0;}
#line 2268 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 29: /* ifnotexists ::= IF NOT EXISTS */
      case 30: /* temp ::= TEMP */ yytestcase(yyruleno==30);
      case 69: /* autoinc ::= AUTOINCR */ yytestcase(yyruleno==69);
      case 84: /* init_deferred_pred_opt ::= INITIALLY DEFERRED */ yytestcase(yyruleno==84);
      case 107: /* ifexists ::= IF EXISTS */ yytestcase(yyruleno==107);
      case 220: /* between_op ::= NOT BETWEEN */ yytestcase(yyruleno==220);
      case 223: /* in_op ::= NOT IN */ yytestcase(yyruleno==223);
      case 248: /* collate ::= COLLATE ID|STRING */ yytestcase(yyruleno==248);
#line 152 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = 1;}
#line 2280 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 32: /* create_table_args ::= LP columnlist conslist_opt RP table_options */
#line 158 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3EndTable(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0,yymsp[0].minor.yy186,0);
}
#line 2287 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 33: /* create_table_args ::= AS select */
#line 161 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3EndTable(pParse,0,0,0,yymsp[0].minor.yy3);
  sqlite3SelectDelete(pParse->db, yymsp[0].minor.yy3);
}
#line 2295 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 34: /* table_options ::= */
#line 166 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy186 = 0;}
#line 2300 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 35: /* table_options ::= WITHOUT nm */
#line 167 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  if( yymsp[0].minor.yy0.n==5 && sqlite3_strnicmp(yymsp[0].minor.yy0.z,"rowid",5)==0 ){
    yygotominor.yy186 = TF_WithoutRowid | TF_NoVisibleRowid;
  }else{
    yygotominor.yy186 = 0;
    sqlite3ErrorMsg(pParse, "unknown table option: %.*s", yymsp[0].minor.yy0.n, yymsp[0].minor.yy0.z);
  }
}
#line 2312 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 38: /* column ::= columnid type carglist */
#line 183 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0.z = yymsp[-2].minor.yy0.z;
  yygotominor.yy0.n = (int)(pParse->sLastToken.z-yymsp[-2].minor.yy0.z) + pParse->sLastToken.n;
}
#line 2320 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 39: /* columnid ::= nm */
#line 187 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3AddColumn(pParse,&yymsp[0].minor.yy0);
  yygotominor.yy0 = yymsp[0].minor.yy0;
  pParse->constraintName.n = 0;
}
#line 2329 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 40: /* nm ::= ID|INDEXED */
      case 41: /* nm ::= STRING */ yytestcase(yyruleno==41);
      case 42: /* nm ::= JOIN_KW */ yytestcase(yyruleno==42);
      case 45: /* typetoken ::= typename */ yytestcase(yyruleno==45);
      case 48: /* typename ::= ID|STRING */ yytestcase(yyruleno==48);
      case 130: /* as ::= AS nm */ yytestcase(yyruleno==130);
      case 131: /* as ::= ID|STRING */ yytestcase(yyruleno==131);
      case 142: /* dbnm ::= DOT nm */ yytestcase(yyruleno==142);
      case 151: /* indexed_opt ::= INDEXED BY nm */ yytestcase(yyruleno==151);
      case 257: /* nmnum ::= plus_num */ yytestcase(yyruleno==257);
      case 258: /* nmnum ::= nm */ yytestcase(yyruleno==258);
      case 259: /* nmnum ::= ON */ yytestcase(yyruleno==259);
      case 260: /* nmnum ::= DELETE */ yytestcase(yyruleno==260);
      case 261: /* nmnum ::= DEFAULT */ yytestcase(yyruleno==261);
      case 262: /* plus_num ::= PLUS INTEGER|FLOAT */ yytestcase(yyruleno==262);
      case 263: /* plus_num ::= INTEGER|FLOAT */ yytestcase(yyruleno==263);
      case 264: /* minus_num ::= MINUS INTEGER|FLOAT */ yytestcase(yyruleno==264);
      case 280: /* trnm ::= nm */ yytestcase(yyruleno==280);
#line 247 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 2351 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 44: /* type ::= typetoken */
#line 257 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddColumnType(pParse,&yymsp[0].minor.yy0);}
#line 2356 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 46: /* typetoken ::= typename LP signed RP */
#line 259 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0.z = yymsp[-3].minor.yy0.z;
  yygotominor.yy0.n = (int)(&yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n] - yymsp[-3].minor.yy0.z);
}
#line 2364 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 47: /* typetoken ::= typename LP signed COMMA signed RP */
#line 263 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0.z = yymsp[-5].minor.yy0.z;
  yygotominor.yy0.n = (int)(&yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n] - yymsp[-5].minor.yy0.z);
}
#line 2372 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 49: /* typename ::= typename ID|STRING */
#line 269 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0.z=yymsp[-1].minor.yy0.z; yygotominor.yy0.n=yymsp[0].minor.yy0.n+(int)(yymsp[0].minor.yy0.z-yymsp[-1].minor.yy0.z);}
#line 2377 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 54: /* ccons ::= CONSTRAINT nm */
      case 92: /* tcons ::= CONSTRAINT nm */ yytestcase(yyruleno==92);
#line 278 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{pParse->constraintName = yymsp[0].minor.yy0;}
#line 2383 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 55: /* ccons ::= DEFAULT term */
      case 57: /* ccons ::= DEFAULT PLUS term */ yytestcase(yyruleno==57);
#line 279 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddDefaultValue(pParse,&yymsp[0].minor.yy346);}
#line 2389 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 56: /* ccons ::= DEFAULT LP expr RP */
#line 280 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddDefaultValue(pParse,&yymsp[-1].minor.yy346);}
#line 2394 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 58: /* ccons ::= DEFAULT MINUS term */
#line 282 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  ExprSpan v;
  v.pExpr = sqlite3PExpr(pParse, TK_UMINUS, yymsp[0].minor.yy346.pExpr, 0, 0);
  v.zStart = yymsp[-1].minor.yy0.z;
  v.zEnd = yymsp[0].minor.yy346.zEnd;
  sqlite3AddDefaultValue(pParse,&v);
}
#line 2405 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 59: /* ccons ::= DEFAULT ID|INDEXED */
#line 289 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  ExprSpan v;
  spanExpr(&v, pParse, TK_STRING, &yymsp[0].minor.yy0);
  sqlite3AddDefaultValue(pParse,&v);
}
#line 2414 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 61: /* ccons ::= NOT NULL onconf */
#line 299 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddNotNull(pParse, yymsp[0].minor.yy328);}
#line 2419 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 62: /* ccons ::= PRIMARY KEY sortorder onconf autoinc */
#line 301 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddPrimaryKey(pParse,0,yymsp[-1].minor.yy328,yymsp[0].minor.yy328,yymsp[-2].minor.yy328);}
#line 2424 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 63: /* ccons ::= UNIQUE onconf */
#line 302 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3CreateIndex(pParse,0,0,0,0,yymsp[0].minor.yy328,0,0,0,0);}
#line 2429 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 64: /* ccons ::= CHECK LP expr RP */
#line 303 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddCheckConstraint(pParse,yymsp[-1].minor.yy346.pExpr);}
#line 2434 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 65: /* ccons ::= REFERENCES nm eidlist_opt refargs */
#line 305 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3CreateForeignKey(pParse,0,&yymsp[-2].minor.yy0,yymsp[-1].minor.yy14,yymsp[0].minor.yy328);}
#line 2439 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 66: /* ccons ::= defer_subclause */
#line 306 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3DeferForeignKey(pParse,yymsp[0].minor.yy328);}
#line 2444 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 67: /* ccons ::= COLLATE ID|STRING */
#line 307 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddCollateType(pParse, &yymsp[0].minor.yy0);}
#line 2449 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 70: /* refargs ::= */
#line 320 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = OE_None*0x0101; /* EV: R-19803-45884 */}
#line 2454 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 71: /* refargs ::= refargs refarg */
#line 321 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = (yymsp[-1].minor.yy328 & ~yymsp[0].minor.yy429.mask) | yymsp[0].minor.yy429.value; }
#line 2459 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 72: /* refarg ::= MATCH nm */
      case 73: /* refarg ::= ON INSERT refact */ yytestcase(yyruleno==73);
#line 323 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy429.value = 0;     yygotominor.yy429.mask = 0x000000; }
#line 2465 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 74: /* refarg ::= ON DELETE refact */
#line 325 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy429.value = yymsp[0].minor.yy328;     yygotominor.yy429.mask = 0x0000ff; }
#line 2470 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 75: /* refarg ::= ON UPDATE refact */
#line 326 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy429.value = yymsp[0].minor.yy328<<8;  yygotominor.yy429.mask = 0x00ff00; }
#line 2475 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 76: /* refact ::= SET NULL */
#line 328 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = OE_SetNull;  /* EV: R-33326-45252 */}
#line 2480 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 77: /* refact ::= SET DEFAULT */
#line 329 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = OE_SetDflt;  /* EV: R-33326-45252 */}
#line 2485 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 78: /* refact ::= CASCADE */
#line 330 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = OE_Cascade;  /* EV: R-33326-45252 */}
#line 2490 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 79: /* refact ::= RESTRICT */
#line 331 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = OE_Restrict; /* EV: R-33326-45252 */}
#line 2495 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 80: /* refact ::= NO ACTION */
#line 332 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = OE_None;     /* EV: R-33326-45252 */}
#line 2500 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 82: /* defer_subclause ::= DEFERRABLE init_deferred_pred_opt */
      case 98: /* defer_subclause_opt ::= defer_subclause */ yytestcase(yyruleno==98);
      case 100: /* onconf ::= ON CONFLICT resolvetype */ yytestcase(yyruleno==100);
      case 103: /* resolvetype ::= raisetype */ yytestcase(yyruleno==103);
#line 335 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = yymsp[0].minor.yy328;}
#line 2508 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 86: /* conslist_opt ::= */
#line 341 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0.n = 0; yygotominor.yy0.z = 0;}
#line 2513 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 87: /* conslist_opt ::= COMMA conslist */
#line 342 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0 = yymsp[-1].minor.yy0;}
#line 2518 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 90: /* tconscomma ::= COMMA */
#line 345 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{pParse->constraintName.n = 0;}
#line 2523 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 93: /* tcons ::= PRIMARY KEY LP sortlist autoinc RP onconf */
#line 349 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddPrimaryKey(pParse,yymsp[-3].minor.yy14,yymsp[0].minor.yy328,yymsp[-2].minor.yy328,0);}
#line 2528 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 94: /* tcons ::= UNIQUE LP sortlist RP onconf */
#line 351 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3CreateIndex(pParse,0,0,0,yymsp[-2].minor.yy14,yymsp[0].minor.yy328,0,0,0,0);}
#line 2533 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 95: /* tcons ::= CHECK LP expr RP onconf */
#line 353 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3AddCheckConstraint(pParse,yymsp[-2].minor.yy346.pExpr);}
#line 2538 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 96: /* tcons ::= FOREIGN KEY LP eidlist RP REFERENCES nm eidlist_opt refargs defer_subclause_opt */
#line 355 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    sqlite3CreateForeignKey(pParse, yymsp[-6].minor.yy14, &yymsp[-3].minor.yy0, yymsp[-2].minor.yy14, yymsp[-1].minor.yy328);
    sqlite3DeferForeignKey(pParse, yymsp[0].minor.yy328);
}
#line 2546 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 99: /* onconf ::= */
#line 369 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_Default;}
#line 2551 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 101: /* orconf ::= */
#line 371 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy186 = OE_Default;}
#line 2556 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 102: /* orconf ::= OR resolvetype */
#line 372 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy186 = (u8)yymsp[0].minor.yy328;}
#line 2561 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 104: /* resolvetype ::= IGNORE */
#line 374 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_Ignore;}
#line 2566 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 105: /* resolvetype ::= REPLACE */
#line 375 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_Replace;}
#line 2571 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 106: /* cmd ::= DROP TABLE ifexists fullname */
#line 379 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3DropTable(pParse, yymsp[0].minor.yy65, 0, yymsp[-1].minor.yy328);
}
#line 2578 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 109: /* cmd ::= createkw temp VIEW ifnotexists nm dbnm eidlist_opt AS select */
#line 390 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3CreateView(pParse, &yymsp[-8].minor.yy0, &yymsp[-4].minor.yy0, &yymsp[-3].minor.yy0, yymsp[-2].minor.yy14, yymsp[0].minor.yy3, yymsp[-7].minor.yy328, yymsp[-5].minor.yy328);
}
#line 2585 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 110: /* cmd ::= DROP VIEW ifexists fullname */
#line 393 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3DropTable(pParse, yymsp[0].minor.yy65, 1, yymsp[-1].minor.yy328);
}
#line 2592 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 111: /* cmd ::= select */
#line 400 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  SelectDest dest = {SRT_Output, 0, 0, 0, 0, 0};
  sqlite3Select(pParse, yymsp[0].minor.yy3, &dest);
  sqlite3SelectDelete(pParse->db, yymsp[0].minor.yy3);
}
#line 2601 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 112: /* select ::= with selectnowith */
#line 437 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Select *p = yymsp[0].minor.yy3;
  if( p ){
    p->pWith = yymsp[-1].minor.yy59;
    parserDoubleLinkSelect(pParse, p);
  }else{
    sqlite3WithDelete(pParse->db, yymsp[-1].minor.yy59);
  }
  yygotominor.yy3 = p;
}
#line 2615 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 113: /* selectnowith ::= oneselect */
      case 119: /* oneselect ::= values */ yytestcase(yyruleno==119);
#line 448 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy3 = yymsp[0].minor.yy3;}
#line 2621 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 114: /* selectnowith ::= selectnowith multiselect_op oneselect */
#line 450 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Select *pRhs = yymsp[0].minor.yy3;
  Select *pLhs = yymsp[-2].minor.yy3;
  if( pRhs && pRhs->pPrior ){
    SrcList *pFrom;
    Token x;
    x.n = 0;
    parserDoubleLinkSelect(pParse, pRhs);
    pFrom = sqlite3SrcListAppendFromTerm(pParse,0,0,0,&x,pRhs,0,0);
    pRhs = sqlite3SelectNew(pParse,0,pFrom,0,0,0,0,0,0,0);
  }
  if( pRhs ){
    pRhs->op = (u8)yymsp[-1].minor.yy328;
    pRhs->pPrior = pLhs;
    if( ALWAYS(pLhs) ) pLhs->selFlags &= ~SF_MultiValue;
    pRhs->selFlags &= ~SF_MultiValue;
    if( yymsp[-1].minor.yy328!=TK_ALL ) pParse->hasCompound = 1;
  }else{
    sqlite3SelectDelete(pParse->db, pLhs);
  }
  yygotominor.yy3 = pRhs;
}
#line 2647 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 116: /* multiselect_op ::= UNION ALL */
#line 474 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = TK_ALL;}
#line 2652 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 118: /* oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt orderby_opt limit_opt */
#line 478 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy3 = sqlite3SelectNew(pParse,yymsp[-6].minor.yy14,yymsp[-5].minor.yy65,yymsp[-4].minor.yy132,yymsp[-3].minor.yy14,yymsp[-2].minor.yy132,yymsp[-1].minor.yy14,yymsp[-7].minor.yy381,yymsp[0].minor.yy476.pLimit,yymsp[0].minor.yy476.pOffset);
#if SELECTTRACE_ENABLED
  /* Populate the Select.zSelName[] string that is used to help with
  ** query planner debugging, to differentiate between multiple Select
  ** objects in a complex query.
  **
  ** If the SELECT keyword is immediately followed by a C-style comment
  ** then extract the first few alphanumeric characters from within that
  ** comment to be the zSelName value.  Otherwise, the label is #N where
  ** is an integer that is incremented with each SELECT statement seen.
  */
  if( yygotominor.yy3!=0 ){
    const char *z = yymsp[-8].minor.yy0.z+6;
    int i;
    sqlite3_snprintf(sizeof(yygotominor.yy3->zSelName), yygotominor.yy3->zSelName, "#%d",
                     ++pParse->nSelect);
    while( z[0]==' ' ) z++;
    if( z[0]=='/' && z[1]=='*' ){
      z += 2;
      while( z[0]==' ' ) z++;
      for(i=0; sqlite3Isalnum(z[i]); i++){}
      sqlite3_snprintf(sizeof(yygotominor.yy3->zSelName), yygotominor.yy3->zSelName, "%.*s", i, z);
    }
  }
#endif /* SELECTRACE_ENABLED */
}
#line 2683 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 120: /* values ::= VALUES LP nexprlist RP */
#line 509 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy3 = sqlite3SelectNew(pParse,yymsp[-1].minor.yy14,0,0,0,0,0,SF_Values,0,0);
}
#line 2690 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 121: /* values ::= values COMMA LP exprlist RP */
#line 512 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Select *pRight, *pLeft = yymsp[-4].minor.yy3;
  pRight = sqlite3SelectNew(pParse,yymsp[-1].minor.yy14,0,0,0,0,0,SF_Values|SF_MultiValue,0,0);
  if( ALWAYS(pLeft) ) pLeft->selFlags &= ~SF_MultiValue;
  if( pRight ){
    pRight->op = TK_ALL;
    pLeft = yymsp[-4].minor.yy3;
    pRight->pPrior = pLeft;
    yygotominor.yy3 = pRight;
  }else{
    yygotominor.yy3 = pLeft;
  }
}
#line 2707 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 122: /* distinct ::= DISTINCT */
#line 530 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy381 = SF_Distinct;}
#line 2712 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 123: /* distinct ::= ALL */
#line 531 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy381 = SF_All;}
#line 2717 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 124: /* distinct ::= */
#line 532 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy381 = 0;}
#line 2722 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 125: /* sclp ::= selcollist COMMA */
      case 244: /* eidlist_opt ::= LP eidlist RP */ yytestcase(yyruleno==244);
#line 543 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy14 = yymsp[-1].minor.yy14;}
#line 2728 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 126: /* sclp ::= */
      case 155: /* orderby_opt ::= */ yytestcase(yyruleno==155);
      case 162: /* groupby_opt ::= */ yytestcase(yyruleno==162);
      case 237: /* exprlist ::= */ yytestcase(yyruleno==237);
      case 243: /* eidlist_opt ::= */ yytestcase(yyruleno==243);
#line 544 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy14 = 0;}
#line 2737 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 127: /* selcollist ::= sclp expr as */
#line 545 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
   yygotominor.yy14 = sqlite3ExprListAppend(pParse, yymsp[-2].minor.yy14, yymsp[-1].minor.yy346.pExpr);
   if( yymsp[0].minor.yy0.n>0 ) sqlite3ExprListSetName(pParse, yygotominor.yy14, &yymsp[0].minor.yy0, 1);
   sqlite3ExprListSetSpan(pParse,yygotominor.yy14,&yymsp[-1].minor.yy346);
}
#line 2746 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 128: /* selcollist ::= sclp STAR */
#line 550 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Expr *p = sqlite3Expr(pParse->db, TK_ALL, 0);
  yygotominor.yy14 = sqlite3ExprListAppend(pParse, yymsp[-1].minor.yy14, p);
}
#line 2754 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 129: /* selcollist ::= sclp nm DOT STAR */
#line 554 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Expr *pRight = sqlite3PExpr(pParse, TK_ALL, 0, 0, &yymsp[0].minor.yy0);
  Expr *pLeft = sqlite3PExpr(pParse, TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *pDot = sqlite3PExpr(pParse, TK_DOT, pLeft, pRight, 0);
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-3].minor.yy14, pDot);
}
#line 2764 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 132: /* as ::= */
#line 567 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0.n = 0;}
#line 2769 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 133: /* from ::= */
#line 579 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy65 = sqlite3DbMallocZero(pParse->db, sizeof(*yygotominor.yy65));}
#line 2774 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 134: /* from ::= FROM seltablist */
#line 580 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy65 = yymsp[0].minor.yy65;
  sqlite3SrcListShiftJoinType(yygotominor.yy65);
}
#line 2782 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 135: /* stl_prefix ::= seltablist joinop */
#line 588 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
   yygotominor.yy65 = yymsp[-1].minor.yy65;
   if( ALWAYS(yygotominor.yy65 && yygotominor.yy65->nSrc>0) ) yygotominor.yy65->a[yygotominor.yy65->nSrc-1].fg.jointype = (u8)yymsp[0].minor.yy328;
}
#line 2790 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 136: /* stl_prefix ::= */
#line 592 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy65 = 0;}
#line 2795 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 137: /* seltablist ::= stl_prefix nm dbnm as indexed_opt on_opt using_opt */
#line 594 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,&yymsp[-5].minor.yy0,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0,0,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
  sqlite3SrcListIndexedBy(pParse, yygotominor.yy65, &yymsp[-2].minor.yy0);
}
#line 2803 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 138: /* seltablist ::= stl_prefix nm dbnm LP exprlist RP as on_opt using_opt */
#line 599 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-8].minor.yy65,&yymsp[-7].minor.yy0,&yymsp[-6].minor.yy0,&yymsp[-2].minor.yy0,0,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
  sqlite3SrcListFuncArgs(pParse, yygotominor.yy65, yymsp[-4].minor.yy14);
}
#line 2811 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 139: /* seltablist ::= stl_prefix LP select RP as on_opt using_opt */
#line 605 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    yygotominor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,0,0,&yymsp[-2].minor.yy0,yymsp[-4].minor.yy3,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
  }
#line 2818 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 140: /* seltablist ::= stl_prefix LP seltablist RP as on_opt using_opt */
#line 609 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    if( yymsp[-6].minor.yy65==0 && yymsp[-2].minor.yy0.n==0 && yymsp[-1].minor.yy132==0 && yymsp[0].minor.yy408==0 ){
      yygotominor.yy65 = yymsp[-4].minor.yy65;
    }else if( yymsp[-4].minor.yy65->nSrc==1 ){
      yygotominor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,0,0,&yymsp[-2].minor.yy0,0,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
      if( yygotominor.yy65 ){
        struct SrcList_item *pNew = &yygotominor.yy65->a[yygotominor.yy65->nSrc-1];
        struct SrcList_item *pOld = yymsp[-4].minor.yy65->a;
        pNew->zName = pOld->zName;
        pNew->zDatabase = pOld->zDatabase;
        pNew->pSelect = pOld->pSelect;
        pOld->zName = pOld->zDatabase = 0;
        pOld->pSelect = 0;
      }
      sqlite3SrcListDelete(pParse->db, yymsp[-4].minor.yy65);
    }else{
      Select *pSubquery;
      sqlite3SrcListShiftJoinType(yymsp[-4].minor.yy65);
      pSubquery = sqlite3SelectNew(pParse,0,yymsp[-4].minor.yy65,0,0,0,0,SF_NestedFrom,0,0);
      yygotominor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,0,0,&yymsp[-2].minor.yy0,pSubquery,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
    }
  }
#line 2844 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 141: /* dbnm ::= */
      case 150: /* indexed_opt ::= */ yytestcase(yyruleno==150);
#line 634 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0.z=0; yygotominor.yy0.n=0;}
#line 2850 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 143: /* fullname ::= nm dbnm */
#line 639 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy65 = sqlite3SrcListAppend(pParse->db,0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0);}
#line 2855 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 144: /* joinop ::= COMMA|JOIN */
#line 643 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = JT_INNER; }
#line 2860 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 145: /* joinop ::= JOIN_KW JOIN */
#line 644 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = sqlite3JoinType(pParse,&yymsp[-1].minor.yy0,0,0); }
#line 2865 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 146: /* joinop ::= JOIN_KW nm JOIN */
#line 645 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = sqlite3JoinType(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0,0); }
#line 2870 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 147: /* joinop ::= JOIN_KW nm nm JOIN */
#line 647 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = sqlite3JoinType(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0); }
#line 2875 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 148: /* on_opt ::= ON expr */
      case 165: /* having_opt ::= HAVING expr */ yytestcase(yyruleno==165);
      case 172: /* where_opt ::= WHERE expr */ yytestcase(yyruleno==172);
      case 232: /* case_else ::= ELSE expr */ yytestcase(yyruleno==232);
      case 234: /* case_operand ::= expr */ yytestcase(yyruleno==234);
#line 651 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy132 = yymsp[0].minor.yy346.pExpr;}
#line 2884 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 149: /* on_opt ::= */
      case 164: /* having_opt ::= */ yytestcase(yyruleno==164);
      case 171: /* where_opt ::= */ yytestcase(yyruleno==171);
      case 233: /* case_else ::= */ yytestcase(yyruleno==233);
      case 235: /* case_operand ::= */ yytestcase(yyruleno==235);
#line 652 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy132 = 0;}
#line 2893 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 152: /* indexed_opt ::= NOT INDEXED */
#line 667 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy0.z=0; yygotominor.yy0.n=1;}
#line 2898 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 153: /* using_opt ::= USING LP idlist RP */
      case 181: /* idlist_opt ::= LP idlist RP */ yytestcase(yyruleno==181);
#line 671 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy408 = yymsp[-1].minor.yy408;}
#line 2904 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 154: /* using_opt ::= */
      case 180: /* idlist_opt ::= */ yytestcase(yyruleno==180);
#line 672 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy408 = 0;}
#line 2910 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 156: /* orderby_opt ::= ORDER BY sortlist */
      case 163: /* groupby_opt ::= GROUP BY nexprlist */ yytestcase(yyruleno==163);
      case 236: /* exprlist ::= nexprlist */ yytestcase(yyruleno==236);
#line 686 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy14 = yymsp[0].minor.yy14;}
#line 2917 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 157: /* sortlist ::= sortlist COMMA expr sortorder */
#line 687 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-3].minor.yy14,yymsp[-1].minor.yy346.pExpr);
  sqlite3ExprListSetSortOrder(yygotominor.yy14,yymsp[0].minor.yy328);
}
#line 2925 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 158: /* sortlist ::= expr sortorder */
#line 691 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,0,yymsp[-1].minor.yy346.pExpr);
  sqlite3ExprListSetSortOrder(yygotominor.yy14,yymsp[0].minor.yy328);
}
#line 2933 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 159: /* sortorder ::= ASC */
#line 698 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = SQLITE_SO_ASC;}
#line 2938 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 160: /* sortorder ::= DESC */
#line 699 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = SQLITE_SO_DESC;}
#line 2943 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 161: /* sortorder ::= */
#line 700 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = SQLITE_SO_UNDEFINED;}
#line 2948 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 166: /* limit_opt ::= */
#line 725 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy476.pLimit = 0; yygotominor.yy476.pOffset = 0;}
#line 2953 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 167: /* limit_opt ::= LIMIT expr */
#line 726 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy476.pLimit = yymsp[0].minor.yy346.pExpr; yygotominor.yy476.pOffset = 0;}
#line 2958 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 168: /* limit_opt ::= LIMIT expr OFFSET expr */
#line 728 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy476.pLimit = yymsp[-2].minor.yy346.pExpr; yygotominor.yy476.pOffset = yymsp[0].minor.yy346.pExpr;}
#line 2963 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 169: /* limit_opt ::= LIMIT expr COMMA expr */
#line 730 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy476.pOffset = yymsp[-2].minor.yy346.pExpr; yygotominor.yy476.pLimit = yymsp[0].minor.yy346.pExpr;}
#line 2968 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 170: /* cmd ::= with DELETE FROM fullname indexed_opt where_opt */
#line 744 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-5].minor.yy59, 1);
  sqlite3SrcListIndexedBy(pParse, yymsp[-2].minor.yy65, &yymsp[-1].minor.yy0);
  sqlite3DeleteFrom(pParse,yymsp[-2].minor.yy65,yymsp[0].minor.yy132);
}
#line 2977 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 173: /* cmd ::= with UPDATE orconf fullname indexed_opt SET setlist where_opt */
#line 771 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-7].minor.yy59, 1);
  sqlite3SrcListIndexedBy(pParse, yymsp[-4].minor.yy65, &yymsp[-3].minor.yy0);
  sqlite3ExprListCheckLength(pParse,yymsp[-1].minor.yy14,"set list"); 
  sqlite3Update(pParse,yymsp[-4].minor.yy65,yymsp[-1].minor.yy14,yymsp[0].minor.yy132,yymsp[-5].minor.yy186);
}
#line 2987 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 174: /* setlist ::= setlist COMMA nm EQ expr */
#line 782 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = sqlite3ExprListAppend(pParse, yymsp[-4].minor.yy14, yymsp[0].minor.yy346.pExpr);
  sqlite3ExprListSetName(pParse, yygotominor.yy14, &yymsp[-2].minor.yy0, 1);
}
#line 2995 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 175: /* setlist ::= nm EQ expr */
#line 786 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = sqlite3ExprListAppend(pParse, 0, yymsp[0].minor.yy346.pExpr);
  sqlite3ExprListSetName(pParse, yygotominor.yy14, &yymsp[-2].minor.yy0, 1);
}
#line 3003 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 176: /* cmd ::= with insert_cmd INTO fullname idlist_opt select */
#line 793 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-5].minor.yy59, 1);
  sqlite3Insert(pParse, yymsp[-2].minor.yy65, yymsp[0].minor.yy3, yymsp[-1].minor.yy408, yymsp[-4].minor.yy186);
}
#line 3011 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 177: /* cmd ::= with insert_cmd INTO fullname idlist_opt DEFAULT VALUES */
#line 798 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-6].minor.yy59, 1);
  sqlite3Insert(pParse, yymsp[-3].minor.yy65, 0, yymsp[-2].minor.yy408, yymsp[-5].minor.yy186);
}
#line 3019 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 178: /* insert_cmd ::= INSERT orconf */
#line 804 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy186 = yymsp[0].minor.yy186;}
#line 3024 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 179: /* insert_cmd ::= REPLACE */
#line 805 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy186 = OE_Replace;}
#line 3029 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 182: /* idlist ::= idlist COMMA nm */
#line 815 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy408 = sqlite3IdListAppend(pParse->db,yymsp[-2].minor.yy408,&yymsp[0].minor.yy0);}
#line 3034 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 183: /* idlist ::= nm */
#line 817 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy408 = sqlite3IdListAppend(pParse->db,0,&yymsp[0].minor.yy0);}
#line 3039 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 184: /* expr ::= term */
#line 848 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy346 = yymsp[0].minor.yy346;}
#line 3044 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 185: /* expr ::= LP expr RP */
#line 849 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy346.pExpr = yymsp[-1].minor.yy346.pExpr; spanSet(&yygotominor.yy346,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0);}
#line 3049 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 186: /* term ::= NULL */
      case 191: /* term ::= INTEGER|FLOAT|BLOB */ yytestcase(yyruleno==191);
      case 192: /* term ::= STRING */ yytestcase(yyruleno==192);
#line 850 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanExpr(&yygotominor.yy346, pParse, yymsp[0].major, &yymsp[0].minor.yy0);}
#line 3056 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 187: /* expr ::= ID|INDEXED */
      case 188: /* expr ::= JOIN_KW */ yytestcase(yyruleno==188);
#line 851 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanExpr(&yygotominor.yy346, pParse, TK_ID, &yymsp[0].minor.yy0);}
#line 3062 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 189: /* expr ::= nm DOT nm */
#line 853 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Expr *temp1 = sqlite3PExpr(pParse, TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *temp2 = sqlite3PExpr(pParse, TK_ID, 0, 0, &yymsp[0].minor.yy0);
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_DOT, temp1, temp2, 0);
  spanSet(&yygotominor.yy346,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0);
}
#line 3072 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 190: /* expr ::= nm DOT nm DOT nm */
#line 859 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Expr *temp1 = sqlite3PExpr(pParse, TK_ID, 0, 0, &yymsp[-4].minor.yy0);
  Expr *temp2 = sqlite3PExpr(pParse, TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *temp3 = sqlite3PExpr(pParse, TK_ID, 0, 0, &yymsp[0].minor.yy0);
  Expr *temp4 = sqlite3PExpr(pParse, TK_DOT, temp2, temp3, 0);
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_DOT, temp1, temp4, 0);
  spanSet(&yygotominor.yy346,&yymsp[-4].minor.yy0,&yymsp[0].minor.yy0);
}
#line 3084 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 193: /* expr ::= VARIABLE */
#line 869 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  if( yymsp[0].minor.yy0.n>=2 && yymsp[0].minor.yy0.z[0]=='#' && sqlite3Isdigit(yymsp[0].minor.yy0.z[1]) ){
    /* When doing a nested parse, one can include terms in an expression
    ** that look like this:   #1 #2 ...  These terms refer to registers
    ** in the virtual machine.  #N is the N-th register. */
    if( pParse->nested==0 ){
      sqlite3ErrorMsg(pParse, "near \"%T\": syntax error", &yymsp[0].minor.yy0);
      yygotominor.yy346.pExpr = 0;
    }else{
      yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_REGISTER, 0, 0, &yymsp[0].minor.yy0);
      if( yygotominor.yy346.pExpr ) sqlite3GetInt32(&yymsp[0].minor.yy0.z[1], &yygotominor.yy346.pExpr->iTable);
    }
  }else{
    spanExpr(&yygotominor.yy346, pParse, TK_VARIABLE, &yymsp[0].minor.yy0);
    sqlite3ExprAssignVarNumber(pParse, yygotominor.yy346.pExpr);
  }
  spanSet(&yygotominor.yy346, &yymsp[0].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3106 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 194: /* expr ::= expr COLLATE ID|STRING */
#line 887 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3ExprAddCollateToken(pParse, yymsp[-2].minor.yy346.pExpr, &yymsp[0].minor.yy0, 1);
  yygotominor.yy346.zStart = yymsp[-2].minor.yy346.zStart;
  yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
}
#line 3115 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 195: /* expr ::= CAST LP expr AS typetoken RP */
#line 893 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_CAST, yymsp[-3].minor.yy346.pExpr, 0, &yymsp[-1].minor.yy0);
  spanSet(&yygotominor.yy346,&yymsp[-5].minor.yy0,&yymsp[0].minor.yy0);
}
#line 3123 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 196: /* expr ::= ID|INDEXED LP distinct exprlist RP */
#line 898 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  if( yymsp[-1].minor.yy14 && yymsp[-1].minor.yy14->nExpr>pParse->db->aLimit[SQLITE_LIMIT_FUNCTION_ARG] ){
    sqlite3ErrorMsg(pParse, "too many arguments on function %T", &yymsp[-4].minor.yy0);
  }
  yygotominor.yy346.pExpr = sqlite3ExprFunction(pParse, yymsp[-1].minor.yy14, &yymsp[-4].minor.yy0);
  spanSet(&yygotominor.yy346,&yymsp[-4].minor.yy0,&yymsp[0].minor.yy0);
  if( yymsp[-2].minor.yy381==SF_Distinct && yygotominor.yy346.pExpr ){
    yygotominor.yy346.pExpr->flags |= EP_Distinct;
  }
}
#line 3137 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 197: /* expr ::= ID|INDEXED LP STAR RP */
#line 908 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3ExprFunction(pParse, 0, &yymsp[-3].minor.yy0);
  spanSet(&yygotominor.yy346,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 3145 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 198: /* term ::= CTIME_KW */
#line 912 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3ExprFunction(pParse, 0, &yymsp[0].minor.yy0);
  spanSet(&yygotominor.yy346, &yymsp[0].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3153 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 199: /* expr ::= expr AND expr */
      case 200: /* expr ::= expr OR expr */ yytestcase(yyruleno==200);
      case 201: /* expr ::= expr LT|GT|GE|LE expr */ yytestcase(yyruleno==201);
      case 202: /* expr ::= expr EQ|NE expr */ yytestcase(yyruleno==202);
      case 203: /* expr ::= expr BITAND|BITOR|LSHIFT|RSHIFT expr */ yytestcase(yyruleno==203);
      case 204: /* expr ::= expr PLUS|MINUS expr */ yytestcase(yyruleno==204);
      case 205: /* expr ::= expr STAR|SLASH|REM expr */ yytestcase(yyruleno==205);
      case 206: /* expr ::= expr CONCAT expr */ yytestcase(yyruleno==206);
#line 934 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanBinaryExpr(&yygotominor.yy346,pParse,yymsp[-1].major,&yymsp[-2].minor.yy346,&yymsp[0].minor.yy346);}
#line 3165 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 207: /* likeop ::= LIKE_KW|MATCH */
#line 947 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy96.eOperator = yymsp[0].minor.yy0; yygotominor.yy96.bNot = 0;}
#line 3170 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 208: /* likeop ::= NOT LIKE_KW|MATCH */
#line 948 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy96.eOperator = yymsp[0].minor.yy0; yygotominor.yy96.bNot = 1;}
#line 3175 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 209: /* expr ::= expr likeop expr */
#line 949 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  ExprList *pList;
  pList = sqlite3ExprListAppend(pParse,0, yymsp[0].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[-2].minor.yy346.pExpr);
  yygotominor.yy346.pExpr = sqlite3ExprFunction(pParse, pList, &yymsp[-1].minor.yy96.eOperator);
  if( yymsp[-1].minor.yy96.bNot ) yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_NOT, yygotominor.yy346.pExpr, 0, 0);
  yygotominor.yy346.zStart = yymsp[-2].minor.yy346.zStart;
  yygotominor.yy346.zEnd = yymsp[0].minor.yy346.zEnd;
  if( yygotominor.yy346.pExpr ) yygotominor.yy346.pExpr->flags |= EP_InfixFunc;
}
#line 3189 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 210: /* expr ::= expr likeop expr ESCAPE expr */
#line 959 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  ExprList *pList;
  pList = sqlite3ExprListAppend(pParse,0, yymsp[-2].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[-4].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[0].minor.yy346.pExpr);
  yygotominor.yy346.pExpr = sqlite3ExprFunction(pParse, pList, &yymsp[-3].minor.yy96.eOperator);
  if( yymsp[-3].minor.yy96.bNot ) yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_NOT, yygotominor.yy346.pExpr, 0, 0);
  yygotominor.yy346.zStart = yymsp[-4].minor.yy346.zStart;
  yygotominor.yy346.zEnd = yymsp[0].minor.yy346.zEnd;
  if( yygotominor.yy346.pExpr ) yygotominor.yy346.pExpr->flags |= EP_InfixFunc;
}
#line 3204 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 211: /* expr ::= expr ISNULL|NOTNULL */
#line 987 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanUnaryPostfix(&yygotominor.yy346,pParse,yymsp[0].major,&yymsp[-1].minor.yy346,&yymsp[0].minor.yy0);}
#line 3209 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 212: /* expr ::= expr NOT NULL */
#line 988 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanUnaryPostfix(&yygotominor.yy346,pParse,TK_NOTNULL,&yymsp[-2].minor.yy346,&yymsp[0].minor.yy0);}
#line 3214 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 213: /* expr ::= expr IS expr */
#line 1009 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  spanBinaryExpr(&yygotominor.yy346,pParse,TK_IS,&yymsp[-2].minor.yy346,&yymsp[0].minor.yy346);
  binaryToUnaryIfNull(pParse, yymsp[0].minor.yy346.pExpr, yygotominor.yy346.pExpr, TK_ISNULL);
}
#line 3222 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 214: /* expr ::= expr IS NOT expr */
#line 1013 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  spanBinaryExpr(&yygotominor.yy346,pParse,TK_ISNOT,&yymsp[-3].minor.yy346,&yymsp[0].minor.yy346);
  binaryToUnaryIfNull(pParse, yymsp[0].minor.yy346.pExpr, yygotominor.yy346.pExpr, TK_NOTNULL);
}
#line 3230 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 215: /* expr ::= NOT expr */
      case 216: /* expr ::= BITNOT expr */ yytestcase(yyruleno==216);
#line 1036 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanUnaryPrefix(&yygotominor.yy346,pParse,yymsp[-1].major,&yymsp[0].minor.yy346,&yymsp[-1].minor.yy0);}
#line 3236 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 217: /* expr ::= MINUS expr */
#line 1039 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanUnaryPrefix(&yygotominor.yy346,pParse,TK_UMINUS,&yymsp[0].minor.yy346,&yymsp[-1].minor.yy0);}
#line 3241 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 218: /* expr ::= PLUS expr */
#line 1041 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{spanUnaryPrefix(&yygotominor.yy346,pParse,TK_UPLUS,&yymsp[0].minor.yy346,&yymsp[-1].minor.yy0);}
#line 3246 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 221: /* expr ::= expr between_op expr AND expr */
#line 1046 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  ExprList *pList = sqlite3ExprListAppend(pParse,0, yymsp[-2].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[0].minor.yy346.pExpr);
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_BETWEEN, yymsp[-4].minor.yy346.pExpr, 0, 0);
  if( yygotominor.yy346.pExpr ){
    yygotominor.yy346.pExpr->x.pList = pList;
  }else{
    sqlite3ExprListDelete(pParse->db, pList);
  } 
  if( yymsp[-3].minor.yy328 ) yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_NOT, yygotominor.yy346.pExpr, 0, 0);
  yygotominor.yy346.zStart = yymsp[-4].minor.yy346.zStart;
  yygotominor.yy346.zEnd = yymsp[0].minor.yy346.zEnd;
}
#line 3263 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 224: /* expr ::= expr in_op LP exprlist RP */
#line 1063 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    if( yymsp[-1].minor.yy14==0 ){
      /* Expressions of the form
      **
      **      expr1 IN ()
      **      expr1 NOT IN ()
      **
      ** simplify to constants 0 (false) and 1 (true), respectively,
      ** regardless of the value of expr1.
      */
      yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_INTEGER, 0, 0, &sqlite3IntTokens[yymsp[-3].minor.yy328]);
      sqlite3ExprDelete(pParse->db, yymsp[-4].minor.yy346.pExpr);
    }else if( yymsp[-1].minor.yy14->nExpr==1 ){
      /* Expressions of the form:
      **
      **      expr1 IN (?1)
      **      expr1 NOT IN (?2)
      **
      ** with exactly one value on the RHS can be simplified to something
      ** like this:
      **
      **      expr1 == ?1
      **      expr1 <> ?2
      **
      ** But, the RHS of the == or <> is marked with the EP_Generic flag
      ** so that it may not contribute to the computation of comparison
      ** affinity or the collating sequence to use for comparison.  Otherwise,
      ** the semantics would be subtly different from IN or NOT IN.
      */
      Expr *pRHS = yymsp[-1].minor.yy14->a[0].pExpr;
      yymsp[-1].minor.yy14->a[0].pExpr = 0;
      sqlite3ExprListDelete(pParse->db, yymsp[-1].minor.yy14);
      /* pRHS cannot be NULL because a malloc error would have been detected
      ** before now and control would have never reached this point */
      if( ALWAYS(pRHS) ){
        pRHS->flags &= ~EP_Collate;
        pRHS->flags |= EP_Generic;
      }
      yygotominor.yy346.pExpr = sqlite3PExpr(pParse, yymsp[-3].minor.yy328 ? TK_NE : TK_EQ, yymsp[-4].minor.yy346.pExpr, pRHS, 0);
    }else{
      yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_IN, yymsp[-4].minor.yy346.pExpr, 0, 0);
      if( yygotominor.yy346.pExpr ){
        yygotominor.yy346.pExpr->x.pList = yymsp[-1].minor.yy14;
        sqlite3ExprSetHeightAndFlags(pParse, yygotominor.yy346.pExpr);
      }else{
        sqlite3ExprListDelete(pParse->db, yymsp[-1].minor.yy14);
      }
      if( yymsp[-3].minor.yy328 ) yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_NOT, yygotominor.yy346.pExpr, 0, 0);
    }
    yygotominor.yy346.zStart = yymsp[-4].minor.yy346.zStart;
    yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
  }
#line 3319 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 225: /* expr ::= LP select RP */
#line 1115 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_SELECT, 0, 0, 0);
    if( yygotominor.yy346.pExpr ){
      yygotominor.yy346.pExpr->x.pSelect = yymsp[-1].minor.yy3;
      ExprSetProperty(yygotominor.yy346.pExpr, EP_xIsSelect|EP_Subquery);
      sqlite3ExprSetHeightAndFlags(pParse, yygotominor.yy346.pExpr);
    }else{
      sqlite3SelectDelete(pParse->db, yymsp[-1].minor.yy3);
    }
    yygotominor.yy346.zStart = yymsp[-2].minor.yy0.z;
    yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
  }
#line 3335 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 226: /* expr ::= expr in_op LP select RP */
#line 1127 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_IN, yymsp[-4].minor.yy346.pExpr, 0, 0);
    if( yygotominor.yy346.pExpr ){
      yygotominor.yy346.pExpr->x.pSelect = yymsp[-1].minor.yy3;
      ExprSetProperty(yygotominor.yy346.pExpr, EP_xIsSelect|EP_Subquery);
      sqlite3ExprSetHeightAndFlags(pParse, yygotominor.yy346.pExpr);
    }else{
      sqlite3SelectDelete(pParse->db, yymsp[-1].minor.yy3);
    }
    if( yymsp[-3].minor.yy328 ) yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_NOT, yygotominor.yy346.pExpr, 0, 0);
    yygotominor.yy346.zStart = yymsp[-4].minor.yy346.zStart;
    yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
  }
#line 3352 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 227: /* expr ::= expr in_op nm dbnm */
#line 1140 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    SrcList *pSrc = sqlite3SrcListAppend(pParse->db, 0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0);
    yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_IN, yymsp[-3].minor.yy346.pExpr, 0, 0);
    if( yygotominor.yy346.pExpr ){
      yygotominor.yy346.pExpr->x.pSelect = sqlite3SelectNew(pParse, 0,pSrc,0,0,0,0,0,0,0);
      ExprSetProperty(yygotominor.yy346.pExpr, EP_xIsSelect|EP_Subquery);
      sqlite3ExprSetHeightAndFlags(pParse, yygotominor.yy346.pExpr);
    }else{
      sqlite3SrcListDelete(pParse->db, pSrc);
    }
    if( yymsp[-2].minor.yy328 ) yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_NOT, yygotominor.yy346.pExpr, 0, 0);
    yygotominor.yy346.zStart = yymsp[-3].minor.yy346.zStart;
    yygotominor.yy346.zEnd = yymsp[0].minor.yy0.z ? &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n] : &yymsp[-1].minor.yy0.z[yymsp[-1].minor.yy0.n];
  }
#line 3370 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 228: /* expr ::= EXISTS LP select RP */
#line 1154 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    Expr *p = yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_EXISTS, 0, 0, 0);
    if( p ){
      p->x.pSelect = yymsp[-1].minor.yy3;
      ExprSetProperty(p, EP_xIsSelect|EP_Subquery);
      sqlite3ExprSetHeightAndFlags(pParse, p);
    }else{
      sqlite3SelectDelete(pParse->db, yymsp[-1].minor.yy3);
    }
    yygotominor.yy346.zStart = yymsp[-3].minor.yy0.z;
    yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
  }
#line 3386 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 229: /* expr ::= CASE case_operand case_exprlist case_else END */
#line 1169 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_CASE, yymsp[-3].minor.yy132, 0, 0);
  if( yygotominor.yy346.pExpr ){
    yygotominor.yy346.pExpr->x.pList = yymsp[-1].minor.yy132 ? sqlite3ExprListAppend(pParse,yymsp[-2].minor.yy14,yymsp[-1].minor.yy132) : yymsp[-2].minor.yy14;
    sqlite3ExprSetHeightAndFlags(pParse, yygotominor.yy346.pExpr);
  }else{
    sqlite3ExprListDelete(pParse->db, yymsp[-2].minor.yy14);
    sqlite3ExprDelete(pParse->db, yymsp[-1].minor.yy132);
  }
  yygotominor.yy346.zStart = yymsp[-4].minor.yy0.z;
  yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
}
#line 3402 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 230: /* case_exprlist ::= case_exprlist WHEN expr THEN expr */
#line 1183 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-4].minor.yy14, yymsp[-2].minor.yy346.pExpr);
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,yygotominor.yy14, yymsp[0].minor.yy346.pExpr);
}
#line 3410 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 231: /* case_exprlist ::= WHEN expr THEN expr */
#line 1187 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,0, yymsp[-2].minor.yy346.pExpr);
  yygotominor.yy14 = sqlite3ExprListAppend(pParse,yygotominor.yy14, yymsp[0].minor.yy346.pExpr);
}
#line 3418 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 238: /* nexprlist ::= nexprlist COMMA expr */
#line 1208 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-2].minor.yy14,yymsp[0].minor.yy346.pExpr);}
#line 3423 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 239: /* nexprlist ::= expr */
#line 1210 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy14 = sqlite3ExprListAppend(pParse,0,yymsp[0].minor.yy346.pExpr);}
#line 3428 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 240: /* cmd ::= createkw uniqueflag INDEX ifnotexists nm dbnm ON nm LP sortlist RP where_opt */
#line 1216 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3CreateIndex(pParse, &yymsp[-7].minor.yy0, &yymsp[-6].minor.yy0, 
                     sqlite3SrcListAppend(pParse->db,0,&yymsp[-4].minor.yy0,0), yymsp[-2].minor.yy14, yymsp[-10].minor.yy328,
                      &yymsp[-11].minor.yy0, yymsp[0].minor.yy132, SQLITE_SO_ASC, yymsp[-8].minor.yy328);
}
#line 3437 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 241: /* uniqueflag ::= UNIQUE */
      case 292: /* raisetype ::= ABORT */ yytestcase(yyruleno==292);
#line 1223 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_Abort;}
#line 3443 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 242: /* uniqueflag ::= */
#line 1224 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_None;}
#line 3448 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 245: /* eidlist ::= eidlist COMMA nm collate sortorder */
#line 1274 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = parserAddExprIdListTerm(pParse, yymsp[-4].minor.yy14, &yymsp[-2].minor.yy0, yymsp[-1].minor.yy328, yymsp[0].minor.yy328);
}
#line 3455 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 246: /* eidlist ::= nm collate sortorder */
#line 1277 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy14 = parserAddExprIdListTerm(pParse, 0, &yymsp[-2].minor.yy0, yymsp[-1].minor.yy328, yymsp[0].minor.yy328);
}
#line 3462 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 249: /* cmd ::= DROP INDEX ifexists fullname */
#line 1288 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3DropIndex(pParse, yymsp[0].minor.yy65, yymsp[-1].minor.yy328);}
#line 3467 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 250: /* cmd ::= VACUUM */
      case 251: /* cmd ::= VACUUM nm */ yytestcase(yyruleno==251);
#line 1294 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Vacuum(pParse);}
#line 3473 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 252: /* cmd ::= PRAGMA nm dbnm */
#line 1302 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Pragma(pParse,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0,0,0);}
#line 3478 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 253: /* cmd ::= PRAGMA nm dbnm EQ nmnum */
#line 1303 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Pragma(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,0);}
#line 3483 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 254: /* cmd ::= PRAGMA nm dbnm LP nmnum RP */
#line 1304 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Pragma(pParse,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0,&yymsp[-1].minor.yy0,0);}
#line 3488 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 255: /* cmd ::= PRAGMA nm dbnm EQ minus_num */
#line 1306 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Pragma(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,1);}
#line 3493 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 256: /* cmd ::= PRAGMA nm dbnm LP minus_num RP */
#line 1308 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Pragma(pParse,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0,&yymsp[-1].minor.yy0,1);}
#line 3498 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 265: /* cmd ::= createkw trigger_decl BEGIN trigger_cmd_list END */
#line 1324 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  Token all;
  all.z = yymsp[-3].minor.yy0.z;
  all.n = (int)(yymsp[0].minor.yy0.z - yymsp[-3].minor.yy0.z) + yymsp[0].minor.yy0.n;
  sqlite3FinishTrigger(pParse, yymsp[-1].minor.yy473, &all);
}
#line 3508 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 266: /* trigger_decl ::= temp TRIGGER ifnotexists nm dbnm trigger_time trigger_event ON fullname foreach_clause when_clause */
#line 1333 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3BeginTrigger(pParse, &yymsp[-7].minor.yy0, &yymsp[-6].minor.yy0, yymsp[-5].minor.yy328, yymsp[-4].minor.yy378.a, yymsp[-4].minor.yy378.b, yymsp[-2].minor.yy65, yymsp[0].minor.yy132, yymsp[-10].minor.yy328, yymsp[-8].minor.yy328);
  yygotominor.yy0 = (yymsp[-6].minor.yy0.n==0?yymsp[-7].minor.yy0:yymsp[-6].minor.yy0);
}
#line 3516 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 267: /* trigger_time ::= BEFORE */
      case 270: /* trigger_time ::= */ yytestcase(yyruleno==270);
#line 1339 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = TK_BEFORE; }
#line 3522 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 268: /* trigger_time ::= AFTER */
#line 1340 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = TK_AFTER;  }
#line 3527 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 269: /* trigger_time ::= INSTEAD OF */
#line 1341 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy328 = TK_INSTEAD;}
#line 3532 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 271: /* trigger_event ::= DELETE|INSERT */
      case 272: /* trigger_event ::= UPDATE */ yytestcase(yyruleno==272);
#line 1346 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy378.a = yymsp[0].major; yygotominor.yy378.b = 0;}
#line 3538 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 273: /* trigger_event ::= UPDATE OF idlist */
#line 1348 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy378.a = TK_UPDATE; yygotominor.yy378.b = yymsp[0].minor.yy408;}
#line 3543 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 276: /* when_clause ::= */
      case 297: /* key_opt ::= */ yytestcase(yyruleno==297);
#line 1355 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy132 = 0; }
#line 3549 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 277: /* when_clause ::= WHEN expr */
      case 298: /* key_opt ::= KEY expr */ yytestcase(yyruleno==298);
#line 1356 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy132 = yymsp[0].minor.yy346.pExpr; }
#line 3555 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 278: /* trigger_cmd_list ::= trigger_cmd_list trigger_cmd SEMI */
#line 1360 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  assert( yymsp[-2].minor.yy473!=0 );
  yymsp[-2].minor.yy473->pLast->pNext = yymsp[-1].minor.yy473;
  yymsp[-2].minor.yy473->pLast = yymsp[-1].minor.yy473;
  yygotominor.yy473 = yymsp[-2].minor.yy473;
}
#line 3565 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 279: /* trigger_cmd_list ::= trigger_cmd SEMI */
#line 1366 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ 
  assert( yymsp[-1].minor.yy473!=0 );
  yymsp[-1].minor.yy473->pLast = yymsp[-1].minor.yy473;
  yygotominor.yy473 = yymsp[-1].minor.yy473;
}
#line 3574 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 281: /* trnm ::= nm DOT nm */
#line 1378 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy0 = yymsp[0].minor.yy0;
  sqlite3ErrorMsg(pParse, 
        "qualified table names are not allowed on INSERT, UPDATE, and DELETE "
        "statements within triggers");
}
#line 3584 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 283: /* tridxby ::= INDEXED BY nm */
#line 1390 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3ErrorMsg(pParse,
        "the INDEXED BY clause is not allowed on UPDATE or DELETE statements "
        "within triggers");
}
#line 3593 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 284: /* tridxby ::= NOT INDEXED */
#line 1395 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3ErrorMsg(pParse,
        "the NOT INDEXED clause is not allowed on UPDATE or DELETE statements "
        "within triggers");
}
#line 3602 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 285: /* trigger_cmd ::= UPDATE orconf trnm tridxby SET setlist where_opt */
#line 1408 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy473 = sqlite3TriggerUpdateStep(pParse->db, &yymsp[-4].minor.yy0, yymsp[-1].minor.yy14, yymsp[0].minor.yy132, yymsp[-5].minor.yy186); }
#line 3607 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 286: /* trigger_cmd ::= insert_cmd INTO trnm idlist_opt select */
#line 1412 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy473 = sqlite3TriggerInsertStep(pParse->db, &yymsp[-2].minor.yy0, yymsp[-1].minor.yy408, yymsp[0].minor.yy3, yymsp[-4].minor.yy186);}
#line 3612 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 287: /* trigger_cmd ::= DELETE FROM trnm tridxby where_opt */
#line 1416 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy473 = sqlite3TriggerDeleteStep(pParse->db, &yymsp[-2].minor.yy0, yymsp[0].minor.yy132);}
#line 3617 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 288: /* trigger_cmd ::= select */
#line 1419 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy473 = sqlite3TriggerSelectStep(pParse->db, yymsp[0].minor.yy3); }
#line 3622 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 289: /* expr ::= RAISE LP IGNORE RP */
#line 1422 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_RAISE, 0, 0, 0); 
  if( yygotominor.yy346.pExpr ){
    yygotominor.yy346.pExpr->affinity = OE_Ignore;
  }
  yygotominor.yy346.zStart = yymsp[-3].minor.yy0.z;
  yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
}
#line 3634 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 290: /* expr ::= RAISE LP raisetype COMMA nm RP */
#line 1430 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy346.pExpr = sqlite3PExpr(pParse, TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  if( yygotominor.yy346.pExpr ) {
    yygotominor.yy346.pExpr->affinity = (char)yymsp[-3].minor.yy328;
  }
  yygotominor.yy346.zStart = yymsp[-5].minor.yy0.z;
  yygotominor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
}
#line 3646 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 291: /* raisetype ::= ROLLBACK */
#line 1441 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_Rollback;}
#line 3651 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 293: /* raisetype ::= FAIL */
#line 1443 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy328 = OE_Fail;}
#line 3656 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 294: /* cmd ::= DROP TRIGGER ifexists fullname */
#line 1448 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3DropTrigger(pParse,yymsp[0].minor.yy65,yymsp[-1].minor.yy328);
}
#line 3663 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 295: /* cmd ::= ATTACH database_kw_opt expr AS expr key_opt */
#line 1455 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3Attach(pParse, yymsp[-3].minor.yy346.pExpr, yymsp[-1].minor.yy346.pExpr, yymsp[0].minor.yy132);
}
#line 3670 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 296: /* cmd ::= DETACH database_kw_opt expr */
#line 1458 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3Detach(pParse, yymsp[0].minor.yy346.pExpr);
}
#line 3677 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 301: /* cmd ::= REINDEX */
#line 1473 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Reindex(pParse, 0, 0);}
#line 3682 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 302: /* cmd ::= REINDEX nm dbnm */
#line 1474 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Reindex(pParse, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);}
#line 3687 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 303: /* cmd ::= ANALYZE */
#line 1479 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Analyze(pParse, 0, 0);}
#line 3692 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 304: /* cmd ::= ANALYZE nm dbnm */
#line 1480 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3Analyze(pParse, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);}
#line 3697 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 305: /* cmd ::= ALTER TABLE fullname RENAME TO nm */
#line 1485 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3AlterRenameTable(pParse,yymsp[-3].minor.yy65,&yymsp[0].minor.yy0);
}
#line 3704 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 306: /* cmd ::= ALTER TABLE add_column_fullname ADD kwcolumn_opt column */
#line 1488 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  sqlite3AlterFinishAddColumn(pParse, &yymsp[0].minor.yy0);
}
#line 3711 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 307: /* add_column_fullname ::= fullname */
#line 1491 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  pParse->db->lookaside.bEnabled = 0;
  sqlite3AlterBeginAddColumn(pParse, yymsp[0].minor.yy65);
}
#line 3719 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 310: /* cmd ::= create_vtab */
#line 1501 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3VtabFinishParse(pParse,0);}
#line 3724 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 311: /* cmd ::= create_vtab LP vtabarglist RP */
#line 1502 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3VtabFinishParse(pParse,&yymsp[0].minor.yy0);}
#line 3729 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 312: /* create_vtab ::= createkw VIRTUAL TABLE ifnotexists nm dbnm USING nm */
#line 1504 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
    sqlite3VtabBeginParse(pParse, &yymsp[-3].minor.yy0, &yymsp[-2].minor.yy0, &yymsp[0].minor.yy0, yymsp[-4].minor.yy328);
}
#line 3736 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 315: /* vtabarg ::= */
#line 1509 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3VtabArgInit(pParse);}
#line 3741 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 317: /* vtabargtoken ::= ANY */
      case 318: /* vtabargtoken ::= lp anylist RP */ yytestcase(yyruleno==318);
      case 319: /* lp ::= LP */ yytestcase(yyruleno==319);
#line 1511 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{sqlite3VtabArgExtend(pParse,&yymsp[0].minor.yy0);}
#line 3748 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 323: /* with ::= */
#line 1526 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{yygotominor.yy59 = 0;}
#line 3753 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 324: /* with ::= WITH wqlist */
      case 325: /* with ::= WITH RECURSIVE wqlist */ yytestcase(yyruleno==325);
#line 1528 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{ yygotominor.yy59 = yymsp[0].minor.yy59; }
#line 3759 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 326: /* wqlist ::= nm eidlist_opt AS LP select RP */
#line 1531 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy59 = sqlite3WithAdd(pParse, 0, &yymsp[-5].minor.yy0, yymsp[-4].minor.yy14, yymsp[-1].minor.yy3);
}
#line 3766 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      case 327: /* wqlist ::= wqlist COMMA nm eidlist_opt AS LP select RP */
#line 1534 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"
{
  yygotominor.yy59 = sqlite3WithAdd(pParse, yymsp[-7].minor.yy59, &yymsp[-5].minor.yy0, yymsp[-4].minor.yy14, yymsp[-1].minor.yy3);
}
#line 3773 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
        break;
      default:
      /* (0) input ::= cmdlist */ yytestcase(yyruleno==0);
      /* (1) cmdlist ::= cmdlist ecmd */ yytestcase(yyruleno==1);
      /* (2) cmdlist ::= ecmd */ yytestcase(yyruleno==2);
      /* (3) ecmd ::= SEMI */ yytestcase(yyruleno==3);
      /* (4) ecmd ::= explain cmdx SEMI */ yytestcase(yyruleno==4);
      /* (10) trans_opt ::= */ yytestcase(yyruleno==10);
      /* (11) trans_opt ::= TRANSACTION */ yytestcase(yyruleno==11);
      /* (12) trans_opt ::= TRANSACTION nm */ yytestcase(yyruleno==12);
      /* (20) savepoint_opt ::= SAVEPOINT */ yytestcase(yyruleno==20);
      /* (21) savepoint_opt ::= */ yytestcase(yyruleno==21);
      /* (25) cmd ::= create_table create_table_args */ yytestcase(yyruleno==25);
      /* (36) columnlist ::= columnlist COMMA column */ yytestcase(yyruleno==36);
      /* (37) columnlist ::= column */ yytestcase(yyruleno==37);
      /* (43) type ::= */ yytestcase(yyruleno==43);
      /* (50) signed ::= plus_num */ yytestcase(yyruleno==50);
      /* (51) signed ::= minus_num */ yytestcase(yyruleno==51);
      /* (52) carglist ::= carglist ccons */ yytestcase(yyruleno==52);
      /* (53) carglist ::= */ yytestcase(yyruleno==53);
      /* (60) ccons ::= NULL onconf */ yytestcase(yyruleno==60);
      /* (88) conslist ::= conslist tconscomma tcons */ yytestcase(yyruleno==88);
      /* (89) conslist ::= tcons */ yytestcase(yyruleno==89);
      /* (91) tconscomma ::= */ yytestcase(yyruleno==91);
      /* (274) foreach_clause ::= */ yytestcase(yyruleno==274);
      /* (275) foreach_clause ::= FOR EACH ROW */ yytestcase(yyruleno==275);
      /* (282) tridxby ::= */ yytestcase(yyruleno==282);
      /* (299) database_kw_opt ::= DATABASE */ yytestcase(yyruleno==299);
      /* (300) database_kw_opt ::= */ yytestcase(yyruleno==300);
      /* (308) kwcolumn_opt ::= */ yytestcase(yyruleno==308);
      /* (309) kwcolumn_opt ::= COLUMNKW */ yytestcase(yyruleno==309);
      /* (313) vtabarglist ::= vtabarg */ yytestcase(yyruleno==313);
      /* (314) vtabarglist ::= vtabarglist COMMA vtabarg */ yytestcase(yyruleno==314);
      /* (316) vtabarg ::= vtabarg vtabargtoken */ yytestcase(yyruleno==316);
      /* (320) anylist ::= */ yytestcase(yyruleno==320);
      /* (321) anylist ::= anylist LP anylist RP */ yytestcase(yyruleno==321);
      /* (322) anylist ::= anylist ANY */ yytestcase(yyruleno==322);
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
  sqlite3ParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
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
  sqlite3ParserARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 32 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.y"

  UNUSED_PARAMETER(yymajor);  /* Silence some compiler warnings */
  assert( TOKEN.z[0] );  /* The tokenizer always gives us a token */
  sqlite3ErrorMsg(pParse, "near \"%T\": syntax error", &TOKEN);
#line 3875 "/home/nmoore/Development/OpenMemDB/database/include/sql/parser/parse.c"
  sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  sqlite3ParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "sqlite3ParserAlloc" which describes the current state of the parser.
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
void sqlite3Parser(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  sqlite3ParserTOKENTYPE yyminor       /* The value for the token */
  sqlite3ParserARG_PDECL               /* Optional %extra_argument parameter */
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
  sqlite3ParserARG_STORE;

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
