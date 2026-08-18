
// Generated from Koda.g4 by ANTLR 4.10.1


#include "KodaVisitor.h"

#include "KodaParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct KodaParserStaticData final {
  KodaParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  KodaParserStaticData(const KodaParserStaticData&) = delete;
  KodaParserStaticData(KodaParserStaticData&&) = delete;
  KodaParserStaticData& operator=(const KodaParserStaticData&) = delete;
  KodaParserStaticData& operator=(KodaParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

std::once_flag kodaParserOnceFlag;
KodaParserStaticData *kodaParserStaticData = nullptr;

void kodaParserInitialize() {
  assert(kodaParserStaticData == nullptr);
  auto staticData = std::make_unique<KodaParserStaticData>(
    std::vector<std::string>{
      "system", "topLevelDeclaration", "topLevelComponent", "typeDeclaration", 
      "fieldDeclaration", "enumDeclaration", "enumValue", "enumLiteral", 
      "mappingDeclaration", "typeReference", "qualifiedName", "argumentList", 
      "argument", "statement", "tasksBlock", "flow", "identList", "varsBlock", 
      "variableStatement", "actionBlock", "serviceBlock", "topicBlock", 
      "rosDefStatement", "reqDefStatement", "eventDefStatement", "strategy", 
      "strategyHandler", "eventStatement", "exprList", "expression", "exprOr", 
      "exprAnd", "exprCmp", "compOp", "exprNot", "exprAdd", "exprMul", "exprUnary", 
      "exprPrimary", "identifier"
    },
    std::vector<std::string>{
      "", "'task'", "'capability'", "'type'", "'enum'", "'extends'", "'list'", 
      "'optional'", "'map'", "'mapping'", "'to'", "'strategy'", "'vars'", 
      "'action'", "'service'", "'topic'", "'trigger'", "'return'", "'abort'", 
      "'error'", "'in'", "'out'", "'on'", "'consumes'", "'produces'", "'req'", 
      "'pro'", "'end'", "'continue'", "'repeat'", "'join'", "'either'", 
      "'within'", "'do'", "'else'", "'then'", "'every'", "'-->'", "'|'", 
      "'=='", "'!='", "'<='", "'>='", "'<'", "'>'", "'='", "'::'", "':'", 
      "','", "';'", "'.'", "'+'", "'-'", "'*'", "'/'", "'!'", "'and'", "'or'", 
      "'('", "')'", "'{'", "'}'", "'['", "']'"
    },
    std::vector<std::string>{
      "", "TASK", "CAPABILITY", "TYPE", "ENUM", "EXTENDS", "LIST", "OPTIONAL", 
      "MAP", "MAPPING", "TO", "STRATEGY", "VARS", "ACTION", "SERVICE", "TOPIC", 
      "TRIGGER", "RETURN", "ABORT", "ERROR", "IN", "OUT", "ON", "CONSUMES", 
      "PRODUCES", "REQ", "PRO", "END", "CONTINUE", "REPEAT", "JOIN", "EITHER", 
      "WITHIN", "DO", "ELSE", "THEN", "EVERY", "ARROW", "PIPE", "EQ", "NEQ", 
      "LEQ", "GEQ", "LT", "GT", "ASSIGN", "DOUBLE_COLON", "COLON", "COMMA", 
      "SEMI", "DOT", "PLUS", "MINUS", "STAR", "SLASH", "NOT", "AND", "OR", 
      "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACK", "RBRACK", "NATURAL", 
      "REAL", "IDENT", "STRING", "ANY", "LINE_COMMENT", "BLOCK_COMMENT", 
      "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,71,553,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,1,0,5,0,82,8,0,10,0,12,0,85,
  	9,0,1,0,1,0,1,1,1,1,1,1,1,1,3,1,93,8,1,1,2,1,2,1,2,1,2,3,2,99,8,2,1,2,
  	1,2,1,2,5,2,104,8,2,10,2,12,2,107,9,2,1,2,1,2,1,2,1,2,1,2,3,2,114,8,2,
  	1,2,1,2,1,2,5,2,119,8,2,10,2,12,2,122,9,2,1,2,3,2,125,8,2,1,3,1,3,1,3,
  	1,3,3,3,131,8,3,1,3,1,3,5,3,135,8,3,10,3,12,3,138,9,3,1,3,1,3,1,3,1,3,
  	1,3,1,3,1,3,1,3,3,3,148,8,3,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,3,5,159,
  	8,5,1,5,1,5,5,5,163,8,5,10,5,12,5,166,9,5,1,5,1,5,1,6,1,6,1,6,3,6,173,
  	8,6,1,6,1,6,1,7,1,7,3,7,179,8,7,1,7,3,7,182,8,7,1,8,1,8,1,8,1,8,1,8,1,
  	9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,
  	3,9,207,8,9,1,10,1,10,1,10,5,10,212,8,10,10,10,12,10,215,9,10,1,11,1,
  	11,1,11,5,11,220,8,11,10,11,12,11,223,9,11,1,12,1,12,1,12,1,12,1,12,1,
  	12,1,12,1,12,1,12,3,12,234,8,12,1,13,1,13,1,13,1,13,1,13,1,13,3,13,242,
  	8,13,1,14,1,14,1,14,4,14,247,8,14,11,14,12,14,248,1,14,1,14,1,15,1,15,
  	1,15,3,15,256,8,15,1,15,3,15,259,8,15,1,15,1,15,1,15,1,15,1,16,1,16,1,
  	16,5,16,268,8,16,10,16,12,16,271,9,16,1,17,1,17,1,17,4,17,276,8,17,11,
  	17,12,17,277,1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,19,1,19,1,
  	19,1,19,1,19,1,19,5,19,295,8,19,10,19,12,19,298,9,19,1,19,1,19,1,20,1,
  	20,1,20,1,20,1,20,1,20,5,20,308,8,20,10,20,12,20,311,9,20,1,20,1,20,1,
  	21,1,21,1,21,1,21,1,21,1,21,5,21,321,8,21,10,21,12,21,324,9,21,1,21,1,
  	21,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,
  	22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,
  	22,1,22,1,22,3,22,358,8,22,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,
  	23,1,23,3,23,370,8,23,1,24,1,24,1,24,1,24,3,24,376,8,24,1,24,1,24,1,25,
  	1,25,1,25,1,25,1,25,1,25,4,25,386,8,25,11,25,12,25,387,1,25,1,25,1,25,
  	1,25,1,25,1,25,1,25,4,25,397,8,25,11,25,12,25,398,1,25,1,25,1,25,1,25,
  	1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,5,25,417,
  	8,25,10,25,12,25,420,9,25,1,25,1,25,1,25,1,25,1,25,5,25,427,8,25,10,25,
  	12,25,430,9,25,1,25,1,25,1,25,1,25,3,25,436,8,25,1,25,1,25,1,25,4,25,
  	441,8,25,11,25,12,25,442,5,25,445,8,25,10,25,12,25,448,9,25,1,26,1,26,
  	1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,3,26,460,8,26,1,27,1,27,1,27,
  	1,27,1,27,3,27,467,8,27,1,27,1,27,1,27,1,27,1,27,3,27,474,8,27,1,27,1,
  	27,3,27,478,8,27,1,28,1,28,1,28,5,28,483,8,28,10,28,12,28,486,9,28,1,
  	29,1,29,1,30,1,30,1,30,5,30,493,8,30,10,30,12,30,496,9,30,1,31,1,31,1,
  	31,5,31,501,8,31,10,31,12,31,504,9,31,1,32,1,32,1,32,1,32,3,32,510,8,
  	32,1,33,1,33,1,34,1,34,1,34,3,34,517,8,34,1,35,1,35,1,35,5,35,522,8,35,
  	10,35,12,35,525,9,35,1,36,1,36,1,36,5,36,530,8,36,10,36,12,36,533,9,36,
  	1,37,1,37,1,37,3,37,538,8,37,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,
  	1,38,3,38,549,8,38,1,39,1,39,1,39,0,1,50,40,0,2,4,6,8,10,12,14,16,18,
  	20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,
  	66,68,70,72,74,76,78,0,4,1,0,39,44,1,0,51,52,1,0,53,54,3,0,16,21,23,24,
  	66,66,591,0,83,1,0,0,0,2,92,1,0,0,0,4,124,1,0,0,0,6,147,1,0,0,0,8,149,
  	1,0,0,0,10,154,1,0,0,0,12,169,1,0,0,0,14,181,1,0,0,0,16,183,1,0,0,0,18,
  	206,1,0,0,0,20,208,1,0,0,0,22,216,1,0,0,0,24,233,1,0,0,0,26,241,1,0,0,
  	0,28,243,1,0,0,0,30,252,1,0,0,0,32,264,1,0,0,0,34,272,1,0,0,0,36,281,
  	1,0,0,0,38,288,1,0,0,0,40,301,1,0,0,0,42,314,1,0,0,0,44,357,1,0,0,0,46,
  	369,1,0,0,0,48,371,1,0,0,0,50,435,1,0,0,0,52,459,1,0,0,0,54,477,1,0,0,
  	0,56,479,1,0,0,0,58,487,1,0,0,0,60,489,1,0,0,0,62,497,1,0,0,0,64,505,
  	1,0,0,0,66,511,1,0,0,0,68,516,1,0,0,0,70,518,1,0,0,0,72,526,1,0,0,0,74,
  	537,1,0,0,0,76,548,1,0,0,0,78,550,1,0,0,0,80,82,3,2,1,0,81,80,1,0,0,0,
  	82,85,1,0,0,0,83,81,1,0,0,0,83,84,1,0,0,0,84,86,1,0,0,0,85,83,1,0,0,0,
  	86,87,5,0,0,1,87,1,1,0,0,0,88,93,3,4,2,0,89,93,3,6,3,0,90,93,3,10,5,0,
  	91,93,3,16,8,0,92,88,1,0,0,0,92,89,1,0,0,0,92,90,1,0,0,0,92,91,1,0,0,
  	0,93,3,1,0,0,0,94,95,5,1,0,0,95,96,5,66,0,0,96,98,5,58,0,0,97,99,3,22,
  	11,0,98,97,1,0,0,0,98,99,1,0,0,0,99,100,1,0,0,0,100,101,5,59,0,0,101,
  	105,5,60,0,0,102,104,3,26,13,0,103,102,1,0,0,0,104,107,1,0,0,0,105,103,
  	1,0,0,0,105,106,1,0,0,0,106,108,1,0,0,0,107,105,1,0,0,0,108,125,5,61,
  	0,0,109,110,5,2,0,0,110,111,5,66,0,0,111,113,5,58,0,0,112,114,3,22,11,
  	0,113,112,1,0,0,0,113,114,1,0,0,0,114,115,1,0,0,0,115,116,5,59,0,0,116,
  	120,5,60,0,0,117,119,3,26,13,0,118,117,1,0,0,0,119,122,1,0,0,0,120,118,
  	1,0,0,0,120,121,1,0,0,0,121,123,1,0,0,0,122,120,1,0,0,0,123,125,5,61,
  	0,0,124,94,1,0,0,0,124,109,1,0,0,0,125,5,1,0,0,0,126,127,5,3,0,0,127,
  	130,3,20,10,0,128,129,5,5,0,0,129,131,3,20,10,0,130,128,1,0,0,0,130,131,
  	1,0,0,0,131,132,1,0,0,0,132,136,5,60,0,0,133,135,3,8,4,0,134,133,1,0,
  	0,0,135,138,1,0,0,0,136,134,1,0,0,0,136,137,1,0,0,0,137,139,1,0,0,0,138,
  	136,1,0,0,0,139,140,5,61,0,0,140,148,1,0,0,0,141,142,5,3,0,0,142,143,
  	3,20,10,0,143,144,5,45,0,0,144,145,3,18,9,0,145,146,5,49,0,0,146,148,
  	1,0,0,0,147,126,1,0,0,0,147,141,1,0,0,0,148,7,1,0,0,0,149,150,5,66,0,
  	0,150,151,5,47,0,0,151,152,3,18,9,0,152,153,5,49,0,0,153,9,1,0,0,0,154,
  	155,5,4,0,0,155,158,3,20,10,0,156,157,5,47,0,0,157,159,3,18,9,0,158,156,
  	1,0,0,0,158,159,1,0,0,0,159,160,1,0,0,0,160,164,5,60,0,0,161,163,3,12,
  	6,0,162,161,1,0,0,0,163,166,1,0,0,0,164,162,1,0,0,0,164,165,1,0,0,0,165,
  	167,1,0,0,0,166,164,1,0,0,0,167,168,5,61,0,0,168,11,1,0,0,0,169,172,5,
  	66,0,0,170,171,5,45,0,0,171,173,3,14,7,0,172,170,1,0,0,0,172,173,1,0,
  	0,0,173,174,1,0,0,0,174,175,5,49,0,0,175,13,1,0,0,0,176,182,5,67,0,0,
  	177,179,5,52,0,0,178,177,1,0,0,0,178,179,1,0,0,0,179,180,1,0,0,0,180,
  	182,5,64,0,0,181,176,1,0,0,0,181,178,1,0,0,0,182,15,1,0,0,0,183,184,5,
  	9,0,0,184,185,3,18,9,0,185,186,5,10,0,0,186,187,5,67,0,0,187,17,1,0,0,
  	0,188,207,3,20,10,0,189,190,5,6,0,0,190,191,5,43,0,0,191,192,3,18,9,0,
  	192,193,5,44,0,0,193,207,1,0,0,0,194,195,5,7,0,0,195,196,5,43,0,0,196,
  	197,3,18,9,0,197,198,5,44,0,0,198,207,1,0,0,0,199,200,5,8,0,0,200,201,
  	5,43,0,0,201,202,3,18,9,0,202,203,5,48,0,0,203,204,3,18,9,0,204,205,5,
  	44,0,0,205,207,1,0,0,0,206,188,1,0,0,0,206,189,1,0,0,0,206,194,1,0,0,
  	0,206,199,1,0,0,0,207,19,1,0,0,0,208,213,5,66,0,0,209,210,5,46,0,0,210,
  	212,5,66,0,0,211,209,1,0,0,0,212,215,1,0,0,0,213,211,1,0,0,0,213,214,
  	1,0,0,0,214,21,1,0,0,0,215,213,1,0,0,0,216,221,3,24,12,0,217,218,5,48,
  	0,0,218,220,3,24,12,0,219,217,1,0,0,0,220,223,1,0,0,0,221,219,1,0,0,0,
  	221,222,1,0,0,0,222,23,1,0,0,0,223,221,1,0,0,0,224,225,3,18,9,0,225,226,
  	5,66,0,0,226,234,1,0,0,0,227,228,5,66,0,0,228,229,5,25,0,0,229,234,3,
  	18,9,0,230,231,5,66,0,0,231,232,5,26,0,0,232,234,3,18,9,0,233,224,1,0,
  	0,0,233,227,1,0,0,0,233,230,1,0,0,0,234,25,1,0,0,0,235,242,3,28,14,0,
  	236,242,3,34,17,0,237,242,3,38,19,0,238,242,3,40,20,0,239,242,3,42,21,
  	0,240,242,3,44,22,0,241,235,1,0,0,0,241,236,1,0,0,0,241,237,1,0,0,0,241,
  	238,1,0,0,0,241,239,1,0,0,0,241,240,1,0,0,0,242,27,1,0,0,0,243,244,5,
  	11,0,0,244,246,5,60,0,0,245,247,3,30,15,0,246,245,1,0,0,0,247,248,1,0,
  	0,0,248,246,1,0,0,0,248,249,1,0,0,0,249,250,1,0,0,0,250,251,5,61,0,0,
  	251,29,1,0,0,0,252,258,5,66,0,0,253,255,5,62,0,0,254,256,3,32,16,0,255,
  	254,1,0,0,0,255,256,1,0,0,0,256,257,1,0,0,0,257,259,5,63,0,0,258,253,
  	1,0,0,0,258,259,1,0,0,0,259,260,1,0,0,0,260,261,5,47,0,0,261,262,3,50,
  	25,0,262,263,5,49,0,0,263,31,1,0,0,0,264,269,5,66,0,0,265,266,5,48,0,
  	0,266,268,5,66,0,0,267,265,1,0,0,0,268,271,1,0,0,0,269,267,1,0,0,0,269,
  	270,1,0,0,0,270,33,1,0,0,0,271,269,1,0,0,0,272,273,5,12,0,0,273,275,5,
  	60,0,0,274,276,3,36,18,0,275,274,1,0,0,0,276,277,1,0,0,0,277,275,1,0,
  	0,0,277,278,1,0,0,0,278,279,1,0,0,0,279,280,5,61,0,0,280,35,1,0,0,0,281,
  	282,3,18,9,0,282,283,5,66,0,0,283,284,5,45,0,0,284,285,3,58,29,0,285,
  	286,5,47,0,0,286,287,3,58,29,0,287,37,1,0,0,0,288,289,5,13,0,0,289,290,
  	5,67,0,0,290,291,5,67,0,0,291,296,5,60,0,0,292,295,3,46,23,0,293,295,
  	3,44,22,0,294,292,1,0,0,0,294,293,1,0,0,0,295,298,1,0,0,0,296,294,1,0,
  	0,0,296,297,1,0,0,0,297,299,1,0,0,0,298,296,1,0,0,0,299,300,5,61,0,0,
  	300,39,1,0,0,0,301,302,5,14,0,0,302,303,5,67,0,0,303,304,5,67,0,0,304,
  	309,5,60,0,0,305,308,3,46,23,0,306,308,3,44,22,0,307,305,1,0,0,0,307,
  	306,1,0,0,0,308,311,1,0,0,0,309,307,1,0,0,0,309,310,1,0,0,0,310,312,1,
  	0,0,0,311,309,1,0,0,0,312,313,5,61,0,0,313,41,1,0,0,0,314,315,5,15,0,
  	0,315,316,5,67,0,0,316,317,5,67,0,0,317,322,5,60,0,0,318,321,3,46,23,
  	0,319,321,3,44,22,0,320,318,1,0,0,0,320,319,1,0,0,0,321,324,1,0,0,0,322,
  	320,1,0,0,0,322,323,1,0,0,0,323,325,1,0,0,0,324,322,1,0,0,0,325,326,5,
  	61,0,0,326,43,1,0,0,0,327,328,5,16,0,0,328,329,5,47,0,0,329,330,3,48,
  	24,0,330,331,5,49,0,0,331,358,1,0,0,0,332,333,5,17,0,0,333,334,5,47,0,
  	0,334,335,3,48,24,0,335,336,5,49,0,0,336,358,1,0,0,0,337,338,5,18,0,0,
  	338,339,5,47,0,0,339,340,3,48,24,0,340,341,5,49,0,0,341,358,1,0,0,0,342,
  	343,5,19,0,0,343,344,5,47,0,0,344,345,3,48,24,0,345,346,5,49,0,0,346,
  	358,1,0,0,0,347,348,5,20,0,0,348,349,5,47,0,0,349,350,3,48,24,0,350,351,
  	5,49,0,0,351,358,1,0,0,0,352,353,5,21,0,0,353,354,5,47,0,0,354,355,3,
  	48,24,0,355,356,5,49,0,0,356,358,1,0,0,0,357,327,1,0,0,0,357,332,1,0,
  	0,0,357,337,1,0,0,0,357,342,1,0,0,0,357,347,1,0,0,0,357,352,1,0,0,0,358,
  	45,1,0,0,0,359,360,5,23,0,0,360,361,5,47,0,0,361,362,3,18,9,0,362,363,
  	5,49,0,0,363,370,1,0,0,0,364,365,5,24,0,0,365,366,5,47,0,0,366,367,3,
  	18,9,0,367,368,5,49,0,0,368,370,1,0,0,0,369,359,1,0,0,0,369,364,1,0,0,
  	0,370,47,1,0,0,0,371,372,5,66,0,0,372,373,3,78,39,0,373,375,5,58,0,0,
  	374,376,3,22,11,0,375,374,1,0,0,0,375,376,1,0,0,0,376,377,1,0,0,0,377,
  	378,5,59,0,0,378,49,1,0,0,0,379,380,6,25,-1,0,380,381,5,30,0,0,381,382,
  	5,58,0,0,382,385,3,50,25,0,383,384,5,38,0,0,384,386,3,50,25,0,385,383,
  	1,0,0,0,386,387,1,0,0,0,387,385,1,0,0,0,387,388,1,0,0,0,388,389,1,0,0,
  	0,389,390,5,59,0,0,390,436,1,0,0,0,391,392,5,31,0,0,392,393,5,58,0,0,
  	393,396,3,50,25,0,394,395,5,38,0,0,395,397,3,50,25,0,396,394,1,0,0,0,
  	397,398,1,0,0,0,398,396,1,0,0,0,398,399,1,0,0,0,399,400,1,0,0,0,400,401,
  	5,59,0,0,401,436,1,0,0,0,402,403,5,32,0,0,403,404,5,64,0,0,404,405,5,
  	33,0,0,405,406,3,50,25,0,406,407,5,34,0,0,407,408,3,50,25,7,408,436,1,
  	0,0,0,409,410,5,29,0,0,410,411,5,64,0,0,411,412,5,64,0,0,412,413,5,58,
  	0,0,413,414,3,50,25,0,414,418,5,59,0,0,415,417,3,52,26,0,416,415,1,0,
  	0,0,417,420,1,0,0,0,418,416,1,0,0,0,418,419,1,0,0,0,419,436,1,0,0,0,420,
  	418,1,0,0,0,421,436,5,27,0,0,422,436,5,28,0,0,423,436,3,78,39,0,424,428,
  	3,54,27,0,425,427,3,52,26,0,426,425,1,0,0,0,427,430,1,0,0,0,428,426,1,
  	0,0,0,428,429,1,0,0,0,429,436,1,0,0,0,430,428,1,0,0,0,431,432,5,58,0,
  	0,432,433,3,50,25,0,433,434,5,59,0,0,434,436,1,0,0,0,435,379,1,0,0,0,
  	435,391,1,0,0,0,435,402,1,0,0,0,435,409,1,0,0,0,435,421,1,0,0,0,435,422,
  	1,0,0,0,435,423,1,0,0,0,435,424,1,0,0,0,435,431,1,0,0,0,436,446,1,0,0,
  	0,437,440,10,10,0,0,438,439,5,37,0,0,439,441,3,50,25,0,440,438,1,0,0,
  	0,441,442,1,0,0,0,442,440,1,0,0,0,442,443,1,0,0,0,443,445,1,0,0,0,444,
  	437,1,0,0,0,445,448,1,0,0,0,446,444,1,0,0,0,446,447,1,0,0,0,447,51,1,
  	0,0,0,448,446,1,0,0,0,449,450,5,22,0,0,450,451,5,19,0,0,451,460,3,50,
  	25,0,452,453,5,22,0,0,453,454,5,18,0,0,454,460,3,50,25,0,455,456,5,22,
  	0,0,456,457,3,54,27,0,457,458,3,50,25,0,458,460,1,0,0,0,459,449,1,0,0,
  	0,459,452,1,0,0,0,459,455,1,0,0,0,460,53,1,0,0,0,461,462,3,78,39,0,462,
  	463,5,50,0,0,463,464,3,78,39,0,464,466,5,58,0,0,465,467,3,56,28,0,466,
  	465,1,0,0,0,466,467,1,0,0,0,467,468,1,0,0,0,468,469,5,59,0,0,469,478,
  	1,0,0,0,470,471,3,78,39,0,471,473,5,58,0,0,472,474,3,56,28,0,473,472,
  	1,0,0,0,473,474,1,0,0,0,474,475,1,0,0,0,475,476,5,59,0,0,476,478,1,0,
  	0,0,477,461,1,0,0,0,477,470,1,0,0,0,478,55,1,0,0,0,479,484,3,58,29,0,
  	480,481,5,48,0,0,481,483,3,58,29,0,482,480,1,0,0,0,483,486,1,0,0,0,484,
  	482,1,0,0,0,484,485,1,0,0,0,485,57,1,0,0,0,486,484,1,0,0,0,487,488,3,
  	60,30,0,488,59,1,0,0,0,489,494,3,62,31,0,490,491,5,57,0,0,491,493,3,62,
  	31,0,492,490,1,0,0,0,493,496,1,0,0,0,494,492,1,0,0,0,494,495,1,0,0,0,
  	495,61,1,0,0,0,496,494,1,0,0,0,497,502,3,64,32,0,498,499,5,56,0,0,499,
  	501,3,64,32,0,500,498,1,0,0,0,501,504,1,0,0,0,502,500,1,0,0,0,502,503,
  	1,0,0,0,503,63,1,0,0,0,504,502,1,0,0,0,505,509,3,68,34,0,506,507,3,66,
  	33,0,507,508,3,68,34,0,508,510,1,0,0,0,509,506,1,0,0,0,509,510,1,0,0,
  	0,510,65,1,0,0,0,511,512,7,0,0,0,512,67,1,0,0,0,513,514,5,55,0,0,514,
  	517,3,68,34,0,515,517,3,70,35,0,516,513,1,0,0,0,516,515,1,0,0,0,517,69,
  	1,0,0,0,518,523,3,72,36,0,519,520,7,1,0,0,520,522,3,72,36,0,521,519,1,
  	0,0,0,522,525,1,0,0,0,523,521,1,0,0,0,523,524,1,0,0,0,524,71,1,0,0,0,
  	525,523,1,0,0,0,526,531,3,74,37,0,527,528,7,2,0,0,528,530,3,74,37,0,529,
  	527,1,0,0,0,530,533,1,0,0,0,531,529,1,0,0,0,531,532,1,0,0,0,532,73,1,
  	0,0,0,533,531,1,0,0,0,534,535,5,52,0,0,535,538,3,74,37,0,536,538,3,76,
  	38,0,537,534,1,0,0,0,537,536,1,0,0,0,538,75,1,0,0,0,539,549,3,54,27,0,
  	540,549,5,66,0,0,541,549,5,67,0,0,542,549,5,64,0,0,543,549,5,65,0,0,544,
  	545,5,58,0,0,545,546,3,58,29,0,546,547,5,59,0,0,547,549,1,0,0,0,548,539,
  	1,0,0,0,548,540,1,0,0,0,548,541,1,0,0,0,548,542,1,0,0,0,548,543,1,0,0,
  	0,548,544,1,0,0,0,549,77,1,0,0,0,550,551,7,3,0,0,551,79,1,0,0,0,54,83,
  	92,98,105,113,120,124,130,136,147,158,164,172,178,181,206,213,221,233,
  	241,248,255,258,269,277,294,296,307,309,320,322,357,369,375,387,398,418,
  	428,435,442,446,459,466,473,477,484,494,502,509,516,523,531,537,548
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  kodaParserStaticData = staticData.release();
}

}

KodaParser::KodaParser(TokenStream *input) : KodaParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

KodaParser::KodaParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  KodaParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *kodaParserStaticData->atn, kodaParserStaticData->decisionToDFA, kodaParserStaticData->sharedContextCache, options);
}

