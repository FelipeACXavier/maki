
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
      "enumDeclaration", "fieldDeclaration", "enumValue", "enumLiteral", 
      "typeReference", "qualifiedName", "argumentList", "argument", "statement", 
      "tasksBlock", "flow", "identList", "varsBlock", "variableStatement", 
      "actionBlock", "serviceBlock", "topicBlock", "rosDefStatement", "eventDefStatement", 
      "eventDefComponentList", "eventDefComponent", "rosData", "whenMode", 
      "timeUnit", "strategy", "strategyHandler", "eventStatement", "exprList", 
      "expression", "exprOr", "exprAnd", "exprCmp", "compOp", "exprNot", 
      "exprAdd", "exprMul", "exprUnary", "exprPrimary", "identifier"
    },
    std::vector<std::string>{
      "", "'task'", "'capability'", "'type'", "'enum'", "'extends'", "'list'", 
      "'optional'", "'map'", "'strategy'", "'vars'", "'action'", "'service'", 
      "'topic'", "'trigger'", "'return'", "'abort'", "'error'", "'in'", 
      "'out'", "'on'", "'req'", "'pro'", "'end'", "'continue'", "'repeat'", 
      "'join'", "'either'", "'let'", "'within'", "'do'", "'else'", "'if'", 
      "'then'", "'guard'", "'every'", "'timeout'", "'allowed'", "'reply'", 
      "'after'", "'once'", "'always'", "'mission'", "'idle'", "'s'", "'ms'", 
      "'us'", "'ns'", "'-->'", "'|'", "'=='", "'!='", "'<='", "'>='", "'<'", 
      "'>'", "'='", "'::'", "':'", "','", "';'", "'.'", "'+'", "'-'", "'*'", 
      "'/'", "'!'", "'and'", "'or'", "'('", "')'", "'{'", "'}'", "'['", 
      "']'"
    },
    std::vector<std::string>{
      "", "TASK", "CAPABILITY", "TYPE", "ENUM", "EXTENDS", "LIST", "OPTIONAL", 
      "MAP", "STRATEGY", "VARS", "ACTION", "SERVICE", "TOPIC", "TRIGGER", 
      "RETURN", "ABORT", "ERROR", "IN", "OUT", "ON", "REQ", "PRO", "END", 
      "CONTINUE", "REPEAT", "JOIN", "EITHER", "LET", "WITHIN", "DO", "ELSE", 
      "IF", "THEN", "GUARD", "EVERY", "TIMEOUT", "ALLOWED", "REPLY", "AFTER", 
      "ONCE", "ALWAYS", "MISSION", "IDLE", "S", "MS", "US", "NS", "ARROW", 
      "PIPE", "EQ", "NEQ", "LEQ", "GEQ", "LT", "GT", "ASSIGN", "DOUBLE_COLON", 
      "COLON", "COMMA", "SEMI", "DOT", "PLUS", "MINUS", "STAR", "SLASH", 
      "NOT", "AND", "OR", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACK", 
      "RBRACK", "NATURAL", "REAL", "IDENT", "STRING", "ANY", "LINE_COMMENT", 
      "BLOCK_COMMENT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,82,630,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,1,0,5,0,88,8,0,10,0,12,0,91,9,0,1,0,1,0,1,1,1,1,1,1,3,1,98,8,1,1,2,
  	1,2,1,2,1,2,3,2,104,8,2,1,2,1,2,1,2,5,2,109,8,2,10,2,12,2,112,9,2,1,2,
  	1,2,1,2,1,2,1,2,3,2,119,8,2,1,2,1,2,1,2,5,2,124,8,2,10,2,12,2,127,9,2,
  	1,2,3,2,130,8,2,1,3,1,3,1,3,1,3,3,3,136,8,3,1,3,1,3,5,3,140,8,3,10,3,
  	12,3,143,9,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,153,8,3,1,4,1,4,1,4,
  	1,4,3,4,159,8,4,1,4,1,4,5,4,163,8,4,10,4,12,4,166,9,4,1,4,1,4,1,5,1,5,
  	1,5,1,5,1,5,1,6,1,6,1,6,3,6,178,8,6,1,6,1,6,1,7,1,7,3,7,184,8,7,1,7,3,
  	7,187,8,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,
  	1,8,1,8,1,8,3,8,207,8,8,1,9,1,9,1,9,5,9,212,8,9,10,9,12,9,215,9,9,1,10,
  	1,10,1,10,5,10,220,8,10,10,10,12,10,223,9,10,1,11,1,11,1,11,1,11,1,11,
  	1,11,1,11,1,11,3,11,233,8,11,1,12,1,12,1,12,1,12,1,12,1,12,3,12,241,8,
  	12,1,13,1,13,1,13,4,13,246,8,13,11,13,12,13,247,1,13,1,13,1,14,1,14,1,
  	14,3,14,255,8,14,1,14,3,14,258,8,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,
  	5,15,267,8,15,10,15,12,15,270,9,15,1,16,1,16,1,16,4,16,275,8,16,11,16,
  	12,16,276,1,16,1,16,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,18,
  	1,18,1,18,5,18,293,8,18,10,18,12,18,296,9,18,1,18,1,18,1,19,1,19,1,19,
  	1,19,1,19,5,19,305,8,19,10,19,12,19,308,9,19,1,19,1,19,1,20,1,20,1,20,
  	1,20,1,20,5,20,317,8,20,10,20,12,20,320,9,20,1,20,1,20,1,21,1,21,1,21,
  	1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,
  	1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,
  	354,8,21,1,22,1,22,1,22,1,22,3,22,360,8,22,1,22,1,22,1,22,3,22,365,8,
  	22,1,23,1,23,1,23,4,23,370,8,23,11,23,12,23,371,1,24,1,24,1,24,1,24,1,
  	24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,
  	24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,5,24,402,8,24,10,24,12,
  	24,405,9,24,1,24,3,24,408,8,24,1,24,1,24,1,24,5,24,413,8,24,10,24,12,
  	24,416,9,24,1,24,3,24,419,8,24,1,24,1,24,1,24,5,24,424,8,24,10,24,12,
  	24,427,9,24,1,24,3,24,430,8,24,3,24,432,8,24,1,25,1,25,1,26,1,26,1,27,
  	1,27,1,28,1,28,1,28,1,28,1,28,1,28,4,28,446,8,28,11,28,12,28,447,1,28,
  	1,28,1,28,1,28,1,28,1,28,1,28,4,28,457,8,28,11,28,12,28,458,1,28,1,28,
  	1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,
  	1,28,1,28,1,28,3,28,480,8,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,5,28,
  	489,8,28,10,28,12,28,492,9,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,
  	1,28,1,28,5,28,504,8,28,10,28,12,28,507,9,28,1,28,1,28,1,28,1,28,3,28,
  	513,8,28,1,28,1,28,1,28,4,28,518,8,28,11,28,12,28,519,5,28,522,8,28,10,
  	28,12,28,525,9,28,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,3,
  	29,537,8,29,1,30,1,30,1,30,1,30,1,30,3,30,544,8,30,1,30,1,30,1,30,1,30,
  	1,30,3,30,551,8,30,1,30,1,30,3,30,555,8,30,1,31,1,31,1,31,5,31,560,8,
  	31,10,31,12,31,563,9,31,1,32,1,32,1,33,1,33,1,33,5,33,570,8,33,10,33,
  	12,33,573,9,33,1,34,1,34,1,34,5,34,578,8,34,10,34,12,34,581,9,34,1,35,
  	1,35,1,35,1,35,3,35,587,8,35,1,36,1,36,1,37,1,37,1,37,3,37,594,8,37,1,
  	38,1,38,1,38,5,38,599,8,38,10,38,12,38,602,9,38,1,39,1,39,1,39,5,39,607,
  	8,39,10,39,12,39,610,9,39,1,40,1,40,1,40,3,40,615,8,40,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,3,41,626,8,41,1,42,1,42,1,42,0,1,56,43,
  	0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,
  	50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,0,7,1,0,11,13,1,
  	0,41,43,1,0,44,47,1,0,50,55,1,0,62,63,1,0,64,65,2,0,14,19,77,77,679,0,
  	89,1,0,0,0,2,97,1,0,0,0,4,129,1,0,0,0,6,152,1,0,0,0,8,154,1,0,0,0,10,
  	169,1,0,0,0,12,174,1,0,0,0,14,186,1,0,0,0,16,206,1,0,0,0,18,208,1,0,0,
  	0,20,216,1,0,0,0,22,232,1,0,0,0,24,240,1,0,0,0,26,242,1,0,0,0,28,251,
  	1,0,0,0,30,263,1,0,0,0,32,271,1,0,0,0,34,280,1,0,0,0,36,287,1,0,0,0,38,
  	299,1,0,0,0,40,311,1,0,0,0,42,353,1,0,0,0,44,355,1,0,0,0,46,366,1,0,0,
  	0,48,431,1,0,0,0,50,433,1,0,0,0,52,435,1,0,0,0,54,437,1,0,0,0,56,512,
  	1,0,0,0,58,536,1,0,0,0,60,554,1,0,0,0,62,556,1,0,0,0,64,564,1,0,0,0,66,
  	566,1,0,0,0,68,574,1,0,0,0,70,582,1,0,0,0,72,588,1,0,0,0,74,593,1,0,0,
  	0,76,595,1,0,0,0,78,603,1,0,0,0,80,614,1,0,0,0,82,625,1,0,0,0,84,627,
  	1,0,0,0,86,88,3,2,1,0,87,86,1,0,0,0,88,91,1,0,0,0,89,87,1,0,0,0,89,90,
  	1,0,0,0,90,92,1,0,0,0,91,89,1,0,0,0,92,93,5,0,0,1,93,1,1,0,0,0,94,98,
  	3,4,2,0,95,98,3,6,3,0,96,98,3,8,4,0,97,94,1,0,0,0,97,95,1,0,0,0,97,96,
  	1,0,0,0,98,3,1,0,0,0,99,100,5,1,0,0,100,101,5,77,0,0,101,103,5,69,0,0,
  	102,104,3,20,10,0,103,102,1,0,0,0,103,104,1,0,0,0,104,105,1,0,0,0,105,
  	106,5,70,0,0,106,110,5,71,0,0,107,109,3,24,12,0,108,107,1,0,0,0,109,112,
  	1,0,0,0,110,108,1,0,0,0,110,111,1,0,0,0,111,113,1,0,0,0,112,110,1,0,0,
  	0,113,130,5,72,0,0,114,115,5,2,0,0,115,116,5,77,0,0,116,118,5,69,0,0,
  	117,119,3,20,10,0,118,117,1,0,0,0,118,119,1,0,0,0,119,120,1,0,0,0,120,
  	121,5,70,0,0,121,125,5,71,0,0,122,124,3,24,12,0,123,122,1,0,0,0,124,127,
  	1,0,0,0,125,123,1,0,0,0,125,126,1,0,0,0,126,128,1,0,0,0,127,125,1,0,0,
  	0,128,130,5,72,0,0,129,99,1,0,0,0,129,114,1,0,0,0,130,5,1,0,0,0,131,132,
  	5,3,0,0,132,135,3,18,9,0,133,134,5,5,0,0,134,136,3,18,9,0,135,133,1,0,
  	0,0,135,136,1,0,0,0,136,137,1,0,0,0,137,141,5,71,0,0,138,140,3,10,5,0,
  	139,138,1,0,0,0,140,143,1,0,0,0,141,139,1,0,0,0,141,142,1,0,0,0,142,144,
  	1,0,0,0,143,141,1,0,0,0,144,145,5,72,0,0,145,153,1,0,0,0,146,147,5,3,
  	0,0,147,148,3,18,9,0,148,149,5,56,0,0,149,150,3,16,8,0,150,151,5,60,0,
  	0,151,153,1,0,0,0,152,131,1,0,0,0,152,146,1,0,0,0,153,7,1,0,0,0,154,155,
  	5,4,0,0,155,158,3,18,9,0,156,157,5,58,0,0,157,159,3,16,8,0,158,156,1,
  	0,0,0,158,159,1,0,0,0,159,160,1,0,0,0,160,164,5,71,0,0,161,163,3,12,6,
  	0,162,161,1,0,0,0,163,166,1,0,0,0,164,162,1,0,0,0,164,165,1,0,0,0,165,
  	167,1,0,0,0,166,164,1,0,0,0,167,168,5,72,0,0,168,9,1,0,0,0,169,170,5,
  	77,0,0,170,171,5,58,0,0,171,172,3,16,8,0,172,173,5,60,0,0,173,11,1,0,
  	0,0,174,177,5,77,0,0,175,176,5,56,0,0,176,178,3,14,7,0,177,175,1,0,0,
  	0,177,178,1,0,0,0,178,179,1,0,0,0,179,180,5,60,0,0,180,13,1,0,0,0,181,
  	187,5,78,0,0,182,184,5,63,0,0,183,182,1,0,0,0,183,184,1,0,0,0,184,185,
  	1,0,0,0,185,187,5,75,0,0,186,181,1,0,0,0,186,183,1,0,0,0,187,15,1,0,0,
  	0,188,207,3,18,9,0,189,190,5,6,0,0,190,191,5,54,0,0,191,192,3,16,8,0,
  	192,193,5,55,0,0,193,207,1,0,0,0,194,195,5,7,0,0,195,196,5,54,0,0,196,
  	197,3,16,8,0,197,198,5,55,0,0,198,207,1,0,0,0,199,200,5,8,0,0,200,201,
  	5,54,0,0,201,202,3,16,8,0,202,203,5,59,0,0,203,204,3,16,8,0,204,205,5,
  	55,0,0,205,207,1,0,0,0,206,188,1,0,0,0,206,189,1,0,0,0,206,194,1,0,0,
  	0,206,199,1,0,0,0,207,17,1,0,0,0,208,213,5,77,0,0,209,210,5,57,0,0,210,
  	212,5,77,0,0,211,209,1,0,0,0,212,215,1,0,0,0,213,211,1,0,0,0,213,214,
  	1,0,0,0,214,19,1,0,0,0,215,213,1,0,0,0,216,221,3,22,11,0,217,218,5,59,
  	0,0,218,220,3,22,11,0,219,217,1,0,0,0,220,223,1,0,0,0,221,219,1,0,0,0,
  	221,222,1,0,0,0,222,21,1,0,0,0,223,221,1,0,0,0,224,225,5,77,0,0,225,233,
  	5,77,0,0,226,227,5,77,0,0,227,228,5,21,0,0,228,233,5,77,0,0,229,230,5,
  	77,0,0,230,231,5,22,0,0,231,233,5,77,0,0,232,224,1,0,0,0,232,226,1,0,
  	0,0,232,229,1,0,0,0,233,23,1,0,0,0,234,241,3,26,13,0,235,241,3,32,16,
  	0,236,241,3,36,18,0,237,241,3,38,19,0,238,241,3,40,20,0,239,241,3,42,
  	21,0,240,234,1,0,0,0,240,235,1,0,0,0,240,236,1,0,0,0,240,237,1,0,0,0,
  	240,238,1,0,0,0,240,239,1,0,0,0,241,25,1,0,0,0,242,243,5,9,0,0,243,245,
  	5,71,0,0,244,246,3,28,14,0,245,244,1,0,0,0,246,247,1,0,0,0,247,245,1,
  	0,0,0,247,248,1,0,0,0,248,249,1,0,0,0,249,250,5,72,0,0,250,27,1,0,0,0,
  	251,257,5,77,0,0,252,254,5,73,0,0,253,255,3,30,15,0,254,253,1,0,0,0,254,
  	255,1,0,0,0,255,256,1,0,0,0,256,258,5,74,0,0,257,252,1,0,0,0,257,258,
  	1,0,0,0,258,259,1,0,0,0,259,260,5,58,0,0,260,261,3,56,28,0,261,262,5,
  	60,0,0,262,29,1,0,0,0,263,268,5,77,0,0,264,265,5,59,0,0,265,267,5,77,
  	0,0,266,264,1,0,0,0,267,270,1,0,0,0,268,266,1,0,0,0,268,269,1,0,0,0,269,
  	31,1,0,0,0,270,268,1,0,0,0,271,272,5,10,0,0,272,274,5,71,0,0,273,275,
  	3,34,17,0,274,273,1,0,0,0,275,276,1,0,0,0,276,274,1,0,0,0,276,277,1,0,
  	0,0,277,278,1,0,0,0,278,279,5,72,0,0,279,33,1,0,0,0,280,281,5,77,0,0,
  	281,282,5,77,0,0,282,283,5,56,0,0,283,284,3,64,32,0,284,285,5,58,0,0,
  	285,286,3,64,32,0,286,35,1,0,0,0,287,288,5,11,0,0,288,289,5,78,0,0,289,
  	290,5,78,0,0,290,294,5,71,0,0,291,293,3,42,21,0,292,291,1,0,0,0,293,296,
  	1,0,0,0,294,292,1,0,0,0,294,295,1,0,0,0,295,297,1,0,0,0,296,294,1,0,0,
  	0,297,298,5,72,0,0,298,37,1,0,0,0,299,300,5,12,0,0,300,301,5,78,0,0,301,
  	302,5,78,0,0,302,306,5,71,0,0,303,305,3,42,21,0,304,303,1,0,0,0,305,308,
  	1,0,0,0,306,304,1,0,0,0,306,307,1,0,0,0,307,309,1,0,0,0,308,306,1,0,0,
  	0,309,310,5,72,0,0,310,39,1,0,0,0,311,312,5,13,0,0,312,313,5,78,0,0,313,
  	314,5,78,0,0,314,318,5,71,0,0,315,317,3,42,21,0,316,315,1,0,0,0,317,320,
  	1,0,0,0,318,316,1,0,0,0,318,319,1,0,0,0,319,321,1,0,0,0,320,318,1,0,0,
  	0,321,322,5,72,0,0,322,41,1,0,0,0,323,324,5,14,0,0,324,325,5,58,0,0,325,
  	326,3,44,22,0,326,327,5,60,0,0,327,354,1,0,0,0,328,329,5,15,0,0,329,330,
  	5,58,0,0,330,331,3,44,22,0,331,332,5,60,0,0,332,354,1,0,0,0,333,334,5,
  	16,0,0,334,335,5,58,0,0,335,336,3,44,22,0,336,337,5,60,0,0,337,354,1,
  	0,0,0,338,339,5,17,0,0,339,340,5,58,0,0,340,341,3,44,22,0,341,342,5,60,
  	0,0,342,354,1,0,0,0,343,344,5,18,0,0,344,345,5,58,0,0,345,346,3,44,22,
  	0,346,347,5,60,0,0,347,354,1,0,0,0,348,349,5,19,0,0,349,350,5,58,0,0,
  	350,351,3,44,22,0,351,352,5,60,0,0,352,354,1,0,0,0,353,323,1,0,0,0,353,
  	328,1,0,0,0,353,333,1,0,0,0,353,338,1,0,0,0,353,343,1,0,0,0,353,348,1,
  	0,0,0,354,43,1,0,0,0,355,356,5,77,0,0,356,357,3,84,42,0,357,359,5,69,
  	0,0,358,360,3,20,10,0,359,358,1,0,0,0,359,360,1,0,0,0,360,361,1,0,0,0,
  	361,364,5,70,0,0,362,363,5,58,0,0,363,365,3,46,23,0,364,362,1,0,0,0,364,
  	365,1,0,0,0,365,45,1,0,0,0,366,369,3,48,24,0,367,368,5,59,0,0,368,370,
  	3,48,24,0,369,367,1,0,0,0,370,371,1,0,0,0,371,369,1,0,0,0,371,372,1,0,
  	0,0,372,47,1,0,0,0,373,374,3,50,25,0,374,375,5,77,0,0,375,376,5,58,0,
  	0,376,377,5,78,0,0,377,378,5,77,0,0,378,379,5,78,0,0,379,380,5,77,0,0,
  	380,381,5,78,0,0,381,432,1,0,0,0,382,383,5,36,0,0,383,384,5,75,0,0,384,
  	385,3,54,27,0,385,386,5,48,0,0,386,387,5,77,0,0,387,432,1,0,0,0,388,389,
  	5,37,0,0,389,390,5,18,0,0,390,432,3,52,26,0,391,392,5,38,0,0,392,393,
  	5,77,0,0,393,432,3,52,26,0,394,395,5,39,0,0,395,432,5,77,0,0,396,397,
  	5,40,0,0,397,398,5,18,0,0,398,407,3,52,26,0,399,403,5,71,0,0,400,402,
  	3,24,12,0,401,400,1,0,0,0,402,405,1,0,0,0,403,401,1,0,0,0,403,404,1,0,
  	0,0,404,406,1,0,0,0,405,403,1,0,0,0,406,408,5,72,0,0,407,399,1,0,0,0,
  	407,408,1,0,0,0,408,432,1,0,0,0,409,418,5,14,0,0,410,414,5,71,0,0,411,
  	413,3,24,12,0,412,411,1,0,0,0,413,416,1,0,0,0,414,412,1,0,0,0,414,415,
  	1,0,0,0,415,417,1,0,0,0,416,414,1,0,0,0,417,419,5,72,0,0,418,410,1,0,
  	0,0,418,419,1,0,0,0,419,432,1,0,0,0,420,429,5,16,0,0,421,425,5,71,0,0,
  	422,424,3,24,12,0,423,422,1,0,0,0,424,427,1,0,0,0,425,423,1,0,0,0,425,
  	426,1,0,0,0,426,428,1,0,0,0,427,425,1,0,0,0,428,430,5,72,0,0,429,421,
  	1,0,0,0,429,430,1,0,0,0,430,432,1,0,0,0,431,373,1,0,0,0,431,382,1,0,0,
  	0,431,388,1,0,0,0,431,391,1,0,0,0,431,394,1,0,0,0,431,396,1,0,0,0,431,
  	409,1,0,0,0,431,420,1,0,0,0,432,49,1,0,0,0,433,434,7,0,0,0,434,51,1,0,
  	0,0,435,436,7,1,0,0,436,53,1,0,0,0,437,438,7,2,0,0,438,55,1,0,0,0,439,
  	440,6,28,-1,0,440,441,5,26,0,0,441,442,5,69,0,0,442,445,3,56,28,0,443,
  	444,5,49,0,0,444,446,3,56,28,0,445,443,1,0,0,0,446,447,1,0,0,0,447,445,
  	1,0,0,0,447,448,1,0,0,0,448,449,1,0,0,0,449,450,5,70,0,0,450,513,1,0,
  	0,0,451,452,5,27,0,0,452,453,5,69,0,0,453,456,3,56,28,0,454,455,5,49,
  	0,0,455,457,3,56,28,0,456,454,1,0,0,0,457,458,1,0,0,0,458,456,1,0,0,0,
  	458,459,1,0,0,0,459,460,1,0,0,0,460,461,5,70,0,0,461,513,1,0,0,0,462,
  	463,5,28,0,0,463,464,5,77,0,0,464,465,5,56,0,0,465,513,3,60,30,0,466,
  	467,5,29,0,0,467,468,5,75,0,0,468,469,5,30,0,0,469,470,3,56,28,0,470,
  	471,5,31,0,0,471,472,3,56,28,9,472,513,1,0,0,0,473,474,5,32,0,0,474,475,
  	3,64,32,0,475,476,5,33,0,0,476,479,3,56,28,0,477,478,5,31,0,0,478,480,
  	3,56,28,0,479,477,1,0,0,0,479,480,1,0,0,0,480,513,1,0,0,0,481,482,5,25,
  	0,0,482,483,5,75,0,0,483,484,5,75,0,0,484,485,5,69,0,0,485,486,3,56,28,
  	0,486,490,5,70,0,0,487,489,3,58,29,0,488,487,1,0,0,0,489,492,1,0,0,0,
  	490,488,1,0,0,0,490,491,1,0,0,0,491,513,1,0,0,0,492,490,1,0,0,0,493,494,
  	5,34,0,0,494,495,5,71,0,0,495,496,3,64,32,0,496,497,5,72,0,0,497,513,
  	1,0,0,0,498,513,5,23,0,0,499,513,5,24,0,0,500,513,3,84,42,0,501,505,3,
  	60,30,0,502,504,3,58,29,0,503,502,1,0,0,0,504,507,1,0,0,0,505,503,1,0,
  	0,0,505,506,1,0,0,0,506,513,1,0,0,0,507,505,1,0,0,0,508,509,5,69,0,0,
  	509,510,3,56,28,0,510,511,5,70,0,0,511,513,1,0,0,0,512,439,1,0,0,0,512,
  	451,1,0,0,0,512,462,1,0,0,0,512,466,1,0,0,0,512,473,1,0,0,0,512,481,1,
  	0,0,0,512,493,1,0,0,0,512,498,1,0,0,0,512,499,1,0,0,0,512,500,1,0,0,0,
  	512,501,1,0,0,0,512,508,1,0,0,0,513,523,1,0,0,0,514,517,10,13,0,0,515,
  	516,5,48,0,0,516,518,3,56,28,0,517,515,1,0,0,0,518,519,1,0,0,0,519,517,
  	1,0,0,0,519,520,1,0,0,0,520,522,1,0,0,0,521,514,1,0,0,0,522,525,1,0,0,
  	0,523,521,1,0,0,0,523,524,1,0,0,0,524,57,1,0,0,0,525,523,1,0,0,0,526,
  	527,5,20,0,0,527,528,5,17,0,0,528,537,3,56,28,0,529,530,5,20,0,0,530,
  	531,5,16,0,0,531,537,3,56,28,0,532,533,5,20,0,0,533,534,3,60,30,0,534,
  	535,3,56,28,0,535,537,1,0,0,0,536,526,1,0,0,0,536,529,1,0,0,0,536,532,
  	1,0,0,0,537,59,1,0,0,0,538,539,3,84,42,0,539,540,5,61,0,0,540,541,3,84,
  	42,0,541,543,5,69,0,0,542,544,3,62,31,0,543,542,1,0,0,0,543,544,1,0,0,
  	0,544,545,1,0,0,0,545,546,5,70,0,0,546,555,1,0,0,0,547,548,3,84,42,0,
  	548,550,5,69,0,0,549,551,3,62,31,0,550,549,1,0,0,0,550,551,1,0,0,0,551,
  	552,1,0,0,0,552,553,5,70,0,0,553,555,1,0,0,0,554,538,1,0,0,0,554,547,
  	1,0,0,0,555,61,1,0,0,0,556,561,3,64,32,0,557,558,5,59,0,0,558,560,3,64,
  	32,0,559,557,1,0,0,0,560,563,1,0,0,0,561,559,1,0,0,0,561,562,1,0,0,0,
  	562,63,1,0,0,0,563,561,1,0,0,0,564,565,3,66,33,0,565,65,1,0,0,0,566,571,
  	3,68,34,0,567,568,5,68,0,0,568,570,3,68,34,0,569,567,1,0,0,0,570,573,
  	1,0,0,0,571,569,1,0,0,0,571,572,1,0,0,0,572,67,1,0,0,0,573,571,1,0,0,
  	0,574,579,3,70,35,0,575,576,5,67,0,0,576,578,3,70,35,0,577,575,1,0,0,
  	0,578,581,1,0,0,0,579,577,1,0,0,0,579,580,1,0,0,0,580,69,1,0,0,0,581,
  	579,1,0,0,0,582,586,3,74,37,0,583,584,3,72,36,0,584,585,3,74,37,0,585,
  	587,1,0,0,0,586,583,1,0,0,0,586,587,1,0,0,0,587,71,1,0,0,0,588,589,7,
  	3,0,0,589,73,1,0,0,0,590,591,5,66,0,0,591,594,3,74,37,0,592,594,3,76,
  	38,0,593,590,1,0,0,0,593,592,1,0,0,0,594,75,1,0,0,0,595,600,3,78,39,0,
  	596,597,7,4,0,0,597,599,3,78,39,0,598,596,1,0,0,0,599,602,1,0,0,0,600,
  	598,1,0,0,0,600,601,1,0,0,0,601,77,1,0,0,0,602,600,1,0,0,0,603,608,3,
  	80,40,0,604,605,7,5,0,0,605,607,3,80,40,0,606,604,1,0,0,0,607,610,1,0,
  	0,0,608,606,1,0,0,0,608,609,1,0,0,0,609,79,1,0,0,0,610,608,1,0,0,0,611,
  	612,5,63,0,0,612,615,3,80,40,0,613,615,3,82,41,0,614,611,1,0,0,0,614,
  	613,1,0,0,0,615,81,1,0,0,0,616,626,3,60,30,0,617,626,5,77,0,0,618,626,
  	5,78,0,0,619,626,5,75,0,0,620,626,5,76,0,0,621,622,5,69,0,0,622,623,3,
  	64,32,0,623,624,5,70,0,0,624,626,1,0,0,0,625,616,1,0,0,0,625,617,1,0,
  	0,0,625,618,1,0,0,0,625,619,1,0,0,0,625,620,1,0,0,0,625,621,1,0,0,0,626,
  	83,1,0,0,0,627,628,7,6,0,0,628,85,1,0,0,0,60,89,97,103,110,118,125,129,
  	135,141,152,158,164,177,183,186,206,213,221,232,240,247,254,257,268,276,
  	294,306,318,353,359,364,371,403,407,414,418,425,429,431,447,458,479,490,
  	505,512,519,523,536,543,550,554,561,571,579,586,593,600,608,614,625
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
    setState(89);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TASK)
      | (1ULL << KodaParser::CAPABILITY)
      | (1ULL << KodaParser::TYPE)
      | (1ULL << KodaParser::ENUM))) != 0)) {
      setState(86);
      topLevelDeclaration();
      setState(91);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(92);
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
    setState(97);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK:
      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 1);
        setState(94);
        topLevelComponent();
        break;
      }

      case KodaParser::TYPE: {
        enterOuterAlt(_localctx, 2);
        setState(95);
        typeDeclaration();
        break;
      }

      case KodaParser::ENUM: {
        enterOuterAlt(_localctx, 3);
        setState(96);
        enumDeclaration();
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
    setState(129);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(99);
        match(KodaParser::TASK);
        setState(100);
        match(KodaParser::IDENT);
        setState(101);
        match(KodaParser::LPAREN);
        setState(103);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::IDENT) {
          setState(102);
          argumentList();
        }
        setState(105);
        match(KodaParser::RPAREN);
        setState(106);
        match(KodaParser::LBRACE);
        setState(110);
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
          setState(107);
          statement();
          setState(112);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(113);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(114);
        match(KodaParser::CAPABILITY);
        setState(115);
        match(KodaParser::IDENT);
        setState(116);
        match(KodaParser::LPAREN);
        setState(118);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::IDENT) {
          setState(117);
          argumentList();
        }
        setState(120);
        match(KodaParser::RPAREN);
        setState(121);
        match(KodaParser::LBRACE);
        setState(125);
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
          setState(122);
          statement();
          setState(127);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(128);
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
    setState(152);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::TypeRecordContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(131);
      match(KodaParser::TYPE);
      setState(132);
      qualifiedName();
      setState(135);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::EXTENDS) {
        setState(133);
        match(KodaParser::EXTENDS);
        setState(134);
        qualifiedName();
      }
      setState(137);
      match(KodaParser::LBRACE);
      setState(141);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::IDENT) {
        setState(138);
        fieldDeclaration();
        setState(143);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(144);
      match(KodaParser::RBRACE);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::TypeAliasContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(146);
      match(KodaParser::TYPE);
      setState(147);
      qualifiedName();
      setState(148);
      match(KodaParser::ASSIGN);
      setState(149);
      typeReference();
      setState(150);
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
  enterRule(_localctx, 8, KodaParser::RuleEnumDeclaration);
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
  enterRule(_localctx, 10, KodaParser::RuleFieldDeclaration);

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
    setState(170);
    match(KodaParser::COLON);
    setState(171);
    typeReference();
    setState(172);
    match(KodaParser::SEMI);
   
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
    setState(174);
    match(KodaParser::IDENT);
    setState(177);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(175);
      match(KodaParser::ASSIGN);
      setState(176);
      enumLiteral();
    }
    setState(179);
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
    setState(186);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(181);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::MINUS:
      case KodaParser::NATURAL: {
        enterOuterAlt(_localctx, 2);
        setState(183);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::MINUS) {
          setState(182);
          match(KodaParser::MINUS);
        }
        setState(185);
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
  enterRule(_localctx, 16, KodaParser::RuleTypeReference);

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
  enterRule(_localctx, 18, KodaParser::RuleQualifiedName);
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
  enterRule(_localctx, 20, KodaParser::RuleArgumentList);
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

