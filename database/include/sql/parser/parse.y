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

expr(X) ::= .   {X; printf("expr\n");}

term(A) ::= .   {A; printf("term\n");}

// DROP TABLE STATEMENT ///////////////////////////////////////////////////////

// SELECT STATEMENT ///////////////////////////////////////////////////////////

// UPDATE STATEMENT ///////////////////////////////////////////////////////////

// INSERT INTO STATEMENT //////////////////////////////////////////////////////

// DELETE STATEMENT ///////////////////////////////////////////////////////////
