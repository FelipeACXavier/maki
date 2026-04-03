
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaLexer : public antlr4::Lexer {
public:
  enum {
    TASK = 1, CAPABILITY = 2, STRATEGY = 3, VARS = 4, ACTION = 5, SERVICE = 6, 
    TOPIC = 7, TRIGGER = 8, RETURN = 9, ABORT = 10, ERROR = 11, IN = 12, 
    OUT = 13, ON = 14, REQ = 15, PRO = 16, END = 17, CONTINUE = 18, REPEAT = 19, 
    JOIN = 20, EITHER = 21, LET = 22, WITHIN = 23, DO = 24, ELSE = 25, IF = 26, 
    THEN = 27, GUARD = 28, EVERY = 29, TIMEOUT = 30, ALLOWED = 31, REPLY = 32, 
    AFTER = 33, ONCE = 34, ALWAYS = 35, MISSION = 36, IDLE = 37, S = 38, 
    MS = 39, US = 40, NS = 41, ARROW = 42, PIPE = 43, EQ = 44, NEQ = 45, 
    LEQ = 46, GEQ = 47, LT = 48, GT = 49, ASSIGN = 50, COLON = 51, COMMA = 52, 
    SEMI = 53, DOT = 54, PLUS = 55, MINUS = 56, STAR = 57, SLASH = 58, NOT = 59, 
    AND = 60, OR = 61, LPAREN = 62, RPAREN = 63, LBRACE = 64, RBRACE = 65, 
    LBRACK = 66, RBRACK = 67, NATURAL = 68, REAL = 69, IDENT = 70, STRING = 71, 
    ANY = 72, LINE_COMMENT = 73, BLOCK_COMMENT = 74, WS = 75
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

