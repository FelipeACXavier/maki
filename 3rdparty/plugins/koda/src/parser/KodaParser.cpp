
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
      "exprPrimary", "recordLiteral", "recordFieldInitializer", "identifier"
    },
    std::vector<std::string>{
      "", "'task'", "'capability'", "'type'", "'enum'", "'extends'", "'list'", 
      "'optional'", "'map'", "'mapping'", "'to'", "'strategy'", "'parameters'", 
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
      "MAP", "MAPPING", "TO", "STRATEGY", "PARAMETERS", "ACTION", "SERVICE", 
      "TOPIC", "TRIGGER", "RETURN", "ABORT", "ERROR", "IN", "OUT", "ON", 
      "CONSUMES", "PRODUCES", "REQ", "PRO", "END", "CONTINUE", "REPEAT", 
      "JOIN", "EITHER", "WITHIN", "DO", "ELSE", "THEN", "EVERY", "ARROW", 
      "PIPE", "EQ", "NEQ", "LEQ", "GEQ", "LT", "GT", "ASSIGN", "DOUBLE_COLON", 
      "COLON", "COMMA", "SEMI", "DOT", "PLUS", "MINUS", "STAR", "SLASH", 
      "NOT", "AND", "OR", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACK", 
      "RBRACK", "NATURAL", "REAL", "BOOLEAN", "IDENT", "STRING", "ANY", 
      "LINE_COMMENT", "BLOCK_COMMENT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,72,571,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,1,0,5,
  	0,86,8,0,10,0,12,0,89,9,0,1,0,1,0,1,1,1,1,1,1,1,1,3,1,97,8,1,1,2,1,2,
  	1,2,1,2,3,2,103,8,2,1,2,1,2,1,2,5,2,108,8,2,10,2,12,2,111,9,2,1,2,1,2,
  	1,2,1,2,1,2,3,2,118,8,2,1,2,1,2,1,2,5,2,123,8,2,10,2,12,2,126,9,2,1,2,
  	3,2,129,8,2,1,3,1,3,1,3,1,3,3,3,135,8,3,1,3,1,3,5,3,139,8,3,10,3,12,3,
  	142,9,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,152,8,3,1,4,1,4,1,4,1,4,1,
  	4,1,5,1,5,1,5,1,5,3,5,163,8,5,1,5,1,5,5,5,167,8,5,10,5,12,5,170,9,5,1,
  	5,1,5,1,6,1,6,1,6,3,6,177,8,6,1,6,1,6,1,7,1,7,3,7,183,8,7,1,7,3,7,186,
  	8,7,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,
  	9,1,9,1,9,1,9,1,9,1,9,1,9,3,9,211,8,9,1,10,1,10,1,10,5,10,216,8,10,10,
  	10,12,10,219,9,10,1,11,1,11,1,11,5,11,224,8,11,10,11,12,11,227,9,11,1,
  	12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,3,12,238,8,12,1,13,1,13,1,
  	13,1,13,1,13,1,13,3,13,246,8,13,1,14,1,14,1,14,4,14,251,8,14,11,14,12,
  	14,252,1,14,1,14,1,15,1,15,1,15,3,15,260,8,15,1,15,3,15,263,8,15,1,15,
  	1,15,1,15,1,15,1,16,1,16,1,16,5,16,272,8,16,10,16,12,16,275,9,16,1,17,
  	1,17,1,17,4,17,280,8,17,11,17,12,17,281,1,17,1,17,1,18,1,18,1,18,1,18,
  	1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,5,19,298,8,19,10,19,12,19,301,
  	9,19,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,5,20,311,8,20,10,20,12,20,
  	314,9,20,1,20,1,20,1,21,1,21,1,21,1,21,1,21,1,21,5,21,324,8,21,10,21,
  	12,21,327,9,21,1,21,1,21,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,
  	1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,
  	1,22,1,22,1,22,1,22,1,22,1,22,1,22,3,22,361,8,22,1,23,1,23,1,23,1,23,
  	1,23,1,23,1,23,1,23,1,23,1,23,3,23,373,8,23,1,24,1,24,1,24,1,24,3,24,
  	379,8,24,1,24,1,24,1,25,1,25,1,25,1,25,1,25,1,25,4,25,389,8,25,11,25,
  	12,25,390,1,25,1,25,1,25,1,25,1,25,1,25,1,25,4,25,400,8,25,11,25,12,25,
  	401,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,
  	1,25,1,25,1,25,5,25,420,8,25,10,25,12,25,423,9,25,1,25,1,25,1,25,1,25,
  	5,25,429,8,25,10,25,12,25,432,9,25,1,25,1,25,1,25,1,25,3,25,438,8,25,
  	1,25,1,25,1,25,4,25,443,8,25,11,25,12,25,444,5,25,447,8,25,10,25,12,25,
  	450,9,25,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,3,26,462,8,
  	26,1,27,1,27,1,27,1,27,1,27,3,27,469,8,27,1,27,1,27,1,27,1,27,1,27,3,
  	27,476,8,27,1,27,1,27,3,27,480,8,27,1,28,1,28,1,28,5,28,485,8,28,10,28,
  	12,28,488,9,28,1,29,1,29,1,30,1,30,1,30,5,30,495,8,30,10,30,12,30,498,
  	9,30,1,31,1,31,1,31,5,31,503,8,31,10,31,12,31,506,9,31,1,32,1,32,1,32,
  	1,32,3,32,512,8,32,1,33,1,33,1,34,1,34,1,34,3,34,519,8,34,1,35,1,35,1,
  	35,5,35,524,8,35,10,35,12,35,527,9,35,1,36,1,36,1,36,5,36,532,8,36,10,
  	36,12,36,535,9,36,1,37,1,37,1,37,3,37,540,8,37,1,38,1,38,1,38,1,38,1,
  	38,1,38,1,38,1,38,1,38,1,38,1,38,3,38,553,8,38,1,39,1,39,5,39,557,8,39,
  	10,39,12,39,560,9,39,1,39,1,39,1,40,1,40,1,40,1,40,1,40,1,41,1,41,1,41,
  	0,1,50,42,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,
  	44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,0,4,1,0,39,
  	44,1,0,51,52,1,0,53,54,3,0,16,21,23,24,67,67,609,0,87,1,0,0,0,2,96,1,
  	0,0,0,4,128,1,0,0,0,6,151,1,0,0,0,8,153,1,0,0,0,10,158,1,0,0,0,12,173,
  	1,0,0,0,14,185,1,0,0,0,16,187,1,0,0,0,18,210,1,0,0,0,20,212,1,0,0,0,22,
  	220,1,0,0,0,24,237,1,0,0,0,26,245,1,0,0,0,28,247,1,0,0,0,30,256,1,0,0,
  	0,32,268,1,0,0,0,34,276,1,0,0,0,36,285,1,0,0,0,38,291,1,0,0,0,40,304,
  	1,0,0,0,42,317,1,0,0,0,44,360,1,0,0,0,46,372,1,0,0,0,48,374,1,0,0,0,50,
  	437,1,0,0,0,52,461,1,0,0,0,54,479,1,0,0,0,56,481,1,0,0,0,58,489,1,0,0,
  	0,60,491,1,0,0,0,62,499,1,0,0,0,64,507,1,0,0,0,66,513,1,0,0,0,68,518,
  	1,0,0,0,70,520,1,0,0,0,72,528,1,0,0,0,74,539,1,0,0,0,76,552,1,0,0,0,78,
  	554,1,0,0,0,80,563,1,0,0,0,82,568,1,0,0,0,84,86,3,2,1,0,85,84,1,0,0,0,
  	86,89,1,0,0,0,87,85,1,0,0,0,87,88,1,0,0,0,88,90,1,0,0,0,89,87,1,0,0,0,
  	90,91,5,0,0,1,91,1,1,0,0,0,92,97,3,4,2,0,93,97,3,6,3,0,94,97,3,10,5,0,
  	95,97,3,16,8,0,96,92,1,0,0,0,96,93,1,0,0,0,96,94,1,0,0,0,96,95,1,0,0,
  	0,97,3,1,0,0,0,98,99,5,1,0,0,99,100,5,67,0,0,100,102,5,58,0,0,101,103,
  	3,22,11,0,102,101,1,0,0,0,102,103,1,0,0,0,103,104,1,0,0,0,104,105,5,59,
  	0,0,105,109,5,60,0,0,106,108,3,26,13,0,107,106,1,0,0,0,108,111,1,0,0,
  	0,109,107,1,0,0,0,109,110,1,0,0,0,110,112,1,0,0,0,111,109,1,0,0,0,112,
  	129,5,61,0,0,113,114,5,2,0,0,114,115,5,67,0,0,115,117,5,58,0,0,116,118,
  	3,22,11,0,117,116,1,0,0,0,117,118,1,0,0,0,118,119,1,0,0,0,119,120,5,59,
  	0,0,120,124,5,60,0,0,121,123,3,26,13,0,122,121,1,0,0,0,123,126,1,0,0,
  	0,124,122,1,0,0,0,124,125,1,0,0,0,125,127,1,0,0,0,126,124,1,0,0,0,127,
  	129,5,61,0,0,128,98,1,0,0,0,128,113,1,0,0,0,129,5,1,0,0,0,130,131,5,3,
  	0,0,131,134,3,20,10,0,132,133,5,5,0,0,133,135,3,20,10,0,134,132,1,0,0,
  	0,134,135,1,0,0,0,135,136,1,0,0,0,136,140,5,60,0,0,137,139,3,8,4,0,138,
  	137,1,0,0,0,139,142,1,0,0,0,140,138,1,0,0,0,140,141,1,0,0,0,141,143,1,
  	0,0,0,142,140,1,0,0,0,143,144,5,61,0,0,144,152,1,0,0,0,145,146,5,3,0,
  	0,146,147,3,20,10,0,147,148,5,45,0,0,148,149,3,18,9,0,149,150,5,49,0,
  	0,150,152,1,0,0,0,151,130,1,0,0,0,151,145,1,0,0,0,152,7,1,0,0,0,153,154,
  	5,67,0,0,154,155,5,47,0,0,155,156,3,18,9,0,156,157,5,49,0,0,157,9,1,0,
  	0,0,158,159,5,4,0,0,159,162,3,20,10,0,160,161,5,47,0,0,161,163,3,18,9,
  	0,162,160,1,0,0,0,162,163,1,0,0,0,163,164,1,0,0,0,164,168,5,60,0,0,165,
  	167,3,12,6,0,166,165,1,0,0,0,167,170,1,0,0,0,168,166,1,0,0,0,168,169,
  	1,0,0,0,169,171,1,0,0,0,170,168,1,0,0,0,171,172,5,61,0,0,172,11,1,0,0,
  	0,173,176,5,67,0,0,174,175,5,45,0,0,175,177,3,14,7,0,176,174,1,0,0,0,
  	176,177,1,0,0,0,177,178,1,0,0,0,178,179,5,49,0,0,179,13,1,0,0,0,180,186,
  	5,68,0,0,181,183,5,52,0,0,182,181,1,0,0,0,182,183,1,0,0,0,183,184,1,0,
  	0,0,184,186,5,64,0,0,185,180,1,0,0,0,185,182,1,0,0,0,186,15,1,0,0,0,187,
  	188,5,9,0,0,188,189,3,18,9,0,189,190,5,10,0,0,190,191,5,68,0,0,191,17,
  	1,0,0,0,192,211,3,20,10,0,193,194,5,6,0,0,194,195,5,43,0,0,195,196,3,
  	18,9,0,196,197,5,44,0,0,197,211,1,0,0,0,198,199,5,7,0,0,199,200,5,43,
  	0,0,200,201,3,18,9,0,201,202,5,44,0,0,202,211,1,0,0,0,203,204,5,8,0,0,
  	204,205,5,43,0,0,205,206,3,18,9,0,206,207,5,48,0,0,207,208,3,18,9,0,208,
  	209,5,44,0,0,209,211,1,0,0,0,210,192,1,0,0,0,210,193,1,0,0,0,210,198,
  	1,0,0,0,210,203,1,0,0,0,211,19,1,0,0,0,212,217,5,67,0,0,213,214,5,46,
  	0,0,214,216,5,67,0,0,215,213,1,0,0,0,216,219,1,0,0,0,217,215,1,0,0,0,
  	217,218,1,0,0,0,218,21,1,0,0,0,219,217,1,0,0,0,220,225,3,24,12,0,221,
  	222,5,48,0,0,222,224,3,24,12,0,223,221,1,0,0,0,224,227,1,0,0,0,225,223,
  	1,0,0,0,225,226,1,0,0,0,226,23,1,0,0,0,227,225,1,0,0,0,228,229,3,18,9,
  	0,229,230,5,67,0,0,230,238,1,0,0,0,231,232,5,67,0,0,232,233,5,25,0,0,
  	233,238,3,18,9,0,234,235,5,67,0,0,235,236,5,26,0,0,236,238,3,18,9,0,237,
  	228,1,0,0,0,237,231,1,0,0,0,237,234,1,0,0,0,238,25,1,0,0,0,239,246,3,
  	28,14,0,240,246,3,34,17,0,241,246,3,38,19,0,242,246,3,40,20,0,243,246,
  	3,42,21,0,244,246,3,44,22,0,245,239,1,0,0,0,245,240,1,0,0,0,245,241,1,
  	0,0,0,245,242,1,0,0,0,245,243,1,0,0,0,245,244,1,0,0,0,246,27,1,0,0,0,
  	247,248,5,11,0,0,248,250,5,60,0,0,249,251,3,30,15,0,250,249,1,0,0,0,251,
  	252,1,0,0,0,252,250,1,0,0,0,252,253,1,0,0,0,253,254,1,0,0,0,254,255,5,
  	61,0,0,255,29,1,0,0,0,256,262,5,67,0,0,257,259,5,62,0,0,258,260,3,32,
  	16,0,259,258,1,0,0,0,259,260,1,0,0,0,260,261,1,0,0,0,261,263,5,63,0,0,
  	262,257,1,0,0,0,262,263,1,0,0,0,263,264,1,0,0,0,264,265,5,47,0,0,265,
  	266,3,50,25,0,266,267,5,49,0,0,267,31,1,0,0,0,268,273,5,67,0,0,269,270,
  	5,48,0,0,270,272,5,67,0,0,271,269,1,0,0,0,272,275,1,0,0,0,273,271,1,0,
  	0,0,273,274,1,0,0,0,274,33,1,0,0,0,275,273,1,0,0,0,276,277,5,12,0,0,277,
  	279,5,60,0,0,278,280,3,36,18,0,279,278,1,0,0,0,280,281,1,0,0,0,281,279,
  	1,0,0,0,281,282,1,0,0,0,282,283,1,0,0,0,283,284,5,61,0,0,284,35,1,0,0,
  	0,285,286,3,18,9,0,286,287,5,67,0,0,287,288,5,45,0,0,288,289,3,58,29,
  	0,289,290,5,49,0,0,290,37,1,0,0,0,291,292,5,13,0,0,292,293,5,68,0,0,293,
  	294,5,68,0,0,294,299,5,60,0,0,295,298,3,46,23,0,296,298,3,44,22,0,297,
  	295,1,0,0,0,297,296,1,0,0,0,298,301,1,0,0,0,299,297,1,0,0,0,299,300,1,
  	0,0,0,300,302,1,0,0,0,301,299,1,0,0,0,302,303,5,61,0,0,303,39,1,0,0,0,
  	304,305,5,14,0,0,305,306,5,68,0,0,306,307,5,68,0,0,307,312,5,60,0,0,308,
  	311,3,46,23,0,309,311,3,44,22,0,310,308,1,0,0,0,310,309,1,0,0,0,311,314,
  	1,0,0,0,312,310,1,0,0,0,312,313,1,0,0,0,313,315,1,0,0,0,314,312,1,0,0,
  	0,315,316,5,61,0,0,316,41,1,0,0,0,317,318,5,15,0,0,318,319,5,68,0,0,319,
  	320,5,68,0,0,320,325,5,60,0,0,321,324,3,46,23,0,322,324,3,44,22,0,323,
  	321,1,0,0,0,323,322,1,0,0,0,324,327,1,0,0,0,325,323,1,0,0,0,325,326,1,
  	0,0,0,326,328,1,0,0,0,327,325,1,0,0,0,328,329,5,61,0,0,329,43,1,0,0,0,
  	330,331,5,16,0,0,331,332,5,47,0,0,332,333,3,48,24,0,333,334,5,49,0,0,
  	334,361,1,0,0,0,335,336,5,17,0,0,336,337,5,47,0,0,337,338,3,48,24,0,338,
  	339,5,49,0,0,339,361,1,0,0,0,340,341,5,18,0,0,341,342,5,47,0,0,342,343,
  	3,48,24,0,343,344,5,49,0,0,344,361,1,0,0,0,345,346,5,19,0,0,346,347,5,
  	47,0,0,347,348,3,48,24,0,348,349,5,49,0,0,349,361,1,0,0,0,350,351,5,20,
  	0,0,351,352,5,47,0,0,352,353,3,48,24,0,353,354,5,49,0,0,354,361,1,0,0,
  	0,355,356,5,21,0,0,356,357,5,47,0,0,357,358,3,48,24,0,358,359,5,49,0,
  	0,359,361,1,0,0,0,360,330,1,0,0,0,360,335,1,0,0,0,360,340,1,0,0,0,360,
  	345,1,0,0,0,360,350,1,0,0,0,360,355,1,0,0,0,361,45,1,0,0,0,362,363,5,
  	23,0,0,363,364,5,47,0,0,364,365,3,18,9,0,365,366,5,49,0,0,366,373,1,0,
  	0,0,367,368,5,24,0,0,368,369,5,47,0,0,369,370,3,18,9,0,370,371,5,49,0,
  	0,371,373,1,0,0,0,372,362,1,0,0,0,372,367,1,0,0,0,373,47,1,0,0,0,374,
  	375,5,67,0,0,375,376,3,82,41,0,376,378,5,58,0,0,377,379,3,22,11,0,378,
  	377,1,0,0,0,378,379,1,0,0,0,379,380,1,0,0,0,380,381,5,59,0,0,381,49,1,
  	0,0,0,382,383,6,25,-1,0,383,384,5,30,0,0,384,385,5,58,0,0,385,388,3,50,
  	25,0,386,387,5,38,0,0,387,389,3,50,25,0,388,386,1,0,0,0,389,390,1,0,0,
  	0,390,388,1,0,0,0,390,391,1,0,0,0,391,392,1,0,0,0,392,393,5,59,0,0,393,
  	438,1,0,0,0,394,395,5,31,0,0,395,396,5,58,0,0,396,399,3,50,25,0,397,398,
  	5,38,0,0,398,400,3,50,25,0,399,397,1,0,0,0,400,401,1,0,0,0,401,399,1,
  	0,0,0,401,402,1,0,0,0,402,403,1,0,0,0,403,404,5,59,0,0,404,438,1,0,0,
  	0,405,406,5,32,0,0,406,407,5,64,0,0,407,408,5,33,0,0,408,409,3,50,25,
  	0,409,410,5,34,0,0,410,411,3,50,25,6,411,438,1,0,0,0,412,413,5,29,0,0,
  	413,414,5,64,0,0,414,415,5,64,0,0,415,416,5,58,0,0,416,417,3,50,25,0,
  	417,421,5,59,0,0,418,420,3,52,26,0,419,418,1,0,0,0,420,423,1,0,0,0,421,
  	419,1,0,0,0,421,422,1,0,0,0,422,438,1,0,0,0,423,421,1,0,0,0,424,438,5,
  	27,0,0,425,438,5,28,0,0,426,430,3,54,27,0,427,429,3,52,26,0,428,427,1,
  	0,0,0,429,432,1,0,0,0,430,428,1,0,0,0,430,431,1,0,0,0,431,438,1,0,0,0,
  	432,430,1,0,0,0,433,434,5,58,0,0,434,435,3,50,25,0,435,436,5,59,0,0,436,
  	438,1,0,0,0,437,382,1,0,0,0,437,394,1,0,0,0,437,405,1,0,0,0,437,412,1,
  	0,0,0,437,424,1,0,0,0,437,425,1,0,0,0,437,426,1,0,0,0,437,433,1,0,0,0,
  	438,448,1,0,0,0,439,442,10,9,0,0,440,441,5,37,0,0,441,443,3,50,25,0,442,
  	440,1,0,0,0,443,444,1,0,0,0,444,442,1,0,0,0,444,445,1,0,0,0,445,447,1,
  	0,0,0,446,439,1,0,0,0,447,450,1,0,0,0,448,446,1,0,0,0,448,449,1,0,0,0,
  	449,51,1,0,0,0,450,448,1,0,0,0,451,452,5,22,0,0,452,453,5,19,0,0,453,
  	462,3,50,25,0,454,455,5,22,0,0,455,456,5,18,0,0,456,462,3,50,25,0,457,
  	458,5,22,0,0,458,459,3,54,27,0,459,460,3,50,25,0,460,462,1,0,0,0,461,
  	451,1,0,0,0,461,454,1,0,0,0,461,457,1,0,0,0,462,53,1,0,0,0,463,464,3,
  	82,41,0,464,465,5,50,0,0,465,466,3,82,41,0,466,468,5,58,0,0,467,469,3,
  	56,28,0,468,467,1,0,0,0,468,469,1,0,0,0,469,470,1,0,0,0,470,471,5,59,
  	0,0,471,480,1,0,0,0,472,473,3,82,41,0,473,475,5,58,0,0,474,476,3,56,28,
  	0,475,474,1,0,0,0,475,476,1,0,0,0,476,477,1,0,0,0,477,478,5,59,0,0,478,
  	480,1,0,0,0,479,463,1,0,0,0,479,472,1,0,0,0,480,55,1,0,0,0,481,486,3,
  	58,29,0,482,483,5,48,0,0,483,485,3,58,29,0,484,482,1,0,0,0,485,488,1,
  	0,0,0,486,484,1,0,0,0,486,487,1,0,0,0,487,57,1,0,0,0,488,486,1,0,0,0,
  	489,490,3,60,30,0,490,59,1,0,0,0,491,496,3,62,31,0,492,493,5,57,0,0,493,
  	495,3,62,31,0,494,492,1,0,0,0,495,498,1,0,0,0,496,494,1,0,0,0,496,497,
  	1,0,0,0,497,61,1,0,0,0,498,496,1,0,0,0,499,504,3,64,32,0,500,501,5,56,
  	0,0,501,503,3,64,32,0,502,500,1,0,0,0,503,506,1,0,0,0,504,502,1,0,0,0,
  	504,505,1,0,0,0,505,63,1,0,0,0,506,504,1,0,0,0,507,511,3,68,34,0,508,
  	509,3,66,33,0,509,510,3,68,34,0,510,512,1,0,0,0,511,508,1,0,0,0,511,512,
  	1,0,0,0,512,65,1,0,0,0,513,514,7,0,0,0,514,67,1,0,0,0,515,516,5,55,0,
  	0,516,519,3,68,34,0,517,519,3,70,35,0,518,515,1,0,0,0,518,517,1,0,0,0,
  	519,69,1,0,0,0,520,525,3,72,36,0,521,522,7,1,0,0,522,524,3,72,36,0,523,
  	521,1,0,0,0,524,527,1,0,0,0,525,523,1,0,0,0,525,526,1,0,0,0,526,71,1,
  	0,0,0,527,525,1,0,0,0,528,533,3,74,37,0,529,530,7,2,0,0,530,532,3,74,
  	37,0,531,529,1,0,0,0,532,535,1,0,0,0,533,531,1,0,0,0,533,534,1,0,0,0,
  	534,73,1,0,0,0,535,533,1,0,0,0,536,537,5,52,0,0,537,540,3,74,37,0,538,
  	540,3,76,38,0,539,536,1,0,0,0,539,538,1,0,0,0,540,75,1,0,0,0,541,553,
  	3,54,27,0,542,553,5,66,0,0,543,553,5,67,0,0,544,553,5,68,0,0,545,553,
  	5,64,0,0,546,553,5,65,0,0,547,553,3,78,39,0,548,549,5,58,0,0,549,550,
  	3,58,29,0,550,551,5,59,0,0,551,553,1,0,0,0,552,541,1,0,0,0,552,542,1,
  	0,0,0,552,543,1,0,0,0,552,544,1,0,0,0,552,545,1,0,0,0,552,546,1,0,0,0,
  	552,547,1,0,0,0,552,548,1,0,0,0,553,77,1,0,0,0,554,558,5,60,0,0,555,557,
  	3,80,40,0,556,555,1,0,0,0,557,560,1,0,0,0,558,556,1,0,0,0,558,559,1,0,
  	0,0,559,561,1,0,0,0,560,558,1,0,0,0,561,562,5,61,0,0,562,79,1,0,0,0,563,
  	564,5,67,0,0,564,565,5,47,0,0,565,566,3,58,29,0,566,567,5,49,0,0,567,
  	81,1,0,0,0,568,569,7,3,0,0,569,83,1,0,0,0,55,87,96,102,109,117,124,128,
  	134,140,151,162,168,176,182,185,210,217,225,237,245,252,259,262,273,281,
  	297,299,310,312,323,325,360,372,378,390,401,421,430,437,444,448,461,468,
  	475,479,486,496,504,511,518,525,533,539,552,558
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
    setState(87);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TASK)
      | (1ULL << KodaParser::CAPABILITY)
      | (1ULL << KodaParser::TYPE)
      | (1ULL << KodaParser::ENUM)
      | (1ULL << KodaParser::MAPPING))) != 0)) {
      setState(84);
      topLevelDeclaration();
      setState(89);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(90);
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
    setState(96);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK:
      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 1);
        setState(92);
        topLevelComponent();
        break;
      }

      case KodaParser::TYPE: {
        enterOuterAlt(_localctx, 2);
        setState(93);
        typeDeclaration();
        break;
      }

      case KodaParser::ENUM: {
        enterOuterAlt(_localctx, 3);
        setState(94);
        enumDeclaration();
        break;
      }

      case KodaParser::MAPPING: {
        enterOuterAlt(_localctx, 4);
        setState(95);
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
    setState(128);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(98);
        match(KodaParser::TASK);
        setState(99);
        match(KodaParser::IDENT);
        setState(100);
        match(KodaParser::LPAREN);
        setState(102);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 6) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
          | (1ULL << (KodaParser::OPTIONAL - 6))
          | (1ULL << (KodaParser::MAP - 6))
          | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
          setState(101);
          argumentList();
        }
        setState(104);
        match(KodaParser::RPAREN);
        setState(105);
        match(KodaParser::LBRACE);
        setState(109);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::STRATEGY)
          | (1ULL << KodaParser::PARAMETERS)
          | (1ULL << KodaParser::ACTION)
          | (1ULL << KodaParser::SERVICE)
          | (1ULL << KodaParser::TOPIC)
          | (1ULL << KodaParser::TRIGGER)
          | (1ULL << KodaParser::RETURN)
          | (1ULL << KodaParser::ABORT)
          | (1ULL << KodaParser::ERROR)
          | (1ULL << KodaParser::IN)
          | (1ULL << KodaParser::OUT))) != 0)) {
          setState(106);
          statement();
          setState(111);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(112);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(113);
        match(KodaParser::CAPABILITY);
        setState(114);
        match(KodaParser::IDENT);
        setState(115);
        match(KodaParser::LPAREN);
        setState(117);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 6) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
          | (1ULL << (KodaParser::OPTIONAL - 6))
          | (1ULL << (KodaParser::MAP - 6))
          | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
          setState(116);
          argumentList();
        }
        setState(119);
        match(KodaParser::RPAREN);
        setState(120);
        match(KodaParser::LBRACE);
        setState(124);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::STRATEGY)
          | (1ULL << KodaParser::PARAMETERS)
          | (1ULL << KodaParser::ACTION)
          | (1ULL << KodaParser::SERVICE)
          | (1ULL << KodaParser::TOPIC)
          | (1ULL << KodaParser::TRIGGER)
          | (1ULL << KodaParser::RETURN)
          | (1ULL << KodaParser::ABORT)
          | (1ULL << KodaParser::ERROR)
          | (1ULL << KodaParser::IN)
          | (1ULL << KodaParser::OUT))) != 0)) {
          setState(121);
          statement();
          setState(126);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(127);
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
    setState(151);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::TypeRecordContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(130);
      match(KodaParser::TYPE);
      setState(131);
      qualifiedName();
      setState(134);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::EXTENDS) {
        setState(132);
        match(KodaParser::EXTENDS);
        setState(133);
        qualifiedName();
      }
      setState(136);
      match(KodaParser::LBRACE);
      setState(140);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::IDENT) {
        setState(137);
        fieldDeclaration();
        setState(142);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(143);
      match(KodaParser::RBRACE);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::TypeAliasContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(145);
      match(KodaParser::TYPE);
      setState(146);
      qualifiedName();
      setState(147);
      match(KodaParser::ASSIGN);
      setState(148);
      typeReference();
      setState(149);
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
    setState(153);
    match(KodaParser::IDENT);
    setState(154);
    match(KodaParser::COLON);
    setState(155);
    typeReference();
    setState(156);
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
    setState(158);
    match(KodaParser::ENUM);
    setState(159);
    qualifiedName();
    setState(162);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(160);
      match(KodaParser::COLON);
      setState(161);
      typeReference();
    }
    setState(164);
    match(KodaParser::LBRACE);
    setState(168);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(165);
      enumValue();
      setState(170);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(171);
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
    setState(173);
    match(KodaParser::IDENT);
    setState(176);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(174);
      match(KodaParser::ASSIGN);
      setState(175);
      enumLiteral();
    }
    setState(178);
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
    setState(185);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(180);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::MINUS:
      case KodaParser::NATURAL: {
        enterOuterAlt(_localctx, 2);
        setState(182);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::MINUS) {
          setState(181);
          match(KodaParser::MINUS);
        }
        setState(184);
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
    setState(187);
    match(KodaParser::MAPPING);
    setState(188);
    typeReference();
    setState(189);
    match(KodaParser::TO);
    setState(190);
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
    setState(210);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IDENT: {
        _localctx = _tracker.createInstance<KodaParser::TypeNamedContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(192);
        qualifiedName();
        break;
      }

      case KodaParser::LIST: {
        _localctx = _tracker.createInstance<KodaParser::TypeListContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(193);
        match(KodaParser::LIST);
        setState(194);
        match(KodaParser::LT);
        setState(195);
        typeReference();
        setState(196);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::OPTIONAL: {
        _localctx = _tracker.createInstance<KodaParser::TypeOptionalContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(198);
        match(KodaParser::OPTIONAL);
        setState(199);
        match(KodaParser::LT);
        setState(200);
        typeReference();
        setState(201);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::MAP: {
        _localctx = _tracker.createInstance<KodaParser::TypeMapContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(203);
        match(KodaParser::MAP);
        setState(204);
        match(KodaParser::LT);
        setState(205);
        typeReference();
        setState(206);
        match(KodaParser::COMMA);
        setState(207);
        typeReference();
        setState(208);
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
    setState(212);
    match(KodaParser::IDENT);
    setState(217);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::DOUBLE_COLON) {
      setState(213);
      match(KodaParser::DOUBLE_COLON);
      setState(214);
      match(KodaParser::IDENT);
      setState(219);
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
    setState(220);
    argument();
    setState(225);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(221);
      match(KodaParser::COMMA);
      setState(222);
      argument();
      setState(227);
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
    setState(237);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(228);
      typeReference();
      setState(229);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(231);
      match(KodaParser::IDENT);
      setState(232);
      match(KodaParser::REQ);
      setState(233);
      typeReference();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(234);
      match(KodaParser::IDENT);
      setState(235);
      match(KodaParser::PRO);
      setState(236);
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
    setState(245);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(239);
        tasksBlock();
        break;
      }

      case KodaParser::PARAMETERS: {
        enterOuterAlt(_localctx, 2);
        setState(240);
        varsBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 3);
        setState(241);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 4);
        setState(242);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 5);
        setState(243);
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
        setState(244);
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
    setState(247);
    match(KodaParser::STRATEGY);
    setState(248);
    match(KodaParser::LBRACE);
    setState(250); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(249);
      flow();
      setState(252); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(254);
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
    setState(256);
    match(KodaParser::IDENT);
    setState(262);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(257);
      match(KodaParser::LBRACK);
      setState(259);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(258);
        identList();
      }
      setState(261);
      match(KodaParser::RBRACK);
    }
    setState(264);
    match(KodaParser::COLON);
    setState(265);
    strategy(0);
    setState(266);
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
    setState(268);
    match(KodaParser::IDENT);
    setState(273);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(269);
      match(KodaParser::COMMA);
      setState(270);
      match(KodaParser::IDENT);
      setState(275);
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