std::vector<tree::TerminalNode *> KodaParser::ArgPlainContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::ArgPlainContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

KodaParser::ArgPlainContext::ArgPlainContext(ArgumentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ArgPlainContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitArgPlain(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ArgReqContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> KodaParser::ArgReqContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::ArgReqContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

tree::TerminalNode* KodaParser::ArgReqContext::REQ() {
  return getToken(KodaParser::REQ, 0);
}

KodaParser::ArgReqContext::ArgReqContext(ArgumentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::ArgReqContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitArgReq(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ArgProContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> KodaParser::ArgProContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::ArgProContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

tree::TerminalNode* KodaParser::ArgProContext::PRO() {
  return getToken(KodaParser::PRO, 0);
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
  enterRule(_localctx, 22, KodaParser::RuleArgument);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(232);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(224);
      match(KodaParser::IDENT);
      setState(225);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(226);
      match(KodaParser::IDENT);
      setState(227);
      match(KodaParser::REQ);
      setState(228);
      match(KodaParser::IDENT);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(229);
      match(KodaParser::IDENT);
      setState(230);
      match(KodaParser::PRO);
      setState(231);
      match(KodaParser::IDENT);
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
  enterRule(_localctx, 24, KodaParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(240);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(234);
        tasksBlock();
        break;
      }

      case KodaParser::VARS: {
        enterOuterAlt(_localctx, 2);
        setState(235);
        varsBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 3);
        setState(236);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 4);
        setState(237);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 5);
        setState(238);
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
        setState(239);
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
  enterRule(_localctx, 26, KodaParser::RuleTasksBlock);
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
    setState(242);
    match(KodaParser::STRATEGY);
    setState(243);
    match(KodaParser::LBRACE);
    setState(245); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(244);
      flow();
      setState(247); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(249);
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
  enterRule(_localctx, 28, KodaParser::RuleFlow);
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
    setState(251);
    match(KodaParser::IDENT);
    setState(257);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(252);
      match(KodaParser::LBRACK);
      setState(254);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(253);
        identList();
      }
      setState(256);
      match(KodaParser::RBRACK);
    }
    setState(259);
    match(KodaParser::COLON);
    setState(260);
    strategy(0);
    setState(261);
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
  enterRule(_localctx, 30, KodaParser::RuleIdentList);
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
    setState(263);
    match(KodaParser::IDENT);
    setState(268);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(264);
      match(KodaParser::COMMA);
      setState(265);
      match(KodaParser::IDENT);
      setState(270);
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
  enterRule(_localctx, 32, KodaParser::RuleVarsBlock);
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
    setState(271);
    match(KodaParser::VARS);
    setState(272);
    match(KodaParser::LBRACE);
    setState(274); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(273);
      variableStatement();
      setState(276); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(278);
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

std::vector<tree::TerminalNode *> KodaParser::VariableStatementContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::VariableStatementContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
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
  enterRule(_localctx, 34, KodaParser::RuleVariableStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(280);
    match(KodaParser::IDENT);
    setState(281);
    match(KodaParser::IDENT);
    setState(282);
    match(KodaParser::ASSIGN);
    setState(283);
    expression();
    setState(284);
    match(KodaParser::COLON);
    setState(285);
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
  enterRule(_localctx, 36, KodaParser::RuleActionBlock);
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
    setState(287);
    match(KodaParser::ACTION);
    setState(288);
    match(KodaParser::STRING);
    setState(289);
    match(KodaParser::STRING);
    setState(290);
    match(KodaParser::LBRACE);
    setState(294);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT))) != 0)) {
      setState(291);
      rosDefStatement();
      setState(296);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(297);
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
  enterRule(_localctx, 38, KodaParser::RuleServiceBlock);
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
    setState(299);
    match(KodaParser::SERVICE);
    setState(300);
    match(KodaParser::STRING);
    setState(301);
    match(KodaParser::STRING);
    setState(302);
    match(KodaParser::LBRACE);
    setState(306);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT))) != 0)) {
      setState(303);
      rosDefStatement();
      setState(308);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(309);
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
  enterRule(_localctx, 40, KodaParser::RuleTopicBlock);
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
    setState(311);
    match(KodaParser::TOPIC);
    setState(312);
    match(KodaParser::STRING);
    setState(313);
    match(KodaParser::STRING);
    setState(314);
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
      | (1ULL << KodaParser::OUT))) != 0)) {
      setState(315);
      rosDefStatement();
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
  enterRule(_localctx, 42, KodaParser::RuleRosDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(353);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(323);
        match(KodaParser::TRIGGER);
        setState(324);
        match(KodaParser::COLON);
        setState(325);
        eventDefStatement();
        setState(326);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(328);
        match(KodaParser::RETURN);
        setState(329);
        match(KodaParser::COLON);
        setState(330);
        eventDefStatement();
        setState(331);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(333);
        match(KodaParser::ABORT);
        setState(334);
        match(KodaParser::COLON);
        setState(335);
        eventDefStatement();
        setState(336);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(338);
        match(KodaParser::ERROR);
        setState(339);
        match(KodaParser::COLON);
        setState(340);
        eventDefStatement();
        setState(341);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(343);
        match(KodaParser::IN);
        setState(344);
        match(KodaParser::COLON);
        setState(345);
        eventDefStatement();
        setState(346);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(348);
        match(KodaParser::OUT);
        setState(349);
        match(KodaParser::COLON);
        setState(350);
        eventDefStatement();
        setState(351);
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

tree::TerminalNode* KodaParser::EventDefStatementContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::EventDefComponentListContext* KodaParser::EventDefStatementContext::eventDefComponentList() {
  return getRuleContext<KodaParser::EventDefComponentListContext>(0);
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
  enterRule(_localctx, 44, KodaParser::RuleEventDefStatement);
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
    match(KodaParser::IDENT);
    setState(356);
    identifier();
    setState(357);
    match(KodaParser::LPAREN);
    setState(359);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::IDENT) {
      setState(358);
      argumentList();
    }
    setState(361);
    match(KodaParser::RPAREN);
    setState(364);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(362);
      match(KodaParser::COLON);
      setState(363);
      eventDefComponentList();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EventDefComponentListContext ------------------------------------------------------------------

KodaParser::EventDefComponentListContext::EventDefComponentListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::EventDefComponentContext *> KodaParser::EventDefComponentListContext::eventDefComponent() {
  return getRuleContexts<KodaParser::EventDefComponentContext>();
}

KodaParser::EventDefComponentContext* KodaParser::EventDefComponentListContext::eventDefComponent(size_t i) {
  return getRuleContext<KodaParser::EventDefComponentContext>(i);
}

std::vector<tree::TerminalNode *> KodaParser::EventDefComponentListContext::COMMA() {
  return getTokens(KodaParser::COMMA);
}

tree::TerminalNode* KodaParser::EventDefComponentListContext::COMMA(size_t i) {
  return getToken(KodaParser::COMMA, i);
}


size_t KodaParser::EventDefComponentListContext::getRuleIndex() const {
  return KodaParser::RuleEventDefComponentList;
}


std::any KodaParser::EventDefComponentListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEventDefComponentList(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::EventDefComponentListContext* KodaParser::eventDefComponentList() {
  EventDefComponentListContext *_localctx = _tracker.createInstance<EventDefComponentListContext>(_ctx, getState());
  enterRule(_localctx, 46, KodaParser::RuleEventDefComponentList);
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
    setState(366);
    eventDefComponent();
    setState(369); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(367);
      match(KodaParser::COMMA);
      setState(368);
      eventDefComponent();
      setState(371); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::COMMA);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EventDefComponentContext ------------------------------------------------------------------

KodaParser::EventDefComponentContext::EventDefComponentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t KodaParser::EventDefComponentContext::getRuleIndex() const {
  return KodaParser::RuleEventDefComponent;
}

void KodaParser::EventDefComponentContext::copyFrom(EventDefComponentContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- EdcOnceInContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcOnceInContext::ONCE() {
  return getToken(KodaParser::ONCE, 0);
}

tree::TerminalNode* KodaParser::EdcOnceInContext::IN() {
  return getToken(KodaParser::IN, 0);
}

KodaParser::WhenModeContext* KodaParser::EdcOnceInContext::whenMode() {
  return getRuleContext<KodaParser::WhenModeContext>(0);
}

tree::TerminalNode* KodaParser::EdcOnceInContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::EdcOnceInContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::StatementContext *> KodaParser::EdcOnceInContext::statement() {
  return getRuleContexts<KodaParser::StatementContext>();
}

KodaParser::StatementContext* KodaParser::EdcOnceInContext::statement(size_t i) {
  return getRuleContext<KodaParser::StatementContext>(i);
}

KodaParser::EdcOnceInContext::EdcOnceInContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcOnceInContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcOnceIn(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcStartContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcStartContext::TRIGGER() {
  return getToken(KodaParser::TRIGGER, 0);
}

tree::TerminalNode* KodaParser::EdcStartContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::EdcStartContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::StatementContext *> KodaParser::EdcStartContext::statement() {
  return getRuleContexts<KodaParser::StatementContext>();
}

KodaParser::StatementContext* KodaParser::EdcStartContext::statement(size_t i) {
  return getRuleContext<KodaParser::StatementContext>(i);
}

KodaParser::EdcStartContext::EdcStartContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcStartContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcStart(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcResetContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcResetContext::ABORT() {
  return getToken(KodaParser::ABORT, 0);
}

tree::TerminalNode* KodaParser::EdcResetContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::EdcResetContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::StatementContext *> KodaParser::EdcResetContext::statement() {
  return getRuleContexts<KodaParser::StatementContext>();
}

KodaParser::StatementContext* KodaParser::EdcResetContext::statement(size_t i) {
  return getRuleContext<KodaParser::StatementContext>(i);
}

KodaParser::EdcResetContext::EdcResetContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcResetContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcReset(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcDependsAfterContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcDependsAfterContext::AFTER() {
  return getToken(KodaParser::AFTER, 0);
}

tree::TerminalNode* KodaParser::EdcDependsAfterContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

KodaParser::EdcDependsAfterContext::EdcDependsAfterContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcDependsAfterContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcDependsAfter(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcWhenAllowedInContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcWhenAllowedInContext::ALLOWED() {
  return getToken(KodaParser::ALLOWED, 0);
}

tree::TerminalNode* KodaParser::EdcWhenAllowedInContext::IN() {
  return getToken(KodaParser::IN, 0);
}

KodaParser::WhenModeContext* KodaParser::EdcWhenAllowedInContext::whenMode() {
  return getRuleContext<KodaParser::WhenModeContext>(0);
}

KodaParser::EdcWhenAllowedInContext::EdcWhenAllowedInContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcWhenAllowedInContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcWhenAllowedIn(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcTimeoutContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcTimeoutContext::TIMEOUT() {
  return getToken(KodaParser::TIMEOUT, 0);
}

tree::TerminalNode* KodaParser::EdcTimeoutContext::NATURAL() {
  return getToken(KodaParser::NATURAL, 0);
}

KodaParser::TimeUnitContext* KodaParser::EdcTimeoutContext::timeUnit() {
  return getRuleContext<KodaParser::TimeUnitContext>(0);
}

tree::TerminalNode* KodaParser::EdcTimeoutContext::ARROW() {
  return getToken(KodaParser::ARROW, 0);
}

tree::TerminalNode* KodaParser::EdcTimeoutContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

KodaParser::EdcTimeoutContext::EdcTimeoutContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcTimeoutContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcTimeout(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcRosEventContext ------------------------------------------------------------------

KodaParser::RosDataContext* KodaParser::EdcRosEventContext::rosData() {
  return getRuleContext<KodaParser::RosDataContext>(0);
}

std::vector<tree::TerminalNode *> KodaParser::EdcRosEventContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::EdcRosEventContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

tree::TerminalNode* KodaParser::EdcRosEventContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

std::vector<tree::TerminalNode *> KodaParser::EdcRosEventContext::STRING() {
  return getTokens(KodaParser::STRING);
}

tree::TerminalNode* KodaParser::EdcRosEventContext::STRING(size_t i) {
  return getToken(KodaParser::STRING, i);
}

KodaParser::EdcRosEventContext::EdcRosEventContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcRosEventContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcRosEvent(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EdcReplyContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::EdcReplyContext::REPLY() {
  return getToken(KodaParser::REPLY, 0);
}

tree::TerminalNode* KodaParser::EdcReplyContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

KodaParser::WhenModeContext* KodaParser::EdcReplyContext::whenMode() {
  return getRuleContext<KodaParser::WhenModeContext>(0);
}

KodaParser::EdcReplyContext::EdcReplyContext(EventDefComponentContext *ctx) { copyFrom(ctx); }


std::any KodaParser::EdcReplyContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitEdcReply(this);
  else
    return visitor->visitChildren(this);
}
KodaParser::EventDefComponentContext* KodaParser::eventDefComponent() {
  EventDefComponentContext *_localctx = _tracker.createInstance<EventDefComponentContext>(_ctx, getState());
  enterRule(_localctx, 48, KodaParser::RuleEventDefComponent);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(431);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::ACTION:
      case KodaParser::SERVICE:
      case KodaParser::TOPIC: {
        _localctx = _tracker.createInstance<KodaParser::EdcRosEventContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(373);
        rosData();
        setState(374);
        match(KodaParser::IDENT);
        setState(375);
        match(KodaParser::COLON);
        setState(376);
        match(KodaParser::STRING);
        setState(377);
        match(KodaParser::IDENT);
        setState(378);
        match(KodaParser::STRING);
        setState(379);
        match(KodaParser::IDENT);
        setState(380);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::TIMEOUT: {
        _localctx = _tracker.createInstance<KodaParser::EdcTimeoutContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(382);
        match(KodaParser::TIMEOUT);
        setState(383);
        match(KodaParser::NATURAL);
        setState(384);
        timeUnit();
        setState(385);
        match(KodaParser::ARROW);
        setState(386);
        match(KodaParser::IDENT);
        break;
      }

      case KodaParser::ALLOWED: {
        _localctx = _tracker.createInstance<KodaParser::EdcWhenAllowedInContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(388);
        match(KodaParser::ALLOWED);
        setState(389);
        match(KodaParser::IN);
        setState(390);
        whenMode();
        break;
      }

      case KodaParser::REPLY: {
        _localctx = _tracker.createInstance<KodaParser::EdcReplyContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(391);
        match(KodaParser::REPLY);
        setState(392);
        match(KodaParser::IDENT);
        setState(393);
        whenMode();
        break;
      }

      case KodaParser::AFTER: {
        _localctx = _tracker.createInstance<KodaParser::EdcDependsAfterContext>(_localctx);
        enterOuterAlt(_localctx, 5);
        setState(394);
        match(KodaParser::AFTER);
        setState(395);
        match(KodaParser::IDENT);
        break;
      }

      case KodaParser::ONCE: {
        _localctx = _tracker.createInstance<KodaParser::EdcOnceInContext>(_localctx);
        enterOuterAlt(_localctx, 6);
        setState(396);
        match(KodaParser::ONCE);
        setState(397);
        match(KodaParser::IN);
        setState(398);
        whenMode();
        setState(407);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::LBRACE) {
          setState(399);
          match(KodaParser::LBRACE);
          setState(403);
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
            setState(400);
            statement();
            setState(405);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
          setState(406);
          match(KodaParser::RBRACE);
        }
        break;
      }

      case KodaParser::TRIGGER: {
        _localctx = _tracker.createInstance<KodaParser::EdcStartContext>(_localctx);
        enterOuterAlt(_localctx, 7);
        setState(409);
        match(KodaParser::TRIGGER);
        setState(418);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::LBRACE) {
          setState(410);
          match(KodaParser::LBRACE);
          setState(414);
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
            setState(411);
            statement();
            setState(416);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
          setState(417);
          match(KodaParser::RBRACE);
        }
        break;
      }

      case KodaParser::ABORT: {
        _localctx = _tracker.createInstance<KodaParser::EdcResetContext>(_localctx);
        enterOuterAlt(_localctx, 8);
        setState(420);
        match(KodaParser::ABORT);
        setState(429);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::LBRACE) {
          setState(421);
          match(KodaParser::LBRACE);
          setState(425);
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
            setState(422);
            statement();
            setState(427);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
          setState(428);
          match(KodaParser::RBRACE);
        }
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

//----------------- RosDataContext ------------------------------------------------------------------

KodaParser::RosDataContext::RosDataContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::RosDataContext::TOPIC() {
  return getToken(KodaParser::TOPIC, 0);
}

tree::TerminalNode* KodaParser::RosDataContext::SERVICE() {
  return getToken(KodaParser::SERVICE, 0);
}

tree::TerminalNode* KodaParser::RosDataContext::ACTION() {
  return getToken(KodaParser::ACTION, 0);
}


size_t KodaParser::RosDataContext::getRuleIndex() const {
  return KodaParser::RuleRosData;
}


std::any KodaParser::RosDataContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitRosData(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::RosDataContext* KodaParser::rosData() {
  RosDataContext *_localctx = _tracker.createInstance<RosDataContext>(_ctx, getState());
  enterRule(_localctx, 50, KodaParser::RuleRosData);
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
    setState(433);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::ACTION)
      | (1ULL << KodaParser::SERVICE)
      | (1ULL << KodaParser::TOPIC))) != 0))) {
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

//----------------- WhenModeContext ------------------------------------------------------------------

KodaParser::WhenModeContext::WhenModeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::WhenModeContext::ALWAYS() {
  return getToken(KodaParser::ALWAYS, 0);
}

tree::TerminalNode* KodaParser::WhenModeContext::MISSION() {
  return getToken(KodaParser::MISSION, 0);
}

tree::TerminalNode* KodaParser::WhenModeContext::IDLE() {
  return getToken(KodaParser::IDLE, 0);
}


size_t KodaParser::WhenModeContext::getRuleIndex() const {
  return KodaParser::RuleWhenMode;
}


std::any KodaParser::WhenModeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitWhenMode(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::WhenModeContext* KodaParser::whenMode() {
  WhenModeContext *_localctx = _tracker.createInstance<WhenModeContext>(_ctx, getState());
  enterRule(_localctx, 52, KodaParser::RuleWhenMode);
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
    setState(435);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::ALWAYS)
      | (1ULL << KodaParser::MISSION)
      | (1ULL << KodaParser::IDLE))) != 0))) {
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

//----------------- TimeUnitContext ------------------------------------------------------------------

KodaParser::TimeUnitContext::TimeUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::TimeUnitContext::S() {
  return getToken(KodaParser::S, 0);
}

tree::TerminalNode* KodaParser::TimeUnitContext::MS() {
  return getToken(KodaParser::MS, 0);
}

tree::TerminalNode* KodaParser::TimeUnitContext::US() {
  return getToken(KodaParser::US, 0);
}

tree::TerminalNode* KodaParser::TimeUnitContext::NS() {
  return getToken(KodaParser::NS, 0);
}


size_t KodaParser::TimeUnitContext::getRuleIndex() const {
  return KodaParser::RuleTimeUnit;
}


std::any KodaParser::TimeUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitTimeUnit(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::TimeUnitContext* KodaParser::timeUnit() {
  TimeUnitContext *_localctx = _tracker.createInstance<TimeUnitContext>(_ctx, getState());
  enterRule(_localctx, 54, KodaParser::RuleTimeUnit);
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
    setState(437);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::S)
      | (1ULL << KodaParser::MS)
      | (1ULL << KodaParser::US)
      | (1ULL << KodaParser::NS))) != 0))) {
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
//----------------- StratIfElseContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratIfElseContext::IF() {
  return getToken(KodaParser::IF, 0);
}

