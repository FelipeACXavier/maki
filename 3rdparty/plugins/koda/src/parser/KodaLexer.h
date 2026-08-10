
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaLexer : public antlr4::Lexer {
public:
  enum {
    TASK = 1, CAPABILITY = 2, TYPE = 3, ENUM = 4, EXTENDS = 5, LIST = 6, 
    OPTIONAL = 7, MAP = 8, STRATEGY = 9, VARS = 10, ACTION = 11, SERVICE = 12, 
    TOPIC = 13, TRIGGER = 14, RETURN = 15, ABORT = 16, ERROR = 17, IN = 18, 
    OUT = 19, ON = 20, REQ = 21, PRO = 22, END = 23, CONTINUE = 24, REPEAT = 25, 
    JOIN = 26, EITHER = 27, LET = 28, WITHIN = 29, DO = 30, ELSE = 31, IF = 32, 
    THEN = 33, GUARD = 34, EVERY = 35, TIMEOUT = 36, ALLOWED = 37, REPLY = 38, 
    AFTER = 39, ONCE = 40, ALWAYS = 41, MISSION = 42, IDLE = 43, S = 44, 
    MS = 45, US = 46, NS = 47, ARROW = 48, PIPE = 49, EQ = 50, NEQ = 51, 
    LEQ = 52, GEQ = 53, LT = 54, GT = 55, ASSIGN = 56, DOUBLE_COLON = 57, 
    COLON = 58, COMMA = 59, SEMI = 60, DOT = 61, PLUS = 62, MINUS = 63, 
    STAR = 64, SLASH = 65, NOT = 66, AND = 67, OR = 68, LPAREN = 69, RPAREN = 70, 
    LBRACE = 71, RBRACE = 72, LBRACK = 73, RBRACK = 74, NATURAL = 75, REAL = 76, 
    IDENT = 77, STRING = 78, ANY = 79, LINE_COMMENT = 80, BLOCK_COMMENT = 81, 
    WS = 82
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

