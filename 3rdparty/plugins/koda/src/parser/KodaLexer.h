
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaLexer : public antlr4::Lexer {
public:
  enum {
    TASK = 1, CAPABILITY = 2, TYPE = 3, ENUM = 4, EXTENDS = 5, LIST = 6, 
    OPTIONAL = 7, MAP = 8, MAPPING = 9, TO = 10, ANNOTATIONS = 11, STRATEGY = 12, 
    PARAMETERS = 13, ACTION = 14, SERVICE = 15, TOPIC = 16, TRIGGER = 17, 
    RETURN = 18, ABORT = 19, ERROR = 20, IN = 21, OUT = 22, ON = 23, CONSUMES = 24, 
    PRODUCES = 25, DATA = 26, REQ = 27, PRO = 28, SUCCESS = 29, FAILURE = 30, 
    CONTINUE = 31, REPEAT = 32, JOIN = 33, EITHER = 34, WHEN = 35, CHOOSE = 36, 
    WITHIN = 37, DO = 38, ELSE = 39, THEN = 40, EVERY = 41, ARROW = 42, 
    PIPE = 43, EQ = 44, NEQ = 45, LEQ = 46, GEQ = 47, LT = 48, GT = 49, 
    ASSIGN = 50, DOUBLE_COLON = 51, COLON = 52, COMMA = 53, SEMI = 54, DOT = 55, 
    PLUS = 56, MINUS = 57, STAR = 58, SLASH = 59, NOT = 60, AND = 61, OR = 62, 
    LPAREN = 63, RPAREN = 64, LBRACE = 65, RBRACE = 66, LBRACK = 67, RBRACK = 68, 
    NATURAL = 69, REAL = 70, BOOLEAN = 71, IDENT = 72, STRING = 73, ANY = 74, 
    LINE_COMMENT = 75, BLOCK_COMMENT = 76, WS = 77
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