KodaParser::ExpressionContext* KodaParser::StratIfElseContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
}

tree::TerminalNode* KodaParser::StratIfElseContext::THEN() {
  return getToken(KodaParser::THEN, 0);
}

std::vector<KodaParser::StrategyContext *> KodaParser::StratIfElseContext::strategy() {
  return getRuleContexts<KodaParser::StrategyContext>();
}

KodaParser::StrategyContext* KodaParser::StratIfElseContext::strategy(size_t i) {
  return getRuleContext<KodaParser::StrategyContext>(i);
}

tree::TerminalNode* KodaParser::StratIfElseContext::ELSE() {
  return getToken(KodaParser::ELSE, 0);
}

KodaParser::StratIfElseContext::StratIfElseContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratIfElseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratIfElse(this);
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
//----------------- StratGuardContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratGuardContext::GUARD() {
  return getToken(KodaParser::GUARD, 0);
}

tree::TerminalNode* KodaParser::StratGuardContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

KodaParser::ExpressionContext* KodaParser::StratGuardContext::expression() {
  return getRuleContext<KodaParser::ExpressionContext>(0);
}

tree::TerminalNode* KodaParser::StratGuardContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

KodaParser::StratGuardContext::StratGuardContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratGuardContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratGuard(this);
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
//----------------- StratLetContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratLetContext::LET() {
  return getToken(KodaParser::LET, 0);
}

