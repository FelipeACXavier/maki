
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
      "fieldDeclaration", "annotationDeclaration", "annotation", "enumDeclaration", 
      "enumValue", "enumLiteral", "mappingDeclaration", "typeReference", 
      "qualifiedName", "argumentList", "argument", "statement", "tasksBlock", 
      "flow", "identList", "varsBlock", "variableStatement", "actionBlock", 
      "serviceBlock", "topicBlock", "rosDefStatement", "reqDefStatement", 
      "eventDefStatement", "dataBlock", "strategy", "chooseWhenStatement", 
      "strategyHandler", "eventStatement", "exprList", "expression", "exprOr", 
      "exprAnd", "exprCmp", "compOp", "exprNot", "exprAdd", "exprMul", "exprUnary", 
      "exprPrimary", "recordLiteral", "recordFieldInitializer", "listLiteral", 
      "mapLiteral", "mapFieldInitializer", "identifier"
    },
    std::vector<std::string>{
      "", "'task'", "'capability'", "'type'", "'enum'", "'extends'", "'list'", 
      "'optional'", "'map'", "'mapping'", "'to'", "'annotations'", "'strategy'", 
      "'parameters'", "'action'", "'service'", "'topic'", "'trigger'", "'return'", 
      "'abort'", "'error'", "'in'", "'out'", "'on'", "'consumes'", "'produces'", 
      "'data'", "'req'", "'pro'", "'success'", "'failure'", "'continue'", 
      "'repeat'", "'join'", "'either'", "'when'", "'choose'", "'within'", 
      "'do'", "'else'", "'then'", "'every'", "'-->'", "'|'", "'=='", "'!='", 
      "'<='", "'>='", "'<'", "'>'", "'='", "'::'", "':'", "','", "';'", 
      "'.'", "'+'", "'-'", "'*'", "'/'", "'!'", "'and'", "'or'", "'('", 
      "')'", "'{'", "'}'", "'['", "']'"
    },
    std::vector<std::string>{
      "", "TASK", "CAPABILITY", "TYPE", "ENUM", "EXTENDS", "LIST", "OPTIONAL", 
      "MAP", "MAPPING", "TO", "ANNOTATIONS", "STRATEGY", "PARAMETERS", "ACTION", 
      "SERVICE", "TOPIC", "TRIGGER", "RETURN", "ABORT", "ERROR", "IN", "OUT", 
      "ON", "CONSUMES", "PRODUCES", "DATA", "REQ", "PRO", "SUCCESS", "FAILURE", 
      "CONTINUE", "REPEAT", "JOIN", "EITHER", "WHEN", "CHOOSE", "WITHIN", 
      "DO", "ELSE", "THEN", "EVERY", "ARROW", "PIPE", "EQ", "NEQ", "LEQ", 
      "GEQ", "LT", "GT", "ASSIGN", "DOUBLE_COLON", "COLON", "COMMA", "SEMI", 
      "DOT", "PLUS", "MINUS", "STAR", "SLASH", "NOT", "AND", "OR", "LPAREN", 
      "RPAREN", "LBRACE", "RBRACE", "LBRACK", "RBRACK", "NATURAL", "REAL", 
      "BOOLEAN", "IDENT", "STRING", "ANY", "LINE_COMMENT", "BLOCK_COMMENT", 
      "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,77,677,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,1,0,5,
  	0,100,8,0,10,0,12,0,103,9,0,1,0,1,0,1,1,1,1,1,1,1,1,3,1,111,8,1,1,2,1,
  	2,1,2,1,2,3,2,117,8,2,1,2,1,2,1,2,5,2,122,8,2,10,2,12,2,125,9,2,1,2,1,
  	2,1,2,1,2,1,2,3,2,132,8,2,1,2,1,2,1,2,5,2,137,8,2,10,2,12,2,140,9,2,1,
  	2,3,2,143,8,2,1,3,1,3,1,3,1,3,3,3,149,8,3,1,3,1,3,5,3,153,8,3,10,3,12,
  	3,156,9,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,166,8,3,1,4,1,4,1,4,1,4,
  	1,4,1,4,3,4,174,8,4,1,5,1,5,1,5,1,5,1,5,5,5,181,8,5,10,5,12,5,184,9,5,
  	3,5,186,8,5,1,5,1,5,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,3,7,198,8,7,1,7,1,
  	7,5,7,202,8,7,10,7,12,7,205,9,7,1,7,1,7,1,8,1,8,1,8,3,8,212,8,8,1,8,1,
  	8,1,9,1,9,3,9,218,8,9,1,9,3,9,221,8,9,1,10,1,10,1,10,1,10,1,10,1,11,1,
  	11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,
  	11,1,11,1,11,3,11,246,8,11,1,12,1,12,1,12,5,12,251,8,12,10,12,12,12,254,
  	9,12,1,13,1,13,1,13,5,13,259,8,13,10,13,12,13,262,9,13,1,14,1,14,1,14,
  	1,14,1,14,1,14,1,14,1,14,1,14,3,14,273,8,14,1,15,1,15,1,15,1,15,1,15,
  	1,15,1,15,3,15,282,8,15,1,16,1,16,1,16,4,16,287,8,16,11,16,12,16,288,
  	1,16,1,16,1,17,1,17,1,17,3,17,296,8,17,1,17,3,17,299,8,17,1,17,1,17,1,
  	17,1,17,1,18,1,18,1,18,5,18,308,8,18,10,18,12,18,311,9,18,1,19,1,19,1,
  	19,4,19,316,8,19,11,19,12,19,317,1,19,1,19,1,20,1,20,1,20,1,20,3,20,326,
  	8,20,1,20,1,20,1,21,1,21,1,21,1,21,1,21,1,21,5,21,336,8,21,10,21,12,21,
  	339,9,21,1,21,1,21,1,22,1,22,1,22,1,22,1,22,1,22,5,22,349,8,22,10,22,
  	12,22,352,9,22,1,22,1,22,1,23,1,23,1,23,1,23,1,23,1,23,5,23,362,8,23,
  	10,23,12,23,365,9,23,1,23,1,23,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,3,24,399,8,24,1,25,1,25,1,25,
  	1,25,1,25,1,25,1,25,1,25,1,25,1,25,3,25,411,8,25,1,26,1,26,1,26,1,26,
  	3,26,417,8,26,1,26,1,26,1,27,1,27,1,27,5,27,424,8,27,10,27,12,27,427,
  	9,27,1,27,1,27,1,28,1,28,1,28,1,28,1,28,1,28,4,28,437,8,28,11,28,12,28,
  	438,1,28,1,28,1,28,1,28,1,28,1,28,1,28,4,28,448,8,28,11,28,12,28,449,
  	1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,
  	1,28,1,28,5,28,468,8,28,10,28,12,28,471,9,28,1,28,1,28,1,28,1,28,1,28,
  	5,28,478,8,28,10,28,12,28,481,9,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,
  	5,28,490,8,28,10,28,12,28,493,9,28,1,28,3,28,496,8,28,1,28,1,28,1,28,
  	4,28,501,8,28,11,28,12,28,502,5,28,505,8,28,10,28,12,28,508,9,28,1,29,
  	1,29,3,29,512,8,29,1,29,1,29,1,29,1,29,1,30,1,30,1,30,1,30,1,30,1,30,
  	1,30,1,30,1,30,1,30,3,30,528,8,30,1,31,1,31,1,31,1,31,1,31,3,31,535,8,
  	31,1,31,1,31,1,31,1,31,1,31,3,31,542,8,31,1,31,1,31,3,31,546,8,31,1,32,
  	1,32,1,32,5,32,551,8,32,10,32,12,32,554,9,32,1,33,1,33,1,34,1,34,1,34,
  	5,34,561,8,34,10,34,12,34,564,9,34,1,35,1,35,1,35,5,35,569,8,35,10,35,
  	12,35,572,9,35,1,36,1,36,1,36,1,36,3,36,578,8,36,1,37,1,37,1,38,1,38,
  	1,38,3,38,585,8,38,1,39,1,39,1,39,5,39,590,8,39,10,39,12,39,593,9,39,
  	1,40,1,40,1,40,5,40,598,8,40,10,40,12,40,601,9,40,1,41,1,41,1,41,3,41,
  	606,8,41,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,
  	1,42,1,42,1,42,1,42,3,42,624,8,42,1,43,1,43,1,43,1,43,5,43,630,8,43,10,
  	43,12,43,633,9,43,3,43,635,8,43,1,43,1,43,1,44,1,44,1,44,1,44,1,45,1,
  	45,1,45,1,45,5,45,647,8,45,10,45,12,45,650,9,45,3,45,652,8,45,1,45,1,
  	45,1,46,1,46,1,46,1,46,5,46,660,8,46,10,46,12,46,663,9,46,3,46,665,8,
  	46,1,46,1,46,1,47,1,47,1,47,1,47,1,47,1,47,1,48,1,48,1,48,0,1,56,49,0,
  	2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,
  	52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,
  	0,4,1,0,44,49,1,0,56,57,1,0,58,59,3,0,17,22,24,25,72,72,726,0,101,1,0,
  	0,0,2,110,1,0,0,0,4,142,1,0,0,0,6,165,1,0,0,0,8,173,1,0,0,0,10,175,1,
  	0,0,0,12,189,1,0,0,0,14,193,1,0,0,0,16,208,1,0,0,0,18,220,1,0,0,0,20,
  	222,1,0,0,0,22,245,1,0,0,0,24,247,1,0,0,0,26,255,1,0,0,0,28,272,1,0,0,
  	0,30,281,1,0,0,0,32,283,1,0,0,0,34,292,1,0,0,0,36,304,1,0,0,0,38,312,
  	1,0,0,0,40,321,1,0,0,0,42,329,1,0,0,0,44,342,1,0,0,0,46,355,1,0,0,0,48,
  	398,1,0,0,0,50,410,1,0,0,0,52,412,1,0,0,0,54,420,1,0,0,0,56,495,1,0,0,
  	0,58,509,1,0,0,0,60,527,1,0,0,0,62,545,1,0,0,0,64,547,1,0,0,0,66,555,
  	1,0,0,0,68,557,1,0,0,0,70,565,1,0,0,0,72,573,1,0,0,0,74,579,1,0,0,0,76,
  	584,1,0,0,0,78,586,1,0,0,0,80,594,1,0,0,0,82,605,1,0,0,0,84,623,1,0,0,
  	0,86,625,1,0,0,0,88,638,1,0,0,0,90,642,1,0,0,0,92,655,1,0,0,0,94,668,
  	1,0,0,0,96,674,1,0,0,0,98,100,3,2,1,0,99,98,1,0,0,0,100,103,1,0,0,0,101,
  	99,1,0,0,0,101,102,1,0,0,0,102,104,1,0,0,0,103,101,1,0,0,0,104,105,5,
  	0,0,1,105,1,1,0,0,0,106,111,3,4,2,0,107,111,3,6,3,0,108,111,3,14,7,0,
  	109,111,3,20,10,0,110,106,1,0,0,0,110,107,1,0,0,0,110,108,1,0,0,0,110,
  	109,1,0,0,0,111,3,1,0,0,0,112,113,5,1,0,0,113,114,5,72,0,0,114,116,5,
  	63,0,0,115,117,3,26,13,0,116,115,1,0,0,0,116,117,1,0,0,0,117,118,1,0,
  	0,0,118,119,5,64,0,0,119,123,5,65,0,0,120,122,3,30,15,0,121,120,1,0,0,
  	0,122,125,1,0,0,0,123,121,1,0,0,0,123,124,1,0,0,0,124,126,1,0,0,0,125,
  	123,1,0,0,0,126,143,5,66,0,0,127,128,5,2,0,0,128,129,5,72,0,0,129,131,
  	5,63,0,0,130,132,3,26,13,0,131,130,1,0,0,0,131,132,1,0,0,0,132,133,1,
  	0,0,0,133,134,5,64,0,0,134,138,5,65,0,0,135,137,3,30,15,0,136,135,1,0,
  	0,0,137,140,1,0,0,0,138,136,1,0,0,0,138,139,1,0,0,0,139,141,1,0,0,0,140,
  	138,1,0,0,0,141,143,5,66,0,0,142,112,1,0,0,0,142,127,1,0,0,0,143,5,1,
  	0,0,0,144,145,5,3,0,0,145,148,3,24,12,0,146,147,5,5,0,0,147,149,3,24,
  	12,0,148,146,1,0,0,0,148,149,1,0,0,0,149,150,1,0,0,0,150,154,5,65,0,0,
  	151,153,3,8,4,0,152,151,1,0,0,0,153,156,1,0,0,0,154,152,1,0,0,0,154,155,
  	1,0,0,0,155,157,1,0,0,0,156,154,1,0,0,0,157,158,5,66,0,0,158,166,1,0,
  	0,0,159,160,5,3,0,0,160,161,3,24,12,0,161,162,5,50,0,0,162,163,3,22,11,
  	0,163,164,5,54,0,0,164,166,1,0,0,0,165,144,1,0,0,0,165,159,1,0,0,0,166,
  	7,1,0,0,0,167,168,5,72,0,0,168,169,5,52,0,0,169,170,3,22,11,0,170,171,
  	5,54,0,0,171,174,1,0,0,0,172,174,3,10,5,0,173,167,1,0,0,0,173,172,1,0,
  	0,0,174,9,1,0,0,0,175,176,5,11,0,0,176,185,5,65,0,0,177,182,3,12,6,0,
  	178,179,5,53,0,0,179,181,3,12,6,0,180,178,1,0,0,0,181,184,1,0,0,0,182,
  	180,1,0,0,0,182,183,1,0,0,0,183,186,1,0,0,0,184,182,1,0,0,0,185,177,1,
  	0,0,0,185,186,1,0,0,0,186,187,1,0,0,0,187,188,5,66,0,0,188,11,1,0,0,0,
  	189,190,5,73,0,0,190,191,5,52,0,0,191,192,5,73,0,0,192,13,1,0,0,0,193,
  	194,5,4,0,0,194,197,3,24,12,0,195,196,5,52,0,0,196,198,3,22,11,0,197,
  	195,1,0,0,0,197,198,1,0,0,0,198,199,1,0,0,0,199,203,5,65,0,0,200,202,
  	3,16,8,0,201,200,1,0,0,0,202,205,1,0,0,0,203,201,1,0,0,0,203,204,1,0,
  	0,0,204,206,1,0,0,0,205,203,1,0,0,0,206,207,5,66,0,0,207,15,1,0,0,0,208,
  	211,5,72,0,0,209,210,5,50,0,0,210,212,3,18,9,0,211,209,1,0,0,0,211,212,
  	1,0,0,0,212,213,1,0,0,0,213,214,5,54,0,0,214,17,1,0,0,0,215,221,5,73,
  	0,0,216,218,5,57,0,0,217,216,1,0,0,0,217,218,1,0,0,0,218,219,1,0,0,0,
  	219,221,5,69,0,0,220,215,1,0,0,0,220,217,1,0,0,0,221,19,1,0,0,0,222,223,
  	5,9,0,0,223,224,3,22,11,0,224,225,5,10,0,0,225,226,5,73,0,0,226,21,1,
  	0,0,0,227,246,3,24,12,0,228,229,5,6,0,0,229,230,5,48,0,0,230,231,3,22,
  	11,0,231,232,5,49,0,0,232,246,1,0,0,0,233,234,5,7,0,0,234,235,5,48,0,
  	0,235,236,3,22,11,0,236,237,5,49,0,0,237,246,1,0,0,0,238,239,5,8,0,0,
  	239,240,5,48,0,0,240,241,3,22,11,0,241,242,5,53,0,0,242,243,3,22,11,0,
  	243,244,5,49,0,0,244,246,1,0,0,0,245,227,1,0,0,0,245,228,1,0,0,0,245,
  	233,1,0,0,0,245,238,1,0,0,0,246,23,1,0,0,0,247,252,5,72,0,0,248,249,5,
  	51,0,0,249,251,5,72,0,0,250,248,1,0,0,0,251,254,1,0,0,0,252,250,1,0,0,
  	0,252,253,1,0,0,0,253,25,1,0,0,0,254,252,1,0,0,0,255,260,3,28,14,0,256,
  	257,5,53,0,0,257,259,3,28,14,0,258,256,1,0,0,0,259,262,1,0,0,0,260,258,
  	1,0,0,0,260,261,1,0,0,0,261,27,1,0,0,0,262,260,1,0,0,0,263,264,3,22,11,
  	0,264,265,5,72,0,0,265,273,1,0,0,0,266,267,5,72,0,0,267,268,5,27,0,0,
  	268,273,3,22,11,0,269,270,5,72,0,0,270,271,5,28,0,0,271,273,3,22,11,0,
  	272,263,1,0,0,0,272,266,1,0,0,0,272,269,1,0,0,0,273,29,1,0,0,0,274,282,
  	3,32,16,0,275,282,3,38,19,0,276,282,3,42,21,0,277,282,3,44,22,0,278,282,
  	3,46,23,0,279,282,3,48,24,0,280,282,3,54,27,0,281,274,1,0,0,0,281,275,
  	1,0,0,0,281,276,1,0,0,0,281,277,1,0,0,0,281,278,1,0,0,0,281,279,1,0,0,
  	0,281,280,1,0,0,0,282,31,1,0,0,0,283,284,5,12,0,0,284,286,5,65,0,0,285,
  	287,3,34,17,0,286,285,1,0,0,0,287,288,1,0,0,0,288,286,1,0,0,0,288,289,
  	1,0,0,0,289,290,1,0,0,0,290,291,5,66,0,0,291,33,1,0,0,0,292,298,5,72,
  	0,0,293,295,5,67,0,0,294,296,3,36,18,0,295,294,1,0,0,0,295,296,1,0,0,
  	0,296,297,1,0,0,0,297,299,5,68,0,0,298,293,1,0,0,0,298,299,1,0,0,0,299,
  	300,1,0,0,0,300,301,5,52,0,0,301,302,3,56,28,0,302,303,5,54,0,0,303,35,
  	1,0,0,0,304,309,5,72,0,0,305,306,5,53,0,0,306,308,5,72,0,0,307,305,1,
  	0,0,0,308,311,1,0,0,0,309,307,1,0,0,0,309,310,1,0,0,0,310,37,1,0,0,0,
  	311,309,1,0,0,0,312,313,5,13,0,0,313,315,5,65,0,0,314,316,3,40,20,0,315,
  	314,1,0,0,0,316,317,1,0,0,0,317,315,1,0,0,0,317,318,1,0,0,0,318,319,1,
  	0,0,0,319,320,5,66,0,0,320,39,1,0,0,0,321,322,3,22,11,0,322,325,5,72,
  	0,0,323,324,5,50,0,0,324,326,3,66,33,0,325,323,1,0,0,0,325,326,1,0,0,
  	0,326,327,1,0,0,0,327,328,5,54,0,0,328,41,1,0,0,0,329,330,5,14,0,0,330,
  	331,5,73,0,0,331,332,5,73,0,0,332,337,5,65,0,0,333,336,3,50,25,0,334,
  	336,3,48,24,0,335,333,1,0,0,0,335,334,1,0,0,0,336,339,1,0,0,0,337,335,
  	1,0,0,0,337,338,1,0,0,0,338,340,1,0,0,0,339,337,1,0,0,0,340,341,5,66,
  	0,0,341,43,1,0,0,0,342,343,5,15,0,0,343,344,5,73,0,0,344,345,5,73,0,0,
  	345,350,5,65,0,0,346,349,3,50,25,0,347,349,3,48,24,0,348,346,1,0,0,0,
  	348,347,1,0,0,0,349,352,1,0,0,0,350,348,1,0,0,0,350,351,1,0,0,0,351,353,
  	1,0,0,0,352,350,1,0,0,0,353,354,5,66,0,0,354,45,1,0,0,0,355,356,5,16,
  	0,0,356,357,5,73,0,0,357,358,5,73,0,0,358,363,5,65,0,0,359,362,3,50,25,
  	0,360,362,3,48,24,0,361,359,1,0,0,0,361,360,1,0,0,0,362,365,1,0,0,0,363,
  	361,1,0,0,0,363,364,1,0,0,0,364,366,1,0,0,0,365,363,1,0,0,0,366,367,5,
  	66,0,0,367,47,1,0,0,0,368,369,5,17,0,0,369,370,5,52,0,0,370,371,3,52,
  	26,0,371,372,5,54,0,0,372,399,1,0,0,0,373,374,5,18,0,0,374,375,5,52,0,
  	0,375,376,3,52,26,0,376,377,5,54,0,0,377,399,1,0,0,0,378,379,5,19,0,0,
  	379,380,5,52,0,0,380,381,3,52,26,0,381,382,5,54,0,0,382,399,1,0,0,0,383,
  	384,5,20,0,0,384,385,5,52,0,0,385,386,3,52,26,0,386,387,5,54,0,0,387,
  	399,1,0,0,0,388,389,5,21,0,0,389,390,5,52,0,0,390,391,3,52,26,0,391,392,
  	5,54,0,0,392,399,1,0,0,0,393,394,5,22,0,0,394,395,5,52,0,0,395,396,3,
  	52,26,0,396,397,5,54,0,0,397,399,1,0,0,0,398,368,1,0,0,0,398,373,1,0,
  	0,0,398,378,1,0,0,0,398,383,1,0,0,0,398,388,1,0,0,0,398,393,1,0,0,0,399,
  	49,1,0,0,0,400,401,5,24,0,0,401,402,5,52,0,0,402,403,3,22,11,0,403,404,
  	5,54,0,0,404,411,1,0,0,0,405,406,5,25,0,0,406,407,5,52,0,0,407,408,3,
  	22,11,0,408,409,5,54,0,0,409,411,1,0,0,0,410,400,1,0,0,0,410,405,1,0,
  	0,0,411,51,1,0,0,0,412,413,5,72,0,0,413,414,3,96,48,0,414,416,5,63,0,
  	0,415,417,3,26,13,0,416,415,1,0,0,0,416,417,1,0,0,0,417,418,1,0,0,0,418,
  	419,5,64,0,0,419,53,1,0,0,0,420,421,5,26,0,0,421,425,5,65,0,0,422,424,
  	3,40,20,0,423,422,1,0,0,0,424,427,1,0,0,0,425,423,1,0,0,0,425,426,1,0,
  	0,0,426,428,1,0,0,0,427,425,1,0,0,0,428,429,5,66,0,0,429,55,1,0,0,0,430,
  	431,6,28,-1,0,431,432,5,33,0,0,432,433,5,63,0,0,433,436,3,56,28,0,434,
  	435,5,43,0,0,435,437,3,56,28,0,436,434,1,0,0,0,437,438,1,0,0,0,438,436,
  	1,0,0,0,438,439,1,0,0,0,439,440,1,0,0,0,440,441,5,64,0,0,441,496,1,0,
  	0,0,442,443,5,34,0,0,443,444,5,63,0,0,444,447,3,56,28,0,445,446,5,43,
  	0,0,446,448,3,56,28,0,447,445,1,0,0,0,448,449,1,0,0,0,449,447,1,0,0,0,
  	449,450,1,0,0,0,450,451,1,0,0,0,451,452,5,64,0,0,452,496,1,0,0,0,453,
  	454,5,37,0,0,454,455,5,69,0,0,455,456,5,38,0,0,456,457,3,56,28,0,457,
  	458,5,39,0,0,458,459,3,56,28,8,459,496,1,0,0,0,460,461,5,32,0,0,461,462,
  	5,69,0,0,462,463,5,69,0,0,463,464,5,63,0,0,464,465,3,56,28,0,465,469,
  	5,64,0,0,466,468,3,60,30,0,467,466,1,0,0,0,468,471,1,0,0,0,469,467,1,
  	0,0,0,469,470,1,0,0,0,470,496,1,0,0,0,471,469,1,0,0,0,472,496,5,29,0,
  	0,473,496,5,30,0,0,474,496,5,31,0,0,475,479,3,62,31,0,476,478,3,60,30,
  	0,477,476,1,0,0,0,478,481,1,0,0,0,479,477,1,0,0,0,479,480,1,0,0,0,480,
  	496,1,0,0,0,481,479,1,0,0,0,482,483,5,63,0,0,483,484,3,56,28,0,484,485,
  	5,64,0,0,485,496,1,0,0,0,486,487,5,36,0,0,487,491,5,65,0,0,488,490,3,
  	58,29,0,489,488,1,0,0,0,490,493,1,0,0,0,491,489,1,0,0,0,491,492,1,0,0,
  	0,492,494,1,0,0,0,493,491,1,0,0,0,494,496,5,66,0,0,495,430,1,0,0,0,495,
  	442,1,0,0,0,495,453,1,0,0,0,495,460,1,0,0,0,495,472,1,0,0,0,495,473,1,
  	0,0,0,495,474,1,0,0,0,495,475,1,0,0,0,495,482,1,0,0,0,495,486,1,0,0,0,
  	496,506,1,0,0,0,497,500,10,11,0,0,498,499,5,42,0,0,499,501,3,56,28,0,
  	500,498,1,0,0,0,501,502,1,0,0,0,502,500,1,0,0,0,502,503,1,0,0,0,503,505,
  	1,0,0,0,504,497,1,0,0,0,505,508,1,0,0,0,506,504,1,0,0,0,506,507,1,0,0,
  	0,507,57,1,0,0,0,508,506,1,0,0,0,509,511,5,35,0,0,510,512,3,66,33,0,511,
  	510,1,0,0,0,511,512,1,0,0,0,512,513,1,0,0,0,513,514,5,52,0,0,514,515,
  	3,56,28,0,515,516,5,54,0,0,516,59,1,0,0,0,517,518,5,23,0,0,518,519,5,
  	20,0,0,519,528,3,56,28,0,520,521,5,23,0,0,521,522,5,19,0,0,522,528,3,
  	56,28,0,523,524,5,23,0,0,524,525,3,62,31,0,525,526,3,56,28,0,526,528,
  	1,0,0,0,527,517,1,0,0,0,527,520,1,0,0,0,527,523,1,0,0,0,528,61,1,0,0,
  	0,529,530,3,96,48,0,530,531,5,55,0,0,531,532,3,96,48,0,532,534,5,63,0,
  	0,533,535,3,64,32,0,534,533,1,0,0,0,534,535,1,0,0,0,535,536,1,0,0,0,536,
  	537,5,64,0,0,537,546,1,0,0,0,538,539,3,96,48,0,539,541,5,63,0,0,540,542,
  	3,64,32,0,541,540,1,0,0,0,541,542,1,0,0,0,542,543,1,0,0,0,543,544,5,64,
  	0,0,544,546,1,0,0,0,545,529,1,0,0,0,545,538,1,0,0,0,546,63,1,0,0,0,547,
  	552,3,66,33,0,548,549,5,53,0,0,549,551,3,66,33,0,550,548,1,0,0,0,551,
  	554,1,0,0,0,552,550,1,0,0,0,552,553,1,0,0,0,553,65,1,0,0,0,554,552,1,
  	0,0,0,555,556,3,68,34,0,556,67,1,0,0,0,557,562,3,70,35,0,558,559,5,62,
  	0,0,559,561,3,70,35,0,560,558,1,0,0,0,561,564,1,0,0,0,562,560,1,0,0,0,
  	562,563,1,0,0,0,563,69,1,0,0,0,564,562,1,0,0,0,565,570,3,72,36,0,566,
  	567,5,61,0,0,567,569,3,72,36,0,568,566,1,0,0,0,569,572,1,0,0,0,570,568,
  	1,0,0,0,570,571,1,0,0,0,571,71,1,0,0,0,572,570,1,0,0,0,573,577,3,76,38,
  	0,574,575,3,74,37,0,575,576,3,76,38,0,576,578,1,0,0,0,577,574,1,0,0,0,
  	577,578,1,0,0,0,578,73,1,0,0,0,579,580,7,0,0,0,580,75,1,0,0,0,581,582,
  	5,60,0,0,582,585,3,76,38,0,583,585,3,78,39,0,584,581,1,0,0,0,584,583,
  	1,0,0,0,585,77,1,0,0,0,586,591,3,80,40,0,587,588,7,1,0,0,588,590,3,80,
  	40,0,589,587,1,0,0,0,590,593,1,0,0,0,591,589,1,0,0,0,591,592,1,0,0,0,
  	592,79,1,0,0,0,593,591,1,0,0,0,594,599,3,82,41,0,595,596,7,2,0,0,596,
  	598,3,82,41,0,597,595,1,0,0,0,598,601,1,0,0,0,599,597,1,0,0,0,599,600,
  	1,0,0,0,600,81,1,0,0,0,601,599,1,0,0,0,602,603,5,57,0,0,603,606,3,82,
  	41,0,604,606,3,84,42,0,605,602,1,0,0,0,605,604,1,0,0,0,606,83,1,0,0,0,
  	607,624,3,62,31,0,608,624,5,71,0,0,609,624,5,72,0,0,610,624,5,73,0,0,
  	611,624,5,69,0,0,612,624,5,70,0,0,613,624,3,86,43,0,614,624,3,90,45,0,
  	615,624,3,92,46,0,616,617,5,72,0,0,617,618,5,55,0,0,618,624,5,72,0,0,
  	619,620,5,63,0,0,620,621,3,66,33,0,621,622,5,64,0,0,622,624,1,0,0,0,623,
  	607,1,0,0,0,623,608,1,0,0,0,623,609,1,0,0,0,623,610,1,0,0,0,623,611,1,
  	0,0,0,623,612,1,0,0,0,623,613,1,0,0,0,623,614,1,0,0,0,623,615,1,0,0,0,
  	623,616,1,0,0,0,623,619,1,0,0,0,624,85,1,0,0,0,625,634,5,65,0,0,626,631,
  	3,88,44,0,627,628,5,53,0,0,628,630,3,88,44,0,629,627,1,0,0,0,630,633,
  	1,0,0,0,631,629,1,0,0,0,631,632,1,0,0,0,632,635,1,0,0,0,633,631,1,0,0,
  	0,634,626,1,0,0,0,634,635,1,0,0,0,635,636,1,0,0,0,636,637,5,66,0,0,637,
  	87,1,0,0,0,638,639,5,72,0,0,639,640,5,52,0,0,640,641,3,66,33,0,641,89,
  	1,0,0,0,642,651,5,67,0,0,643,648,3,66,33,0,644,645,5,53,0,0,645,647,3,
  	66,33,0,646,644,1,0,0,0,647,650,1,0,0,0,648,646,1,0,0,0,648,649,1,0,0,
  	0,649,652,1,0,0,0,650,648,1,0,0,0,651,643,1,0,0,0,651,652,1,0,0,0,652,
  	653,1,0,0,0,653,654,5,68,0,0,654,91,1,0,0,0,655,664,5,65,0,0,656,661,
  	3,94,47,0,657,658,5,53,0,0,658,660,3,94,47,0,659,657,1,0,0,0,660,663,
  	1,0,0,0,661,659,1,0,0,0,661,662,1,0,0,0,662,665,1,0,0,0,663,661,1,0,0,
  	0,664,656,1,0,0,0,664,665,1,0,0,0,665,666,1,0,0,0,666,667,5,66,0,0,667,
  	93,1,0,0,0,668,669,5,48,0,0,669,670,3,66,33,0,670,671,5,53,0,0,671,672,
  	3,66,33,0,672,673,5,49,0,0,673,95,1,0,0,0,674,675,7,3,0,0,675,97,1,0,
  	0,0,67,101,110,116,123,131,138,142,148,154,165,173,182,185,197,203,211,
  	217,220,245,252,260,272,281,288,295,298,309,317,325,335,337,348,350,361,
  	363,398,410,416,425,438,449,469,479,491,495,502,506,511,527,534,541,545,
  	552,562,570,577,584,591,599,605,623,631,634,648,651,661,664
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
    setState(101);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TASK)
      | (1ULL << KodaParser::CAPABILITY)
      | (1ULL << KodaParser::TYPE)
      | (1ULL << KodaParser::ENUM)
      | (1ULL << KodaParser::MAPPING))) != 0)) {
      setState(98);
      topLevelDeclaration();
      setState(103);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(104);
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
    setState(110);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK:
      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 1);
        setState(106);
        topLevelComponent();
        break;
      }

      case KodaParser::TYPE: {
        enterOuterAlt(_localctx, 2);
        setState(107);
        typeDeclaration();
        break;
      }

      case KodaParser::ENUM: {
        enterOuterAlt(_localctx, 3);
        setState(108);
        enumDeclaration();
        break;
      }

      case KodaParser::MAPPING: {
        enterOuterAlt(_localctx, 4);
        setState(109);
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
    setState(142);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(112);
        match(KodaParser::TASK);
        setState(113);
        match(KodaParser::IDENT);
        setState(114);
        match(KodaParser::LPAREN);
        setState(116);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::LIST)
          | (1ULL << KodaParser::OPTIONAL)
          | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
          setState(115);
          argumentList();
        }
        setState(118);
        match(KodaParser::RPAREN);
        setState(119);
        match(KodaParser::LBRACE);
        setState(123);
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
          | (1ULL << KodaParser::OUT)
          | (1ULL << KodaParser::DATA))) != 0)) {
          setState(120);
          statement();
          setState(125);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(126);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(127);
        match(KodaParser::CAPABILITY);
        setState(128);
        match(KodaParser::IDENT);
        setState(129);
        match(KodaParser::LPAREN);
        setState(131);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::LIST)
          | (1ULL << KodaParser::OPTIONAL)
          | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
          setState(130);
          argumentList();
        }
        setState(133);
        match(KodaParser::RPAREN);
        setState(134);
        match(KodaParser::LBRACE);
        setState(138);
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
          | (1ULL << KodaParser::OUT)
          | (1ULL << KodaParser::DATA))) != 0)) {
          setState(135);
          statement();
          setState(140);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(141);
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
    setState(165);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::TypeRecordContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(144);
      match(KodaParser::TYPE);
      setState(145);
      qualifiedName();
      setState(148);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::EXTENDS) {
        setState(146);
        match(KodaParser::EXTENDS);
        setState(147);
        qualifiedName();
      }
      setState(150);
      match(KodaParser::LBRACE);
      setState(154);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::ANNOTATIONS

      || _la == KodaParser::IDENT) {
        setState(151);
        fieldDeclaration();
        setState(156);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(157);
      match(KodaParser::RBRACE);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::TypeAliasContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(159);
      match(KodaParser::TYPE);
      setState(160);
      qualifiedName();
      setState(161);
      match(KodaParser::ASSIGN);
      setState(162);
      typeReference();
      setState(163);
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

KodaParser::AnnotationDeclarationContext* KodaParser::FieldDeclarationContext::annotationDeclaration() {
  return getRuleContext<KodaParser::AnnotationDeclarationContext>(0);
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
    setState(173);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IDENT: {
        enterOuterAlt(_localctx, 1);
        setState(167);
        match(KodaParser::IDENT);
        setState(168);
        match(KodaParser::COLON);
        setState(169);
        typeReference();
        setState(170);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ANNOTATIONS: {
        enterOuterAlt(_localctx, 2);
        setState(172);
        annotationDeclaration();
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

//----------------- AnnotationDeclarationContext ------------------------------------------------------------------

KodaParser::AnnotationDeclarationContext::AnnotationDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::AnnotationDeclarationContext::ANNOTATIONS() {
  return getToken(KodaParser::ANNOTATIONS, 0);
}

tree::TerminalNode* KodaParser::AnnotationDeclarationContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::AnnotationDeclarationContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::AnnotationContext *> KodaParser::AnnotationDeclarationContext::annotation() {
  return getRuleContexts<KodaParser::AnnotationContext>();
}

KodaParser::AnnotationContext* KodaParser::AnnotationDeclarationContext::annotation(size_t i) {
  return getRuleContext<KodaParser::AnnotationContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::AnnotationDeclarationContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::AnnotationDeclarationContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::AnnotationDeclarationContext::getRuleIndex() const {
  return KodaParser::RuleAnnotationDeclaration;
}


std::any KodaParser::AnnotationDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitAnnotationDeclaration(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::AnnotationDeclarationContext* KodaParser::annotationDeclaration() {
  AnnotationDeclarationContext *_localctx = _tracker.createInstance<AnnotationDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 10, KodaParser::RuleAnnotationDeclaration);
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
    setState(175);
    match(KodaParser::ANNOTATIONS);
    setState(176);
    match(KodaParser::LBRACE);
    setState(185);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::STRING) {
      setState(177);
      annotation();
      setState(182);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(178);
        match(KodaParser::COMMA);
        setState(179);
        annotation();
        setState(184);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(187);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AnnotationContext ------------------------------------------------------------------

KodaParser::AnnotationContext::AnnotationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> KodaParser::AnnotationContext::STRING() {
  return getTokens(KodaParser::STRING);
}

tree::TerminalNode* KodaParser::AnnotationContext::STRING(size_t i) {
  return getToken(KodaParser::STRING, i);
}

tree::TerminalNode* KodaParser::AnnotationContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}


size_t KodaParser::AnnotationContext::getRuleIndex() const {
  return KodaParser::RuleAnnotation;
}


std::any KodaParser::AnnotationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitAnnotation(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::AnnotationContext* KodaParser::annotation() {
  AnnotationContext *_localctx = _tracker.createInstance<AnnotationContext>(_ctx, getState());
  enterRule(_localctx, 12, KodaParser::RuleAnnotation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(189);
    match(KodaParser::STRING);
    setState(190);
    match(KodaParser::COLON);
    setState(191);
    match(KodaParser::STRING);
   
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
  enterRule(_localctx, 14, KodaParser::RuleEnumDeclaration);
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
    setState(193);
    match(KodaParser::ENUM);
    setState(194);
    qualifiedName();
    setState(197);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(195);
      match(KodaParser::COLON);
      setState(196);
      typeReference();
    }
    setState(199);
    match(KodaParser::LBRACE);
    setState(203);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(200);
      enumValue();
      setState(205);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(206);
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
  enterRule(_localctx, 16, KodaParser::RuleEnumValue);
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
    setState(211);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(209);
      match(KodaParser::ASSIGN);
      setState(210);
      enumLiteral();
    }
    setState(213);
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
  enterRule(_localctx, 18, KodaParser::RuleEnumLiteral);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(220);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(215);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::MINUS:
      case KodaParser::NATURAL: {
        enterOuterAlt(_localctx, 2);
        setState(217);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::MINUS) {
          setState(216);
          match(KodaParser::MINUS);
        }
        setState(219);
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
  enterRule(_localctx, 20, KodaParser::RuleMappingDeclaration);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(222);
    match(KodaParser::MAPPING);
    setState(223);
    typeReference();
    setState(224);
    match(KodaParser::TO);
    setState(225);
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
  enterRule(_localctx, 22, KodaParser::RuleTypeReference);

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
      case KodaParser::IDENT: {
        _localctx = _tracker.createInstance<KodaParser::TypeNamedContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(227);
        qualifiedName();
        break;
      }

      case KodaParser::LIST: {
        _localctx = _tracker.createInstance<KodaParser::TypeListContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(228);
        match(KodaParser::LIST);
        setState(229);
        match(KodaParser::LT);
        setState(230);
        typeReference();
        setState(231);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::OPTIONAL: {
        _localctx = _tracker.createInstance<KodaParser::TypeOptionalContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(233);
        match(KodaParser::OPTIONAL);
        setState(234);
        match(KodaParser::LT);
        setState(235);
        typeReference();
        setState(236);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::MAP: {
        _localctx = _tracker.createInstance<KodaParser::TypeMapContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(238);
        match(KodaParser::MAP);
        setState(239);
        match(KodaParser::LT);
        setState(240);
        typeReference();
        setState(241);
        match(KodaParser::COMMA);
        setState(242);
        typeReference();
        setState(243);
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
  enterRule(_localctx, 24, KodaParser::RuleQualifiedName);
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
    match(KodaParser::IDENT);
    setState(252);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::DOUBLE_COLON) {
      setState(248);
      match(KodaParser::DOUBLE_COLON);
      setState(249);
      match(KodaParser::IDENT);
      setState(254);
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
  enterRule(_localctx, 26, KodaParser::RuleArgumentList);
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
    setState(255);
    argument();
    setState(260);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(256);
      match(KodaParser::COMMA);
      setState(257);
      argument();
      setState(262);
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
  enterRule(_localctx, 28, KodaParser::RuleArgument);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(272);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(263);
      typeReference();
      setState(264);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(266);
      match(KodaParser::IDENT);
      setState(267);
      match(KodaParser::REQ);
      setState(268);
      typeReference();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(269);
      match(KodaParser::IDENT);
      setState(270);
      match(KodaParser::PRO);
      setState(271);
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

KodaParser::DataBlockContext* KodaParser::StatementContext::dataBlock() {
  return getRuleContext<KodaParser::DataBlockContext>(0);
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
  enterRule(_localctx, 30, KodaParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(281);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(274);
        tasksBlock();
        break;
      }

      case KodaParser::PARAMETERS: {
        enterOuterAlt(_localctx, 2);
        setState(275);
        varsBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 3);
        setState(276);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 4);
        setState(277);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 5);
        setState(278);
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
        setState(279);
        rosDefStatement();
        break;
      }

      case KodaParser::DATA: {
        enterOuterAlt(_localctx, 7);
        setState(280);
        dataBlock();
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
  enterRule(_localctx, 32, KodaParser::RuleTasksBlock);
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
    setState(283);
    match(KodaParser::STRATEGY);
    setState(284);
    match(KodaParser::LBRACE);
    setState(286); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(285);
      flow();
      setState(288); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(290);
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
  enterRule(_localctx, 34, KodaParser::RuleFlow);
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
    setState(292);
    match(KodaParser::IDENT);
    setState(298);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(293);
      match(KodaParser::LBRACK);
      setState(295);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(294);
        identList();
      }
      setState(297);
      match(KodaParser::RBRACK);
    }
    setState(300);
    match(KodaParser::COLON);
    setState(301);
    strategy(0);
    setState(302);
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
  enterRule(_localctx, 36, KodaParser::RuleIdentList);
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
    match(KodaParser::IDENT);
    setState(309);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(305);
      match(KodaParser::COMMA);
      setState(306);
      match(KodaParser::IDENT);
      setState(311);
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
  enterRule(_localctx, 38, KodaParser::RuleVarsBlock);
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
    setState(312);
    match(KodaParser::PARAMETERS);
    setState(313);
    match(KodaParser::LBRACE);
    setState(315); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(314);
      variableStatement();
      setState(317); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::LIST)
      | (1ULL << KodaParser::OPTIONAL)
      | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT);
    setState(319);
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

tree::TerminalNode* KodaParser::VariableStatementContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

tree::TerminalNode* KodaParser::VariableStatementContext::ASSIGN() {
  return getToken(KodaParser::ASSIGN, 0);
}

KodaParser::ExpressionContext* KodaParser::VariableStatementContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
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
  enterRule(_localctx, 40, KodaParser::RuleVariableStatement);
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
    setState(321);
    typeReference();
    setState(322);
    match(KodaParser::IDENT);
    setState(325);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(323);
      match(KodaParser::ASSIGN);
      setState(324);
      expression();
    }
    setState(327);
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
  enterRule(_localctx, 42, KodaParser::RuleActionBlock);
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
    setState(329);
    match(KodaParser::ACTION);
    setState(330);
    match(KodaParser::STRING);
    setState(331);
    match(KodaParser::STRING);
    setState(332);
    match(KodaParser::LBRACE);
    setState(337);
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
      setState(335);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(333);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(334);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(339);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(340);
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
  enterRule(_localctx, 44, KodaParser::RuleServiceBlock);
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
    setState(342);
    match(KodaParser::SERVICE);
    setState(343);
    match(KodaParser::STRING);
    setState(344);
    match(KodaParser::STRING);
    setState(345);
    match(KodaParser::LBRACE);
    setState(350);
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
      setState(348);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(346);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(347);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(352);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(353);
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
  enterRule(_localctx, 46, KodaParser::RuleTopicBlock);
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
    setState(355);
    match(KodaParser::TOPIC);
    setState(356);
    match(KodaParser::STRING);
    setState(357);
    match(KodaParser::STRING);
    setState(358);
    match(KodaParser::LBRACE);
    setState(363);
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
      setState(361);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(359);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(360);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(365);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(366);
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
  enterRule(_localctx, 48, KodaParser::RuleRosDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(398);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(368);
        match(KodaParser::TRIGGER);
        setState(369);
        match(KodaParser::COLON);
        setState(370);
        eventDefStatement();
        setState(371);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(373);
        match(KodaParser::RETURN);
        setState(374);
        match(KodaParser::COLON);
        setState(375);
        eventDefStatement();
        setState(376);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(378);
        match(KodaParser::ABORT);
        setState(379);
        match(KodaParser::COLON);
        setState(380);
        eventDefStatement();
        setState(381);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(383);
        match(KodaParser::ERROR);
        setState(384);
        match(KodaParser::COLON);
        setState(385);
        eventDefStatement();
        setState(386);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(388);
        match(KodaParser::IN);
        setState(389);
        match(KodaParser::COLON);
        setState(390);
        eventDefStatement();
        setState(391);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(393);
        match(KodaParser::OUT);
        setState(394);
        match(KodaParser::COLON);
        setState(395);
        eventDefStatement();
        setState(396);
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
  enterRule(_localctx, 50, KodaParser::RuleReqDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(410);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::CONSUMES: {
        enterOuterAlt(_localctx, 1);
        setState(400);
        match(KodaParser::CONSUMES);
        setState(401);
        match(KodaParser::COLON);
        setState(402);
        typeReference();
        setState(403);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::PRODUCES: {
        enterOuterAlt(_localctx, 2);
        setState(405);
        match(KodaParser::PRODUCES);
        setState(406);
        match(KodaParser::COLON);
        setState(407);
        typeReference();
        setState(408);
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
  enterRule(_localctx, 52, KodaParser::RuleEventDefStatement);
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
    setState(412);
    match(KodaParser::IDENT);
    setState(413);
    identifier();
    setState(414);
    match(KodaParser::LPAREN);
    setState(416);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::LIST)
      | (1ULL << KodaParser::OPTIONAL)
      | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
      setState(415);
      argumentList();
    }
    setState(418);
    match(KodaParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DataBlockContext ------------------------------------------------------------------

KodaParser::DataBlockContext::DataBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::DataBlockContext::DATA() {
  return getToken(KodaParser::DATA, 0);
}

tree::TerminalNode* KodaParser::DataBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::DataBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::VariableStatementContext *> KodaParser::DataBlockContext::variableStatement() {
  return getRuleContexts<KodaParser::VariableStatementContext>();
}

KodaParser::VariableStatementContext* KodaParser::DataBlockContext::variableStatement(size_t i) {
  return getRuleContext<KodaParser::VariableStatementContext>(i);
}


size_t KodaParser::DataBlockContext::getRuleIndex() const {
  return KodaParser::RuleDataBlock;
}


std::any KodaParser::DataBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitDataBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::DataBlockContext* KodaParser::dataBlock() {
  DataBlockContext *_localctx = _tracker.createInstance<DataBlockContext>(_ctx, getState());
  enterRule(_localctx, 54, KodaParser::RuleDataBlock);
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
    setState(420);
    match(KodaParser::DATA);
    setState(421);
    match(KodaParser::LBRACE);
    setState(425);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::LIST)
      | (1ULL << KodaParser::OPTIONAL)
      | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
      setState(422);
      variableStatement();
      setState(427);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(428);
    match(KodaParser::RBRACE);
   
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
//----------------- StratSuccessContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratSuccessContext::SUCCESS() {
  return getToken(KodaParser::SUCCESS, 0);
}

KodaParser::StratSuccessContext::StratSuccessContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratSuccessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratSuccess(this);
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
//----------------- StratFailureContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratFailureContext::FAILURE() {
  return getToken(KodaParser::FAILURE, 0);
}

KodaParser::StratFailureContext::StratFailureContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratFailureContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratFailure(this);
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
//----------------- StratChooseContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratChooseContext::CHOOSE() {
  return getToken(KodaParser::CHOOSE, 0);
}

tree::TerminalNode* KodaParser::StratChooseContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::StratChooseContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::ChooseWhenStatementContext *> KodaParser::StratChooseContext::chooseWhenStatement() {
  return getRuleContexts<KodaParser::ChooseWhenStatementContext>();
}

KodaParser::ChooseWhenStatementContext* KodaParser::StratChooseContext::chooseWhenStatement(size_t i) {
  return getRuleContext<KodaParser::ChooseWhenStatementContext>(i);
}

KodaParser::StratChooseContext::StratChooseContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratChooseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratChoose(this);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, KodaParser::RuleStrategy, precedence);

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
    setState(495);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::JOIN: {
        _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(431);
        match(KodaParser::JOIN);
        setState(432);
        match(KodaParser::LPAREN);
        setState(433);
        strategy(0);
        setState(436); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(434);
          match(KodaParser::PIPE);
          setState(435);
          strategy(0);
          setState(438); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(440);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::EITHER: {
        _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(442);
        match(KodaParser::EITHER);
        setState(443);
        match(KodaParser::LPAREN);
        setState(444);
        strategy(0);
        setState(447); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(445);
          match(KodaParser::PIPE);
          setState(446);
          strategy(0);
          setState(449); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(451);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::WITHIN: {
        _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(453);
        match(KodaParser::WITHIN);
        setState(454);
        match(KodaParser::NATURAL);
        setState(455);
        match(KodaParser::DO);
        setState(456);
        strategy(0);
        setState(457);
        match(KodaParser::ELSE);
        setState(458);
        strategy(8);
        break;
      }

      case KodaParser::REPEAT: {
        _localctx = _tracker.createInstance<StratRepeatContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(460);
        match(KodaParser::REPEAT);
        setState(461);
        match(KodaParser::NATURAL);
        setState(462);
        match(KodaParser::NATURAL);
        setState(463);
        match(KodaParser::LPAREN);
        setState(464);
        strategy(0);
        setState(465);
        match(KodaParser::RPAREN);
        setState(469);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(466);
            strategyHandler(); 
          }
          setState(471);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
        }
        break;
      }

      case KodaParser::SUCCESS: {
        _localctx = _tracker.createInstance<StratSuccessContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(472);
        match(KodaParser::SUCCESS);
        break;
      }

      case KodaParser::FAILURE: {
        _localctx = _tracker.createInstance<StratFailureContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(473);
        match(KodaParser::FAILURE);
        break;
      }

      case KodaParser::CONTINUE: {
        _localctx = _tracker.createInstance<StratContinueContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(474);
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
        setState(475);
        eventStatement();
        setState(479);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(476);
            strategyHandler(); 
          }
          setState(481);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
        }
        break;
      }

      case KodaParser::LPAREN: {
        _localctx = _tracker.createInstance<StratParenContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(482);
        match(KodaParser::LPAREN);
        setState(483);
        strategy(0);
        setState(484);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::CHOOSE: {
        _localctx = _tracker.createInstance<StratChooseContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(486);
        match(KodaParser::CHOOSE);
        setState(487);
        match(KodaParser::LBRACE);
        setState(491);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == KodaParser::WHEN) {
          setState(488);
          chooseWhenStatement();
          setState(493);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(494);
        match(KodaParser::RBRACE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(506);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<StratSeqContext>(_tracker.createInstance<StrategyContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleStrategy);
        setState(497);

        if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
        setState(500); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(498);
                  match(KodaParser::ARROW);
                  setState(499);
                  strategy(0);
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(502); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER); 
      }
      setState(508);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ChooseWhenStatementContext ------------------------------------------------------------------

KodaParser::ChooseWhenStatementContext::ChooseWhenStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::ChooseWhenStatementContext::getRuleIndex() const {
  return KodaParser::RuleChooseWhenStatement;
}

void KodaParser::ChooseWhenStatementContext::copyFrom(ChooseWhenStatementContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- WhenStatementContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::WhenStatementContext::WHEN() {
  return getToken(KodaParser::WHEN, 0);
}

tree::TerminalNode* KodaParser::WhenStatementContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::StrategyContext* KodaParser::WhenStatementContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

tree::TerminalNode* KodaParser::WhenStatementContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}

KodaParser::ExpressionContext* KodaParser::WhenStatementContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
}

