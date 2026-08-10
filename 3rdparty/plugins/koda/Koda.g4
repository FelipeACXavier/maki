grammar Koda;

// =============================================================================
// Entry
// =============================================================================

system
  : topLevelDeclaration* EOF
  ;

// =============================================================================
// Top-level
// =============================================================================

topLevelDeclaration
  : topLevelComponent
  | typeDeclaration
  | enumDeclaration
  ;

topLevelComponent
  : TASK  IDENT LPAREN argumentList? RPAREN LBRACE statement* RBRACE
  | CAPABILITY IDENT LPAREN argumentList? RPAREN LBRACE statement* RBRACE
  ;

// =============================================================================
// Type system
// =============================================================================
typeDeclaration
  : TYPE qualifiedName (EXTENDS qualifiedName)? LBRACE fieldDeclaration* RBRACE # typeRecord
  | TYPE qualifiedName ASSIGN typeReference SEMI                                # typeAlias
  ;

fieldDeclaration
  : IDENT COLON typeReference SEMI
  ;

enumDeclaration
  : ENUM qualifiedName (COLON typeReference)? LBRACE enumValue* RBRACE
  ;

enumValue
  : IDENT (ASSIGN enumLiteral)? SEMI
  ;

enumLiteral
  : STRING
  | MINUS? NATURAL
  ;

typeReference
  : qualifiedName                                                   # typeNamed
  | LIST LT typeReference GT                                        # typeList
  | OPTIONAL LT typeReference GT                                    # typeOptional
  | MAP LT typeReference COMMA typeReference GT                     # typeMap
  ;

qualifiedName
  : IDENT (DOUBLE_COLON IDENT)*
  ;

// =============================================================================
// Arguments
// =============================================================================
argumentList
  : argument (COMMA argument)*
  ;

argument
  : IDENT IDENT                 # argPlain
  | IDENT REQ IDENT             # argReq
  | IDENT PRO IDENT             # argPro
  ;

// =============================================================================
// Statements
// =============================================================================
statement
  : tasksBlock
  | varsBlock
  | actionBlock
  | serviceBlock
  | topicBlock
  | rosDefStatement
  ;

tasksBlock
  : STRATEGY LBRACE flow+ RBRACE
  ;

flow
  : IDENT (LBRACK identList? RBRACK)? COLON strategy SEMI
  ;

identList
  : IDENT (COMMA IDENT)*
  ;

varsBlock
  : VARS LBRACE variableStatement+ RBRACE
  ;

variableStatement
  : IDENT IDENT ASSIGN expression COLON expression
  ;

actionBlock
  : ACTION STRING STRING LBRACE rosDefStatement* RBRACE
  ;

serviceBlock
  : SERVICE STRING STRING LBRACE rosDefStatement* RBRACE
  ;

topicBlock
  : TOPIC STRING STRING LBRACE rosDefStatement* RBRACE
  ;

// =============================================================================
// ROS def statements / event defs
// =============================================================================

rosDefStatement
  : TRIGGER COLON eventDefStatement SEMI
  | RETURN  COLON eventDefStatement SEMI
  | ABORT   COLON eventDefStatement SEMI
  | ERROR   COLON eventDefStatement SEMI
  | IN      COLON eventDefStatement SEMI
  | OUT     COLON eventDefStatement SEMI
  ;

eventDefStatement
  : IDENT identifier LPAREN argumentList? RPAREN (COLON eventDefComponentList)?
  ;

eventDefComponentList
  : eventDefComponent (COMMA eventDefComponent)+
  ;

eventDefComponent
  : rosData IDENT COLON STRING IDENT STRING IDENT STRING         # edcRosEvent
  | TIMEOUT NATURAL timeUnit ARROW IDENT                         # edcTimeout
  | ALLOWED IN whenMode                                          # edcWhenAllowedIn
  | REPLY IDENT whenMode                                         # edcReply
  | AFTER IDENT                                                  # edcDependsAfter
  | ONCE IN whenMode (LBRACE statement* RBRACE)?                 # edcOnceIn
  | TRIGGER (LBRACE statement* RBRACE)?                          # edcStart
  | ABORT (LBRACE statement* RBRACE)?                            # edcReset
  ;

rosData
  : TOPIC
  | SERVICE
  | ACTION
  ;

whenMode
  : ALWAYS
  | MISSION
  | IDLE
  ;

timeUnit
  : S
  | MS
  | US
  | NS
  ;

// =============================================================================
// Strategy language
// =============================================================================

strategy
  : strategy (ARROW strategy)+                                     # stratSeq
  | JOIN   LPAREN strategy (PIPE strategy)+ RPAREN                 # stratJoin
  | EITHER LPAREN strategy (PIPE strategy)+ RPAREN                 # stratEither
  | LET IDENT ASSIGN eventStatement                                # stratLet
  | WITHIN NATURAL DO strategy ELSE strategy                       # stratWithin
  | IF expression THEN strategy (ELSE strategy)?                   # stratIfElse
  | REPEAT NATURAL NATURAL LPAREN strategy RPAREN strategyHandler* # stratRepeat
  | GUARD LBRACE expression RBRACE                                 # stratGuard
  | END                                                            # stratEnd
  | CONTINUE                                                       # stratContinue
  | identifier                                                     # stratRef
  | eventStatement strategyHandler*                                # stratTask
  | LPAREN strategy RPAREN                                         # stratParen
  ;