tree::TerminalNode* KodaParser::StratLetContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::StratLetContext::ASSIGN() {
  return getToken(KodaParser::ASSIGN, 0);
}

KodaParser::EventStatementContext* KodaParser::StratLetContext::eventStatement() {
  return getRuleContext<KodaParser::EventStatementContext>(0);
}

KodaParser::StratLetContext::StratLetContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratLetContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratLet(this);
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
    setState(512);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(440);
      match(KodaParser::JOIN);
      setState(441);
      match(KodaParser::LPAREN);
      setState(442);
      strategy(0);
      setState(445); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(443);
        match(KodaParser::PIPE);
        setState(444);
        strategy(0);
        setState(447); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == KodaParser::PIPE);
      setState(449);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(451);
      match(KodaParser::EITHER);
      setState(452);
      match(KodaParser::LPAREN);
      setState(453);
      strategy(0);
      setState(456); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(454);
        match(KodaParser::PIPE);
        setState(455);
        strategy(0);
        setState(458); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == KodaParser::PIPE);
      setState(460);
      match(KodaParser::RPAREN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<StratLetContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(462);
      match(KodaParser::LET);
      setState(463);
      match(KodaParser::IDENT);
      setState(464);
      match(KodaParser::ASSIGN);
      setState(465);
      eventStatement();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(466);
      match(KodaParser::WITHIN);
      setState(467);
      match(KodaParser::NATURAL);
      setState(468);
      match(KodaParser::DO);
      setState(469);
      strategy(0);
      setState(470);
      match(KodaParser::ELSE);
      setState(471);
      strategy(9);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<StratIfElseContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(473);
      match(KodaParser::IF);
      setState(474);
      expression();
      setState(475);
      match(KodaParser::THEN);
      setState(476);
      strategy(0);
      setState(479);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx)) {
      case 1: {
        setState(477);
        match(KodaParser::ELSE);
        setState(478);
        strategy(0);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<StratRepeatContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(481);
      match(KodaParser::REPEAT);
      setState(482);
      match(KodaParser::NATURAL);
      setState(483);
      match(KodaParser::NATURAL);
      setState(484);
      match(KodaParser::LPAREN);
      setState(485);
      strategy(0);
      setState(486);
      match(KodaParser::RPAREN);
      setState(490);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(487);
          strategyHandler(); 
        }
        setState(492);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
      }
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<StratGuardContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(493);
      match(KodaParser::GUARD);
      setState(494);
      match(KodaParser::LBRACE);
      setState(495);
      expression();
      setState(496);
      match(KodaParser::RBRACE);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<StratEndContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(498);
      match(KodaParser::END);
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<StratContinueContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(499);
      match(KodaParser::CONTINUE);
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<StratRefContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(500);
      identifier();
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<StratTaskContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(501);
      eventStatement();
      setState(505);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(502);
          strategyHandler(); 
        }
        setState(507);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
      }
      break;
    }

    case 12: {
      _localctx = _tracker.createInstance<StratParenContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(508);
      match(KodaParser::LPAREN);
      setState(509);
      strategy(0);
      setState(510);
      match(KodaParser::RPAREN);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(523);
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
        setState(514);

        if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
        setState(517); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(515);
                  match(KodaParser::ARROW);
                  setState(516);
                  strategy(0);
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(519); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER); 
      }
      setState(525);
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
  enterRule(_localctx, 58, KodaParser::RuleStrategyHandler);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(536);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(526);
      match(KodaParser::ON);
      setState(527);
      match(KodaParser::ERROR);
      setState(528);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(529);
      match(KodaParser::ON);
      setState(530);
      match(KodaParser::ABORT);
      setState(531);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(532);
      match(KodaParser::ON);
      setState(533);
      eventStatement();
      setState(534);
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
  enterRule(_localctx, 60, KodaParser::RuleEventStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(554);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(538);
      identifier();
      setState(539);
      match(KodaParser::DOT);
      setState(540);
      identifier();
      setState(541);
      match(KodaParser::LPAREN);
      setState(543);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
        | (1ULL << KodaParser::RETURN)
        | (1ULL << KodaParser::ABORT)
        | (1ULL << KodaParser::ERROR)
        | (1ULL << KodaParser::IN)
        | (1ULL << KodaParser::OUT)
        | (1ULL << KodaParser::MINUS))) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 66)) & ((1ULL << (KodaParser::NOT - 66))
        | (1ULL << (KodaParser::LPAREN - 66))
        | (1ULL << (KodaParser::NATURAL - 66))
        | (1ULL << (KodaParser::REAL - 66))
        | (1ULL << (KodaParser::IDENT - 66))
        | (1ULL << (KodaParser::STRING - 66)))) != 0)) {
        setState(542);
        exprList();
      }
      setState(545);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(547);
      identifier();
      setState(548);
      match(KodaParser::LPAREN);
      setState(550);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
        | (1ULL << KodaParser::RETURN)
        | (1ULL << KodaParser::ABORT)
        | (1ULL << KodaParser::ERROR)
        | (1ULL << KodaParser::IN)
        | (1ULL << KodaParser::OUT)
        | (1ULL << KodaParser::MINUS))) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 66)) & ((1ULL << (KodaParser::NOT - 66))
        | (1ULL << (KodaParser::LPAREN - 66))
        | (1ULL << (KodaParser::NATURAL - 66))
        | (1ULL << (KodaParser::REAL - 66))
        | (1ULL << (KodaParser::IDENT - 66))
        | (1ULL << (KodaParser::STRING - 66)))) != 0)) {
        setState(549);
        exprList();
      }
      setState(552);
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
  enterRule(_localctx, 62, KodaParser::RuleExprList);
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
    setState(556);
    expression();
    setState(561);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(557);
      match(KodaParser::COMMA);
      setState(558);
      expression();
      setState(563);
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
  enterRule(_localctx, 64, KodaParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(564);
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
  enterRule(_localctx, 66, KodaParser::RuleExprOr);
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
    setState(566);
    exprAnd();
    setState(571);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(567);
      match(KodaParser::OR);
      setState(568);
      exprAnd();
      setState(573);
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
  enterRule(_localctx, 68, KodaParser::RuleExprAnd);
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
    setState(574);
    exprCmp();
    setState(579);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(575);
      match(KodaParser::AND);
      setState(576);
      exprCmp();
      setState(581);
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
  enterRule(_localctx, 70, KodaParser::RuleExprCmp);
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
    setState(582);
    exprNot();
    setState(586);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::EQ)
      | (1ULL << KodaParser::NEQ)
      | (1ULL << KodaParser::LEQ)
      | (1ULL << KodaParser::GEQ)
      | (1ULL << KodaParser::LT)
      | (1ULL << KodaParser::GT))) != 0)) {
      setState(583);
      compOp();
      setState(584);
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
  enterRule(_localctx, 72, KodaParser::RuleCompOp);
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
    setState(588);
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
  enterRule(_localctx, 74, KodaParser::RuleExprNot);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(593);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(590);
        match(KodaParser::NOT);
        setState(591);
        exprNot();
        break;
      }

      case KodaParser::TRIGGER:
      case KodaParser::RETURN:
      case KodaParser::ABORT:
      case KodaParser::ERROR:
      case KodaParser::IN:
      case KodaParser::OUT:
      case KodaParser::MINUS:
      case KodaParser::LPAREN:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(592);
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
  enterRule(_localctx, 76, KodaParser::RuleExprAdd);
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
    setState(595);
    exprMul();
    setState(600);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(596);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(597);
      exprMul();
      setState(602);
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
  enterRule(_localctx, 78, KodaParser::RuleExprMul);
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
    setState(603);
    exprUnary();
    setState(608);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(604);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(605);
      exprUnary();
      setState(610);
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
  enterRule(_localctx, 80, KodaParser::RuleExprUnary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(614);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(611);
        match(KodaParser::MINUS);
        setState(612);
        exprUnary();
        break;
      }

      case KodaParser::TRIGGER:
      case KodaParser::RETURN:
      case KodaParser::ABORT:
      case KodaParser::ERROR:
      case KodaParser::IN:
      case KodaParser::OUT:
      case KodaParser::LPAREN:
      case KodaParser::NATURAL:
      case KodaParser::REAL:
      case KodaParser::IDENT:
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(613);
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
  enterRule(_localctx, 82, KodaParser::RuleExprPrimary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(625);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(616);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(617);
      match(KodaParser::IDENT);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(618);
      match(KodaParser::STRING);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(619);
      match(KodaParser::NATURAL);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(620);
      match(KodaParser::REAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(621);
      match(KodaParser::LPAREN);
      setState(622);
      expression();
      setState(623);
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
  enterRule(_localctx, 84, KodaParser::RuleIdentifier);
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
    setState(627);
    _la = _input->LA(1);
    if (!(((((_la - 14) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 14)) & ((1ULL << (KodaParser::TRIGGER - 14))
      | (1ULL << (KodaParser::RETURN - 14))
      | (1ULL << (KodaParser::ABORT - 14))
      | (1ULL << (KodaParser::ERROR - 14))
      | (1ULL << (KodaParser::IN - 14))
      | (1ULL << (KodaParser::OUT - 14))
      | (1ULL << (KodaParser::IDENT - 14)))) != 0))) {
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
    case 0: return precpred(_ctx, 13);

  default:
    break;
  }
  return true;
}

void KodaParser::initialize() {
  std::call_once(kodaParserOnceFlag, kodaParserInitialize);
}
