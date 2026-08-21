
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
      "exprPrimary", "recordLiteral", "recordFieldInitializer", "listLiteral", 
      "mapLiteral", "mapFieldInitializer", "identifier"
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
  	4,1,72,613,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,1,0,5,0,92,8,0,10,0,12,0,95,9,0,1,0,1,0,1,1,1,
  	1,1,1,1,1,3,1,103,8,1,1,2,1,2,1,2,1,2,3,2,109,8,2,1,2,1,2,1,2,5,2,114,
  	8,2,10,2,12,2,117,9,2,1,2,1,2,1,2,1,2,1,2,3,2,124,8,2,1,2,1,2,1,2,5,2,
  	129,8,2,10,2,12,2,132,9,2,1,2,3,2,135,8,2,1,3,1,3,1,3,1,3,3,3,141,8,3,
  	1,3,1,3,5,3,145,8,3,10,3,12,3,148,9,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,
  	3,3,158,8,3,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,3,5,169,8,5,1,5,1,5,5,
  	5,173,8,5,10,5,12,5,176,9,5,1,5,1,5,1,6,1,6,1,6,3,6,183,8,6,1,6,1,6,1,
  	7,1,7,3,7,189,8,7,1,7,3,7,192,8,7,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,9,1,9,
  	1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,3,9,217,8,9,1,
  	10,1,10,1,10,5,10,222,8,10,10,10,12,10,225,9,10,1,11,1,11,1,11,5,11,230,
  	8,11,10,11,12,11,233,9,11,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,
  	3,12,244,8,12,1,13,1,13,1,13,1,13,1,13,1,13,3,13,252,8,13,1,14,1,14,1,
  	14,4,14,257,8,14,11,14,12,14,258,1,14,1,14,1,15,1,15,1,15,3,15,266,8,
  	15,1,15,3,15,269,8,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,5,16,278,8,16,
  	10,16,12,16,281,9,16,1,17,1,17,1,17,4,17,286,8,17,11,17,12,17,287,1,17,
  	1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,5,19,
  	304,8,19,10,19,12,19,307,9,19,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,
  	5,20,317,8,20,10,20,12,20,320,9,20,1,20,1,20,1,21,1,21,1,21,1,21,1,21,
  	1,21,5,21,330,8,21,10,21,12,21,333,9,21,1,21,1,21,1,22,1,22,1,22,1,22,
  	1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,
  	1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,3,22,367,
  	8,22,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,3,23,379,8,23,
  	1,24,1,24,1,24,1,24,3,24,385,8,24,1,24,1,24,1,25,1,25,1,25,1,25,1,25,
  	1,25,4,25,395,8,25,11,25,12,25,396,1,25,1,25,1,25,1,25,1,25,1,25,1,25,
  	4,25,406,8,25,11,25,12,25,407,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,
  	1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,5,25,426,8,25,10,25,12,25,429,
  	9,25,1,25,1,25,1,25,1,25,5,25,435,8,25,10,25,12,25,438,9,25,1,25,1,25,
  	1,25,1,25,3,25,444,8,25,1,25,1,25,1,25,4,25,449,8,25,11,25,12,25,450,
  	5,25,453,8,25,10,25,12,25,456,9,25,1,26,1,26,1,26,1,26,1,26,1,26,1,26,
  	1,26,1,26,1,26,3,26,468,8,26,1,27,1,27,1,27,1,27,1,27,3,27,475,8,27,1,
  	27,1,27,1,27,1,27,1,27,3,27,482,8,27,1,27,1,27,3,27,486,8,27,1,28,1,28,
  	1,28,5,28,491,8,28,10,28,12,28,494,9,28,1,29,1,29,1,30,1,30,1,30,5,30,
  	501,8,30,10,30,12,30,504,9,30,1,31,1,31,1,31,5,31,509,8,31,10,31,12,31,
  	512,9,31,1,32,1,32,1,32,1,32,3,32,518,8,32,1,33,1,33,1,34,1,34,1,34,3,
  	34,525,8,34,1,35,1,35,1,35,5,35,530,8,35,10,35,12,35,533,9,35,1,36,1,
  	36,1,36,5,36,538,8,36,10,36,12,36,541,9,36,1,37,1,37,1,37,3,37,546,8,
  	37,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,3,
  	38,561,8,38,1,39,1,39,1,39,1,39,5,39,567,8,39,10,39,12,39,570,9,39,3,
  	39,572,8,39,1,39,1,39,1,40,1,40,1,40,1,40,1,40,1,41,1,41,1,41,1,41,5,
  	41,585,8,41,10,41,12,41,588,9,41,3,41,590,8,41,1,41,1,41,1,42,1,42,1,
  	42,1,42,5,42,598,8,42,10,42,12,42,601,9,42,3,42,603,8,42,1,42,1,42,1,
  	43,1,43,1,43,1,43,1,44,1,44,1,44,0,1,50,45,0,2,4,6,8,10,12,14,16,18,20,
  	22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,
  	68,70,72,74,76,78,80,82,84,86,88,0,4,1,0,39,44,1,0,51,52,1,0,53,54,3,
  	0,16,21,23,24,67,67,655,0,93,1,0,0,0,2,102,1,0,0,0,4,134,1,0,0,0,6,157,
  	1,0,0,0,8,159,1,0,0,0,10,164,1,0,0,0,12,179,1,0,0,0,14,191,1,0,0,0,16,
  	193,1,0,0,0,18,216,1,0,0,0,20,218,1,0,0,0,22,226,1,0,0,0,24,243,1,0,0,
  	0,26,251,1,0,0,0,28,253,1,0,0,0,30,262,1,0,0,0,32,274,1,0,0,0,34,282,
  	1,0,0,0,36,291,1,0,0,0,38,297,1,0,0,0,40,310,1,0,0,0,42,323,1,0,0,0,44,
  	366,1,0,0,0,46,378,1,0,0,0,48,380,1,0,0,0,50,443,1,0,0,0,52,467,1,0,0,
  	0,54,485,1,0,0,0,56,487,1,0,0,0,58,495,1,0,0,0,60,497,1,0,0,0,62,505,
  	1,0,0,0,64,513,1,0,0,0,66,519,1,0,0,0,68,524,1,0,0,0,70,526,1,0,0,0,72,
  	534,1,0,0,0,74,545,1,0,0,0,76,560,1,0,0,0,78,562,1,0,0,0,80,575,1,0,0,
  	0,82,580,1,0,0,0,84,593,1,0,0,0,86,606,1,0,0,0,88,610,1,0,0,0,90,92,3,
  	2,1,0,91,90,1,0,0,0,92,95,1,0,0,0,93,91,1,0,0,0,93,94,1,0,0,0,94,96,1,
  	0,0,0,95,93,1,0,0,0,96,97,5,0,0,1,97,1,1,0,0,0,98,103,3,4,2,0,99,103,
  	3,6,3,0,100,103,3,10,5,0,101,103,3,16,8,0,102,98,1,0,0,0,102,99,1,0,0,
  	0,102,100,1,0,0,0,102,101,1,0,0,0,103,3,1,0,0,0,104,105,5,1,0,0,105,106,
  	5,67,0,0,106,108,5,58,0,0,107,109,3,22,11,0,108,107,1,0,0,0,108,109,1,
  	0,0,0,109,110,1,0,0,0,110,111,5,59,0,0,111,115,5,60,0,0,112,114,3,26,
  	13,0,113,112,1,0,0,0,114,117,1,0,0,0,115,113,1,0,0,0,115,116,1,0,0,0,
  	116,118,1,0,0,0,117,115,1,0,0,0,118,135,5,61,0,0,119,120,5,2,0,0,120,
  	121,5,67,0,0,121,123,5,58,0,0,122,124,3,22,11,0,123,122,1,0,0,0,123,124,
  	1,0,0,0,124,125,1,0,0,0,125,126,5,59,0,0,126,130,5,60,0,0,127,129,3,26,
  	13,0,128,127,1,0,0,0,129,132,1,0,0,0,130,128,1,0,0,0,130,131,1,0,0,0,
  	131,133,1,0,0,0,132,130,1,0,0,0,133,135,5,61,0,0,134,104,1,0,0,0,134,
  	119,1,0,0,0,135,5,1,0,0,0,136,137,5,3,0,0,137,140,3,20,10,0,138,139,5,
  	5,0,0,139,141,3,20,10,0,140,138,1,0,0,0,140,141,1,0,0,0,141,142,1,0,0,
  	0,142,146,5,60,0,0,143,145,3,8,4,0,144,143,1,0,0,0,145,148,1,0,0,0,146,
  	144,1,0,0,0,146,147,1,0,0,0,147,149,1,0,0,0,148,146,1,0,0,0,149,150,5,
  	61,0,0,150,158,1,0,0,0,151,152,5,3,0,0,152,153,3,20,10,0,153,154,5,45,
  	0,0,154,155,3,18,9,0,155,156,5,49,0,0,156,158,1,0,0,0,157,136,1,0,0,0,
  	157,151,1,0,0,0,158,7,1,0,0,0,159,160,5,67,0,0,160,161,5,47,0,0,161,162,
  	3,18,9,0,162,163,5,49,0,0,163,9,1,0,0,0,164,165,5,4,0,0,165,168,3,20,
  	10,0,166,167,5,47,0,0,167,169,3,18,9,0,168,166,1,0,0,0,168,169,1,0,0,
  	0,169,170,1,0,0,0,170,174,5,60,0,0,171,173,3,12,6,0,172,171,1,0,0,0,173,
  	176,1,0,0,0,174,172,1,0,0,0,174,175,1,0,0,0,175,177,1,0,0,0,176,174,1,
  	0,0,0,177,178,5,61,0,0,178,11,1,0,0,0,179,182,5,67,0,0,180,181,5,45,0,
  	0,181,183,3,14,7,0,182,180,1,0,0,0,182,183,1,0,0,0,183,184,1,0,0,0,184,
  	185,5,49,0,0,185,13,1,0,0,0,186,192,5,68,0,0,187,189,5,52,0,0,188,187,
  	1,0,0,0,188,189,1,0,0,0,189,190,1,0,0,0,190,192,5,64,0,0,191,186,1,0,
  	0,0,191,188,1,0,0,0,192,15,1,0,0,0,193,194,5,9,0,0,194,195,3,18,9,0,195,
  	196,5,10,0,0,196,197,5,68,0,0,197,17,1,0,0,0,198,217,3,20,10,0,199,200,
  	5,6,0,0,200,201,5,43,0,0,201,202,3,18,9,0,202,203,5,44,0,0,203,217,1,
  	0,0,0,204,205,5,7,0,0,205,206,5,43,0,0,206,207,3,18,9,0,207,208,5,44,
  	0,0,208,217,1,0,0,0,209,210,5,8,0,0,210,211,5,43,0,0,211,212,3,18,9,0,
  	212,213,5,48,0,0,213,214,3,18,9,0,214,215,5,44,0,0,215,217,1,0,0,0,216,
  	198,1,0,0,0,216,199,1,0,0,0,216,204,1,0,0,0,216,209,1,0,0,0,217,19,1,
  	0,0,0,218,223,5,67,0,0,219,220,5,46,0,0,220,222,5,67,0,0,221,219,1,0,
  	0,0,222,225,1,0,0,0,223,221,1,0,0,0,223,224,1,0,0,0,224,21,1,0,0,0,225,
  	223,1,0,0,0,226,231,3,24,12,0,227,228,5,48,0,0,228,230,3,24,12,0,229,
  	227,1,0,0,0,230,233,1,0,0,0,231,229,1,0,0,0,231,232,1,0,0,0,232,23,1,
  	0,0,0,233,231,1,0,0,0,234,235,3,18,9,0,235,236,5,67,0,0,236,244,1,0,0,
  	0,237,238,5,67,0,0,238,239,5,25,0,0,239,244,3,18,9,0,240,241,5,67,0,0,
  	241,242,5,26,0,0,242,244,3,18,9,0,243,234,1,0,0,0,243,237,1,0,0,0,243,
  	240,1,0,0,0,244,25,1,0,0,0,245,252,3,28,14,0,246,252,3,34,17,0,247,252,
  	3,38,19,0,248,252,3,40,20,0,249,252,3,42,21,0,250,252,3,44,22,0,251,245,
  	1,0,0,0,251,246,1,0,0,0,251,247,1,0,0,0,251,248,1,0,0,0,251,249,1,0,0,
  	0,251,250,1,0,0,0,252,27,1,0,0,0,253,254,5,11,0,0,254,256,5,60,0,0,255,
  	257,3,30,15,0,256,255,1,0,0,0,257,258,1,0,0,0,258,256,1,0,0,0,258,259,
  	1,0,0,0,259,260,1,0,0,0,260,261,5,61,0,0,261,29,1,0,0,0,262,268,5,67,
  	0,0,263,265,5,62,0,0,264,266,3,32,16,0,265,264,1,0,0,0,265,266,1,0,0,
  	0,266,267,1,0,0,0,267,269,5,63,0,0,268,263,1,0,0,0,268,269,1,0,0,0,269,
  	270,1,0,0,0,270,271,5,47,0,0,271,272,3,50,25,0,272,273,5,49,0,0,273,31,
  	1,0,0,0,274,279,5,67,0,0,275,276,5,48,0,0,276,278,5,67,0,0,277,275,1,
  	0,0,0,278,281,1,0,0,0,279,277,1,0,0,0,279,280,1,0,0,0,280,33,1,0,0,0,
  	281,279,1,0,0,0,282,283,5,12,0,0,283,285,5,60,0,0,284,286,3,36,18,0,285,
  	284,1,0,0,0,286,287,1,0,0,0,287,285,1,0,0,0,287,288,1,0,0,0,288,289,1,
  	0,0,0,289,290,5,61,0,0,290,35,1,0,0,0,291,292,3,18,9,0,292,293,5,67,0,
  	0,293,294,5,45,0,0,294,295,3,58,29,0,295,296,5,49,0,0,296,37,1,0,0,0,
  	297,298,5,13,0,0,298,299,5,68,0,0,299,300,5,68,0,0,300,305,5,60,0,0,301,
  	304,3,46,23,0,302,304,3,44,22,0,303,301,1,0,0,0,303,302,1,0,0,0,304,307,
  	1,0,0,0,305,303,1,0,0,0,305,306,1,0,0,0,306,308,1,0,0,0,307,305,1,0,0,
  	0,308,309,5,61,0,0,309,39,1,0,0,0,310,311,5,14,0,0,311,312,5,68,0,0,312,
  	313,5,68,0,0,313,318,5,60,0,0,314,317,3,46,23,0,315,317,3,44,22,0,316,
  	314,1,0,0,0,316,315,1,0,0,0,317,320,1,0,0,0,318,316,1,0,0,0,318,319,1,
  	0,0,0,319,321,1,0,0,0,320,318,1,0,0,0,321,322,5,61,0,0,322,41,1,0,0,0,
  	323,324,5,15,0,0,324,325,5,68,0,0,325,326,5,68,0,0,326,331,5,60,0,0,327,
  	330,3,46,23,0,328,330,3,44,22,0,329,327,1,0,0,0,329,328,1,0,0,0,330,333,
  	1,0,0,0,331,329,1,0,0,0,331,332,1,0,0,0,332,334,1,0,0,0,333,331,1,0,0,
  	0,334,335,5,61,0,0,335,43,1,0,0,0,336,337,5,16,0,0,337,338,5,47,0,0,338,
  	339,3,48,24,0,339,340,5,49,0,0,340,367,1,0,0,0,341,342,5,17,0,0,342,343,
  	5,47,0,0,343,344,3,48,24,0,344,345,5,49,0,0,345,367,1,0,0,0,346,347,5,
  	18,0,0,347,348,5,47,0,0,348,349,3,48,24,0,349,350,5,49,0,0,350,367,1,
  	0,0,0,351,352,5,19,0,0,352,353,5,47,0,0,353,354,3,48,24,0,354,355,5,49,
  	0,0,355,367,1,0,0,0,356,357,5,20,0,0,357,358,5,47,0,0,358,359,3,48,24,
  	0,359,360,5,49,0,0,360,367,1,0,0,0,361,362,5,21,0,0,362,363,5,47,0,0,
  	363,364,3,48,24,0,364,365,5,49,0,0,365,367,1,0,0,0,366,336,1,0,0,0,366,
  	341,1,0,0,0,366,346,1,0,0,0,366,351,1,0,0,0,366,356,1,0,0,0,366,361,1,
  	0,0,0,367,45,1,0,0,0,368,369,5,23,0,0,369,370,5,47,0,0,370,371,3,18,9,
  	0,371,372,5,49,0,0,372,379,1,0,0,0,373,374,5,24,0,0,374,375,5,47,0,0,
  	375,376,3,18,9,0,376,377,5,49,0,0,377,379,1,0,0,0,378,368,1,0,0,0,378,
  	373,1,0,0,0,379,47,1,0,0,0,380,381,5,67,0,0,381,382,3,88,44,0,382,384,
  	5,58,0,0,383,385,3,22,11,0,384,383,1,0,0,0,384,385,1,0,0,0,385,386,1,
  	0,0,0,386,387,5,59,0,0,387,49,1,0,0,0,388,389,6,25,-1,0,389,390,5,30,
  	0,0,390,391,5,58,0,0,391,394,3,50,25,0,392,393,5,38,0,0,393,395,3,50,
  	25,0,394,392,1,0,0,0,395,396,1,0,0,0,396,394,1,0,0,0,396,397,1,0,0,0,
  	397,398,1,0,0,0,398,399,5,59,0,0,399,444,1,0,0,0,400,401,5,31,0,0,401,
  	402,5,58,0,0,402,405,3,50,25,0,403,404,5,38,0,0,404,406,3,50,25,0,405,
  	403,1,0,0,0,406,407,1,0,0,0,407,405,1,0,0,0,407,408,1,0,0,0,408,409,1,
  	0,0,0,409,410,5,59,0,0,410,444,1,0,0,0,411,412,5,32,0,0,412,413,5,64,
  	0,0,413,414,5,33,0,0,414,415,3,50,25,0,415,416,5,34,0,0,416,417,3,50,
  	25,6,417,444,1,0,0,0,418,419,5,29,0,0,419,420,5,64,0,0,420,421,5,64,0,
  	0,421,422,5,58,0,0,422,423,3,50,25,0,423,427,5,59,0,0,424,426,3,52,26,
  	0,425,424,1,0,0,0,426,429,1,0,0,0,427,425,1,0,0,0,427,428,1,0,0,0,428,
  	444,1,0,0,0,429,427,1,0,0,0,430,444,5,27,0,0,431,444,5,28,0,0,432,436,
  	3,54,27,0,433,435,3,52,26,0,434,433,1,0,0,0,435,438,1,0,0,0,436,434,1,
  	0,0,0,436,437,1,0,0,0,437,444,1,0,0,0,438,436,1,0,0,0,439,440,5,58,0,
  	0,440,441,3,50,25,0,441,442,5,59,0,0,442,444,1,0,0,0,443,388,1,0,0,0,
  	443,400,1,0,0,0,443,411,1,0,0,0,443,418,1,0,0,0,443,430,1,0,0,0,443,431,
  	1,0,0,0,443,432,1,0,0,0,443,439,1,0,0,0,444,454,1,0,0,0,445,448,10,9,
  	0,0,446,447,5,37,0,0,447,449,3,50,25,0,448,446,1,0,0,0,449,450,1,0,0,
  	0,450,448,1,0,0,0,450,451,1,0,0,0,451,453,1,0,0,0,452,445,1,0,0,0,453,
  	456,1,0,0,0,454,452,1,0,0,0,454,455,1,0,0,0,455,51,1,0,0,0,456,454,1,
  	0,0,0,457,458,5,22,0,0,458,459,5,19,0,0,459,468,3,50,25,0,460,461,5,22,
  	0,0,461,462,5,18,0,0,462,468,3,50,25,0,463,464,5,22,0,0,464,465,3,54,
  	27,0,465,466,3,50,25,0,466,468,1,0,0,0,467,457,1,0,0,0,467,460,1,0,0,
  	0,467,463,1,0,0,0,468,53,1,0,0,0,469,470,3,88,44,0,470,471,5,50,0,0,471,
  	472,3,88,44,0,472,474,5,58,0,0,473,475,3,56,28,0,474,473,1,0,0,0,474,
  	475,1,0,0,0,475,476,1,0,0,0,476,477,5,59,0,0,477,486,1,0,0,0,478,479,
  	3,88,44,0,479,481,5,58,0,0,480,482,3,56,28,0,481,480,1,0,0,0,481,482,
  	1,0,0,0,482,483,1,0,0,0,483,484,5,59,0,0,484,486,1,0,0,0,485,469,1,0,
  	0,0,485,478,1,0,0,0,486,55,1,0,0,0,487,492,3,58,29,0,488,489,5,48,0,0,
  	489,491,3,58,29,0,490,488,1,0,0,0,491,494,1,0,0,0,492,490,1,0,0,0,492,
  	493,1,0,0,0,493,57,1,0,0,0,494,492,1,0,0,0,495,496,3,60,30,0,496,59,1,
  	0,0,0,497,502,3,62,31,0,498,499,5,57,0,0,499,501,3,62,31,0,500,498,1,
  	0,0,0,501,504,1,0,0,0,502,500,1,0,0,0,502,503,1,0,0,0,503,61,1,0,0,0,
  	504,502,1,0,0,0,505,510,3,64,32,0,506,507,5,56,0,0,507,509,3,64,32,0,
  	508,506,1,0,0,0,509,512,1,0,0,0,510,508,1,0,0,0,510,511,1,0,0,0,511,63,
  	1,0,0,0,512,510,1,0,0,0,513,517,3,68,34,0,514,515,3,66,33,0,515,516,3,
  	68,34,0,516,518,1,0,0,0,517,514,1,0,0,0,517,518,1,0,0,0,518,65,1,0,0,
  	0,519,520,7,0,0,0,520,67,1,0,0,0,521,522,5,55,0,0,522,525,3,68,34,0,523,
  	525,3,70,35,0,524,521,1,0,0,0,524,523,1,0,0,0,525,69,1,0,0,0,526,531,
  	3,72,36,0,527,528,7,1,0,0,528,530,3,72,36,0,529,527,1,0,0,0,530,533,1,
  	0,0,0,531,529,1,0,0,0,531,532,1,0,0,0,532,71,1,0,0,0,533,531,1,0,0,0,
  	534,539,3,74,37,0,535,536,7,2,0,0,536,538,3,74,37,0,537,535,1,0,0,0,538,
  	541,1,0,0,0,539,537,1,0,0,0,539,540,1,0,0,0,540,73,1,0,0,0,541,539,1,
  	0,0,0,542,543,5,52,0,0,543,546,3,74,37,0,544,546,3,76,38,0,545,542,1,
  	0,0,0,545,544,1,0,0,0,546,75,1,0,0,0,547,561,3,54,27,0,548,561,5,66,0,
  	0,549,561,5,67,0,0,550,561,5,68,0,0,551,561,5,64,0,0,552,561,5,65,0,0,
  	553,561,3,78,39,0,554,561,3,82,41,0,555,561,3,84,42,0,556,557,5,58,0,
  	0,557,558,3,58,29,0,558,559,5,59,0,0,559,561,1,0,0,0,560,547,1,0,0,0,
  	560,548,1,0,0,0,560,549,1,0,0,0,560,550,1,0,0,0,560,551,1,0,0,0,560,552,
  	1,0,0,0,560,553,1,0,0,0,560,554,1,0,0,0,560,555,1,0,0,0,560,556,1,0,0,
  	0,561,77,1,0,0,0,562,571,5,60,0,0,563,568,3,80,40,0,564,565,5,48,0,0,
  	565,567,3,80,40,0,566,564,1,0,0,0,567,570,1,0,0,0,568,566,1,0,0,0,568,
  	569,1,0,0,0,569,572,1,0,0,0,570,568,1,0,0,0,571,563,1,0,0,0,571,572,1,
  	0,0,0,572,573,1,0,0,0,573,574,5,61,0,0,574,79,1,0,0,0,575,576,5,67,0,
  	0,576,577,5,47,0,0,577,578,3,58,29,0,578,579,5,49,0,0,579,81,1,0,0,0,
  	580,589,5,62,0,0,581,586,3,58,29,0,582,583,5,48,0,0,583,585,3,58,29,0,
  	584,582,1,0,0,0,585,588,1,0,0,0,586,584,1,0,0,0,586,587,1,0,0,0,587,590,
  	1,0,0,0,588,586,1,0,0,0,589,581,1,0,0,0,589,590,1,0,0,0,590,591,1,0,0,
  	0,591,592,5,63,0,0,592,83,1,0,0,0,593,602,5,60,0,0,594,599,3,86,43,0,
  	595,596,5,48,0,0,596,598,3,86,43,0,597,595,1,0,0,0,598,601,1,0,0,0,599,
  	597,1,0,0,0,599,600,1,0,0,0,600,603,1,0,0,0,601,599,1,0,0,0,602,594,1,
  	0,0,0,602,603,1,0,0,0,603,604,1,0,0,0,604,605,5,61,0,0,605,85,1,0,0,0,
  	606,607,3,58,29,0,607,608,5,47,0,0,608,609,3,58,29,0,609,87,1,0,0,0,610,
  	611,7,3,0,0,611,89,1,0,0,0,60,93,102,108,115,123,130,134,140,146,157,
  	168,174,182,188,191,216,223,231,243,251,258,265,268,279,287,303,305,316,
  	318,329,331,366,378,384,396,407,427,436,443,450,454,467,474,481,485,492,
  	502,510,517,524,531,539,545,560,568,571,586,589,599,602
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
    setState(93);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TASK)
      | (1ULL << KodaParser::CAPABILITY)
      | (1ULL << KodaParser::TYPE)
      | (1ULL << KodaParser::ENUM)
      | (1ULL << KodaParser::MAPPING))) != 0)) {
      setState(90);
      topLevelDeclaration();
      setState(95);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(96);
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
    setState(102);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK:
      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 1);
        setState(98);
        topLevelComponent();
        break;
      }

      case KodaParser::TYPE: {
        enterOuterAlt(_localctx, 2);
        setState(99);
        typeDeclaration();
        break;
      }

      case KodaParser::ENUM: {
        enterOuterAlt(_localctx, 3);
        setState(100);
        enumDeclaration();
        break;
      }

      case KodaParser::MAPPING: {
        enterOuterAlt(_localctx, 4);
        setState(101);
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
    setState(134);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(104);
        match(KodaParser::TASK);
        setState(105);
        match(KodaParser::IDENT);
        setState(106);
        match(KodaParser::LPAREN);
        setState(108);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 6) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
          | (1ULL << (KodaParser::OPTIONAL - 6))
          | (1ULL << (KodaParser::MAP - 6))
          | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
          setState(107);
          argumentList();
        }
        setState(110);
        match(KodaParser::RPAREN);
        setState(111);
        match(KodaParser::LBRACE);
        setState(115);
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
          setState(112);
          statement();
          setState(117);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(118);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(119);
        match(KodaParser::CAPABILITY);
        setState(120);
        match(KodaParser::IDENT);
        setState(121);
        match(KodaParser::LPAREN);
        setState(123);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 6) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
          | (1ULL << (KodaParser::OPTIONAL - 6))
          | (1ULL << (KodaParser::MAP - 6))
          | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
          setState(122);
          argumentList();
        }
        setState(125);
        match(KodaParser::RPAREN);
        setState(126);
        match(KodaParser::LBRACE);
        setState(130);
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
          setState(127);
          statement();
          setState(132);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(133);
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
    setState(157);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::TypeRecordContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(136);
      match(KodaParser::TYPE);
      setState(137);
      qualifiedName();
      setState(140);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::EXTENDS) {
        setState(138);
        match(KodaParser::EXTENDS);
        setState(139);
        qualifiedName();
      }
      setState(142);
      match(KodaParser::LBRACE);
      setState(146);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::IDENT) {
        setState(143);
        fieldDeclaration();
        setState(148);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(149);
      match(KodaParser::RBRACE);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::TypeAliasContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(151);
      match(KodaParser::TYPE);
      setState(152);
      qualifiedName();
      setState(153);
      match(KodaParser::ASSIGN);
      setState(154);
      typeReference();
      setState(155);
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
    setState(159);
    match(KodaParser::IDENT);
    setState(160);
    match(KodaParser::COLON);
    setState(161);
    typeReference();
    setState(162);
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
    setState(164);
    match(KodaParser::ENUM);
    setState(165);
    qualifiedName();
    setState(168);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(166);
      match(KodaParser::COLON);
      setState(167);
      typeReference();
    }
    setState(170);
    match(KodaParser::LBRACE);
    setState(174);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(171);
      enumValue();
      setState(176);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(177);
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
    setState(179);
    match(KodaParser::IDENT);
    setState(182);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(180);
      match(KodaParser::ASSIGN);
      setState(181);
      enumLiteral();
    }
    setState(184);
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
    setState(191);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(186);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::MINUS:
      case KodaParser::NATURAL: {
        enterOuterAlt(_localctx, 2);
        setState(188);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::MINUS) {
          setState(187);
          match(KodaParser::MINUS);
        }
        setState(190);
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
    setState(193);
    match(KodaParser::MAPPING);
    setState(194);
    typeReference();
    setState(195);
    match(KodaParser::TO);
    setState(196);
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
    setState(216);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IDENT: {
        _localctx = _tracker.createInstance<KodaParser::TypeNamedContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(198);
        qualifiedName();
        break;
      }

      case KodaParser::LIST: {
        _localctx = _tracker.createInstance<KodaParser::TypeListContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(199);
        match(KodaParser::LIST);
        setState(200);
        match(KodaParser::LT);
        setState(201);
        typeReference();
        setState(202);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::OPTIONAL: {
        _localctx = _tracker.createInstance<KodaParser::TypeOptionalContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(204);
        match(KodaParser::OPTIONAL);
        setState(205);
        match(KodaParser::LT);
        setState(206);
        typeReference();
        setState(207);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::MAP: {
        _localctx = _tracker.createInstance<KodaParser::TypeMapContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(209);
        match(KodaParser::MAP);
        setState(210);
        match(KodaParser::LT);
        setState(211);
        typeReference();
        setState(212);
        match(KodaParser::COMMA);
        setState(213);
        typeReference();
        setState(214);
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
    setState(218);
    match(KodaParser::IDENT);
    setState(223);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::DOUBLE_COLON) {
      setState(219);
      match(KodaParser::DOUBLE_COLON);
      setState(220);
      match(KodaParser::IDENT);
      setState(225);
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
    setState(226);
    argument();
    setState(231);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(227);
      match(KodaParser::COMMA);
      setState(228);
      argument();
      setState(233);
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
    setState(243);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(234);
      typeReference();
      setState(235);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(237);
      match(KodaParser::IDENT);
      setState(238);
      match(KodaParser::REQ);
      setState(239);
      typeReference();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(240);
      match(KodaParser::IDENT);
      setState(241);
      match(KodaParser::PRO);
      setState(242);
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
    setState(251);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(245);
        tasksBlock();
        break;
      }

      case KodaParser::PARAMETERS: {
        enterOuterAlt(_localctx, 2);
        setState(246);
        varsBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 3);
        setState(247);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 4);
        setState(248);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 5);
        setState(249);
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
        setState(250);
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
    setState(253);
    match(KodaParser::STRATEGY);
    setState(254);
    match(KodaParser::LBRACE);
    setState(256); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(255);
      flow();
      setState(258); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(260);
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
    setState(262);
    match(KodaParser::IDENT);
    setState(268);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(263);
      match(KodaParser::LBRACK);
      setState(265);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(264);
        identList();
      }
      setState(267);
      match(KodaParser::RBRACK);
    }
    setState(270);
    match(KodaParser::COLON);
    setState(271);
    strategy(0);
    setState(272);
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
    setState(274);
    match(KodaParser::IDENT);
    setState(279);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(275);
      match(KodaParser::COMMA);
      setState(276);
      match(KodaParser::IDENT);
      setState(281);
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
    setState(282);
    match(KodaParser::PARAMETERS);
    setState(283);
    match(KodaParser::LBRACE);
    setState(285); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(284);
      variableStatement();
      setState(287); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((((_la - 6) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
      | (1ULL << (KodaParser::OPTIONAL - 6))
      | (1ULL << (KodaParser::MAP - 6))
      | (1ULL << (KodaParser::IDENT - 6)))) != 0));
    setState(289);
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
    setState(291);
    typeReference();
    setState(292);
    match(KodaParser::IDENT);
    setState(293);
    match(KodaParser::ASSIGN);
    setState(294);
    expression();
    setState(295);
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
    setState(297);
    match(KodaParser::ACTION);
    setState(298);
    match(KodaParser::STRING);
    setState(299);
    match(KodaParser::STRING);
    setState(300);
    match(KodaParser::LBRACE);
    setState(305);
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
      setState(303);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(301);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(302);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(307);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(308);
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
    setState(310);
    match(KodaParser::SERVICE);
    setState(311);
    match(KodaParser::STRING);
    setState(312);
    match(KodaParser::STRING);
    setState(313);
    match(KodaParser::LBRACE);
    setState(318);
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
      setState(316);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(314);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(315);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(320);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(321);
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
    setState(323);
    match(KodaParser::TOPIC);
    setState(324);
    match(KodaParser::STRING);
    setState(325);
    match(KodaParser::STRING);
    setState(326);
    match(KodaParser::LBRACE);
    setState(331);
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
      setState(329);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(327);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(328);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(333);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(334);
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
    setState(366);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(336);
        match(KodaParser::TRIGGER);
        setState(337);
        match(KodaParser::COLON);
        setState(338);
        eventDefStatement();
        setState(339);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(341);
        match(KodaParser::RETURN);
        setState(342);
        match(KodaParser::COLON);
        setState(343);
        eventDefStatement();
        setState(344);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(346);
        match(KodaParser::ABORT);
        setState(347);
        match(KodaParser::COLON);
        setState(348);
        eventDefStatement();
        setState(349);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(351);
        match(KodaParser::ERROR);
        setState(352);
        match(KodaParser::COLON);
        setState(353);
        eventDefStatement();
        setState(354);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(356);
        match(KodaParser::IN);
        setState(357);
        match(KodaParser::COLON);
        setState(358);
        eventDefStatement();
        setState(359);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(361);
        match(KodaParser::OUT);
        setState(362);
        match(KodaParser::COLON);
        setState(363);
        eventDefStatement();
        setState(364);
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
    setState(378);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::CONSUMES: {
        enterOuterAlt(_localctx, 1);
        setState(368);
        match(KodaParser::CONSUMES);
        setState(369);
        match(KodaParser::COLON);
        setState(370);
        typeReference();
        setState(371);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::PRODUCES: {
        enterOuterAlt(_localctx, 2);
        setState(373);
        match(KodaParser::PRODUCES);
        setState(374);
        match(KodaParser::COLON);
        setState(375);
        typeReference();
        setState(376);
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
    setState(380);
    match(KodaParser::IDENT);
    setState(381);
    identifier();
    setState(382);
    match(KodaParser::LPAREN);
    setState(384);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 6) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 6)) & ((1ULL << (KodaParser::LIST - 6))
      | (1ULL << (KodaParser::OPTIONAL - 6))
      | (1ULL << (KodaParser::MAP - 6))
      | (1ULL << (KodaParser::IDENT - 6)))) != 0)) {
      setState(383);
      argumentList();
    }
    setState(386);
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
    setState(443);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::JOIN: {
        _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(389);
        match(KodaParser::JOIN);
        setState(390);
        match(KodaParser::LPAREN);
        setState(391);
        strategy(0);
        setState(394); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(392);
          match(KodaParser::PIPE);
          setState(393);
          strategy(0);
          setState(396); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(398);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::EITHER: {
        _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(400);
        match(KodaParser::EITHER);
        setState(401);
        match(KodaParser::LPAREN);
        setState(402);
        strategy(0);
        setState(405); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(403);
          match(KodaParser::PIPE);
          setState(404);
          strategy(0);
          setState(407); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(409);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::WITHIN: {
        _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(411);
        match(KodaParser::WITHIN);
        setState(412);
        match(KodaParser::NATURAL);
        setState(413);
        match(KodaParser::DO);
        setState(414);
        strategy(0);
        setState(415);
        match(KodaParser::ELSE);
        setState(416);
        strategy(6);
        break;
      }

      case KodaParser::REPEAT: {
        _localctx = _tracker.createInstance<StratRepeatContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(418);
        match(KodaParser::REPEAT);
        setState(419);
        match(KodaParser::NATURAL);
        setState(420);
        match(KodaParser::NATURAL);
        setState(421);
        match(KodaParser::LPAREN);
        setState(422);
        strategy(0);
        setState(423);
        match(KodaParser::RPAREN);
        setState(427);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(424);
            strategyHandler(); 
          }
          setState(429);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
        }
        break;
      }

      case KodaParser::END: {
        _localctx = _tracker.createInstance<StratEndContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(430);
        match(KodaParser::END);
        break;
      }

      case KodaParser::CONTINUE: {
        _localctx = _tracker.createInstance<StratContinueContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(431);
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
        setState(432);
        eventStatement();
        setState(436);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(433);
            strategyHandler(); 
          }
          setState(438);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
        }
        break;
      }

      case KodaParser::LPAREN: {
        _localctx = _tracker.createInstance<StratParenContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(439);
        match(KodaParser::LPAREN);
        setState(440);
        strategy(0);
        setState(441);
        match(KodaParser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(454);
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
        setState(445);

        if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
        setState(448); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(446);
                  match(KodaParser::ARROW);
                  setState(447);
                  strategy(0);
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(450); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER); 
      }
      setState(456);
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
    setState(467);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(457);
      match(KodaParser::ON);
      setState(458);
      match(KodaParser::ERROR);
      setState(459);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(460);
      match(KodaParser::ON);
      setState(461);
      match(KodaParser::ABORT);
      setState(462);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(463);
      match(KodaParser::ON);
      setState(464);
      eventStatement();
      setState(465);
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
    setState(485);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(469);
      identifier();
      setState(470);
      match(KodaParser::DOT);
      setState(471);
      identifier();
      setState(472);
      match(KodaParser::LPAREN);
      setState(474);
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
        | (1ULL << (KodaParser::LBRACK - 16))
        | (1ULL << (KodaParser::NATURAL - 16))
        | (1ULL << (KodaParser::REAL - 16))
        | (1ULL << (KodaParser::BOOLEAN - 16))
        | (1ULL << (KodaParser::IDENT - 16))
        | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
        setState(473);
        exprList();
      }
      setState(476);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(478);
      identifier();
      setState(479);
      match(KodaParser::LPAREN);
      setState(481);
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
        | (1ULL << (KodaParser::LBRACK - 16))
        | (1ULL << (KodaParser::NATURAL - 16))
        | (1ULL << (KodaParser::REAL - 16))
        | (1ULL << (KodaParser::BOOLEAN - 16))
        | (1ULL << (KodaParser::IDENT - 16))
        | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
        setState(480);
        exprList();
      }
      setState(483);
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
    setState(487);
    expression();
    setState(492);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(488);
      match(KodaParser::COMMA);
      setState(489);
      expression();
      setState(494);
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
    setState(495);
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
    setState(497);
    exprAnd();
    setState(502);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(498);
      match(KodaParser::OR);
      setState(499);
      exprAnd();
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
    setState(505);
    exprCmp();
    setState(510);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(506);
      match(KodaParser::AND);
      setState(507);
      exprCmp();
      setState(512);
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
    setState(513);
    exprNot();
    setState(517);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::EQ)
      | (1ULL << KodaParser::NEQ)
      | (1ULL << KodaParser::LEQ)
      | (1ULL << KodaParser::GEQ)
      | (1ULL << KodaParser::LT)
      | (1ULL << KodaParser::GT))) != 0)) {
      setState(514);
      compOp();
      setState(515);
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
    setState(519);
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
    setState(524);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(521);
        match(KodaParser::NOT);
        setState(522);
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
      case KodaParser::LBRACK:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::BOOLEAN:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(523);
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
    setState(526);
    exprMul();
    setState(531);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(527);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(528);
      exprMul();
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
    setState(534);
    exprUnary();
    setState(539);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(535);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(536);
      exprUnary();
      setState(541);
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
    setState(545);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(542);
        match(KodaParser::MINUS);
        setState(543);
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
      case KodaParser::LBRACK:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::BOOLEAN:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(544);
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
//----------------- ExprMapLiteralContext ------------------------------------------------------------------

KodaParser::MapLiteralContext* KodaParser::ExprMapLiteralContext::mapLiteral() {
  return getRuleContext<KodaParser::MapLiteralContext>(0);
}

KodaParser::ExprMapLiteralContext::ExprMapLiteralContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprMapLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprMapLiteral(this);
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
//----------------- ExprListLiteralContext ------------------------------------------------------------------

KodaParser::ListLiteralContext* KodaParser::ExprListLiteralContext::listLiteral() {
  return getRuleContext<KodaParser::ListLiteralContext>(0);
}

KodaParser::ExprListLiteralContext::ExprListLiteralContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprListLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprListLiteral(this);
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
    setState(560);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(547);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprBooleanContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(548);
      match(KodaParser::BOOLEAN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(549);
      match(KodaParser::IDENT);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(550);
      match(KodaParser::STRING);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(551);
      match(KodaParser::NATURAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(552);
      match(KodaParser::REAL);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<KodaParser::ExprRecordContext>(_localctx);
      enterOuterAlt(_localctx, 7);
      setState(553);
      recordLiteral();
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<KodaParser::ExprListLiteralContext>(_localctx);
      enterOuterAlt(_localctx, 8);
      setState(554);
      listLiteral();
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<KodaParser::ExprMapLiteralContext>(_localctx);
      enterOuterAlt(_localctx, 9);
      setState(555);
      mapLiteral();
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 10);
      setState(556);
      match(KodaParser::LPAREN);
      setState(557);
      expression();
      setState(558);
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

std::vector<tree::TerminalNode *> KodaParser::RecordLiteralContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::RecordLiteralContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
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
    setState(562);
    match(KodaParser::LBRACE);
    setState(571);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::IDENT) {
      setState(563);
      recordFieldInitializer();
      setState(568);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(564);
        match(KodaParser::COMMA);
        setState(565);
        recordFieldInitializer();
        setState(570);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(573);
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
    setState(575);
    match(KodaParser::IDENT);
    setState(576);
    match(KodaParser::COLON);
    setState(577);
    expression();
    setState(578);
    match(KodaParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ListLiteralContext ------------------------------------------------------------------

KodaParser::ListLiteralContext::ListLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::ListLiteralContext::LBRACK() {
  return getToken(KodaParser::LBRACK, 0);
}

tree::TerminalNode* KodaParser::ListLiteralContext::RBRACK() {
  return getToken(KodaParser::RBRACK, 0);
}

std::vector<KodaParser::ExpressionContext *> KodaParser::ListLiteralContext::expression() {
  return getRuleContexts<KodaParser::ExpressionContext>();
}

KodaParser::ExpressionContext* KodaParser::ListLiteralContext::expression(size_t i) {
  return getRuleContext<KodaParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::ListLiteralContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::ListLiteralContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::ListLiteralContext::getRuleIndex() const {
  return KodaParser::RuleListLiteral;
}


std::any KodaParser::ListLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitListLiteral(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::ListLiteralContext* KodaParser::listLiteral() {
  ListLiteralContext *_localctx = _tracker.createInstance<ListLiteralContext>(_ctx, getState());
  enterRule(_localctx, 82, KodaParser::RuleListLiteral);
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
    setState(580);
    match(KodaParser::LBRACK);
    setState(589);
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
      | (1ULL << (KodaParser::LBRACK - 16))
      | (1ULL << (KodaParser::NATURAL - 16))
      | (1ULL << (KodaParser::REAL - 16))
      | (1ULL << (KodaParser::BOOLEAN - 16))
      | (1ULL << (KodaParser::IDENT - 16))
      | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
      setState(581);
      expression();
      setState(586);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(582);
        match(KodaParser::COMMA);
        setState(583);
        expression();
        setState(588);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(591);
    match(KodaParser::RBRACK);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MapLiteralContext ------------------------------------------------------------------

KodaParser::MapLiteralContext::MapLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::MapLiteralContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::MapLiteralContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::MapFieldInitializerContext *> KodaParser::MapLiteralContext::mapFieldInitializer() {
  return getRuleContexts<KodaParser::MapFieldInitializerContext>();
}

KodaParser::MapFieldInitializerContext* KodaParser::MapLiteralContext::mapFieldInitializer(size_t i) {
  return getRuleContext<KodaParser::MapFieldInitializerContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::MapLiteralContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::MapLiteralContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::MapLiteralContext::getRuleIndex() const {
  return KodaParser::RuleMapLiteral;
}


std::any KodaParser::MapLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitMapLiteral(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::MapLiteralContext* KodaParser::mapLiteral() {
  MapLiteralContext *_localctx = _tracker.createInstance<MapLiteralContext>(_ctx, getState());
  enterRule(_localctx, 84, KodaParser::RuleMapLiteral);
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
    setState(593);
    match(KodaParser::LBRACE);
    setState(602);
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
      | (1ULL << (KodaParser::LBRACK - 16))
      | (1ULL << (KodaParser::NATURAL - 16))
      | (1ULL << (KodaParser::REAL - 16))
      | (1ULL << (KodaParser::BOOLEAN - 16))
      | (1ULL << (KodaParser::IDENT - 16))
      | (1ULL << (KodaParser::STRING - 16)))) != 0)) {
      setState(594);
      mapFieldInitializer();
      setState(599);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(595);
        match(KodaParser::COMMA);
        setState(596);
        mapFieldInitializer();
        setState(601);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(604);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MapFieldInitializerContext ------------------------------------------------------------------

KodaParser::MapFieldInitializerContext::MapFieldInitializerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::ExpressionContext *> KodaParser::MapFieldInitializerContext::expression() {
  return getRuleContexts<KodaParser::ExpressionContext>();
}

KodaParser::ExpressionContext* KodaParser::MapFieldInitializerContext::expression(size_t i) {
  return getRuleContext<KodaParser::ExpressionContext>(i);
}

tree::TerminalNode* KodaParser::MapFieldInitializerContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}


size_t KodaParser::MapFieldInitializerContext::getRuleIndex() const {
  return KodaParser::RuleMapFieldInitializer;
}


std::any KodaParser::MapFieldInitializerContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitMapFieldInitializer(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::MapFieldInitializerContext* KodaParser::mapFieldInitializer() {
  MapFieldInitializerContext *_localctx = _tracker.createInstance<MapFieldInitializerContext>(_ctx, getState());
  enterRule(_localctx, 86, KodaParser::RuleMapFieldInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(606);
    expression();
    setState(607);
    match(KodaParser::COLON);
    setState(608);
    expression();
   
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
  enterRule(_localctx, 88, KodaParser::RuleIdentifier);
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
    setState(610);
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
