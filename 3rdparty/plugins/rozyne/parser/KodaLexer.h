
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaLexer : public antlr4::Lexer {
public:
  enum {
    TASK = 1, CAPABILITY = 2, STRATEGY = 3, VARS = 4, ACTION = 5, SERVICE = 6, 
    TOPIC = 7, TRIGGER = 8, RETURN = 9, ABORT = 10, ERROR = 11, IN = 12, 
    OUT = 13, ON = 14, REQ = 15, PRO = 16, END = 17, REPEAT = 18, JOIN = 19, 
    EITHER = 20, LET = 21, WITHIN = 22, DO = 23, ELSE = 24, IF = 25, THEN = 26, 
    GUARD = 27, EVERY = 28, TIMEOUT = 29, ALLOWED = 30, REPLY = 31, AFTER = 32, 
    ONCE = 33, ALWAYS = 34, MISSION = 35, IDLE = 36, S = 37, MS = 38, US = 39, 
    NS = 40, ARROW = 41, PIPE = 42, EQ = 43, NEQ = 44, LEQ = 45, GEQ = 46, 
    LT = 47, GT = 48, ASSIGN = 49, COLON = 50, COMMA = 51, SEMI = 52, DOT = 53, 
    PLUS = 54, MINUS = 55, STAR = 56, SLASH = 57, NOT = 58, AND = 59, OR = 60, 
    LPAREN = 61, RPAREN = 62, LBRACE = 63, RBRACE = 64, LBRACK = 65, RBRACK = 66, 
    NATURAL = 67, REAL = 68, IDENT = 69, STRING = 70, ANY = 71, LINE_COMMENT = 72, 
    BLOCK_COMMENT = 73, WS = 74
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

