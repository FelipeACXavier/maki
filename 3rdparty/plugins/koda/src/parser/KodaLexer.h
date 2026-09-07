
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
    DATA = 25, REQ = 26, PRO = 27, END = 28, CONTINUE = 29, REPEAT = 30, 
    JOIN = 31, EITHER = 32, WHEN = 33, CHOOSE = 34, WITHIN = 35, DO = 36, 
    ELSE = 37, THEN = 38, EVERY = 39, ARROW = 40, PIPE = 41, EQ = 42, NEQ = 43, 
    LEQ = 44, GEQ = 45, LT = 46, GT = 47, ASSIGN = 48, DOUBLE_COLON = 49, 
    COLON = 50, COMMA = 51, SEMI = 52, DOT = 53, PLUS = 54, MINUS = 55, 
    STAR = 56, SLASH = 57, NOT = 58, AND = 59, OR = 60, LPAREN = 61, RPAREN = 62, 
    LBRACE = 63, RBRACE = 64, LBRACK = 65, RBRACK = 66, NATURAL = 67, REAL = 68, 
    BOOLEAN = 69, IDENT = 70, STRING = 71, ANY = 72, LINE_COMMENT = 73, 
    BLOCK_COMMENT = 74, WS = 75
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