KodaParser::WhenStatementContext::WhenStatementContext(ChooseWhenStatementContext *ctx) { copyFrom(ctx); }


std::any KodaParser::WhenStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitWhenStatement(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::ChooseWhenStatementContext* KodaParser::chooseWhenStatement() {
  ChooseWhenStatementContext *_localctx = _tracker.createInstance<ChooseWhenStatementContext>(_ctx, getState());
  enterRule(_localctx, 58, KodaParser::RuleChooseWhenStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    _localctx = _tracker.createInstance<KodaParser::WhenStatementContext>(_localctx);
    enterOuterAlt(_localctx, 1);
    setState(509);
    match(KodaParser::WHEN);
    setState(511);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 17) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 17)) & ((1ULL << (KodaParser::TRIGGER - 17))
      | (1ULL << (KodaParser::RETURN - 17))
      | (1ULL << (KodaParser::ABORT - 17))
      | (1ULL << (KodaParser::ERROR - 17))
      | (1ULL << (KodaParser::IN - 17))
      | (1ULL << (KodaParser::OUT - 17))
      | (1ULL << (KodaParser::CONSUMES - 17))
      | (1ULL << (KodaParser::PRODUCES - 17))
      | (1ULL << (KodaParser::MINUS - 17))
      | (1ULL << (KodaParser::NOT - 17))
      | (1ULL << (KodaParser::LPAREN - 17))
      | (1ULL << (KodaParser::LBRACE - 17))
      | (1ULL << (KodaParser::LBRACK - 17))
      | (1ULL << (KodaParser::NATURAL - 17))
      | (1ULL << (KodaParser::REAL - 17))
      | (1ULL << (KodaParser::BOOLEAN - 17))
      | (1ULL << (KodaParser::IDENT - 17))
      | (1ULL << (KodaParser::STRING - 17)))) != 0)) {
      setState(510);
      expression();
    }
    setState(513);
    match(KodaParser::COLON);
    setState(514);
    strategy(0);
    setState(515);
    match(KodaParser::SEMI);
   
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
  enterRule(_localctx, 60, KodaParser::RuleStrategyHandler);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(527);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(517);
      match(KodaParser::ON);
      setState(518);
      match(KodaParser::ERROR);
      setState(519);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(520);
      match(KodaParser::ON);
      setState(521);
      match(KodaParser::ABORT);
      setState(522);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(523);
      match(KodaParser::ON);
      setState(524);
      eventStatement();
      setState(525);
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
  enterRule(_localctx, 62, KodaParser::RuleEventStatement);
  size_t _la = 0;

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
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(529);
      identifier();
      setState(530);
      match(KodaParser::DOT);
      setState(531);
      identifier();
      setState(532);
      match(KodaParser::LPAREN);
      setState(534);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 17) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 17)) & ((1ULL << (KodaParser::TRIGGER - 17))
        | (1ULL << (KodaParser::RETURN - 17))
        | (1ULL << (KodaParser::ABORT - 17))
        | (1ULL << (KodaParser::ERROR - 17))
        | (1ULL << (KodaParser::IN - 17))
        | (1ULL << (KodaParser::OUT - 17))
        | (1ULL << (KodaParser::CONSUMES - 17))
        | (1ULL << (KodaParser::PRODUCES - 17))
        | (1ULL << (KodaParser::MINUS - 17))
        | (1ULL << (KodaParser::NOT - 17))
        | (1ULL << (KodaParser::LPAREN - 17))
        | (1ULL << (KodaParser::LBRACE - 17))
        | (1ULL << (KodaParser::LBRACK - 17))
        | (1ULL << (KodaParser::NATURAL - 17))
        | (1ULL << (KodaParser::REAL - 17))
        | (1ULL << (KodaParser::BOOLEAN - 17))
        | (1ULL << (KodaParser::IDENT - 17))
        | (1ULL << (KodaParser::STRING - 17)))) != 0)) {
        setState(533);
        exprList();
      }
      setState(536);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(538);
      identifier();
      setState(539);
      match(KodaParser::LPAREN);
      setState(541);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 17) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 17)) & ((1ULL << (KodaParser::TRIGGER - 17))
        | (1ULL << (KodaParser::RETURN - 17))
        | (1ULL << (KodaParser::ABORT - 17))
        | (1ULL << (KodaParser::ERROR - 17))
        | (1ULL << (KodaParser::IN - 17))
        | (1ULL << (KodaParser::OUT - 17))
        | (1ULL << (KodaParser::CONSUMES - 17))
        | (1ULL << (KodaParser::PRODUCES - 17))
        | (1ULL << (KodaParser::MINUS - 17))
        | (1ULL << (KodaParser::NOT - 17))
        | (1ULL << (KodaParser::LPAREN - 17))
        | (1ULL << (KodaParser::LBRACE - 17))
        | (1ULL << (KodaParser::LBRACK - 17))
        | (1ULL << (KodaParser::NATURAL - 17))
        | (1ULL << (KodaParser::REAL - 17))
        | (1ULL << (KodaParser::BOOLEAN - 17))
        | (1ULL << (KodaParser::IDENT - 17))
        | (1ULL << (KodaParser::STRING - 17)))) != 0)) {
        setState(540);
        exprList();
      }
      setState(543);
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
  enterRule(_localctx, 64, KodaParser::RuleExprList);
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
    setState(547);
    expression();
    setState(552);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(548);
      match(KodaParser::COMMA);
      setState(549);
      expression();
      setState(554);
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
  enterRule(_localctx, 66, KodaParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(555);
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
  enterRule(_localctx, 68, KodaParser::RuleExprOr);
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
    setState(557);
    exprAnd();
    setState(562);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(558);
      match(KodaParser::OR);
      setState(559);
      exprAnd();
      setState(564);
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
  enterRule(_localctx, 70, KodaParser::RuleExprAnd);
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
    setState(565);
    exprCmp();
    setState(570);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(566);
      match(KodaParser::AND);
      setState(567);
      exprCmp();
      setState(572);
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
  enterRule(_localctx, 72, KodaParser::RuleExprCmp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(573);
    exprNot();
    setState(577);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
    case 1: {
      setState(574);
      compOp();
      setState(575);
      exprNot();
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
  enterRule(_localctx, 74, KodaParser::RuleCompOp);
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
    setState(579);
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
  enterRule(_localctx, 76, KodaParser::RuleExprNot);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(584);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(581);
        match(KodaParser::NOT);
        setState(582);
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
        setState(583);
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
  enterRule(_localctx, 78, KodaParser::RuleExprAdd);
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
    setState(586);
    exprMul();
    setState(591);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(587);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(588);
      exprMul();
      setState(593);
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
  enterRule(_localctx, 80, KodaParser::RuleExprMul);
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
    setState(594);
    exprUnary();
    setState(599);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(595);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(596);
      exprUnary();
      setState(601);
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
  enterRule(_localctx, 82, KodaParser::RuleExprUnary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(605);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(602);
        match(KodaParser::MINUS);
        setState(603);
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
        setState(604);
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
//----------------- ExprDataAccessContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> KodaParser::ExprDataAccessContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::ExprDataAccessContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

tree::TerminalNode* KodaParser::ExprDataAccessContext::DOT() {
  return getToken(KodaParser::DOT, 0);
}

KodaParser::ExprDataAccessContext::ExprDataAccessContext(ExprPrimaryContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ExprDataAccessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitExprDataAccess(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::ExprPrimaryContext* KodaParser::exprPrimary() {
  ExprPrimaryContext *_localctx = _tracker.createInstance<ExprPrimaryContext>(_ctx, getState());
  enterRule(_localctx, 84, KodaParser::RuleExprPrimary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(623);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 60, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(607);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprBooleanContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(608);
      match(KodaParser::BOOLEAN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(609);
      match(KodaParser::IDENT);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(610);
      match(KodaParser::STRING);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(611);
      match(KodaParser::NATURAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(612);
      match(KodaParser::REAL);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<KodaParser::ExprRecordContext>(_localctx);
      enterOuterAlt(_localctx, 7);
      setState(613);
      recordLiteral();
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<KodaParser::ExprListLiteralContext>(_localctx);
      enterOuterAlt(_localctx, 8);
      setState(614);
      listLiteral();
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<KodaParser::ExprMapLiteralContext>(_localctx);
      enterOuterAlt(_localctx, 9);
      setState(615);
      mapLiteral();
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<KodaParser::ExprDataAccessContext>(_localctx);
      enterOuterAlt(_localctx, 10);
      setState(616);
      match(KodaParser::IDENT);
      setState(617);
      match(KodaParser::DOT);
      setState(618);
      match(KodaParser::IDENT);
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 11);
      setState(619);
      match(KodaParser::LPAREN);
      setState(620);
      expression();
      setState(621);
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
  enterRule(_localctx, 86, KodaParser::RuleRecordLiteral);
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
    setState(625);
    match(KodaParser::LBRACE);
    setState(634);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::IDENT) {
      setState(626);
      recordFieldInitializer();
      setState(631);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(627);
        match(KodaParser::COMMA);
        setState(628);
        recordFieldInitializer();
        setState(633);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(636);
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
  enterRule(_localctx, 88, KodaParser::RuleRecordFieldInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(638);
    match(KodaParser::IDENT);
    setState(639);
    match(KodaParser::COLON);
    setState(640);
    expression();
   
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
  enterRule(_localctx, 90, KodaParser::RuleListLiteral);
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
    setState(642);
    match(KodaParser::LBRACK);
    setState(651);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 17) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 17)) & ((1ULL << (KodaParser::TRIGGER - 17))
      | (1ULL << (KodaParser::RETURN - 17))
      | (1ULL << (KodaParser::ABORT - 17))
      | (1ULL << (KodaParser::ERROR - 17))
      | (1ULL << (KodaParser::IN - 17))
      | (1ULL << (KodaParser::OUT - 17))
      | (1ULL << (KodaParser::CONSUMES - 17))
      | (1ULL << (KodaParser::PRODUCES - 17))
      | (1ULL << (KodaParser::MINUS - 17))
      | (1ULL << (KodaParser::NOT - 17))
      | (1ULL << (KodaParser::LPAREN - 17))
      | (1ULL << (KodaParser::LBRACE - 17))
      | (1ULL << (KodaParser::LBRACK - 17))
      | (1ULL << (KodaParser::NATURAL - 17))
      | (1ULL << (KodaParser::REAL - 17))
      | (1ULL << (KodaParser::BOOLEAN - 17))
      | (1ULL << (KodaParser::IDENT - 17))
      | (1ULL << (KodaParser::STRING - 17)))) != 0)) {
      setState(643);
      expression();
      setState(648);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(644);
        match(KodaParser::COMMA);
        setState(645);
        expression();
        setState(650);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(653);
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
  enterRule(_localctx, 92, KodaParser::RuleMapLiteral);
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
    setState(655);
    match(KodaParser::LBRACE);
    setState(664);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LT) {
      setState(656);
      mapFieldInitializer();
      setState(661);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(657);
        match(KodaParser::COMMA);
        setState(658);
        mapFieldInitializer();
        setState(663);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(666);
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

tree::TerminalNode* KodaParser::MapFieldInitializerContext::LT() {
  return getToken(KodaParser::LT, 0);
}

std::vector<KodaParser::ExpressionContext *> KodaParser::MapFieldInitializerContext::expression() {
  return getRuleContexts<KodaParser::ExpressionContext>();
}

KodaParser::ExpressionContext* KodaParser::MapFieldInitializerContext::expression(size_t i) {
  return getRuleContext<KodaParser::ExpressionContext>(i);
}

tree::TerminalNode* KodaParser::MapFieldInitializerContext::COMMA() {
  return getToken(KodaParser::COMMA, 0);
}

tree::TerminalNode* KodaParser::MapFieldInitializerContext::GT() {
  return getToken(KodaParser::GT, 0);
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
  enterRule(_localctx, 94, KodaParser::RuleMapFieldInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(668);
    match(KodaParser::LT);
    setState(669);
    expression();
    setState(670);
    match(KodaParser::COMMA);
    setState(671);
    expression();
    setState(672);
    match(KodaParser::GT);
   
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
  enterRule(_localctx, 96, KodaParser::RuleIdentifier);
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
    setState(674);
    _la = _input->LA(1);
    if (!(((((_la - 17) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 17)) & ((1ULL << (KodaParser::TRIGGER - 17))
      | (1ULL << (KodaParser::RETURN - 17))
      | (1ULL << (KodaParser::ABORT - 17))
      | (1ULL << (KodaParser::ERROR - 17))
      | (1ULL << (KodaParser::IN - 17))
      | (1ULL << (KodaParser::OUT - 17))
      | (1ULL << (KodaParser::CONSUMES - 17))
      | (1ULL << (KodaParser::PRODUCES - 17))
      | (1ULL << (KodaParser::IDENT - 17)))) != 0))) {
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
    case 28: return strategySempred(antlrcpp::downCast<StrategyContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool KodaParser::strategySempred(StrategyContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 11);

  default:
    break;
  }
  return true;
}

void KodaParser::initialize() {
  std::call_once(kodaParserOnceFlag, kodaParserInitialize);
}
