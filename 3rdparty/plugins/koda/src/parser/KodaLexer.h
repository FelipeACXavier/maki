
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
    REQ = 25, PRO = 26, END = 27, CONTINUE = 28, REPEAT = 29, JOIN = 30, 
    EITHER = 31, WITHIN = 32, DO = 33, ELSE = 34, THEN = 35, EVERY = 36, 
    ARROW = 37, PIPE = 38, EQ = 39, NEQ = 40, LEQ = 41, GEQ = 42, LT = 43, 
    GT = 44, ASSIGN = 45, DOUBLE_COLON = 46, COLON = 47, COMMA = 48, SEMI = 49, 
    DOT = 50, PLUS = 51, MINUS = 52, STAR = 53, SLASH = 54, NOT = 55, AND = 56, 
    OR = 57, LPAREN = 58, RPAREN = 59, LBRACE = 60, RBRACE = 61, LBRACK = 62, 
    RBRACK = 63, NATURAL = 64, REAL = 65, BOOLEAN = 66, IDENT = 67, STRING = 68, 
    ANY = 69, LINE_COMMENT = 70, BLOCK_COMMENT = 71, WS = 72
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

