
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaLexer : public antlr4::Lexer {
public:
  enum {
    TASK = 1, CAPABILITY = 2, TYPE = 3, ENUM = 4, EXTENDS = 5, LIST = 6, 
    OPTIONAL = 7, MAP = 8, MAPPING = 9, TO = 10, STRATEGY = 11, PARAMETERS = 12, 
    ACTION = 13, SERVICE = 14, TOPIC = 15, TRIGGER = 16, RETURN = 17, ABORT = 18, 
    ERROR = 19, IN = 20, OUT = 21, ON = 22, CONSUMES = 23, PRODUCES = 24, 
    DATA = 25, REQ = 26, PRO = 27, SUCCESS = 28, FAILURE = 29, CONTINUE = 30, 
    REPEAT = 31, JOIN = 32, EITHER = 33, WHEN = 34, CHOOSE = 35, WITHIN = 36, 
    DO = 37, ELSE = 38, THEN = 39, EVERY = 40, ARROW = 41, PIPE = 42, EQ = 43, 
    NEQ = 44, LEQ = 45, GEQ = 46, LT = 47, GT = 48, ASSIGN = 49, DOUBLE_COLON = 50, 
    COLON = 51, COMMA = 52, SEMI = 53, DOT = 54, PLUS = 55, MINUS = 56, 
    STAR = 57, SLASH = 58, NOT = 59, AND = 60, OR = 61, LPAREN = 62, RPAREN = 63, 
    LBRACE = 64, RBRACE = 65, LBRACK = 66, RBRACK = 67, NATURAL = 68, REAL = 69, 
    BOOLEAN = 70, IDENT = 71, STRING = 72, ANY = 73, LINE_COMMENT = 74, 
    BLOCK_COMMENT = 75, WS = 76
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