KodaParser::~KodaParser() {
  delete _interpreter;
}

const atn::ATN& KodaParser::getATN() const {
  return *kodaParserStaticData->atn;
}

std::string KodaParser::getGrammarFileName() const {
  return "Koda.g4";
}

const std::vector<std::string>& KodaParser::getRuleNames() const {
  return kodaParserStaticData->ruleNames;
}

const dfa::Vocabulary& KodaParser::getVocabulary() const {
  return kodaParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView KodaParser::getSerializedATN() const {
  return kodaParserStaticData->serializedATN;
}


//----------------- SystemContext ------------------------------------------------------------------

KodaParser::SystemContext::SystemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::SystemContext::EOF() {
  return getToken(KodaParser::EOF, 0);
}

std::vector<KodaParser::TopLevelDeclarationContext *> KodaParser::SystemContext::topLevelDeclaration() {
  return getRuleContexts<KodaParser::TopLevelDeclarationContext>();
}

KodaParser::TopLevelDeclarationContext* KodaParser::SystemContext::topLevelDeclaration(size_t i) {
  return getRuleContext<KodaParser::TopLevelDeclarationContext>(i);
}


size_t KodaParser::SystemContext::getRuleIndex() const {
  return KodaParser::RuleSystem;
}


std::any KodaParser::SystemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitSystem(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::SystemContext* KodaParser::system() {
  SystemContext *_localctx = _tracker.createInstance<SystemContext>(_ctx, getState());
  enterRule(_localctx, 0, KodaParser::RuleSystem);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(83);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TASK)
      | (1ULL << KodaParser::CAPABILITY)
      | (1ULL << KodaParser::TYPE)
      | (1ULL << KodaParser::ENUM)
      | (1ULL << KodaParser::MAPPING))) != 0)) {
      setState(80);
      topLevelDeclaration();
      setState(85);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(86);
    match(KodaParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopLevelDeclarationContext ------------------------------------------------------------------

KodaParser::TopLevelDeclarationContext::TopLevelDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::TopLevelComponentContext* KodaParser::TopLevelDeclarationContext::topLevelComponent() {
  return getRuleContext<KodaParser::TopLevelComponentContext>(0);
}

KodaParser::TypeDeclarationContext* KodaParser::TopLevelDeclarationContext::typeDeclaration() {
  return getRuleContext<KodaParser::TypeDeclarationContext>(0);
}

KodaParser::EnumDeclarationContext* KodaParser::TopLevelDeclarationContext::enumDeclaration() {
  return getRuleContext<KodaParser::EnumDeclarationContext>(0);
}

KodaParser::MappingDeclarationContext* KodaParser::TopLevelDeclarationContext::mappingDeclaration() {
  return getRuleContext<KodaParser::MappingDeclarationContext>(0);
}


size_t KodaParser::TopLevelDeclarationContext::getRuleIndex() const {
  return KodaParser::RuleTopLevelDeclaration;
}


std::any KodaParser::TopLevelDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTopLevelDeclaration(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::TopLevelDeclarationContext* KodaParser::topLevelDeclaration() {
  TopLevelDeclarationContext *_localctx = _tracker.createInstance<TopLevelDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 2, KodaParser::RuleTopLevelDeclaration);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(92);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK:
      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 1);
        setState(88);
        topLevelComponent();
        break;
      }

      case KodaParser::TYPE: {
        enterOuterAlt(_localctx, 2);
        setState(89);
        typeDeclaration();
        break;
      }

      case KodaParser::ENUM: {
        enterOuterAlt(_localctx, 3);
        setState(90);
        enumDeclaration();
        break;
      }

      case KodaParser::MAPPING: {
        enterOuterAlt(_localctx, 4);
        setState(91);
        mappingDeclaration();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopLevelComponentContext ------------------------------------------------------------------

KodaParser::TopLevelComponentContext::TopLevelComponentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::TASK() {
  return getToken(KodaParser::TASK, 0);
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

KodaParser::ArgumentListContext* KodaParser::TopLevelComponentContext::argumentList() {
  return getRuleContext<KodaParser::ArgumentListContext>(0);
}

std::vector<KodaParser::StatementContext *> KodaParser::TopLevelComponentContext::statement() {
  return getRuleContexts<KodaParser::StatementContext>();
}

KodaParser::StatementContext* KodaParser::TopLevelComponentContext::statement(size_t i) {
  return getRuleContext<KodaParser::StatementContext>(i);
}

tree::TerminalNode* KodaParser::TopLevelComponentContext::CAPABILITY() {
  return getToken(KodaParser::CAPABILITY, 0);
}


size_t KodaParser::TopLevelComponentContext::getRuleIndex() const {
  return KodaParser::RuleTopLevelComponent;
}


std::any KodaParser::TopLevelComponentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTopLevelComponent(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::TopLevelComponentContext* KodaParser::topLevelComponent() {
  TopLevelComponentContext *_localctx = _tracker.createInstance<TopLevelComponentContext>(_ctx, getState());
  enterRule(_localctx, 4, KodaParser::RuleTopLevelComponent);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(124);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(94);
        match(KodaParser::TASK);
        setState(95);
        match(KodaParser::IDENT);
        setState(96);
        match(KodaParser::LPAREN);
        setState(98);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 6) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
          | (1ULL << (KodaParser::OPTIONAL - 6))
          | (1ULL << (KodaParser::MAP - 6))
          | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
          setState(97);
          argumentList();
        }
        setState(100);
        match(KodaParser::RPAREN);
        setState(101);
        match(KodaParser::LBRACE);
        setState(105);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::STRATEGY)
          | (1ULL << KodaParser::VARS)
          | (1ULL << KodaParser::ACTION)
          | (1ULL << KodaParser::SERVICE)
          | (1ULL << KodaParser::TOPIC)
          | (1ULL << KodaParser::TRIGGER)
          | (1ULL << KodaParser::RETURN)
          | (1ULL << KodaParser::ABORT)
          | (1ULL << KodaParser::ERROR)
          | (1ULL << KodaParser::IN)
          | (1ULL << KodaParser::OUT))) != 0)) {
          setState(102);
          statement();
          setState(107);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(108);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(109);
        match(KodaParser::CAPABILITY);
        setState(110);
        match(KodaParser::IDENT);
        setState(111);
        match(KodaParser::LPAREN);
        setState(113);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 6) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
          | (1ULL << (KodaParser::OPTIONAL - 6))
          | (1ULL << (KodaParser::MAP - 6))
          | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
          setState(112);
          argumentList();
        }
        setState(115);
        match(KodaParser::RPAREN);
        setState(116);
        match(KodaParser::LBRACE);
        setState(120);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::STRATEGY)
          | (1ULL << KodaParser::VARS)
          | (1ULL << KodaParser::ACTION)
          | (1ULL << KodaParser::SERVICE)
          | (1ULL << KodaParser::TOPIC)
          | (1ULL << KodaParser::TRIGGER)
          | (1ULL << KodaParser::RETURN)
          | (1ULL << KodaParser::ABORT)
          | (1ULL << KodaParser::ERROR)
          | (1ULL << KodaParser::IN)
          | (1ULL << KodaParser::OUT))) != 0)) {
          setState(117);
          statement();
          setState(122);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(123);
        match(KodaParser::RBRACE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeDeclarationContext ------------------------------------------------------------------

KodaParser::TypeDeclarationContext::TypeDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::TypeDeclarationContext::getRuleIndex() const {
  return KodaParser::RuleTypeDeclaration;
}

void KodaParser::TypeDeclarationContext::copyFrom(TypeDeclarationContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- TypeAliasContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::TypeAliasContext::TYPE() {
  return getToken(KodaParser::TYPE, 0);
}

KodaParser::QualifiedNameContext* KodaParser::TypeAliasContext::qualifiedName() {
  return getRuleContext<KodaParser::QualifiedNameContext>(0);
}

tree::TerminalNode* KodaParser::TypeAliasContext::ASSIGN() {
  return getToken(KodaParser::ASSIGN, 0);
}

KodaParser::TypeReferenceContext* KodaParser::TypeAliasContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::TypeAliasContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

KodaParser::TypeAliasContext::TypeAliasContext(TypeDeclarationContext *ctx) { copyFrom(ctx); }


std::any KodaParser::TypeAliasContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTypeAlias(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TypeRecordContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::TypeRecordContext::TYPE() {
  return getToken(KodaParser::TYPE, 0);
}

std::vector<KodaParser::QualifiedNameContext *> KodaParser::TypeRecordContext::qualifiedName() {
  return getRuleContexts<KodaParser::QualifiedNameContext>();
}

KodaParser::QualifiedNameContext* KodaParser::TypeRecordContext::qualifiedName(size_t i) {
  return getRuleContext<KodaParser::QualifiedNameContext>(i);
}

tree::TerminalNode* KodaParser::TypeRecordContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::TypeRecordContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

tree::TerminalNode* KodaParser::TypeRecordContext::EXTENDS() {
  return getToken(KodaParser::EXTENDS, 0);
}

std::vector<KodaParser::FieldDeclarationContext *> KodaParser::TypeRecordContext::fieldDeclaration() {
  return getRuleContexts<KodaParser::FieldDeclarationContext>();
}

KodaParser::FieldDeclarationContext* KodaParser::TypeRecordContext::fieldDeclaration(size_t i) {
  return getRuleContext<KodaParser::FieldDeclarationContext>(i);
}

KodaParser::TypeRecordContext::TypeRecordContext(TypeDeclarationContext *ctx) { copyFrom(ctx); }


std::any KodaParser::TypeRecordContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTypeRecord(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::TypeDeclarationContext* KodaParser::typeDeclaration() {
  TypeDeclarationContext *_localctx = _tracker.createInstance<TypeDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 6, KodaParser::RuleTypeDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(147);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::TypeRecordContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(126);
      match(KodaParser::TYPE);
      setState(127);
      qualifiedName();
      setState(130);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::EXTENDS) {
        setState(128);
        match(KodaParser::EXTENDS);
        setState(129);
        qualifiedName();
      }
      setState(132);
      match(KodaParser::LBRACE);
      setState(136);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::IDENT) {
        setState(133);
        fieldDeclaration();
        setState(138);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(139);
      match(KodaParser::RBRACE);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::TypeAliasContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(141);
      match(KodaParser::TYPE);
      setState(142);
      qualifiedName();
      setState(143);
      match(KodaParser::ASSIGN);
      setState(144);
      typeReference();
      setState(145);
      match(KodaParser::SEMI);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FieldDeclarationContext ------------------------------------------------------------------

KodaParser::FieldDeclarationContext::FieldDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::FieldDeclarationContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::FieldDeclarationContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::TypeReferenceContext* KodaParser::FieldDeclarationContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::FieldDeclarationContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}


size_t KodaParser::FieldDeclarationContext::getRuleIndex() const {
  return KodaParser::RuleFieldDeclaration;
}


std::any KodaParser::FieldDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitFieldDeclaration(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::FieldDeclarationContext* KodaParser::fieldDeclaration() {
  FieldDeclarationContext *_localctx = _tracker.createInstance<FieldDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 8, KodaParser::RuleFieldDeclaration);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(149);
    match(KodaParser::IDENT);
    setState(150);
    match(KodaParser::COLON);
    setState(151);
    typeReference();
    setState(152);
    match(KodaParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumDeclarationContext ------------------------------------------------------------------

KodaParser::EnumDeclarationContext::EnumDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::EnumDeclarationContext::ENUM() {
  return getToken(KodaParser::ENUM, 0);
}

KodaParser::QualifiedNameContext* KodaParser::EnumDeclarationContext::qualifiedName() {
  return getRuleContext<KodaParser::QualifiedNameContext>(0);
}

tree::TerminalNode* KodaParser::EnumDeclarationContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::EnumDeclarationContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

tree::TerminalNode* KodaParser::EnumDeclarationContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::TypeReferenceContext* KodaParser::EnumDeclarationContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

std::vector<KodaParser::EnumValueContext *> KodaParser::EnumDeclarationContext::enumValue() {
  return getRuleContexts<KodaParser::EnumValueContext>();
}

KodaParser::EnumValueContext* KodaParser::EnumDeclarationContext::enumValue(size_t i) {
  return getRuleContext<KodaParser::EnumValueContext>(i);
}


size_t KodaParser::EnumDeclarationContext::getRuleIndex() const {
  return KodaParser::RuleEnumDeclaration;
}


std::any KodaParser::EnumDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEnumDeclaration(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::EnumDeclarationContext* KodaParser::enumDeclaration() {
  EnumDeclarationContext *_localctx = _tracker.createInstance<EnumDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 10, KodaParser::RuleEnumDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(154);
    match(KodaParser::ENUM);
    setState(155);
    qualifiedName();
    setState(158);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(156);
      match(KodaParser::COLON);
      setState(157);
      typeReference();
    }
    setState(160);
    match(KodaParser::LBRACE);
    setState(164);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(161);
      enumValue();
      setState(166);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(167);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumValueContext ------------------------------------------------------------------

KodaParser::EnumValueContext::EnumValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::EnumValueContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::EnumValueContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

tree::TerminalNode* KodaParser::EnumValueContext::ASSIGN() {
  return getToken(KodaParser::ASSIGN, 0);
}

KodaParser::EnumLiteralContext* KodaParser::EnumValueContext::enumLiteral() {
  return getRuleContext<KodaParser::EnumLiteralContext>(0);
}


size_t KodaParser::EnumValueContext::getRuleIndex() const {
  return KodaParser::RuleEnumValue;
}


std::any KodaParser::EnumValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEnumValue(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::EnumValueContext* KodaParser::enumValue() {
  EnumValueContext *_localctx = _tracker.createInstance<EnumValueContext>(_ctx, getState());
  enterRule(_localctx, 12, KodaParser::RuleEnumValue);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(169);
    match(KodaParser::IDENT);
    setState(172);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(170);
      match(KodaParser::ASSIGN);
      setState(171);
      enumLiteral();
    }
    setState(174);
    match(KodaParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumLiteralContext ------------------------------------------------------------------

KodaParser::EnumLiteralContext::EnumLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::EnumLiteralContext::STRING() {
  return getToken(KodaParser::STRING, 0);
}

tree::TerminalNode* KodaParser::EnumLiteralContext::NATURAL() {
  return getToken(KodaParser::NATURAL, 0);
}

tree::TerminalNode* KodaParser::EnumLiteralContext::MINUS() {
  return getToken(KodaParser::MINUS, 0);
}


size_t KodaParser::EnumLiteralContext::getRuleIndex() const {
  return KodaParser::RuleEnumLiteral;
}


std::any KodaParser::EnumLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEnumLiteral(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::EnumLiteralContext* KodaParser::enumLiteral() {
  EnumLiteralContext *_localctx = _tracker.createInstance<EnumLiteralContext>(_ctx, getState());
  enterRule(_localctx, 14, KodaParser::RuleEnumLiteral);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(181);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(176);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::MINUS:
      case KodaParser::NATURAL: {
        enterOuterAlt(_localctx, 2);
        setState(178);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::MINUS) {
          setState(177);
          match(KodaParser::MINUS);
        }
        setState(180);
        match(KodaParser::NATURAL);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MappingDeclarationContext ------------------------------------------------------------------

KodaParser::MappingDeclarationContext::MappingDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::MappingDeclarationContext::MAPPING() {
  return getToken(KodaParser::MAPPING, 0);
}

KodaParser::TypeReferenceContext* KodaParser::MappingDeclarationContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::MappingDeclarationContext::TO() {
  return getToken(KodaParser::TO, 0);
}

tree::TerminalNode* KodaParser::MappingDeclarationContext::STRING() {
  return getToken(KodaParser::STRING, 0);
}


size_t KodaParser::MappingDeclarationContext::getRuleIndex() const {
  return KodaParser::RuleMappingDeclaration;
}


std::any KodaParser::MappingDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitMappingDeclaration(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::MappingDeclarationContext* KodaParser::mappingDeclaration() {
  MappingDeclarationContext *_localctx = _tracker.createInstance<MappingDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 16, KodaParser::RuleMappingDeclaration);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(183);
    match(KodaParser::MAPPING);
    setState(184);
    typeReference();
    setState(185);
    match(KodaParser::TO);
    setState(186);
    match(KodaParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeReferenceContext ------------------------------------------------------------------

KodaParser::TypeReferenceContext::TypeReferenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::TypeReferenceContext::getRuleIndex() const {
  return KodaParser::RuleTypeReference;
}

void KodaParser::TypeReferenceContext::copyFrom(TypeReferenceContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- TypeNamedContext ------------------------------------------------------------------

KodaParser::QualifiedNameContext* KodaParser::TypeNamedContext::qualifiedName() {
  return getRuleContext<KodaParser::QualifiedNameContext>(0);
}

KodaParser::TypeNamedContext::TypeNamedContext(TypeReferenceContext *ctx) { copyFrom(ctx); }


std::any KodaParser::TypeNamedContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTypeNamed(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TypeListContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::TypeListContext::LIST() {
  return getToken(KodaParser::LIST, 0);
}

tree::TerminalNode* KodaParser::TypeListContext::LT() {
  return getToken(KodaParser::LT, 0);
}

KodaParser::TypeReferenceContext* KodaParser::TypeListContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::TypeListContext::GT() {
  return getToken(KodaParser::GT, 0);
}

KodaParser::TypeListContext::TypeListContext(TypeReferenceContext *ctx) { copyFrom(ctx); }


std::any KodaParser::TypeListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTypeList(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TypeMapContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::TypeMapContext::MAP() {
  return getToken(KodaParser::MAP, 0);
}

tree::TerminalNode* KodaParser::TypeMapContext::LT() {
  return getToken(KodaParser::LT, 0);
}

std::vector<KodaParser::TypeReferenceContext *> KodaParser::TypeMapContext::typeReference() {
  return getRuleContexts<KodaParser::TypeReferenceContext>();
}

KodaParser::TypeReferenceContext* KodaParser::TypeMapContext::typeReference(size_t i) {
  return getRuleContext<KodaParser::TypeReferenceContext>(i);
}

tree::TerminalNode* KodaParser::TypeMapContext::COMMA() {
  return getToken(KodaParser::COMMA, 0);
}

tree::TerminalNode* KodaParser::TypeMapContext::GT() {
  return getToken(KodaParser::GT, 0);
}

KodaParser::TypeMapContext::TypeMapContext(TypeReferenceContext *ctx) { copyFrom(ctx); }


std::any KodaParser::TypeMapContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTypeMap(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TypeOptionalContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::TypeOptionalContext::OPTIONAL() {
  return getToken(KodaParser::OPTIONAL, 0);
}

tree::TerminalNode* KodaParser::TypeOptionalContext::LT() {
  return getToken(KodaParser::LT, 0);
}

KodaParser::TypeReferenceContext* KodaParser::TypeOptionalContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::TypeOptionalContext::GT() {
  return getToken(KodaParser::GT, 0);
}

KodaParser::TypeOptionalContext::TypeOptionalContext(TypeReferenceContext *ctx) { copyFrom(ctx); }


std::any KodaParser::TypeOptionalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTypeOptional(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::TypeReferenceContext* KodaParser::typeReference() {
  TypeReferenceContext *_localctx = _tracker.createInstance<TypeReferenceContext>(_ctx, getState());
  enterRule(_localctx, 18, KodaParser::RuleTypeReference);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(206);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IDENT: {
        _localctx = _tracker.createInstance<KodaParser::TypeNamedContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(188);
        qualifiedName();
        break;
      }

      case KodaParser::LIST: {
        _localctx = _tracker.createInstance<KodaParser::TypeListContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(189);
        match(KodaParser::LIST);
        setState(190);
        match(KodaParser::LT);
        setState(191);
        typeReference();
        setState(192);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::OPTIONAL: {
        _localctx = _tracker.createInstance<KodaParser::TypeOptionalContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(194);
        match(KodaParser::OPTIONAL);
        setState(195);
        match(KodaParser::LT);
        setState(196);
        typeReference();
        setState(197);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::MAP: {
        _localctx = _tracker.createInstance<KodaParser::TypeMapContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(199);
        match(KodaParser::MAP);
        setState(200);
        match(KodaParser::LT);
        setState(201);
        typeReference();
        setState(202);
        match(KodaParser::COMMA);
        setState(203);
        typeReference();
        setState(204);
        match(KodaParser::GT);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QualifiedNameContext ------------------------------------------------------------------

KodaParser::QualifiedNameContext::QualifiedNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> KodaParser::QualifiedNameContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::QualifiedNameContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

std::vector<tree::TerminalNode *> KodaParser::QualifiedNameContext::DOUBLE_COLON() {
  return getTokens(KodaParser::DOUBLE_COLON);
}

tree::TerminalNode* KodaParser::QualifiedNameContext::DOUBLE_COLON(size_t i) {
  return getToken(KodaParser::DOUBLE_COLON, i);
}


size_t KodaParser::QualifiedNameContext::getRuleIndex() const {
  return KodaParser::RuleQualifiedName;
}


std::any KodaParser::QualifiedNameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitQualifiedName(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::QualifiedNameContext* KodaParser::qualifiedName() {
  QualifiedNameContext *_localctx = _tracker.createInstance<QualifiedNameContext>(_ctx, getState());
  enterRule(_localctx, 20, KodaParser::RuleQualifiedName);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(208);
    match(KodaParser::IDENT);
    setState(213);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::DOUBLE_COLON) {
      setState(209);
      match(KodaParser::DOUBLE_COLON);
      setState(210);
      match(KodaParser::IDENT);
      setState(215);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentListContext ------------------------------------------------------------------

KodaParser::ArgumentListContext::ArgumentListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ArgumentContext *> KodaParser::ArgumentListContext::argument() {
  return getRuleContexts<KodaParser::ArgumentContext>();
}

KodaParser::ArgumentContext* KodaParser::ArgumentListContext::argument(size_t i) {
  return getRuleContext<KodaParser::ArgumentContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ArgumentListContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::ArgumentListContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::ArgumentListContext::getRuleIndex() const {
  return KodaParser::RuleArgumentList;
}


std::any KodaParser::ArgumentListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitArgumentList(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ArgumentListContext* KodaParser::argumentList() {
  ArgumentListContext *_localctx = _tracker.createInstance<ArgumentListContext>(_ctx, getState());
  enterRule(_localctx, 22, KodaParser::RuleArgumentList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(216);
    argument();
    setState(221);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(217);
      match(KodaParser::COMMA);
      setState(218);
      argument();
      setState(223);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentContext ------------------------------------------------------------------

KodaParser::ArgumentContext::ArgumentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::ArgumentContext::getRuleIndex() const {
  return KodaParser::RuleArgument;
}

void KodaParser::ArgumentContext::copyFrom(ArgumentContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ArgPlainContext ------------------------------------------------------------------

KodaParser::TypeReferenceContext* KodaParser::ArgPlainContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::ArgPlainContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

KodaParser::ArgPlainContext::ArgPlainContext(ArgumentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ArgPlainContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitArgPlain(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ArgReqContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ArgReqContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::ArgReqContext::REQ() {
  return getToken(KodaParser::REQ, 0);
}

KodaParser::TypeReferenceContext* KodaParser::ArgReqContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

KodaParser::ArgReqContext::ArgReqContext(ArgumentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ArgReqContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitArgReq(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ArgProContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ArgProContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::ArgProContext::PRO() {
  return getToken(KodaParser::PRO, 0);
}

KodaParser::TypeReferenceContext* KodaParser::ArgProContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

KodaParser::ArgProContext::ArgProContext(ArgumentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ArgProContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitArgPro(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::ArgumentContext* KodaParser::argument() {
  ArgumentContext *_localctx = _tracker.createInstance<ArgumentContext>(_ctx, getState());
  enterRule(_localctx, 24, KodaParser::RuleArgument);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(233);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(224);
      typeReference();
      setState(225);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(227);
      match(KodaParser::IDENT);
      setState(228);
      match(KodaParser::REQ);
      setState(229);
      typeReference();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(230);
      match(KodaParser::IDENT);
      setState(231);
      match(KodaParser::PRO);
      setState(232);
      typeReference();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

KodaParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::TasksBlockContext* KodaParser::StatementContext::tasksBlock() {
  return getRuleContext<KodaParser::TasksBlockContext>(0);
}

KodaParser::VarsBlockContext* KodaParser::StatementContext::varsBlock() {
  return getRuleContext<KodaParser::VarsBlockContext>(0);
}

KodaParser::ActionBlockContext* KodaParser::StatementContext::actionBlock() {
  return getRuleContext<KodaParser::ActionBlockContext>(0);
}

KodaParser::ServiceBlockContext* KodaParser::StatementContext::serviceBlock() {
  return getRuleContext<KodaParser::ServiceBlockContext>(0);
}

KodaParser::TopicBlockContext* KodaParser::StatementContext::topicBlock() {
  return getRuleContext<KodaParser::TopicBlockContext>(0);
}

KodaParser::RosDefStatementContext* KodaParser::StatementContext::rosDefStatement() {
  return getRuleContext<KodaParser::RosDefStatementContext>(0);
}


size_t KodaParser::StatementContext::getRuleIndex() const {
  return KodaParser::RuleStatement;
}


std::any KodaParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::StatementContext* KodaParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 26, KodaParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(241);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(235);
        tasksBlock();
        break;
      }

      case KodaParser::VARS: {
        enterOuterAlt(_localctx, 2);
        setState(236);
        varsBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 3);
        setState(237);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 4);
        setState(238);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 5);
        setState(239);
        topicBlock();
        break;
      }

      case KodaParser::TRIGGER:
      case KodaParser::RETURN:
      case KodaParser::ABORT:
      case KodaParser::ERROR:
      case KodaParser::IN:
      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(240);
        rosDefStatement();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TasksBlockContext ------------------------------------------------------------------

KodaParser::TasksBlockContext::TasksBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::TasksBlockContext::STRATEGY() {
  return getToken(KodaParser::STRATEGY, 0);
}

tree::TerminalNode* KodaParser::TasksBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::TasksBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::FlowContext *> KodaParser::TasksBlockContext::flow() {
  return getRuleContexts<KodaParser::FlowContext>();
}

KodaParser::FlowContext* KodaParser::TasksBlockContext::flow(size_t i) {
  return getRuleContext<KodaParser::FlowContext>(i);
}


size_t KodaParser::TasksBlockContext::getRuleIndex() const {
  return KodaParser::RuleTasksBlock;
}


std::any KodaParser::TasksBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTasksBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::TasksBlockContext* KodaParser::tasksBlock() {
  TasksBlockContext *_localctx = _tracker.createInstance<TasksBlockContext>(_ctx, getState());
  enterRule(_localctx, 28, KodaParser::RuleTasksBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(243);
    match(KodaParser::STRATEGY);
    setState(244);
    match(KodaParser::LBRACE);
    setState(246); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(245);
      flow();
      setState(248); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(250);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FlowContext ------------------------------------------------------------------

KodaParser::FlowContext::FlowContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::FlowContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::FlowContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::StrategyContext* KodaParser::FlowContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

tree::TerminalNode* KodaParser::FlowContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

tree::TerminalNode* KodaParser::FlowContext::LBRACK() {
  return getToken(KodaParser::LBRACK, 0);
}

tree::TerminalNode* KodaParser::FlowContext::RBRACK() {
  return getToken(KodaParser::RBRACK, 0);
}

KodaParser::IdentListContext* KodaParser::FlowContext::identList() {
  return getRuleContext<KodaParser::IdentListContext>(0);
}


size_t KodaParser::FlowContext::getRuleIndex() const {
  return KodaParser::RuleFlow;
}


std::any KodaParser::FlowContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitFlow(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::FlowContext* KodaParser::flow() {
  FlowContext *_localctx = _tracker.createInstance<FlowContext>(_ctx, getState());
  enterRule(_localctx, 30, KodaParser::RuleFlow);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(252);
    match(KodaParser::IDENT);
    setState(258);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(253);
      match(KodaParser::LBRACK);
      setState(255);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(254);
        identList();
      }
      setState(257);
      match(KodaParser::RBRACK);
    }
    setState(260);
    match(KodaParser::COLON);
    setState(261);
    strategy(0);
    setState(262);
    match(KodaParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentListContext ------------------------------------------------------------------

KodaParser::IdentListContext::IdentListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> KodaParser::IdentListContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::IdentListContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

std::vector<tree::TerminalNode *> KodaParser::IdentListContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::IdentListContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::IdentListContext::getRuleIndex() const {
  return KodaParser::RuleIdentList;
}


std::any KodaParser::IdentListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitIdentList(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::IdentListContext* KodaParser::identList() {
  IdentListContext *_localctx = _tracker.createInstance<IdentListContext>(_ctx, getState());
  enterRule(_localctx, 32, KodaParser::RuleIdentList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(264);
    match(KodaParser::IDENT);
    setState(269);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(265);
      match(KodaParser::COMMA);
      setState(266);
      match(KodaParser::IDENT);
      setState(271);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarsBlockContext ------------------------------------------------------------------

KodaParser::VarsBlockContext::VarsBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::VarsBlockContext::VARS() {
  return getToken(KodaParser::VARS, 0);
}

tree::TerminalNode* KodaParser::VarsBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::VarsBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::VariableStatementContext *> KodaParser::VarsBlockContext::variableStatement() {
  return getRuleContexts<KodaParser::VariableStatementContext>();
}

KodaParser::VariableStatementContext* KodaParser::VarsBlockContext::variableStatement(size_t i) {
  return getRuleContext<KodaParser::VariableStatementContext>(i);
}


size_t KodaParser::VarsBlockContext::getRuleIndex() const {
  return KodaParser::RuleVarsBlock;
}


std::any KodaParser::VarsBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitVarsBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::VarsBlockContext* KodaParser::varsBlock() {
  VarsBlockContext *_localctx = _tracker.createInstance<VarsBlockContext>(_ctx, getState());
  enterRule(_localctx, 34, KodaParser::RuleVarsBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(272);
    match(KodaParser::VARS);
    setState(273);
    match(KodaParser::LBRACE);
    setState(275); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(274);
      variableStatement();
      setState(277); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((((_la - 6) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
      | (1ULL << (KodaParser::OPTIONAL - 6))
      | (1ULL << (KodaParser::MAP - 6))
      | (1ULL << (KodaParser::IDENT - 6)))) != 0));
    setState(279);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableStatementContext ------------------------------------------------------------------

KodaParser::VariableStatementContext::VariableStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::TypeReferenceContext* KodaParser::VariableStatementContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::VariableStatementContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::VariableStatementContext::ASSIGN() {
  return getToken(KodaParser::ASSIGN, 0);
}

std::vector<KodaParser::ExpressionContext *> KodaParser::VariableStatementContext::expression() {
  return getRuleContexts<KodaParser::ExpressionContext>();
}

KodaParser::ExpressionContext* KodaParser::VariableStatementContext::expression(size_t i) {
  return getRuleContext<KodaParser::ExpressionContext>(i);
}

tree::TerminalNode* KodaParser::VariableStatementContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}


size_t KodaParser::VariableStatementContext::getRuleIndex() const {
  return KodaParser::RuleVariableStatement;
}


std::any KodaParser::VariableStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitVariableStatement(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::VariableStatementContext* KodaParser::variableStatement() {
  VariableStatementContext *_localctx = _tracker.createInstance<VariableStatementContext>(_ctx, getState());
  enterRule(_localctx, 36, KodaParser::RuleVariableStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(281);
    typeReference();
    setState(282);
    match(KodaParser::IDENT);
    setState(283);
    match(KodaParser::ASSIGN);
    setState(284);
    expression();
    setState(285);
    match(KodaParser::COLON);
    setState(286);
    expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ActionBlockContext ------------------------------------------------------------------

KodaParser::ActionBlockContext::ActionBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::ActionBlockContext::ACTION() {
  return getToken(KodaParser::ACTION, 0);
}

std::vector<tree::TerminalNode *> KodaParser::ActionBlockContext::STRING() {
  return getTokens(KodaParser::STRING);
}

tree::TerminalNode* KodaParser::ActionBlockContext::STRING(size_t i) {
  return getToken(KodaParser::STRING, i);
}

tree::TerminalNode* KodaParser::ActionBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::ActionBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::ReqDefStatementContext *> KodaParser::ActionBlockContext::reqDefStatement() {
  return getRuleContexts<KodaParser::ReqDefStatementContext>();
}

KodaParser::ReqDefStatementContext* KodaParser::ActionBlockContext::reqDefStatement(size_t i) {
  return getRuleContext<KodaParser::ReqDefStatementContext>(i);
}

std::vector<KodaParser::RosDefStatementContext *> KodaParser::ActionBlockContext::rosDefStatement() {
  return getRuleContexts<KodaParser::RosDefStatementContext>();
}

KodaParser::RosDefStatementContext* KodaParser::ActionBlockContext::rosDefStatement(size_t i) {
  return getRuleContext<KodaParser::RosDefStatementContext>(i);
}


size_t KodaParser::ActionBlockContext::getRuleIndex() const {
  return KodaParser::RuleActionBlock;
}


std::any KodaParser::ActionBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitActionBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ActionBlockContext* KodaParser::actionBlock() {
  ActionBlockContext *_localctx = _tracker.createInstance<ActionBlockContext>(_ctx, getState());
  enterRule(_localctx, 38, KodaParser::RuleActionBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(288);
    match(KodaParser::ACTION);
    setState(289);
    match(KodaParser::STRING);
    setState(290);
    match(KodaParser::STRING);
    setState(291);
    match(KodaParser::LBRACE);
    setState(296);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT)
      | (1ULL << KodaParser::CONSUMES)
      | (1ULL << KodaParser::PRODUCES))) != 0)) {
      setState(294);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(292);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(293);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(298);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(299);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ServiceBlockContext ------------------------------------------------------------------

KodaParser::ServiceBlockContext::ServiceBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::ServiceBlockContext::SERVICE() {
  return getToken(KodaParser::SERVICE, 0);
}

std::vector<tree::TerminalNode *> KodaParser::ServiceBlockContext::STRING() {
  return getTokens(KodaParser::STRING);
}

tree::TerminalNode* KodaParser::ServiceBlockContext::STRING(size_t i) {
  return getToken(KodaParser::STRING, i);
}

tree::TerminalNode* KodaParser::ServiceBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::ServiceBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::ReqDefStatementContext *> KodaParser::ServiceBlockContext::reqDefStatement() {
  return getRuleContexts<KodaParser::ReqDefStatementContext>();
}

KodaParser::ReqDefStatementContext* KodaParser::ServiceBlockContext::reqDefStatement(size_t i) {
  return getRuleContext<KodaParser::ReqDefStatementContext>(i);
}

std::vector<KodaParser::RosDefStatementContext *> KodaParser::ServiceBlockContext::rosDefStatement() {
  return getRuleContexts<KodaParser::RosDefStatementContext>();
}

KodaParser::RosDefStatementContext* KodaParser::ServiceBlockContext::rosDefStatement(size_t i) {
  return getRuleContext<KodaParser::RosDefStatementContext>(i);
}


size_t KodaParser::ServiceBlockContext::getRuleIndex() const {
  return KodaParser::RuleServiceBlock;
}


std::any KodaParser::ServiceBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitServiceBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ServiceBlockContext* KodaParser::serviceBlock() {
  ServiceBlockContext *_localctx = _tracker.createInstance<ServiceBlockContext>(_ctx, getState());
  enterRule(_localctx, 40, KodaParser::RuleServiceBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(301);
    match(KodaParser::SERVICE);
    setState(302);
    match(KodaParser::STRING);
    setState(303);
    match(KodaParser::STRING);
    setState(304);
    match(KodaParser::LBRACE);
    setState(309);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT)
      | (1ULL << KodaParser::CONSUMES)
      | (1ULL << KodaParser::PRODUCES))) != 0)) {
      setState(307);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(305);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(306);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(311);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(312);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopicBlockContext ------------------------------------------------------------------

KodaParser::TopicBlockContext::TopicBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::TopicBlockContext::TOPIC() {
  return getToken(KodaParser::TOPIC, 0);
}

std::vector<tree::TerminalNode *> KodaParser::TopicBlockContext::STRING() {
  return getTokens(KodaParser::STRING);
}

tree::TerminalNode* KodaParser::TopicBlockContext::STRING(size_t i) {
  return getToken(KodaParser::STRING, i);
}

tree::TerminalNode* KodaParser::TopicBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::TopicBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::ReqDefStatementContext *> KodaParser::TopicBlockContext::reqDefStatement() {
  return getRuleContexts<KodaParser::ReqDefStatementContext>();
}

KodaParser::ReqDefStatementContext* KodaParser::TopicBlockContext::reqDefStatement(size_t i) {
  return getRuleContext<KodaParser::ReqDefStatementContext>(i);
}

std::vector<KodaParser::RosDefStatementContext *> KodaParser::TopicBlockContext::rosDefStatement() {
  return getRuleContexts<KodaParser::RosDefStatementContext>();
}

KodaParser::RosDefStatementContext* KodaParser::TopicBlockContext::rosDefStatement(size_t i) {
  return getRuleContext<KodaParser::RosDefStatementContext>(i);
}


size_t KodaParser::TopicBlockContext::getRuleIndex() const {
  return KodaParser::RuleTopicBlock;
}


std::any KodaParser::TopicBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTopicBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::TopicBlockContext* KodaParser::topicBlock() {
  TopicBlockContext *_localctx = _tracker.createInstance<TopicBlockContext>(_ctx, getState());
  enterRule(_localctx, 42, KodaParser::RuleTopicBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(314);
    match(KodaParser::TOPIC);
    setState(315);
    match(KodaParser::STRING);
    setState(316);
    match(KodaParser::STRING);
    setState(317);
    match(KodaParser::LBRACE);
    setState(322);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT)
      | (1ULL << KodaParser::CONSUMES)
      | (1ULL << KodaParser::PRODUCES))) != 0)) {
      setState(320);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(318);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(319);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(324);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(325);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RosDefStatementContext ------------------------------------------------------------------

KodaParser::RosDefStatementContext::RosDefStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::RosDefStatementContext::TRIGGER() {
  return getToken(KodaParser::TRIGGER, 0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::EventDefStatementContext* KodaParser::RosDefStatementContext::eventDefStatement() {
  return getRuleContext<KodaParser::EventDefStatementContext>(0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::RETURN() {
  return getToken(KodaParser::RETURN, 0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::ABORT() {
  return getToken(KodaParser::ABORT, 0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::ERROR() {
  return getToken(KodaParser::ERROR, 0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::IN() {
  return getToken(KodaParser::IN, 0);
}

tree::TerminalNode* KodaParser::RosDefStatementContext::OUT() {
  return getToken(KodaParser::OUT, 0);
}


size_t KodaParser::RosDefStatementContext::getRuleIndex() const {
  return KodaParser::RuleRosDefStatement;
}


std::any KodaParser::RosDefStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitRosDefStatement(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::RosDefStatementContext* KodaParser::rosDefStatement() {
  RosDefStatementContext *_localctx = _tracker.createInstance<RosDefStatementContext>(_ctx, getState());
  enterRule(_localctx, 44, KodaParser::RuleRosDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(357);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(327);
        match(KodaParser::TRIGGER);
        setState(328);
        match(KodaParser::COLON);
        setState(329);
        eventDefStatement();
        setState(330);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(332);
        match(KodaParser::RETURN);
        setState(333);
        match(KodaParser::COLON);
        setState(334);
        eventDefStatement();
        setState(335);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(337);
        match(KodaParser::ABORT);
        setState(338);
        match(KodaParser::COLON);
        setState(339);
        eventDefStatement();
        setState(340);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(342);
        match(KodaParser::ERROR);
        setState(343);
        match(KodaParser::COLON);
        setState(344);
        eventDefStatement();
        setState(345);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(347);
        match(KodaParser::IN);
        setState(348);
        match(KodaParser::COLON);
        setState(349);
        eventDefStatement();
        setState(350);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(352);
        match(KodaParser::OUT);
        setState(353);
        match(KodaParser::COLON);
        setState(354);
        eventDefStatement();
        setState(355);
        match(KodaParser::SEMI);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ReqDefStatementContext ------------------------------------------------------------------

KodaParser::ReqDefStatementContext::ReqDefStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::ReqDefStatementContext::CONSUMES() {
  return getToken(KodaParser::CONSUMES, 0);
}

tree::TerminalNode* KodaParser::ReqDefStatementContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::TypeReferenceContext* KodaParser::ReqDefStatementContext::typeReference() {
  return getRuleContext<KodaParser::TypeReferenceContext>(0);
}

tree::TerminalNode* KodaParser::ReqDefStatementContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

tree::TerminalNode* KodaParser::ReqDefStatementContext::PRODUCES() {
  return getToken(KodaParser::PRODUCES, 0);
}


size_t KodaParser::ReqDefStatementContext::getRuleIndex() const {
  return KodaParser::RuleReqDefStatement;
}


std::any KodaParser::ReqDefStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitReqDefStatement(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ReqDefStatementContext* KodaParser::reqDefStatement() {
  ReqDefStatementContext *_localctx = _tracker.createInstance<ReqDefStatementContext>(_ctx, getState());
  enterRule(_localctx, 46, KodaParser::RuleReqDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(369);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::CONSUMES: {
        enterOuterAlt(_localctx, 1);
        setState(359);
        match(KodaParser::CONSUMES);
        setState(360);
        match(KodaParser::COLON);
        setState(361);
        typeReference();
        setState(362);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::PRODUCES: {
        enterOuterAlt(_localctx, 2);
        setState(364);
        match(KodaParser::PRODUCES);
        setState(365);
        match(KodaParser::COLON);
        setState(366);
        typeReference();
        setState(367);
        match(KodaParser::SEMI);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EventDefStatementContext ------------------------------------------------------------------

KodaParser::EventDefStatementContext::EventDefStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::EventDefStatementContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

KodaParser::IdentifierContext* KodaParser::EventDefStatementContext::identifier() {
  return getRuleContext<KodaParser::IdentifierContext>(0);
}

tree::TerminalNode* KodaParser::EventDefStatementContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

tree::TerminalNode* KodaParser::EventDefStatementContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

KodaParser::ArgumentListContext* KodaParser::EventDefStatementContext::argumentList() {
  return getRuleContext<KodaParser::ArgumentListContext>(0);
}


size_t KodaParser::EventDefStatementContext::getRuleIndex() const {
  return KodaParser::RuleEventDefStatement;
}


std::any KodaParser::EventDefStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEventDefStatement(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::EventDefStatementContext* KodaParser::eventDefStatement() {
  EventDefStatementContext *_localctx = _tracker.createInstance<EventDefStatementContext>(_ctx, getState());
  enterRule(_localctx, 48, KodaParser::RuleEventDefStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(371);
    match(KodaParser::IDENT);
    setState(372);
    identifier();
    setState(373);
    match(KodaParser::LPAREN);
    setState(375);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 6) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
      | (1ULL << (KodaParser::OPTIONAL - 6))
      | (1ULL << (KodaParser::MAP - 6))
      | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
      setState(374);
      argumentList();
    }
    setState(377);
    match(KodaParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StrategyContext ------------------------------------------------------------------

KodaParser::StrategyContext::StrategyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::StrategyContext::getRuleIndex() const {
  return KodaParser::RuleStrategy;
}

void KodaParser::StrategyContext::copyFrom(StrategyContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- StratRefContext ------------------------------------------------------------------

KodaParser::IdentifierContext* KodaParser::StratRefContext::identifier() {
  return getRuleContext<KodaParser::IdentifierContext>(0);
}

KodaParser::StratRefContext::StratRefContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratRef(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratSeqContext ------------------------------------------------------------------

std::vector<KodaParser::StrategyContext *> KodaParser::StratSeqContext::strategy() {
  return getRuleContexts<KodaParser::StrategyContext>();
}

KodaParser::StrategyContext* KodaParser::StratSeqContext::strategy(size_t i) {
  return getRuleContext<KodaParser::StrategyContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::StratSeqContext::ARROW() {
  return getTokens(KodaParser::ARROW);
}

tree::TerminalNode* KodaParser::StratSeqContext::ARROW(size_t i) {
  return getToken(KodaParser::ARROW, i);
}

KodaParser::StratSeqContext::StratSeqContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratSeqContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratSeq(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratTaskContext ------------------------------------------------------------------

KodaParser::EventStatementContext* KodaParser::StratTaskContext::eventStatement() {
  return getRuleContext<KodaParser::EventStatementContext>(0);
}

std::vector<KodaParser::StrategyHandlerContext *> KodaParser::StratTaskContext::strategyHandler() {
  return getRuleContexts<KodaParser::StrategyHandlerContext>();
}

KodaParser::StrategyHandlerContext* KodaParser::StratTaskContext::strategyHandler(size_t i) {
  return getRuleContext<KodaParser::StrategyHandlerContext>(i);
}

KodaParser::StratTaskContext::StratTaskContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratTaskContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratTask(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratJoinContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratJoinContext::JOIN() {
  return getToken(KodaParser::JOIN, 0);
}

tree::TerminalNode* KodaParser::StratJoinContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

std::vector<KodaParser::StrategyContext *> KodaParser::StratJoinContext::strategy() {
  return getRuleContexts<KodaParser::StrategyContext>();
}

KodaParser::StrategyContext* KodaParser::StratJoinContext::strategy(size_t i) {
  return getRuleContext<KodaParser::StrategyContext>(i);
}

tree::TerminalNode* KodaParser::StratJoinContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> KodaParser::StratJoinContext::PIPE() {
  return getTokens(KodaParser::PIPE);
}

tree::TerminalNode* KodaParser::StratJoinContext::PIPE(size_t i) {
  return getToken(KodaParser::PIPE, i);
}

KodaParser::StratJoinContext::StratJoinContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratJoinContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratJoin(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratEitherContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratEitherContext::EITHER() {
  return getToken(KodaParser::EITHER, 0);
}

tree::TerminalNode* KodaParser::StratEitherContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

std::vector<KodaParser::StrategyContext *> KodaParser::StratEitherContext::strategy() {
  return getRuleContexts<KodaParser::StrategyContext>();
}

KodaParser::StrategyContext* KodaParser::StratEitherContext::strategy(size_t i) {
  return getRuleContext<KodaParser::StrategyContext>(i);
}

tree::TerminalNode* KodaParser::StratEitherContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> KodaParser::StratEitherContext::PIPE() {
  return getTokens(KodaParser::PIPE);
}

tree::TerminalNode* KodaParser::StratEitherContext::PIPE(size_t i) {
  return getToken(KodaParser::PIPE, i);
}

KodaParser::StratEitherContext::StratEitherContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratEitherContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratEither(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratEndContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratEndContext::END() {
  return getToken(KodaParser::END, 0);
}

KodaParser::StratEndContext::StratEndContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratEndContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratEnd(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratWithinContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratWithinContext::WITHIN() {
  return getToken(KodaParser::WITHIN, 0);
}

tree::TerminalNode* KodaParser::StratWithinContext::NATURAL() {
  return getToken(KodaParser::NATURAL, 0);
}

tree::TerminalNode* KodaParser::StratWithinContext::DO() {
  return getToken(KodaParser::DO, 0);
}

std::vector<KodaParser::StrategyContext *> KodaParser::StratWithinContext::strategy() {
  return getRuleContexts<KodaParser::StrategyContext>();
}

KodaParser::StrategyContext* KodaParser::StratWithinContext::strategy(size_t i) {
  return getRuleContext<KodaParser::StrategyContext>(i);
}

tree::TerminalNode* KodaParser::StratWithinContext::ELSE() {
  return getToken(KodaParser::ELSE, 0);
}

KodaParser::StratWithinContext::StratWithinContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratWithinContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratWithin(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratRepeatContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratRepeatContext::REPEAT() {
  return getToken(KodaParser::REPEAT, 0);
}

std::vector<tree::TerminalNode *> KodaParser::StratRepeatContext::NATURAL() {
  return getTokens(KodaParser::NATURAL);
}

tree::TerminalNode* KodaParser::StratRepeatContext::NATURAL(size_t i) {
  return getToken(KodaParser::NATURAL, i);
}

tree::TerminalNode* KodaParser::StratRepeatContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

KodaParser::StrategyContext* KodaParser::StratRepeatContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

tree::TerminalNode* KodaParser::StratRepeatContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

std::vector<KodaParser::StrategyHandlerContext *> KodaParser::StratRepeatContext::strategyHandler() {
  return getRuleContexts<KodaParser::StrategyHandlerContext>();
}

KodaParser::StrategyHandlerContext* KodaParser::StratRepeatContext::strategyHandler(size_t i) {
  return getRuleContext<KodaParser::StrategyHandlerContext>(i);
}

KodaParser::StratRepeatContext::StratRepeatContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratRepeatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratRepeat(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratContinueContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratContinueContext::CONTINUE() {
  return getToken(KodaParser::CONTINUE, 0);
}

KodaParser::StratContinueContext::StratContinueContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratContinueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratContinue(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratParenContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratParenContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

KodaParser::StrategyContext* KodaParser::StratParenContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

tree::TerminalNode* KodaParser::StratParenContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

KodaParser::StratParenContext::StratParenContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratParenContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratParen(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::StrategyContext* KodaParser::strategy() {
   return strategy(0);
}

KodaParser::StrategyContext* KodaParser::strategy(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  KodaParser::StrategyContext *_localctx = _tracker.createInstance<StrategyContext>(_ctx, parentState);
  KodaParser::StrategyContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 50;
  enterRecursionRule(_localctx, 50, KodaParser::RuleStrategy, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(435);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(380);
      match(KodaParser::JOIN);
      setState(381);
      match(KodaParser::LPAREN);
      setState(382);
      strategy(0);
      setState(385); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(383);
        match(KodaParser::PIPE);
        setState(384);
        strategy(0);
        setState(387); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == KodaParser::PIPE);
      setState(389);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(391);
      match(KodaParser::EITHER);
      setState(392);
      match(KodaParser::LPAREN);
      setState(393);
      strategy(0);
      setState(396); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(394);
        match(KodaParser::PIPE);
        setState(395);
        strategy(0);
        setState(398); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == KodaParser::PIPE);
      setState(400);
      match(KodaParser::RPAREN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(402);
      match(KodaParser::WITHIN);
      setState(403);
      match(KodaParser::NATURAL);
      setState(404);
      match(KodaParser::DO);
      setState(405);
      strategy(0);
      setState(406);
      match(KodaParser::ELSE);
      setState(407);
      strategy(7);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<StratRepeatContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(409);
      match(KodaParser::REPEAT);
      setState(410);
      match(KodaParser::NATURAL);
      setState(411);
      match(KodaParser::NATURAL);
      setState(412);
      match(KodaParser::LPAREN);
      setState(413);
      strategy(0);
      setState(414);
      match(KodaParser::RPAREN);
      setState(418);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(415);
          strategyHandler(); 
        }
        setState(420);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
      }
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<StratEndContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(421);
      match(KodaParser::END);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<StratContinueContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(422);
      match(KodaParser::CONTINUE);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<StratRefContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(423);
      identifier();
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<StratTaskContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(424);
      eventStatement();
      setState(428);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(425);
          strategyHandler(); 
        }
        setState(430);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
      }
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<StratParenContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(431);
      match(KodaParser::LPAREN);
      setState(432);
      strategy(0);
      setState(433);
      match(KodaParser::RPAREN);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(446);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<StratSeqContext>(_tracker.createInstance<StrategyContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleStrategy);
        setState(437);

        if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
        setState(440); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(438);
                  match(KodaParser::ARROW);
                  setState(439);
                  strategy(0);
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(442); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER); 
      }
      setState(448);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- StrategyHandlerContext ------------------------------------------------------------------

KodaParser::StrategyHandlerContext::StrategyHandlerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::StrategyHandlerContext::getRuleIndex() const {
  return KodaParser::RuleStrategyHandler;
}

void KodaParser::StrategyHandlerContext::copyFrom(StrategyHandlerContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- HandlerOnErrorContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::HandlerOnErrorContext::ON() {
  return getToken(KodaParser::ON, 0);
}

tree::TerminalNode* KodaParser::HandlerOnErrorContext::ERROR() {
  return getToken(KodaParser::ERROR, 0);
}

KodaParser::StrategyContext* KodaParser::HandlerOnErrorContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

KodaParser::HandlerOnErrorContext::HandlerOnErrorContext(StrategyHandlerContext *ctx) { copyFrom(ctx); }


std::any KodaParser::HandlerOnErrorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitHandlerOnError(this);
  else
    return visitor->visitChildren(this);
}
//----------------- HandlerOnAbortContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::HandlerOnAbortContext::ON() {
  return getToken(KodaParser::ON, 0);
}

tree::TerminalNode* KodaParser::HandlerOnAbortContext::ABORT() {
  return getToken(KodaParser::ABORT, 0);
}

KodaParser::StrategyContext* KodaParser::HandlerOnAbortContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

KodaParser::HandlerOnAbortContext::HandlerOnAbortContext(StrategyHandlerContext *ctx) { copyFrom(ctx); }


std::any KodaParser::HandlerOnAbortContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitHandlerOnAbort(this);
  else
    return visitor->visitChildren(this);
}
//----------------- HandlerOnEmitterContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::HandlerOnEmitterContext::ON() {
  return getToken(KodaParser::ON, 0);
}

KodaParser::EventStatementContext* KodaParser::HandlerOnEmitterContext::eventStatement() {
  return getRuleContext<KodaParser::EventStatementContext>(0);
}

KodaParser::StrategyContext* KodaParser::HandlerOnEmitterContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

KodaParser::HandlerOnEmitterContext::HandlerOnEmitterContext(StrategyHandlerContext *ctx) { copyFrom(ctx); }


std::any KodaParser::HandlerOnEmitterContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitHandlerOnEmitter(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::StrategyHandlerContext* KodaParser::strategyHandler() {
  StrategyHandlerContext *_localctx = _tracker.createInstance<StrategyHandlerContext>(_ctx, getState());
  enterRule(_localctx, 52, KodaParser::RuleStrategyHandler);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(459);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(449);
      match(KodaParser::ON);
      setState(450);
      match(KodaParser::ERROR);
      setState(451);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(452);
      match(KodaParser::ON);
      setState(453);
      match(KodaParser::ABORT);
      setState(454);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(455);
      match(KodaParser::ON);
      setState(456);
      eventStatement();
      setState(457);
      strategy(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EventStatementContext ------------------------------------------------------------------

KodaParser::EventStatementContext::EventStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::EventStatementContext::getRuleIndex() const {
  return KodaParser::RuleEventStatement;
}

void KodaParser::EventStatementContext::copyFrom(EventStatementContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- EvCallContext ------------------------------------------------------------------

KodaParser::IdentifierContext* KodaParser::EvCallContext::identifier() {
  return getRuleContext<KodaParser::IdentifierContext>(0);
}

tree::TerminalNode* KodaParser::EvCallContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

tree::TerminalNode* KodaParser::EvCallContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

KodaParser::ExprListContext* KodaParser::EvCallContext::exprList() {
  return getRuleContext<KodaParser::ExprListContext>(0);
}

KodaParser::EvCallContext::EvCallContext(EventStatementContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EvCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEvCall(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EvQualifiedCallContext ------------------------------------------------------------------

std::vector<KodaParser::IdentifierContext *> KodaParser::EvQualifiedCallContext::identifier() {
  return getRuleContexts<KodaParser::IdentifierContext>();
}

KodaParser::IdentifierContext* KodaParser::EvQualifiedCallContext::identifier(size_t i) {
  return getRuleContext<KodaParser::IdentifierContext>(i);
}

tree::TerminalNode* KodaParser::EvQualifiedCallContext::DOT() {
  return getToken(KodaParser::DOT, 0);
}

tree::TerminalNode* KodaParser::EvQualifiedCallContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

tree::TerminalNode* KodaParser::EvQualifiedCallContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

KodaParser::ExprListContext* KodaParser::EvQualifiedCallContext::exprList() {
  return getRuleContext<KodaParser::ExprListContext>(0);
}

KodaParser::EvQualifiedCallContext::EvQualifiedCallContext(EventStatementContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EvQualifiedCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEvQualifiedCall(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::EventStatementContext* KodaParser::eventStatement() {
  EventStatementContext *_localctx = _tracker.createInstance<EventStatementContext>(_ctx, getState());
  enterRule(_localctx, 54, KodaParser::RuleEventStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(477);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(461);
      identifier();
      setState(462);
      match(KodaParser::DOT);
      setState(463);
      identifier();
      setState(464);
      match(KodaParser::LPAREN);
      setState(466);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 16) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 16)) & ((1ULL << (KodaParser::TRIGGER - 16))
        | (1ULL << (KodaParser::RETURN - 16))
        | (1ULL << (KodaParser::ABORT - 16))
        | (1ULL << (KodaParser::ERROR - 16))
        | (1ULL << (KodaParser::IN - 16))
        | (1ULL << (KodaParser::OUT - 16))
        | (1ULL << (KodaParser::CONSUMES - 16))
        | (1ULL << (KodaParser::PRODUCES - 16))
        | (1ULL << (KodaParser::MINUS - 16))
        | (1ULL << (KodaParser::NOT - 16))
        | (1ULL << (KodaParser::LPAREN - 16))
        | (1ULL << (KodaParser::NATURAL - 16))
        | (1ULL << (KodaParser::REAL - 16))
        | (1ULL << (KodaParser::IDENT - 16))
        | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
        setState(465);
        exprList();
      }
      setState(468);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(470);
      identifier();
      setState(471);
      match(KodaParser::LPAREN);
      setState(473);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 16) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 16)) & ((1ULL << (KodaParser::TRIGGER - 16))
        | (1ULL << (KodaParser::RETURN - 16))
        | (1ULL << (KodaParser::ABORT - 16))
        | (1ULL << (KodaParser::ERROR - 16))
        | (1ULL << (KodaParser::IN - 16))
        | (1ULL << (KodaParser::OUT - 16))
        | (1ULL << (KodaParser::CONSUMES - 16))
        | (1ULL << (KodaParser::PRODUCES - 16))
        | (1ULL << (KodaParser::MINUS - 16))
        | (1ULL << (KodaParser::NOT - 16))
        | (1ULL << (KodaParser::LPAREN - 16))
        | (1ULL << (KodaParser::NATURAL - 16))
        | (1ULL << (KodaParser::REAL - 16))
        | (1ULL << (KodaParser::IDENT - 16))
        | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
        setState(472);
        exprList();
      }
      setState(475);
      match(KodaParser::RPAREN);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprListContext ------------------------------------------------------------------

KodaParser::ExprListContext::ExprListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExpressionContext *> KodaParser::ExprListContext::expression() {
  return getRuleContexts<KodaParser::ExpressionContext>();
}

KodaParser::ExpressionContext* KodaParser::ExprListContext::expression(size_t i) {
  return getRuleContext<KodaParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprListContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::ExprListContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::ExprListContext::getRuleIndex() const {
  return KodaParser::RuleExprList;
}


std::any KodaParser::ExprListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprList(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprListContext* KodaParser::exprList() {
  ExprListContext *_localctx = _tracker.createInstance<ExprListContext>(_ctx, getState());
  enterRule(_localctx, 56, KodaParser::RuleExprList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(479);
    expression();
    setState(484);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(480);
      match(KodaParser::COMMA);
      setState(481);
      expression();
      setState(486);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

KodaParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::ExprOrContext* KodaParser::ExpressionContext::exprOr() {
  return getRuleContext<KodaParser::ExprOrContext>(0);
}


size_t KodaParser::ExpressionContext::getRuleIndex() const {
  return KodaParser::RuleExpression;
}


std::any KodaParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExpressionContext* KodaParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 58, KodaParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(487);
    exprOr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprOrContext ------------------------------------------------------------------

KodaParser::ExprOrContext::ExprOrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExprAndContext *> KodaParser::ExprOrContext::exprAnd() {
  return getRuleContexts<KodaParser::ExprAndContext>();
}

KodaParser::ExprAndContext* KodaParser::ExprOrContext::exprAnd(size_t i) {
  return getRuleContext<KodaParser::ExprAndContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprOrContext::OR() {
  return getTokens(KodaParser::OR);
}

tree::TerminalNode* KodaParser::ExprOrContext::OR(size_t i) {
  return getToken(KodaParser::OR, i);
}


size_t KodaParser::ExprOrContext::getRuleIndex() const {
  return KodaParser::RuleExprOr;
}


std::any KodaParser::ExprOrContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprOr(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprOrContext* KodaParser::exprOr() {
  ExprOrContext *_localctx = _tracker.createInstance<ExprOrContext>(_ctx, getState());
  enterRule(_localctx, 60, KodaParser::RuleExprOr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(489);
    exprAnd();
    setState(494);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(490);
      match(KodaParser::OR);
      setState(491);
      exprAnd();
      setState(496);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprAndContext ------------------------------------------------------------------

KodaParser::ExprAndContext::ExprAndContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExprCmpContext *> KodaParser::ExprAndContext::exprCmp() {
  return getRuleContexts<KodaParser::ExprCmpContext>();
}

KodaParser::ExprCmpContext* KodaParser::ExprAndContext::exprCmp(size_t i) {
  return getRuleContext<KodaParser::ExprCmpContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprAndContext::AND() {
  return getTokens(KodaParser::AND);
}

tree::TerminalNode* KodaParser::ExprAndContext::AND(size_t i) {
  return getToken(KodaParser::AND, i);
}


size_t KodaParser::ExprAndContext::getRuleIndex() const {
  return KodaParser::RuleExprAnd;
}


std::any KodaParser::ExprAndContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprAnd(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprAndContext* KodaParser::exprAnd() {
  ExprAndContext *_localctx = _tracker.createInstance<ExprAndContext>(_ctx, getState());
  enterRule(_localctx, 62, KodaParser::RuleExprAnd);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(497);
    exprCmp();
    setState(502);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(498);
      match(KodaParser::AND);
      setState(499);
      exprCmp();
      setState(504);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprCmpContext ------------------------------------------------------------------

KodaParser::ExprCmpContext::ExprCmpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExprNotContext *> KodaParser::ExprCmpContext::exprNot() {
  return getRuleContexts<KodaParser::ExprNotContext>();
}

KodaParser::ExprNotContext* KodaParser::ExprCmpContext::exprNot(size_t i) {
  return getRuleContext<KodaParser::ExprNotContext>(i);
}

KodaParser::CompOpContext* KodaParser::ExprCmpContext::compOp() {
  return getRuleContext<KodaParser::CompOpContext>(0);
}


size_t KodaParser::ExprCmpContext::getRuleIndex() const {
  return KodaParser::RuleExprCmp;
}


std::any KodaParser::ExprCmpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprCmp(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprCmpContext* KodaParser::exprCmp() {
  ExprCmpContext *_localctx = _tracker.createInstance<ExprCmpContext>(_ctx, getState());
  enterRule(_localctx, 64, KodaParser::RuleExprCmp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(505);
    exprNot();
    setState(509);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::EQ)
      | (1ULL << KodaParser::NEQ)
      | (1ULL << KodaParser::LEQ)
      | (1ULL << KodaParser::GEQ)
      | (1ULL << KodaParser::LT)
      | (1ULL << KodaParser::GT))) != 0)) {
      setState(506);
      compOp();
      setState(507);
      exprNot();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CompOpContext ------------------------------------------------------------------

KodaParser::CompOpContext::CompOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::CompOpContext::EQ() {
  return getToken(KodaParser::EQ, 0);
}

tree::TerminalNode* KodaParser::CompOpContext::NEQ() {
  return getToken(KodaParser::NEQ, 0);
}

tree::TerminalNode* KodaParser::CompOpContext::LEQ() {
  return getToken(KodaParser::LEQ, 0);
}

tree::TerminalNode* KodaParser::CompOpContext::GEQ() {
  return getToken(KodaParser::GEQ, 0);
}

tree::TerminalNode* KodaParser::CompOpContext::LT() {
  return getToken(KodaParser::LT, 0);
}

tree::TerminalNode* KodaParser::CompOpContext::GT() {
  return getToken(KodaParser::GT, 0);
}


size_t KodaParser::CompOpContext::getRuleIndex() const {
  return KodaParser::RuleCompOp;
}


std::any KodaParser::CompOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitCompOp(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::CompOpContext* KodaParser::compOp() {
  CompOpContext *_localctx = _tracker.createInstance<CompOpContext>(_ctx, getState());
  enterRule(_localctx, 66, KodaParser::RuleCompOp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(511);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::EQ)
      | (1ULL << KodaParser::NEQ)
      | (1ULL << KodaParser::LEQ)
      | (1ULL << KodaParser::GEQ)
      | (1ULL << KodaParser::LT)
      | (1ULL << KodaParser::GT))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprNotContext ------------------------------------------------------------------

KodaParser::ExprNotContext::ExprNotContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::ExprNotContext::NOT() {
  return getToken(KodaParser::NOT, 0);
}

KodaParser::ExprNotContext* KodaParser::ExprNotContext::exprNot() {
  return getRuleContext<KodaParser::ExprNotContext>(0);
}

KodaParser::ExprAddContext* KodaParser::ExprNotContext::exprAdd() {
  return getRuleContext<KodaParser::ExprAddContext>(0);
}


size_t KodaParser::ExprNotContext::getRuleIndex() const {
  return KodaParser::RuleExprNot;
}


std::any KodaParser::ExprNotContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprNot(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprNotContext* KodaParser::exprNot() {
  ExprNotContext *_localctx = _tracker.createInstance<ExprNotContext>(_ctx, getState());
  enterRule(_localctx, 68, KodaParser::RuleExprNot);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(516);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(513);
        match(KodaParser::NOT);
        setState(514);
        exprNot();
        break;
      }

      case KodaParser::TRIGGER:
      case KodaParser::RETURN:
      case KodaParser::ABORT:
      case KodaParser::ERROR:
      case KodaParser::IN:
      case KodaParser::OUT:
      case KodaParser::CONSUMES:
      case KodaParser::PRODUCES:
      case KodaParser::MINUS:
      case KodaParser::LPAREN:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(515);
        exprAdd();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprAddContext ------------------------------------------------------------------

KodaParser::ExprAddContext::ExprAddContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExprMulContext *> KodaParser::ExprAddContext::exprMul() {
  return getRuleContexts<KodaParser::ExprMulContext>();
}

KodaParser::ExprMulContext* KodaParser::ExprAddContext::exprMul(size_t i) {
  return getRuleContext<KodaParser::ExprMulContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprAddContext::PLUS() {
  return getTokens(KodaParser::PLUS);
}

tree::TerminalNode* KodaParser::ExprAddContext::PLUS(size_t i) {
  return getToken(KodaParser::PLUS, i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprAddContext::MINUS() {
  return getTokens(KodaParser::MINUS);
}

tree::TerminalNode* KodaParser::ExprAddContext::MINUS(size_t i) {
  return getToken(KodaParser::MINUS, i);
}


size_t KodaParser::ExprAddContext::getRuleIndex() const {
  return KodaParser::RuleExprAdd;
}


std::any KodaParser::ExprAddContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprAdd(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprAddContext* KodaParser::exprAdd() {
  ExprAddContext *_localctx = _tracker.createInstance<ExprAddContext>(_ctx, getState());
  enterRule(_localctx, 70, KodaParser::RuleExprAdd);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(518);
    exprMul();
    setState(523);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(519);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(520);
      exprMul();
      setState(525);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprMulContext ------------------------------------------------------------------

KodaParser::ExprMulContext::ExprMulContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExprUnaryContext *> KodaParser::ExprMulContext::exprUnary() {
  return getRuleContexts<KodaParser::ExprUnaryContext>();
}

KodaParser::ExprUnaryContext* KodaParser::ExprMulContext::exprUnary(size_t i) {
  return getRuleContext<KodaParser::ExprUnaryContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprMulContext::STAR() {
  return getTokens(KodaParser::STAR);
}

tree::TerminalNode* KodaParser::ExprMulContext::STAR(size_t i) {
  return getToken(KodaParser::STAR, i);
}

std::vector<tree::TerminalNode *> KodaParser::ExprMulContext::SLASH() {
  return getTokens(KodaParser::SLASH);
}

tree::TerminalNode* KodaParser::ExprMulContext::SLASH(size_t i) {
  return getToken(KodaParser::SLASH, i);
}


size_t KodaParser::ExprMulContext::getRuleIndex() const {
  return KodaParser::RuleExprMul;
}


std::any KodaParser::ExprMulContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprMul(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprMulContext* KodaParser::exprMul() {
  ExprMulContext *_localctx = _tracker.createInstance<ExprMulContext>(_ctx, getState());
  enterRule(_localctx, 72, KodaParser::RuleExprMul);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(526);
    exprUnary();
    setState(531);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(527);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(528);
      exprUnary();
      setState(533);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprUnaryContext ------------------------------------------------------------------

KodaParser::ExprUnaryContext::ExprUnaryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::ExprUnaryContext::MINUS() {
  return getToken(KodaParser::MINUS, 0);
}

KodaParser::ExprUnaryContext* KodaParser::ExprUnaryContext::exprUnary() {
  return getRuleContext<KodaParser::ExprUnaryContext>(0);
}

KodaParser::ExprPrimaryContext* KodaParser::ExprUnaryContext::exprPrimary() {
  return getRuleContext<KodaParser::ExprPrimaryContext>(0);
}


size_t KodaParser::ExprUnaryContext::getRuleIndex() const {
  return KodaParser::RuleExprUnary;
}


std::any KodaParser::ExprUnaryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprUnary(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ExprUnaryContext* KodaParser::exprUnary() {
  ExprUnaryContext *_localctx = _tracker.createInstance<ExprUnaryContext>(_ctx, getState());
  enterRule(_localctx, 74, KodaParser::RuleExprUnary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(537);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(534);
        match(KodaParser::MINUS);
        setState(535);
        exprUnary();
        break;
      }

      case KodaParser::TRIGGER:
      case KodaParser::RETURN:
      case KodaParser::ABORT:
      case KodaParser::ERROR:
      case KodaParser::IN:
      case KodaParser::OUT:
      case KodaParser::CONSUMES:
      case KodaParser::PRODUCES:
      case KodaParser::LPAREN:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(536);
        exprPrimary();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprPrimaryContext ------------------------------------------------------------------

KodaParser::ExprPrimaryContext::ExprPrimaryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::ExprPrimaryContext::getRuleIndex() const {
  return KodaParser::RuleExprPrimary;
}

void KodaParser::ExprPrimaryContext::copyFrom(ExprPrimaryContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ExprIntContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ExprIntContext::NATURAL() {
  return getToken(KodaParser::NATURAL, 0);
}

KodaParser::ExprIntContext::ExprIntContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprIntContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprInt(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExprParenContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ExprParenContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

KodaParser::ExpressionContext* KodaParser::ExprParenContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
}

tree::TerminalNode* KodaParser::ExprParenContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

KodaParser::ExprParenContext::ExprParenContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprParenContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprParen(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExprCallContext ------------------------------------------------------------------

KodaParser::EventStatementContext* KodaParser::ExprCallContext::eventStatement() {
  return getRuleContext<KodaParser::EventStatementContext>(0);
}

KodaParser::ExprCallContext::ExprCallContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprCall(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExprStringContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ExprStringContext::STRING() {
  return getToken(KodaParser::STRING, 0);
}

KodaParser::ExprStringContext::ExprStringContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprStringContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprString(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExprIdContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ExprIdContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

KodaParser::ExprIdContext::ExprIdContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprIdContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprId(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExprFloatContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ExprFloatContext::REAL() {
  return getToken(KodaParser::REAL, 0);
}

KodaParser::ExprFloatContext::ExprFloatContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprFloatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprFloat(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::ExprPrimaryContext* KodaParser::exprPrimary() {
  ExprPrimaryContext *_localctx = _tracker.createInstance<ExprPrimaryContext>(_ctx, getState());
  enterRule(_localctx, 76, KodaParser::RuleExprPrimary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(548);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(539);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(540);
      match(KodaParser::IDENT);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(541);
      match(KodaParser::STRING);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(542);
      match(KodaParser::NATURAL);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(543);
      match(KodaParser::REAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(544);
      match(KodaParser::LPAREN);
      setState(545);
      expression();
      setState(546);
      match(KodaParser::RPAREN);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentifierContext ------------------------------------------------------------------

KodaParser::IdentifierContext::IdentifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::IdentifierContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::ABORT() {
  return getToken(KodaParser::ABORT, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::TRIGGER() {
  return getToken(KodaParser::TRIGGER, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::RETURN() {
  return getToken(KodaParser::RETURN, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::ERROR() {
  return getToken(KodaParser::ERROR, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::IN() {
  return getToken(KodaParser::IN, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::OUT() {
  return getToken(KodaParser::OUT, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::CONSUMES() {
  return getToken(KodaParser::CONSUMES, 0);
}

tree::TerminalNode* KodaParser::IdentifierContext::PRODUCES() {
  return getToken(KodaParser::PRODUCES, 0);
}


size_t KodaParser::IdentifierContext::getRuleIndex() const {
  return KodaParser::RuleIdentifier;
}


std::any KodaParser::IdentifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitIdentifier(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::IdentifierContext* KodaParser::identifier() {
  IdentifierContext *_localctx = _tracker.createInstance<IdentifierContext>(_ctx, getState());
  enterRule(_localctx, 78, KodaParser::RuleIdentifier);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(550);
    _la = _input->LA(1);
    if (!(((((_la - 16) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 16)) & ((1ULL << (KodaParser::TRIGGER - 16))
      | (1ULL << (KodaParser::RETURN - 16))
      | (1ULL << (KodaParser::ABORT - 16))
      | (1ULL << (KodaParser::ERROR - 16))
      | (1ULL << (KodaParser::IN - 16))
      | (1ULL << (KodaParser::OUT - 16))
      | (1ULL << (KodaParser::CONSUMES - 16))
      | (1ULL << (KodaParser::PRODUCES - 16))
      | (1ULL << (KodaParser::IDENT - 16)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool KodaParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 25: return strategySempred(antlrcpp::downCast<StrategyContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool KodaParser::strategySempred(StrategyContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 10);

  default:
    break;
  }
  return true;
}

void KodaParser::initialize() {
  std::call_once(kodaParserOnceFlag, kodaParserInitialize);
}
