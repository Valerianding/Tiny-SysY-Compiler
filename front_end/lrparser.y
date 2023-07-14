%{

#include "ast.h"
#include "bstrlib.h"
#include "symtab.h"
#include <stdio.h>
int yylex(void);
void yyerror(char *);
extern char* yytext;
extern Symtab *this;
extern FILE* yyin;
extern int return_index;
extern int return_stmt_num[20];
extern HashMap *tokenMap;

extern int def_call;

%}

%union{
	int	iVal;
	float   fVal;
	bstring sVal;
	past	pAst;
};


%token <sVal> IDent
%token <iVal> num_int
%token <fVal> num_float
%token IF
%token ELSE WHILE BREAK CONTINUE RETURN
%token INT FLOAT VOID
%token CONST SEMICOLON COMMA
%token ASSIGN
%token LPAR RPAR LBRACKET RBRACKET LSQUARE RSQUARE
%token AND OR NOT LESSEQ GREATEQ NOTEQ EQ GREAT LESS

%type  <pAst>   CompUnits CompUnit Decl ConstDecl ConstDefList BType IdentArray ConstDef ConstExpList ConstInitVal VarDecl VarDef VarDefList InitVal InitValList FuncDef FuncFParams FuncFParam ExpArray Block BlockItem BlockItemList Stmt Exp Cond LVal PrimaryExp Number UnaryExp FuncRParams MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp ScopeStart ScopeEnd

%start CompUnits
%left ASSIGN
%left OR AND NOT

%%

CompUnits
    : CompUnit                       {if($1 != NULL) $$ = newFollowNode("CompUnit", $1, NULL);
                                       else $$ = NULL;
                                       TRoot = $$;}
    | CompUnits CompUnit             {if($1 != NULL && $2 != NULL)
                                           $$ = newFollowNode("CompUnit", $1, $2);
                                      else if($1 != NULL && $2 == NULL);
                                      else if($1 == NULL && $2 != NULL)
                                           $$ = newFollowNode("CompUnit", $2, NULL);
                                      else $$ = NULL;
                                      TRoot = $$;}
	;
CompUnit
      : Decl                        {if($1 == NULL)  $$ = NULL;
                                     else  $$ = $1;}
      | FuncDef                     {$$ = $1;}
      ;

Decl
    : ConstDecl                                     {$$ = $1;}
    | VarDecl                                      {if($1 == NULL)  $$ = NULL;
                                                    else  $$ = $1;}
    ;


ConstDecl
    : CONST BType ConstDefList SEMICOLON       {past con=prefixNode("const",NULL);past prefix_type=prefixNode(NULL,$2);
                                                con->next=prefix_type;
                                                $$ = newAnotherNode("ConstDecl",con,$3);
                                                insert_var_into_symtab($2,$3->left);}
    ;

/*新加的,形如ConstDef,ConstDef,ConstDef*/
ConstDefList
    : ConstDef                                     {$$ = newFollowNode("ConstDefList",$1,NULL);}
    | ConstDefList COMMA ConstDef                {$$ = newFollowNode("ConstDefList",$1,$3);}

BType
    : INT                                        {$$ = prefixNode("int",NULL);}
    | FLOAT                                      {$$ = prefixNode("float",NULL);}
    ;

/*新加的,形如IDent[ConstExp][ConstExp]*/
IdentArray
    :IDent LSQUARE ConstExp RSQUARE               {$$ = newFollowNode("IdentArray",newIdent($1),$3);}
    |IdentArray LSQUARE ConstExp RSQUARE         {$$ = newFollowNode("IdentArray",$1,$3);}

ConstDef
    : IDent ASSIGN ConstInitVal                     {$$ = newAnotherNode("ConstDef",newIdent($1),$3);}
    | IdentArray ASSIGN ConstInitVal               {$$ = newAnotherNode("ConstDef_array_init",$1,$3);}
    ;

