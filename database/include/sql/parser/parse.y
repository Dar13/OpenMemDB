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
  builderClean(builder);
}

%parse_accept {
  printf("Parse accepts input!\n");
}

input ::= cmd_list.

cmd_list ::= cmd_list end_cmd.
cmd_list ::= end_cmd.

end_cmd ::= SEMICOLON.  { printf("semicolon found\n");}
end_cmd ::= explain cmdx SEMICOLON. { printf("command found and semicolon found\n");}

explain ::= . { }

cmdx ::= cmd.

// CREATE TABLE STATEMENT /////////////////////////////////////////////////////

cmd ::= create_table create_table_args.

create_table ::= CREATE TABLE name(A).  {builderStartCreateTable(builder, A);}

create_table_args ::= LPAREN column_list RPAREN .

column_list ::= column_list COMMA column.
column_list ::= column.

column(A) ::= column_id(X) type(Y) column_args(Z).
{
  (void)A;

  builderAddColumn(builder, X, Y, Z);
}

column_id(A) ::= name(X).   {A = X;}

%token_class id ID.
%token_class ids ID|STRING.

%type name {Token}
name(A) ::= id(X).      {A = X; token_print(X);}
name(A) ::= STRING(X).  {A = X; token_print(X);}

%type typetoken {Token}
type(A) ::= . { A = nullptr; }
type(A) ::= typetoken(X).  {A = X;}

typetoken(A) ::= typename(X).   {A = X;}

%type typename {Token}
typename(A) ::= ids(X). {A = X;}

column_args ::= column_args column_constraints.
column_args ::= .

column_constraints ::= DEFAULT LPAREN expr(X) RPAREN. {(void)X; printf("col_const\n");}
column_constraints ::= DEFAULT term(X).               {(void)X; printf("col_const\n");}
//column_constraints ::= DEFAULT id(X).               {(void)X; printf("col_const\n");}

column_constraints ::= NOT NULL.        {printf("Token: NOT NULL\n");}
column_constraints ::= UNIQUE.          {printf("Token: UNIQUE\n");}
column_constraints ::= AUTO_INCREMENT.  {printf("Token: AUTO_INCREMENT\n");}

// Time to define operator precedence
%left OR.
%left AND.
%right NOT.
%left BETWEEN IN ISNULL NOTNULL NE EQ.
%left GT LE LT GE.

expr ::= expr AND|OR(OP) expr. { builderStartNestedExpr(builder, OP);}
expr ::= term(X) NE|EQ(OP) term(Y). { builderAddValueExpr(builder, OP, X, Y); }

term(A) ::= name(X) DOT column_id(Y). { 
    std::string* tmp = new (std::nothrow) std::string();
    // TODO: Error handling
    *tmp = (*X->text + "." + *Y->text);
    A = new (std::nothrow) TokenData(tmp, X->text, Y->text);
    // TODO: Error handling
}
term(A) ::= column_id(X). { A = X; printf("expression term\n"); }
term(A) ::= INTEGER|FLOAT(X).
{ 
  A = X;
}

// DROP TABLE STATEMENT ///////////////////////////////////////////////////////

cmd ::= DROP TABLE name(A). {builderStartDropTable(builder, A);}

// SELECT STATEMENT ///////////////////////////////////////////////////////////

cmd ::= SELECT set_quantifier select_columns select_table.

set_quantifier ::= DISTINCT|ALL.
set_quantifier ::= .

select_columns ::= select_columns COMMA select_column.
select_columns ::= select_column.

select_column ::= name(X) DOT ASTERISK. { builderAddSelectAllColumns(builder, X); }
select_column ::= name(X) DOT name(Y) as_clause(Z). {
  builderAddQualifiedSelectColumn(builder, X, Y, Z);
}

as_clause(A) ::= AS name(X).  { A = X; }
as_clause(A) ::= .            { A = nullptr; printf("Empty AS\n"); }

select_table ::= FROM table_references where_clause group_by_clause.
select_table ::= FROM name(A).  { (void)A; }

table_references ::= table_references COMMA table_reference.
table_references ::= table_reference.

table_reference ::= name(X) as_clause(Y). 
{
  (void)X; 
  (void)Y; 
  printf("Table reference: %s as %s\n", X->text->c_str(), Y->text->c_str());
}

table_reference ::= . { printf("Empty table reference\n"); }

// TODO: consider other clauses
where_clause ::= WHERE search_condition. { printf("WHERE clause\n"); }

group_by_clause ::= .

// TODO: consider other predicates
search_condition ::= comparison_predicate.

comparison_predicate ::= expr. { builderGeneratePredicates(builder); }

// UPDATE STATEMENT ///////////////////////////////////////////////////////////

// INSERT INTO STATEMENT //////////////////////////////////////////////////////

// DELETE STATEMENT ///////////////////////////////////////////////////////////
