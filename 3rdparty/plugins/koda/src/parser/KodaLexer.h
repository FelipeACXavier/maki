
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaLexer : public antlr4::Lexer {
public:
  enum {
    TASK = 1, CAPABILITY = 2, TYPE = 3, ENUM = 4, EXTENDS = 5, LIST = 6, 
    OPTIONAL = 7, MAP = 8, MAPPING = 9, TO = 10, ANNOTATIONS = 11, STRATEGY = 12, 
    PARAMETERS = 13, PROPERTIES = 14, ACTION = 15, SERVICE = 16, TOPIC = 17, 
    IMPLIES = 18, WHILE = 19, ALWAYS = 20, EVENTUALLY = 21, NEXT = 22, NEVER = 23, 
    NEGATION = 24, IF = 25, BETWEEN = 26, UNTIL = 27, UNLESS = 28, IS = 29, 
    WAS = 30, RUNNING = 31, STARTED = 32, SUCCEEDED = 33, FAILED = 34, REJECTED = 35, 
    STOPPED = 36, ABORTED = 37, TRIGGER = 38, RETURN = 39, ABORT = 40, ERROR = 41, 
    IN = 42, OUT = 43, ON = 44, CONSUMES = 45, PRODUCES = 46, DATA = 47, 
    REQ = 48, PRO = 49, SUCCESS = 50, FAILURE = 51, CONTINUE = 52, REPEAT = 53, 
    RETRY = 54, JOIN = 55, EITHER = 56, WHEN = 57, CHOOSE = 58, WITHIN = 59, 
    DO = 60, ELSE = 61, THEN = 62, EVERY = 63, ARROW = 64, PIPE = 65, EQ = 66, 
    NEQ = 67, LEQ = 68, GEQ = 69, LT = 70, GT = 71, ASSIGN = 72, DOUBLE_COLON = 73, 
    COLON = 74, COMMA = 75, SEMI = 76, DOT = 77, PLUS = 78, MINUS = 79, 
    STAR = 80, SLASH = 81, NOT = 82, AND = 83, OR = 84, LPAREN = 85, RPAREN = 86, 
    LBRACE = 87, RBRACE = 88, LBRACK = 89, RBRACK = 90, NATURAL = 91, REAL = 92, 
    BOOLEAN = 93, IDENT = 94, STRING = 95, ANY = 96, LINE_COMMENT = 97, 
    BLOCK_COMMENT = 98, WS = 99
  };

  explicit KodaLexer(antlr4::CharStream *input);

  ~KodaLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