strategyHandler
  : ON ERROR strategy                                            # handlerOnError
  | ON ABORT strategy                                            # handlerOnAbort
  | ON eventStatement strategy                                   # handlerOnEmitter
  ;

eventStatement
  : identifier DOT identifier LPAREN exprList? RPAREN            # evQualifiedCall
  | identifier LPAREN exprList? RPAREN                           # evCall
  ;

exprList
  : expression (COMMA expression)*
  ;

// =============================================================================
// Expression language (precedence mirrors Rascal as closely as practical)
// =============================================================================

expression
  : exprOr
  ;

exprOr
  : exprAnd (OR exprAnd)*
  ;

exprAnd
  : exprCmp (AND exprCmp)*
  ;

// Comparisons are non-assoc in Rascal; we implement as "at most one" comparator.
exprCmp
  : exprNot (compOp exprNot)?
  ;

compOp
  : EQ
  | NEQ
  | LEQ
  | GEQ
  | LT
  | GT
  ;

exprNot
  : NOT exprNot
  | exprAdd
  ;

exprAdd
  : exprMul ((PLUS | MINUS) exprMul)*
  ;

exprMul
  : exprUnary ((STAR | SLASH) exprUnary)*
  ;

exprUnary
  : MINUS exprUnary
  | exprPrimary
  ;

exprPrimary
  : eventStatement                                               # exprCall
  | IDENT                                                        # exprId
  | STRING                                                       # exprString
  | NATURAL                                                      # exprInt
  | REAL                                                         # exprFloat
  | LPAREN expression RPAREN                                     # exprParen
  ;

// =============================================================================
// Lexer
// =============================================================================
identifier
  : IDENT
  | ABORT
  | TRIGGER
  | RETURN
  | ERROR
  | IN
  | OUT
  ;

// --- Keywords / fixed words (put before IDENT so they win) ---
TASK       : 'task';
CAPABILITY : 'capability';

TYPE       : 'type';
ENUM       : 'enum';
EXTENDS    : 'extends';
LIST       : 'list';
OPTIONAL   : 'optional';
MAP        : 'map';

STRATEGY   : 'strategy';
VARS       : 'vars';

ACTION     : 'action';
SERVICE    : 'service';
TOPIC      : 'topic';

TRIGGER    : 'trigger';
RETURN     : 'return';
ABORT      : 'abort';
ERROR      : 'error';
IN         : 'in';
OUT        : 'out';
ON         : 'on';

REQ        : 'req';
PRO        : 'pro';

END        : 'end';
CONTINUE   : 'continue';
REPEAT     : 'repeat';
JOIN       : 'join';
EITHER     : 'either';

LET        : 'let';
WITHIN     : 'within';
DO         : 'do';
ELSE       : 'else';
IF         : 'if';
THEN       : 'then';
GUARD      : 'guard';
EVERY      : 'every';

TIMEOUT    : 'timeout';
ALLOWED    : 'allowed';
REPLY      : 'reply';
AFTER      : 'after';
ONCE       : 'once';

ALWAYS     : 'always';
MISSION    : 'mission';
IDLE       : 'idle';

S          : 's';
MS         : 'ms';
US         : 'us';
NS         : 'ns';

// --- Operators / punctuation ---
ARROW      : '-->';
PIPE       : '|';

EQ         : '==';
NEQ        : '!=';
LEQ        : '<=';
GEQ        : '>=';
LT         : '<';
GT         : '>';

ASSIGN       : '=';
DOUBLE_COLON : '::';
COLON        : ':';
COMMA        : ',';
SEMI         : ';';

DOT        : '.';

PLUS       : '+';
MINUS      : '-';
STAR       : '*';
SLASH      : '/';
NOT        : '!';

AND        : 'and';
OR         : 'or';

LPAREN     : '(';
RPAREN     : ')';
LBRACE     : '{';
RBRACE     : '}';
LBRACK     : '[';
RBRACK     : ']';

// --- Literals ---
NATURAL    : [0-9]+ ;
REAL       : [0-9]+ '.' [0-9]+ ;

// Rascal Ident: [a-zA-Z_\-:$][a-zA-Z0-9_\-:$]* (excluding Keywords)
// In ANTLR: keywords are separate tokens above, so IDENT won't match them.
IDENT      : [a-zA-Z_][a-zA-Z0-9_\-]* ;

// String with escapes similar to your Rascal version.
STRING
  : '"' ( ~["\\] | '\\' (["\\/bfnrt] | 'u' HEX HEX HEX HEX | 'U' HEX HEX HEX HEX HEX HEX) )* '"'
  ;

fragment HEX : [0-9a-fA-F];

// Rascal Any: "<(" ![$]* ")>"
// You can tighten this if you truly want to forbid '$' inside.
ANY        : '<(' .*? ')>' ;

// --- Whitespace and comments (skip) ---
LINE_COMMENT
  : '//' ~[\r\n]* -> skip
  ;

BLOCK_COMMENT
  : '/*' .*? '*/' -> skip
  ;

WS
  : [ \t\r\n\f]+ -> skip
  ;
