
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
    NEGATION = 24, IF = 25, BETWEEN = 26, UNTIL = 27, IS = 28, WAS = 29, 
    RUNNING = 30, STARTED = 31, REJECTED = 32, STOPPED = 33, ABORTED = 34, 
    TRIGGER = 35, RETURN = 36, ABORT = 37, ERROR = 38, IN = 39, OUT = 40, 
    ON = 41, CONSUMES = 42, PRODUCES = 43, DATA = 44, REQ = 45, PRO = 46, 
    SUCCESS = 47, FAILURE = 48, CONTINUE = 49, REPEAT = 50, JOIN = 51, EITHER = 52, 
    WHEN = 53, CHOOSE = 54, WITHIN = 55, DO = 56, ELSE = 57, THEN = 58, 
    EVERY = 59, ARROW = 60, PIPE = 61, EQ = 62, NEQ = 63, LEQ = 64, GEQ = 65, 
    LT = 66, GT = 67, ASSIGN = 68, DOUBLE_COLON = 69, COLON = 70, COMMA = 71, 
    SEMI = 72, DOT = 73, PLUS = 74, MINUS = 75, STAR = 76, SLASH = 77, NOT = 78, 
    AND = 79, OR = 80, LPAREN = 81, RPAREN = 82, LBRACE = 83, RBRACE = 84, 
    LBRACK = 85, RBRACK = 86, NATURAL = 87, REAL = 88, BOOLEAN = 89, IDENT = 90, 
    STRING = 91, ANY = 92, LINE_COMMENT = 93, BLOCK_COMMENT = 94, WS = 95
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