/*形如ConstExp,ConstExp*/
ConstExpList
    : ConstInitVal                                        {$$ = newFollowNode("ConstExpList",$1,NULL);}
    | ConstExpList COMMA ConstInitVal                   {$$ = newFollowNode("ConstExpList",$1,$3);}

ConstInitVal
    : ConstExp                                        {$$ = $1;}
    | LPAR RPAR                                   {$$ = newAnotherNode("ConstExpList",NULL,NULL);}
    | LPAR ConstExpList RPAR                      {$$ = $2;}


VarDecl
    : BType VarDefList SEMICOLON                    {if($2 == NULL)  $$ = NULL;
                                                     else {
                                                           $$ = newAnotherNode("VarDecl",$1,$2);
                                                           insert_var_into_symtab($1,$2->left);
                                                       }
                                                     }
    ;

/*新加的，形如VarDef,VarDef,VarDef*/
VarDefList
    : VarDef                                          {if($1 == NULL)  $$ = NULL;
                                                       else $$ = newFollowNode("VarDefList",$1,NULL);}
    | VarDefList COMMA VarDef                       { if ($1 == NULL && $3 == NULL)  $$ = NULL;
                                                      else if($1 !=NULL && $3 == NULL);
                                                      else if($1 == NULL && $3 !=NULL)
                                                          $$ = newFollowNode("VarDefList",$3,NULL);
                                                      else $$ = newFollowNode("VarDefList",$1,$3);}


VarDef
    : IDent                                            {unsigned int key = HashKey(bstr2cstr($1,"\0"));
                                                         int* cnt = HashMapGet(tokenMap, (void*)key);
                                                            if(*cnt == 0) {$$ = NULL;}
                                                            else  $$ = newIdent($1);}
    | IdentArray                                      {$$ = $1;}
    | IDent ASSIGN InitVal                           {
                                                         if(def_call == 0)
                                                         {
                                                             unsigned int key = HashKey(bstr2cstr($1,"\0"));
                                                             int* cnt = HashMapGet(tokenMap, (void*)key);
                                                             if(*cnt == 0) $$ = NULL;
                                                              else  $$ = newAnotherNode("VarDef_init",newIdent($1),$3);
                                                         }
                                                         else
                                                         {
                                                             $$ = newAnotherNode("VarDef_init",newIdent($1),$3);
                                                             def_call=0;
                                                         }
                                                    }
    | IdentArray ASSIGN InitVal                     {$$ = newAnotherNode("VarDef_array_init",$1,$3);}
    ;


InitVal
    : Exp                                             {$$ = $1;}
    | LPAR RPAR                                   {$$ = newAnotherNode("InitValList",NULL,NULL);}
    | LPAR InitValList RPAR                       {$$=$2;}
    ;

/*新加的，形如InitVal,InitVal,InitVal*/
InitValList
    : InitVal                                         {$$ = newAnotherNode("InitValList",$1,NULL);}
    | InitValList COMMA InitVal                     {$$ = newFollowNode("InitValList",$1,$3);}


FuncDef
    : BType IDent LBRACKET RBRACKET Block                 {past id=newIdent($2);past prefix=prefixNode(NULL,$1);prefix->next=id;
                                                           $$ = newAnotherNode("FuncDef",prefix,$5);
                                                           insert_func_into_symtab(prefix,id,NULL);
                                                           return_index++;}
    | BType IDent LBRACKET FuncFParams RBRACKET Block     {past id=newIdent($2);id->left=$4;past prefix=prefixNode(NULL,$1);prefix->next=id;
                                                          $$ = newAnotherNode("FuncDef",prefix,$6);
                                                          insert_func_into_symtab(prefix,id,$4->left);
                                                           insert_func_params($4->left);
                                                           return_index++;}
    | VOID IDent LBRACKET RBRACKET Block                 {past id=newIdent($2);past prefix=prefixNode("void",NULL);prefix->next=id;
                                                          $$ = newAnotherNode("FuncDef",prefix,$5);
                                                          insert_func_into_symtab(prefix,id,NULL);
                                                          return_index++;}
    | VOID IDent LBRACKET FuncFParams RBRACKET Block     {past id=newIdent($2);id->left=$4;past prefix=prefixNode("void",NULL);prefix->next=id;
                                                               $$ = newAnotherNode("FuncDef",prefix,$6);
                                                               insert_func_into_symtab(prefix,id,$4->left);
                                                               insert_func_params($4->left);
                                                               return_index++;}
    ;