tree::TerminalNode* KodaParser::VarsBlockContext::PARAMETERS() {
  return getToken(KodaParser::PARAMETERS, 0);
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
    setState(276);
    match(KodaParser::PARAMETERS);
    setState(277);
    match(KodaParser::LBRACE);
    setState(279); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(278);
      variableStatement();
      setState(281); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((((_la - 6) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
      | (1ULL << (KodaParser::OPTIONAL - 6))
      | (1ULL << (KodaParser::MAP - 6))
      | (1ULL << (KodaParser::IDENT - 6)))) != 0));
    setState(283);
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

KodaParser::ExpressionContext* KodaParser::VariableStatementContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
}

tree::TerminalNode* KodaParser::VariableStatementContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
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
    setState(285);
    typeReference();
    setState(286);
    match(KodaParser::IDENT);
    setState(287);
    match(KodaParser::ASSIGN);
    setState(288);
    expression();
    setState(289);
    match(KodaParser::SEMI);
   
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
    setState(291);
    match(KodaParser::ACTION);
    setState(292);
    match(KodaParser::STRING);
    setState(293);
    match(KodaParser::STRING);
    setState(294);
    match(KodaParser::LBRACE);
    setState(299);
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
      setState(297);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(295);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(296);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(301);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(302);
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
    setState(304);
    match(KodaParser::SERVICE);
    setState(305);
    match(KodaParser::STRING);
    setState(306);
    match(KodaParser::STRING);
    setState(307);
    match(KodaParser::LBRACE);
    setState(312);
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
      setState(310);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(308);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(309);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(314);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(315);
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
    setState(317);
    match(KodaParser::TOPIC);
    setState(318);
    match(KodaParser::STRING);
    setState(319);
    match(KodaParser::STRING);
    setState(320);
    match(KodaParser::LBRACE);
    setState(325);
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
      setState(323);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(321);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(322);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(327);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(328);
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
    setState(360);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(330);
        match(KodaParser::TRIGGER);
        setState(331);
        match(KodaParser::COLON);
        setState(332);
        eventDefStatement();
        setState(333);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(335);
        match(KodaParser::RETURN);
        setState(336);
        match(KodaParser::COLON);
        setState(337);
        eventDefStatement();
        setState(338);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(340);
        match(KodaParser::ABORT);
        setState(341);
        match(KodaParser::COLON);
        setState(342);
        eventDefStatement();
        setState(343);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(345);
        match(KodaParser::ERROR);
        setState(346);
        match(KodaParser::COLON);
        setState(347);
        eventDefStatement();
        setState(348);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(350);
        match(KodaParser::IN);
        setState(351);
        match(KodaParser::COLON);
        setState(352);
        eventDefStatement();
        setState(353);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(355);
        match(KodaParser::OUT);
        setState(356);
        match(KodaParser::COLON);
        setState(357);
        eventDefStatement();
        setState(358);
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
    setState(372);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::CONSUMES: {
        enterOuterAlt(_localctx, 1);
        setState(362);
        match(KodaParser::CONSUMES);
        setState(363);
        match(KodaParser::COLON);
        setState(364);
        typeReference();
        setState(365);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::PRODUCES: {
        enterOuterAlt(_localctx, 2);
        setState(367);
        match(KodaParser::PRODUCES);
        setState(368);
        match(KodaParser::COLON);
        setState(369);
        typeReference();
        setState(370);
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
    setState(374);
    match(KodaParser::IDENT);
    setState(375);
    identifier();
    setState(376);
    match(KodaParser::LPAREN);
    setState(378);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 6) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
      | (1ULL << (KodaParser::OPTIONAL - 6))
      | (1ULL << (KodaParser::MAP - 6))
      | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
      setState(377);
      argumentList();
    }
    setState(380);
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
    setState(437);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::JOIN: {
        _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(383);
        match(KodaParser::JOIN);
        setState(384);
        match(KodaParser::LPAREN);
        setState(385);
        strategy(0);
        setState(388); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(386);
          match(KodaParser::PIPE);
          setState(387);
          strategy(0);
          setState(390); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(392);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::EITHER: {
        _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(394);
        match(KodaParser::EITHER);
        setState(395);
        match(KodaParser::LPAREN);
        setState(396);
        strategy(0);
        setState(399); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(397);
          match(KodaParser::PIPE);
          setState(398);
          strategy(0);
          setState(401); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(403);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::WITHIN: {
        _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(405);
        match(KodaParser::WITHIN);
        setState(406);
        match(KodaParser::NATURAL);
        setState(407);
        match(KodaParser::DO);
        setState(408);
        strategy(0);
        setState(409);
        match(KodaParser::ELSE);
        setState(410);
        strategy(6);
        break;
      }

      case KodaParser::REPEAT: {
        _localctx = _tracker.createInstance<StratRepeatContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(412);
        match(KodaParser::REPEAT);
        setState(413);
        match(KodaParser::NATURAL);
        setState(414);
        match(KodaParser::NATURAL);
        setState(415);
        match(KodaParser::LPAREN);
        setState(416);
        strategy(0);
        setState(417);
        match(KodaParser::RPAREN);
        setState(421);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(418);
            strategyHandler(); 
          }
          setState(423);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
        }
        break;
      }

      case KodaParser::END: {
        _localctx = _tracker.createInstance<StratEndContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(424);
        match(KodaParser::END);
        break;
      }

      case KodaParser::CONTINUE: {
        _localctx = _tracker.createInstance<StratContinueContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(425);
        match(KodaParser::CONTINUE);
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
      case KodaParser::IDENT: {
        _localctx = _tracker.createInstance<StratTaskContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(426);
        eventStatement();
        setState(430);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(427);
            strategyHandler(); 
          }
          setState(432);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
        }
        break;
      }

      case KodaParser::LPAREN: {
        _localctx = _tracker.createInstance<StratParenContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(433);
        match(KodaParser::LPAREN);
        setState(434);
        strategy(0);
        setState(435);
        match(KodaParser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(448);
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
        setState(439);

        if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
        setState(442); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(440);
                  match(KodaParser::ARROW);
                  setState(441);
                  strategy(0);
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(444); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER); 
      }
      setState(450);
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
    setState(461);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(451);
      match(KodaParser::ON);
      setState(452);
      match(KodaParser::ERROR);
      setState(453);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(454);
      match(KodaParser::ON);
      setState(455);
      match(KodaParser::ABORT);
      setState(456);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(457);
      match(KodaParser::ON);
      setState(458);
      eventStatement();
      setState(459);
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
    setState(479);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(463);
      identifier();
      setState(464);
      match(KodaParser::DOT);
      setState(465);
      identifier();
      setState(466);
      match(KodaParser::LPAREN);
      setState(468);
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
        | (1ULL << (KodaParser::LBRACE - 16))
        | (1ULL << (KodaParser::NATURAL - 16))
        | (1ULL << (KodaParser::REAL - 16))
        | (1ULL << (KodaParser::BOOLEAN - 16))
        | (1ULL << (KodaParser::IDENT - 16))
        | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
        setState(467);
        exprList();
      }
      setState(470);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(472);
      identifier();
      setState(473);
      match(KodaParser::LPAREN);
      setState(475);
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
        | (1ULL << (KodaParser::LBRACE - 16))
        | (1ULL << (KodaParser::NATURAL - 16))
        | (1ULL << (KodaParser::REAL - 16))
        | (1ULL << (KodaParser::BOOLEAN - 16))
        | (1ULL << (KodaParser::IDENT - 16))
        | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
        setState(474);
        exprList();
      }
      setState(477);
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
    setState(481);
    expression();
    setState(486);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(482);
      match(KodaParser::COMMA);
      setState(483);
      expression();
      setState(488);
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
    setState(489);
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
    setState(491);
    exprAnd();
    setState(496);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(492);
      match(KodaParser::OR);
      setState(493);
      exprAnd();
      setState(498);
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
    setState(499);
    exprCmp();
    setState(504);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(500);
      match(KodaParser::AND);
      setState(501);
      exprCmp();
      setState(506);
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
    setState(507);
    exprNot();
    setState(511);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::EQ)
      | (1ULL << KodaParser::NEQ)
      | (1ULL << KodaParser::LEQ)
      | (1ULL << KodaParser::GEQ)
      | (1ULL << KodaParser::LT)
      | (1ULL << KodaParser::GT))) != 0)) {
      setState(508);
      compOp();
      setState(509);
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
    setState(513);
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
    setState(518);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(515);
        match(KodaParser::NOT);
        setState(516);
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
      case KodaParser::LBRACE:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::BOOLEAN:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(517);
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
    setState(520);
    exprMul();
    setState(525);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(521);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(522);
      exprMul();
      setState(527);
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
    setState(528);
    exprUnary();
    setState(533);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(529);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(530);
      exprUnary();
      setState(535);
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
    setState(539);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(536);
        match(KodaParser::MINUS);
        setState(537);
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
      case KodaParser::LBRACE:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::BOOLEAN:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(538);
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
//----------------- ExprBooleanContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::ExprBooleanContext::BOOLEAN() {
  return getToken(KodaParser::BOOLEAN, 0);
}

