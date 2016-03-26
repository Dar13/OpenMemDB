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

%token_prefix TK_

%include {#include <sql/omdb_parser.h>}

%token_type   {Token}
%default_type {Token}

%extra_argument { StatementBuilder* builder}

%syntax_error {
  int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
  for(int i = 0; i < n; ++i) {
    int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
    if (a < YYNSTATE + YYNRULE) {
      //printf("possible token: %s\n", yyTokenName[i]);
    }
  }
}

%parse_failure {
  builderClean(builder);
}

%parse_accept {
  //printf("Parse accepts input!\n");
}

input ::= cmd_list.

cmd_list ::= cmd_list end_cmd.
cmd_list ::= end_cmd.

end_cmd ::= SEMICOLON.
end_cmd ::= explain cmdx SEMICOLON. 

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
name(A) ::= id(X).      {A = X;}
name(A) ::= STRING(X).  {A = X;}

%type typetoken {Token}
type(A) ::= . { A = nullptr; }
type(A) ::= typetoken(X).  {A = X;}

typetoken(A) ::= typename(X).   {A = X;}

%type typename {Token}
typename(A) ::= ids(X). {A = X;}

column_args ::= column_args column_constraints.
column_args ::= .

column_constraints ::= DEFAULT term(X).
{
  builderAddColumnConstraint(builder, SQLConstraintType::DEFAULT, X);
}

column_constraints ::= NOT NULL. 
{
  builderAddColumnConstraint(builder, SQLConstraintType::NOT_NULL);
}

column_constraints ::= AUTO_INCREMENT.
{
  builderAddColumnConstraint(builder, SQLConstraintType::AUTO_INCREMENT);
}

// Time to define operator precedence
%left OR.
%left AND.
%right NOT.
%left BETWEEN IN ISNULL NOTNULL NE EQ.
%left GT LE LT GE.

expr ::= expr AND|OR(OP) expr. { builderStartNestedExpr(builder, OP);}
expr ::= term(X) NE|EQ(OP) term(Y). { builderAddValueExpr(builder, OP, X, Y); }

term(A) ::= name(X) DOT column_id(Y). { 
    X->table_name = X->text;
    X->column_name = Y->text;
    X->text = X->text + "." + Y->text;
    //printf("Reference: %s.%s\n", X->table_name.c_str(), X->column_name.c_str());
    X->is_column = true;
    A = X;
}
term(A) ::= column_id(X). { A = X; /*printf("expression term\n");*/ }
term(A) ::= INTEGER|FLOAT|DATE|TIME(X).
{ 
  //printf("Numeric term handling\n");
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
as_clause(A) ::= .            { A = nullptr; }

select_table ::= FROM table_references where_clause group_by_clause.
select_table ::= FROM name(A).  { (void)A; }

table_references ::= table_references COMMA table_reference(X). { (void)X; }
table_references ::= table_reference(X). { (void)X; }

table_reference(A) ::= name(X) as_clause(Y). 
{
  (void)A;
  (void)X; 
  (void)Y; 
  //printf("Table reference: %s as %s\n", X->text->c_str(), Y->text->c_str());
}

table_reference ::= .

// TODO: consider other clauses
where_clause ::= WHERE search_condition.

group_by_clause ::= .

// TODO: consider other predicates
search_condition ::= comparison_predicate.

comparison_predicate ::= expr. { }

// UPDATE STATEMENT ///////////////////////////////////////////////////////////

cmd ::= UPDATE update_table SET update_expr_list where_clause.

update_table(A) ::= name(X).
{
  A = X;
  // TODO: update builder functions
}

update_expr_list ::= update_expr_list COMMA update_expr.
update_expr ::= column_id(X) EQ(OP) term(Y). { builderAddUpdateExpr(builder, OP, X, Y); }

// INSERT INTO STATEMENT //////////////////////////////////////////////////////

cmd ::= INSERT INTO insert_table VALUES LPAREN insert_values RPAREN.

insert_table(A) ::= name(X). 
{ 
  A = X; 
  builderStartInsertCommand(builder);
  builderAddTableName(builder, A);
  //printf("Starting INSERT INTO statement parse\n");
}

insert_values ::= insert_values COMMA insert_term.
insert_values ::= insert_term.

insert_term(A) ::= INTEGER|FLOAT|DATE|TIME|NULL(X).
{
  A = X;
  builderAddDataItem(builder, X);
}

// DELETE STATEMENT ///////////////////////////////////////////////////////////

cmd ::= DELETE FROM delete_table where_clause.

delete_table(A) ::= name(X).
{
  A = X;
  // TODO: Update builder functions
}
