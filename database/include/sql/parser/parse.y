%token_prefix TK_

%include {#include <sql/omdb_parser.h>}

%token_type   {Token}
%default_type {Token}

%extra_argument { StatementBuilder* builder}

%syntax_error {
  printf("Syntax Error!\n");
  int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
  for(int i = 0; i < n; ++i) {
    int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
    if (a < YYNSTATE + YYNRULE) {
      printf("possible token: %s\n", yyTokenName[i]);
    }
  }
}

%parse_failure {
  printf("Parse failed!\n");
}

%parse_accept {
  printf("Parse accepts input!\n");
}

input ::= cmd_list.

cmd_list ::= cmd_list end_cmd.
cmd_list ::= end_cmd.

end_cmd ::= SEMICOLON.  { printf("semicolon found\n");}
end_cmd ::= explain cmdx SEMICOLON. { printf("command found and semicolon found\n");}

explain ::= . { printf("explain\n"); }

cmdx ::= cmd.

// CREATE TABLE STATEMENT /////////////////////////////////////////////////////

cmd ::= create_table create_table_args.

create_table ::= CREATE TABLE name(A).  {builderStartCreateTable(builder, A);}

create_table_args ::= LPAREN column_list RPAREN .

column_list ::= column_list COMMA column.
column_list ::= column.

column(A) ::= column_id(X) type column_args.  {A = X; builderAddColumnName(builder, X);}

column_id(A) ::= name(X).   {A = X;}

%token_class id ID.
%token_class ids ID|STRING.

%type name {Token}
name(A) ::= id(X).      {A = X; token_print(X);}
name(A) ::= STRING(X).  {A = X; token_print(X);}

%type typetoken {Token}
type ::= .
type ::= typetoken(X).  {X;}

typetoken(A) ::= typename(X).   {A = X;}

%type typename {Token}
typename(A) ::= ids(X). {A; builderAddColumnType(builder, X);}

column_args ::= column_args column_constraints.
column_args ::= .

column_constraints ::= DEFAULT LPAREN expr(X) RPAREN. {X; printf("col_const\n");}
column_constraints ::= DEFAULT term(X).               {X; printf("col_const\n");}
column_constraints ::= DEFAULT id(X).                 {X; printf("col_const\n");}

column_constraints ::= NOT NULL.        {printf("Token: NOT NULL\n");}
column_constraints ::= UNIQUE.          {printf("Token: UNIQUE\n");}
column_constraints ::= AUTO_INCREMENT.  {printf("Token: AUTO_INCREMENT\n");}

// Time to define operator precedence
%left OR.
%left AND.
%right NOT.
%left BETWEEN IN NE EQ.
%left GT LE LT GE.

expr(A) ::= expr(X) AND|OR(OP) expr(Y).       { A; builderAddExpressions(builder, X, Y, OP); }
expr(A) ::= expr(X) LT|GT|GE|LE(OP) expr(Y).  { A; builderAddExpressions(builder, X, Y, OP); }
expr(A) ::= expr(X) EQ|NE(OP) expr(Y).        { A; builderAddExpressions(builder, X, Y, OP); }

term(A) ::= .   {A; printf("term\n");}

// DROP TABLE STATEMENT ///////////////////////////////////////////////////////

cmd ::= DROP TABLE name(A). {builderStartDropTable(builder, A);}

// SELECT STATEMENT ///////////////////////////////////////////////////////////

cmd ::= SELECT set_quantifier select_columns select_table.  {
    builderStartSelectQuery(builder);
}

set_quantifier ::= DISTINCT|ALL.
set_quantifier ::= .

select_columns ::= select_columns COMMA select_column.
select_columns ::= select_column.

select_column ::= name(X) DOT ASTERISK. { builderAddSelectAllColumns(builder, X); }
select_column ::= name(X) DOT name(Y) as_clause(Z). {
  builderAddQualifiedSelectColumn(builder, X, Y, Z);
}

as_clause(A) ::= AS name(X).  { X = A; }
as_clause(A) ::= .            { A = nullptr; }

select_table ::= FROM table_references where_clause group_by_clause.
select_table ::= FROM name(A).  { A; }

table_references ::= table_references COMMA table_reference.
table_references ::= table_reference.

// TODO: consider other clauses
table_reference ::= name(X) as_clause(Y). {X; Y;}

where_clause ::= WHERE search_condition.

group_by_clause ::= .

// TODO: consider other predicates
search_condition ::= comparison_predicate.

comparison_predicate ::= expr(X). { builderAddSelectPredicate(builder, X); }

// UPDATE STATEMENT ///////////////////////////////////////////////////////////

// INSERT INTO STATEMENT //////////////////////////////////////////////////////

// DELETE STATEMENT ///////////////////////////////////////////////////////////