KodaParser::ExprBooleanContext::ExprBooleanContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprBooleanContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprBoolean(this);
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
//----------------- ExprRecordContext ------------------------------------------------------------------

KodaParser::RecordLiteralContext* KodaParser::ExprRecordContext::recordLiteral() {
  return getRuleContext<KodaParser::RecordLiteralContext>(0);
}

KodaParser::ExprRecordContext::ExprRecordContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprRecordContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprRecord(this);
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
    setState(552);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(541);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprBooleanContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(542);
      match(KodaParser::BOOLEAN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(543);
      match(KodaParser::IDENT);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(544);
      match(KodaParser::STRING);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(545);
      match(KodaParser::NATURAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(546);
      match(KodaParser::REAL);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<KodaParser::ExprRecordContext>(_localctx);
      enterOuterAlt(_localctx, 7);
      setState(547);
      recordLiteral();
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 8);
      setState(548);
      match(KodaParser::LPAREN);
      setState(549);
      expression();
      setState(550);
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

//----------------- RecordLiteralContext ------------------------------------------------------------------

KodaParser::RecordLiteralContext::RecordLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::RecordLiteralContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::RecordLiteralContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::RecordFieldInitializerContext *> KodaParser::RecordLiteralContext::recordFieldInitializer() {
  return getRuleContexts<KodaParser::RecordFieldInitializerContext>();
}

KodaParser::RecordFieldInitializerContext* KodaParser::RecordLiteralContext::recordFieldInitializer(size_t i) {
  return getRuleContext<KodaParser::RecordFieldInitializerContext>(i);
}


size_t KodaParser::RecordLiteralContext::getRuleIndex() const {
  return KodaParser::RuleRecordLiteral;
}


std::any KodaParser::RecordLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitRecordLiteral(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::RecordLiteralContext* KodaParser::recordLiteral() {
  RecordLiteralContext *_localctx = _tracker.createInstance<RecordLiteralContext>(_ctx, getState());
  enterRule(_localctx, 78, KodaParser::RuleRecordLiteral);
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
    setState(554);
    match(KodaParser::LBRACE);
    setState(558);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(555);
      recordFieldInitializer();
      setState(560);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(561);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RecordFieldInitializerContext ------------------------------------------------------------------

KodaParser::RecordFieldInitializerContext::RecordFieldInitializerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::RecordFieldInitializerContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::RecordFieldInitializerContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::ExpressionContext* KodaParser::RecordFieldInitializerContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
}

tree::TerminalNode* KodaParser::RecordFieldInitializerContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}


size_t KodaParser::RecordFieldInitializerContext::getRuleIndex() const {
  return KodaParser::RuleRecordFieldInitializer;
}


std::any KodaParser::RecordFieldInitializerContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitRecordFieldInitializer(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::RecordFieldInitializerContext* KodaParser::recordFieldInitializer() {
  RecordFieldInitializerContext *_localctx = _tracker.createInstance<RecordFieldInitializerContext>(_ctx, getState());
  enterRule(_localctx, 80, KodaParser::RuleRecordFieldInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(563);
    match(KodaParser::IDENT);
    setState(564);
    match(KodaParser::COLON);
    setState(565);
    expression();
    setState(566);
    match(KodaParser::SEMI);
   
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
  enterRule(_localctx, 82, KodaParser::RuleIdentifier);
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
    setState(568);
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
    case 0: return precpred(_ctx, 9);

  default:
    break;
  }
  return true;
}

void KodaParser::initialize() {
  std::call_once(kodaParserOnceFlag, kodaParserInitialize);
}