FuncFParams
    : FuncFParam                                     {$$ = newFollowNode("FuncFParams",$1,NULL);}
    | FuncFParams COMMA FuncFParam                 {$$ = newFollowNode("FuncFParams",$1,$3);}
    ;


FuncFParam
    : BType IDent                                      {past prefix=prefixNode(NULL,$1);past id=newIdent($2);prefix->next=id;
                                                       $$ = newAnotherNode("FuncFParam",prefix,NULL);}
    | BType IDent LSQUARE RSQUARE                  {past prefix=prefixNode(NULL,$1);past id=newIdent($2);prefix->next=id;
                                                    $$ = newAnotherNode("FuncFParam",prefix,newNumInt(1));}
    | BType IDent LSQUARE RSQUARE ExpArray         {past prefix=prefixNode(NULL,$1);past id=newIdent($2);prefix->next=id;
                                                     $$ = newAnotherNode("FuncFParam",prefix,$5);}
    ;

/*形如[Exp][Exp]*/
ExpArray
    : LSQUARE Exp RSQUARE                           {$$ = newFollowNode("ExpArray",$2,NULL); def_call=0;}
    | ExpArray LSQUARE Exp RSQUARE                  {$$ = newFollowNode("ExpArray",$1,$3); def_call=0;}

ScopeStart
    : LPAR                                          {scope_start(this);}
    ;

ScopeEnd
    : RPAR                                           {scope_end(this);}
    ;

Block
    : LPAR RPAR                                    {$$ = newAnotherNode("Block_EMPTY",NULL,NULL);}
    | ScopeStart BlockItemList ScopeEnd                       {if($2 == NULL) $$ = newAnotherNode("Block_EMPTY",NULL,NULL);
                                                                else $$ = $2;}
    ;

BlockItem
    : Decl                                              {if($1 == NULL) $$ =NULL;
                                                          else $$ = $1;}
    | Stmt                                              {$$ = $1;}
    ;

/*新加的,形如BlockItem BlockItem BlockItem*/
BlockItemList
    : BlockItem                                          {if($1 == NULL) $$ = NULL;
                                                          else $$ = newAnotherNode("BlockItemList",$1,NULL);}
    | BlockItemList BlockItem                            {if($1 != NULL && $2 != NULL)
                                                             $$ = newFollowNode("BlockItemList",$1,$2);
                                                           else if($1 != NULL && $2 == NULL);
                                                           else if($1 == NULL && $2 != NULL)
                                                             $$ = newAnotherNode("BlockItemList",$2,NULL);
                                                           else $$ = NULL;
                                                           }
    ;

Stmt
    : LVal ASSIGN Exp SEMICOLON                      {$$ = newAnotherNode("Assign_Stmt",$1,$3); def_call=0;}
    | SEMICOLON                                        {$$ = newAnotherNode("Empty_Stmt",NULL,NULL);}
    | Exp SEMICOLON                                    {$$ = $1; def_call=0;}
    | Block                                              {$$ = $1;}
    | IF LBRACKET Cond RBRACKET Stmt ELSE Stmt   {$$ = newAnotherNode("IfElse_Stmt",$3,newAnotherNode("If_Else",$5,$7));}
    | IF LBRACKET Cond RBRACKET Stmt               {$$ = newAnotherNode("IF_Stmt",$3,$5);}
    | WHILE LBRACKET Cond RBRACKET Stmt            {$$ = newAnotherNode("While_Stmt",$3,$5);}
    | BREAK SEMICOLON                                {$$ = newAnotherNode("Break_Stmt",NULL,NULL);}
    | CONTINUE SEMICOLON                             {$$ = newAnotherNode("Continue_Stmt",NULL,NULL);}
    | RETURN SEMICOLON                               {$$ = newAnotherNode("Return_Stmt",NULL,NULL);
                                                        return_stmt_num[return_index]++;}
    | RETURN Exp SEMICOLON                           {$$ = newAnotherNode("Return_Stmt",$2,NULL);
                                                        return_stmt_num[return_index]++; def_call=0;}
    ;

Exp
    : AddExp                                            {$$ = $1;}
    ;

Cond
    : LOrExp                                            {$$ = $1;}
    ;


LVal
    : IDent                                              {$$ = newIdent($1);}
    | IDent ExpArray                                     {$$ = newAnotherNode("LValArray",newIdent($1),$2);}
    ;



PrimaryExp
    : LBRACKET Exp RBRACKET                         {$$ = $2; def_call=0;}
    | LVal                                              {$$ = $1;}
    | Number                                            {$$ = $1;}
    ;

Number
    : num_int                                           {$$ = newNumInt($1);}
    | num_float                                         {$$ = newNumFloat($1);}
    ;

UnaryExp
    : PrimaryExp                                        {$$ = $1;}
    | IDent LBRACKET RBRACKET                        {$$ = newAnotherNode("Call_Func",newIdent($1),NULL); def_call=1;}
    | IDent LBRACKET FuncRParams RBRACKET            {$$ = newAnotherNode("Call_Func",newIdent($1),$3); def_call=1;}
    | '+' UnaryExp                                  {$$ = newExpr(newNumInt(0),'+',$2);}
    | '-' UnaryExp                                  {$$ = newExpr(newNumInt(0),'-',$2);}
    | '!' UnaryExp                                  {$$ = newExpr(newNumInt(0),'!',$2);}
    ;

FuncRParams
    : Exp                                               {$$ = newFollowNode("FuncRParams",$1,NULL); def_call=0;}
    | FuncRParams COMMA Exp                           {$$ = newFollowNode("FuncRParams",$1,$3); def_call=0;}
    ;


MulExp
    : UnaryExp                                          {$$ = $1;}
    | MulExp '*' UnaryExp                             {$$ = newExpr($1,'*',$3);}
    | MulExp '/' UnaryExp                             {$$ = newExpr($1,'/',$3);}
    | MulExp '%' UnaryExp                             {$$ = newExpr($1,'%',$3);}
    ;

AddExp
    : MulExp                                            {$$ = $1;}
    | AddExp '+' MulExp                               {$$ = newExpr($1,'+',$3);}
    | AddExp '-' MulExp                               {$$ = newExpr($1,'-',$3);}
    ;


RelExp
    : AddExp                                            {$$ = $1;}
    | RelExp LESS AddExp                              {$$ = newLogicExpr($1,"<",$3);}
    | RelExp GREAT AddExp                             {$$ = newLogicExpr($1,">",$3);}
    | RelExp LESSEQ AddExp                            {$$ = newLogicExpr($1,"<=",$3);}
    | RelExp GREATEQ AddExp                           {$$ = newLogicExpr($1,">=",$3);}
    ;


EqExp
    : RelExp                                            {$$ = $1;}
    | EqExp EQ RelExp                                 {$$ = newLogicExpr($1,"==",$3);}
    | EqExp NOTEQ RelExp                              {$$ = newLogicExpr($1,"!=",$3);}
    ;


LAndExp
    : EqExp                                             {$$ = $1;}
    | LAndExp AND EqExp                               {$$ = newLogicExpr($1,"&&",$3);}
    ;


LOrExp
    : LAndExp                                           {$$ = $1;}
    | LOrExp OR LAndExp                               {$$ = newLogicExpr($1,"||",$3);}
    ;


ConstExp
    : AddExp                                            {$$ = $1;}
    ;

%%