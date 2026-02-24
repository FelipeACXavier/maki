
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
      "system", "topLevelComponent", "argumentList", "argument", "statement", 
      "tasksBlock", "flow", "identList", "varsBlock", "variableStatement", 
      "actionBlock", "serviceBlock", "topicBlock", "rosDefStatement", "eventDefStatement", 
      "eventDefComponentList", "eventDefComponent", "rosData", "whenMode", 
      "timeUnit", "strategy", "strategyHandler", "eventStatement", "exprList", 
      "expression", "exprOr", "exprAnd", "exprCmp", "compOp", "exprNot", 
      "exprAdd", "exprMul", "exprUnary", "exprPrimary", "identifier"
    },
    std::vector<std::string>{
      "", "'task'", "'capability'", "'strategy'", "'vars'", "'action'", 
      "'service'", "'topic'", "'trigger'", "'return'", "'abort'", "'error'", 
      "'in'", "'out'", "'on'", "'req'", "'pro'", "'end'", "'repeat'", "'join'", 
      "'either'", "'let'", "'within'", "'do'", "'else'", "'if'", "'then'", 
      "'guard'", "'every'", "'timeout'", "'allowed'", "'reply'", "'after'", 
      "'once'", "'always'", "'mission'", "'idle'", "'s'", "'ms'", "'us'", 
      "'ns'", "'-->'", "'|'", "'=='", "'!='", "'<='", "'>='", "'<'", "'>'", 
      "'='", "':'", "','", "';'", "'.'", "'+'", "'-'", "'*'", "'/'", "'!'", 
      "'and'", "'or'", "'('", "')'", "'{'", "'}'", "'['", "']'"
    },
    std::vector<std::string>{
      "", "TASK", "CAPABILITY", "STRATEGY", "VARS", "ACTION", "SERVICE", 
      "TOPIC", "TRIGGER", "RETURN", "ABORT", "ERROR", "IN", "OUT", "ON", 
      "REQ", "PRO", "END", "REPEAT", "JOIN", "EITHER", "LET", "WITHIN", 
      "DO", "ELSE", "IF", "THEN", "GUARD", "EVERY", "TIMEOUT", "ALLOWED", 
      "REPLY", "AFTER", "ONCE", "ALWAYS", "MISSION", "IDLE", "S", "MS", 
      "US", "NS", "ARROW", "PIPE", "EQ", "NEQ", "LEQ", "GEQ", "LT", "GT", 
      "ASSIGN", "COLON", "COMMA", "SEMI", "DOT", "PLUS", "MINUS", "STAR", 
      "SLASH", "NOT", "AND", "OR", "LPAREN", "RPAREN", "LBRACE", "RBRACE", 
      "LBRACK", "RBRACK", "NATURAL", "REAL", "IDENT", "STRING", "ANY", "LINE_COMMENT", 
      "BLOCK_COMMENT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,74,523,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,1,0,5,
  	0,72,8,0,10,0,12,0,75,9,0,1,0,1,0,1,1,1,1,1,1,1,1,3,1,83,8,1,1,1,1,1,
  	1,1,5,1,88,8,1,10,1,12,1,91,9,1,1,1,1,1,1,1,1,1,1,1,3,1,98,8,1,1,1,1,
  	1,1,1,5,1,103,8,1,10,1,12,1,106,9,1,1,1,3,1,109,8,1,1,2,1,2,1,2,5,2,114,
  	8,2,10,2,12,2,117,9,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,127,8,3,1,4,
  	1,4,1,4,1,4,1,4,1,4,3,4,135,8,4,1,5,1,5,1,5,4,5,140,8,5,11,5,12,5,141,
  	1,5,1,5,1,6,1,6,1,6,3,6,149,8,6,1,6,3,6,152,8,6,1,6,1,6,1,6,1,6,1,7,1,
  	7,1,7,5,7,161,8,7,10,7,12,7,164,9,7,1,8,1,8,1,8,4,8,169,8,8,11,8,12,8,
  	170,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,10,5,10,
  	187,8,10,10,10,12,10,190,9,10,1,10,1,10,1,11,1,11,1,11,1,11,1,11,5,11,
  	199,8,11,10,11,12,11,202,9,11,1,11,1,11,1,12,1,12,1,12,1,12,1,12,5,12,
  	211,8,12,10,12,12,12,214,9,12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,
  	1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,
  	1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,248,8,13,1,14,
  	1,14,1,14,1,14,3,14,254,8,14,1,14,1,14,1,14,3,14,259,8,14,1,15,1,15,1,
  	15,4,15,264,8,15,11,15,12,15,265,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,
  	16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,
  	16,1,16,1,16,1,16,1,16,1,16,1,16,5,16,296,8,16,10,16,12,16,299,9,16,1,
  	16,3,16,302,8,16,1,16,1,16,1,16,5,16,307,8,16,10,16,12,16,310,9,16,1,
  	16,3,16,313,8,16,1,16,1,16,1,16,5,16,318,8,16,10,16,12,16,321,9,16,1,
  	16,3,16,324,8,16,3,16,326,8,16,1,17,1,17,1,18,1,18,1,19,1,19,1,20,1,20,
  	1,20,1,20,1,20,1,20,4,20,340,8,20,11,20,12,20,341,1,20,1,20,1,20,1,20,
  	1,20,1,20,1,20,4,20,351,8,20,11,20,12,20,352,1,20,1,20,1,20,1,20,1,20,
  	1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,
  	3,20,374,8,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,
  	1,20,1,20,1,20,1,20,1,20,5,20,392,8,20,10,20,12,20,395,9,20,1,20,1,20,
  	1,20,1,20,5,20,401,8,20,10,20,12,20,404,9,20,1,20,1,20,1,20,1,20,3,20,
  	410,8,20,1,20,1,20,1,20,5,20,415,8,20,10,20,12,20,418,9,20,1,21,1,21,
  	1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,430,8,21,1,22,1,22,1,22,
  	1,22,1,22,3,22,437,8,22,1,22,1,22,1,22,1,22,1,22,3,22,444,8,22,1,22,1,
  	22,3,22,448,8,22,1,23,1,23,1,23,5,23,453,8,23,10,23,12,23,456,9,23,1,
  	24,1,24,1,25,1,25,1,25,5,25,463,8,25,10,25,12,25,466,9,25,1,26,1,26,1,
  	26,5,26,471,8,26,10,26,12,26,474,9,26,1,27,1,27,1,27,1,27,3,27,480,8,
  	27,1,28,1,28,1,29,1,29,1,29,3,29,487,8,29,1,30,1,30,1,30,5,30,492,8,30,
  	10,30,12,30,495,9,30,1,31,1,31,1,31,5,31,500,8,31,10,31,12,31,503,9,31,
  	1,32,1,32,1,32,3,32,508,8,32,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,
  	1,33,3,33,519,8,33,1,34,1,34,1,34,0,1,40,35,0,2,4,6,8,10,12,14,16,18,
  	20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,
  	66,68,0,7,1,0,5,7,1,0,34,36,1,0,37,40,1,0,43,48,1,0,54,55,1,0,56,57,2,
  	0,8,13,69,69,565,0,73,1,0,0,0,2,108,1,0,0,0,4,110,1,0,0,0,6,126,1,0,0,
  	0,8,134,1,0,0,0,10,136,1,0,0,0,12,145,1,0,0,0,14,157,1,0,0,0,16,165,1,
  	0,0,0,18,174,1,0,0,0,20,181,1,0,0,0,22,193,1,0,0,0,24,205,1,0,0,0,26,
  	247,1,0,0,0,28,249,1,0,0,0,30,260,1,0,0,0,32,325,1,0,0,0,34,327,1,0,0,
  	0,36,329,1,0,0,0,38,331,1,0,0,0,40,409,1,0,0,0,42,429,1,0,0,0,44,447,
  	1,0,0,0,46,449,1,0,0,0,48,457,1,0,0,0,50,459,1,0,0,0,52,467,1,0,0,0,54,
  	475,1,0,0,0,56,481,1,0,0,0,58,486,1,0,0,0,60,488,1,0,0,0,62,496,1,0,0,
  	0,64,507,1,0,0,0,66,518,1,0,0,0,68,520,1,0,0,0,70,72,3,2,1,0,71,70,1,
  	0,0,0,72,75,1,0,0,0,73,71,1,0,0,0,73,74,1,0,0,0,74,76,1,0,0,0,75,73,1,
  	0,0,0,76,77,5,0,0,1,77,1,1,0,0,0,78,79,5,1,0,0,79,80,5,69,0,0,80,82,5,
  	61,0,0,81,83,3,4,2,0,82,81,1,0,0,0,82,83,1,0,0,0,83,84,1,0,0,0,84,85,
  	5,62,0,0,85,89,5,63,0,0,86,88,3,8,4,0,87,86,1,0,0,0,88,91,1,0,0,0,89,
  	87,1,0,0,0,89,90,1,0,0,0,90,92,1,0,0,0,91,89,1,0,0,0,92,109,5,64,0,0,
  	93,94,5,2,0,0,94,95,5,69,0,0,95,97,5,61,0,0,96,98,3,4,2,0,97,96,1,0,0,
  	0,97,98,1,0,0,0,98,99,1,0,0,0,99,100,5,62,0,0,100,104,5,63,0,0,101,103,
  	3,8,4,0,102,101,1,0,0,0,103,106,1,0,0,0,104,102,1,0,0,0,104,105,1,0,0,
  	0,105,107,1,0,0,0,106,104,1,0,0,0,107,109,5,64,0,0,108,78,1,0,0,0,108,
  	93,1,0,0,0,109,3,1,0,0,0,110,115,3,6,3,0,111,112,5,51,0,0,112,114,3,6,
  	3,0,113,111,1,0,0,0,114,117,1,0,0,0,115,113,1,0,0,0,115,116,1,0,0,0,116,
  	5,1,0,0,0,117,115,1,0,0,0,118,119,5,69,0,0,119,127,5,69,0,0,120,121,5,
  	69,0,0,121,122,5,15,0,0,122,127,5,69,0,0,123,124,5,69,0,0,124,125,5,16,
  	0,0,125,127,5,69,0,0,126,118,1,0,0,0,126,120,1,0,0,0,126,123,1,0,0,0,
  	127,7,1,0,0,0,128,135,3,10,5,0,129,135,3,16,8,0,130,135,3,20,10,0,131,
  	135,3,22,11,0,132,135,3,24,12,0,133,135,3,26,13,0,134,128,1,0,0,0,134,
  	129,1,0,0,0,134,130,1,0,0,0,134,131,1,0,0,0,134,132,1,0,0,0,134,133,1,
  	0,0,0,135,9,1,0,0,0,136,137,5,3,0,0,137,139,5,63,0,0,138,140,3,12,6,0,
  	139,138,1,0,0,0,140,141,1,0,0,0,141,139,1,0,0,0,141,142,1,0,0,0,142,143,
  	1,0,0,0,143,144,5,64,0,0,144,11,1,0,0,0,145,151,5,69,0,0,146,148,5,65,
  	0,0,147,149,3,14,7,0,148,147,1,0,0,0,148,149,1,0,0,0,149,150,1,0,0,0,
  	150,152,5,66,0,0,151,146,1,0,0,0,151,152,1,0,0,0,152,153,1,0,0,0,153,
  	154,5,50,0,0,154,155,3,40,20,0,155,156,5,52,0,0,156,13,1,0,0,0,157,162,
  	5,69,0,0,158,159,5,51,0,0,159,161,5,69,0,0,160,158,1,0,0,0,161,164,1,
  	0,0,0,162,160,1,0,0,0,162,163,1,0,0,0,163,15,1,0,0,0,164,162,1,0,0,0,
  	165,166,5,4,0,0,166,168,5,63,0,0,167,169,3,18,9,0,168,167,1,0,0,0,169,
  	170,1,0,0,0,170,168,1,0,0,0,170,171,1,0,0,0,171,172,1,0,0,0,172,173,5,
  	64,0,0,173,17,1,0,0,0,174,175,5,69,0,0,175,176,5,69,0,0,176,177,5,49,
  	0,0,177,178,3,48,24,0,178,179,5,50,0,0,179,180,3,48,24,0,180,19,1,0,0,
  	0,181,182,5,5,0,0,182,183,5,70,0,0,183,184,5,70,0,0,184,188,5,63,0,0,
  	185,187,3,26,13,0,186,185,1,0,0,0,187,190,1,0,0,0,188,186,1,0,0,0,188,
  	189,1,0,0,0,189,191,1,0,0,0,190,188,1,0,0,0,191,192,5,64,0,0,192,21,1,
  	0,0,0,193,194,5,6,0,0,194,195,5,70,0,0,195,196,5,70,0,0,196,200,5,63,
  	0,0,197,199,3,26,13,0,198,197,1,0,0,0,199,202,1,0,0,0,200,198,1,0,0,0,
  	200,201,1,0,0,0,201,203,1,0,0,0,202,200,1,0,0,0,203,204,5,64,0,0,204,
  	23,1,0,0,0,205,206,5,7,0,0,206,207,5,70,0,0,207,208,5,70,0,0,208,212,
  	5,63,0,0,209,211,3,26,13,0,210,209,1,0,0,0,211,214,1,0,0,0,212,210,1,
  	0,0,0,212,213,1,0,0,0,213,215,1,0,0,0,214,212,1,0,0,0,215,216,5,64,0,
  	0,216,25,1,0,0,0,217,218,5,8,0,0,218,219,5,50,0,0,219,220,3,28,14,0,220,
  	221,5,52,0,0,221,248,1,0,0,0,222,223,5,9,0,0,223,224,5,50,0,0,224,225,
  	3,28,14,0,225,226,5,52,0,0,226,248,1,0,0,0,227,228,5,10,0,0,228,229,5,
  	50,0,0,229,230,3,28,14,0,230,231,5,52,0,0,231,248,1,0,0,0,232,233,5,11,
  	0,0,233,234,5,50,0,0,234,235,3,28,14,0,235,236,5,52,0,0,236,248,1,0,0,
  	0,237,238,5,12,0,0,238,239,5,50,0,0,239,240,3,28,14,0,240,241,5,52,0,
  	0,241,248,1,0,0,0,242,243,5,13,0,0,243,244,5,50,0,0,244,245,3,28,14,0,
  	245,246,5,52,0,0,246,248,1,0,0,0,247,217,1,0,0,0,247,222,1,0,0,0,247,
  	227,1,0,0,0,247,232,1,0,0,0,247,237,1,0,0,0,247,242,1,0,0,0,248,27,1,
  	0,0,0,249,250,5,69,0,0,250,251,3,68,34,0,251,253,5,61,0,0,252,254,3,4,
  	2,0,253,252,1,0,0,0,253,254,1,0,0,0,254,255,1,0,0,0,255,258,5,62,0,0,
  	256,257,5,50,0,0,257,259,3,30,15,0,258,256,1,0,0,0,258,259,1,0,0,0,259,
  	29,1,0,0,0,260,263,3,32,16,0,261,262,5,51,0,0,262,264,3,32,16,0,263,261,
  	1,0,0,0,264,265,1,0,0,0,265,263,1,0,0,0,265,266,1,0,0,0,266,31,1,0,0,
  	0,267,268,3,34,17,0,268,269,5,69,0,0,269,270,5,50,0,0,270,271,5,70,0,
  	0,271,272,5,69,0,0,272,273,5,70,0,0,273,274,5,69,0,0,274,275,5,70,0,0,
  	275,326,1,0,0,0,276,277,5,29,0,0,277,278,5,67,0,0,278,279,3,38,19,0,279,
  	280,5,41,0,0,280,281,5,69,0,0,281,326,1,0,0,0,282,283,5,30,0,0,283,284,
  	5,12,0,0,284,326,3,36,18,0,285,286,5,31,0,0,286,287,5,69,0,0,287,326,
  	3,36,18,0,288,289,5,32,0,0,289,326,5,69,0,0,290,291,5,33,0,0,291,292,
  	5,12,0,0,292,301,3,36,18,0,293,297,5,63,0,0,294,296,3,8,4,0,295,294,1,
  	0,0,0,296,299,1,0,0,0,297,295,1,0,0,0,297,298,1,0,0,0,298,300,1,0,0,0,
  	299,297,1,0,0,0,300,302,5,64,0,0,301,293,1,0,0,0,301,302,1,0,0,0,302,
  	326,1,0,0,0,303,312,5,8,0,0,304,308,5,63,0,0,305,307,3,8,4,0,306,305,
  	1,0,0,0,307,310,1,0,0,0,308,306,1,0,0,0,308,309,1,0,0,0,309,311,1,0,0,
  	0,310,308,1,0,0,0,311,313,5,64,0,0,312,304,1,0,0,0,312,313,1,0,0,0,313,
  	326,1,0,0,0,314,323,5,10,0,0,315,319,5,63,0,0,316,318,3,8,4,0,317,316,
  	1,0,0,0,318,321,1,0,0,0,319,317,1,0,0,0,319,320,1,0,0,0,320,322,1,0,0,
  	0,321,319,1,0,0,0,322,324,5,64,0,0,323,315,1,0,0,0,323,324,1,0,0,0,324,
  	326,1,0,0,0,325,267,1,0,0,0,325,276,1,0,0,0,325,282,1,0,0,0,325,285,1,
  	0,0,0,325,288,1,0,0,0,325,290,1,0,0,0,325,303,1,0,0,0,325,314,1,0,0,0,
  	326,33,1,0,0,0,327,328,7,0,0,0,328,35,1,0,0,0,329,330,7,1,0,0,330,37,
  	1,0,0,0,331,332,7,2,0,0,332,39,1,0,0,0,333,334,6,20,-1,0,334,335,5,19,
  	0,0,335,336,5,61,0,0,336,339,3,40,20,0,337,338,5,42,0,0,338,340,3,40,
  	20,0,339,337,1,0,0,0,340,341,1,0,0,0,341,339,1,0,0,0,341,342,1,0,0,0,
  	342,343,1,0,0,0,343,344,5,62,0,0,344,410,1,0,0,0,345,346,5,20,0,0,346,
  	347,5,61,0,0,347,350,3,40,20,0,348,349,5,42,0,0,349,351,3,40,20,0,350,
  	348,1,0,0,0,351,352,1,0,0,0,352,350,1,0,0,0,352,353,1,0,0,0,353,354,1,
  	0,0,0,354,355,5,62,0,0,355,410,1,0,0,0,356,357,5,21,0,0,357,358,5,69,
  	0,0,358,359,5,49,0,0,359,410,3,44,22,0,360,361,5,22,0,0,361,362,5,67,
  	0,0,362,363,5,23,0,0,363,364,3,40,20,0,364,365,5,24,0,0,365,366,3,40,
  	20,9,366,410,1,0,0,0,367,368,5,25,0,0,368,369,3,48,24,0,369,370,5,26,
  	0,0,370,373,3,40,20,0,371,372,5,24,0,0,372,374,3,40,20,0,373,371,1,0,
  	0,0,373,374,1,0,0,0,374,410,1,0,0,0,375,376,5,18,0,0,376,377,5,61,0,0,
  	377,378,3,40,20,0,378,379,5,62,0,0,379,410,1,0,0,0,380,381,5,27,0,0,381,
  	382,5,63,0,0,382,383,3,48,24,0,383,384,5,64,0,0,384,410,1,0,0,0,385,386,
  	5,28,0,0,386,387,5,67,0,0,387,388,5,63,0,0,388,389,3,40,20,0,389,393,
  	5,64,0,0,390,392,3,42,21,0,391,390,1,0,0,0,392,395,1,0,0,0,393,391,1,
  	0,0,0,393,394,1,0,0,0,394,410,1,0,0,0,395,393,1,0,0,0,396,410,5,17,0,
  	0,397,410,3,68,34,0,398,402,3,44,22,0,399,401,3,42,21,0,400,399,1,0,0,
  	0,401,404,1,0,0,0,402,400,1,0,0,0,402,403,1,0,0,0,403,410,1,0,0,0,404,
  	402,1,0,0,0,405,406,5,61,0,0,406,407,3,40,20,0,407,408,5,62,0,0,408,410,
  	1,0,0,0,409,333,1,0,0,0,409,345,1,0,0,0,409,356,1,0,0,0,409,360,1,0,0,
  	0,409,367,1,0,0,0,409,375,1,0,0,0,409,380,1,0,0,0,409,385,1,0,0,0,409,
  	396,1,0,0,0,409,397,1,0,0,0,409,398,1,0,0,0,409,405,1,0,0,0,410,416,1,
  	0,0,0,411,412,10,13,0,0,412,413,5,41,0,0,413,415,3,40,20,14,414,411,1,
  	0,0,0,415,418,1,0,0,0,416,414,1,0,0,0,416,417,1,0,0,0,417,41,1,0,0,0,
  	418,416,1,0,0,0,419,420,5,14,0,0,420,421,5,11,0,0,421,430,3,40,20,0,422,
  	423,5,14,0,0,423,424,5,10,0,0,424,430,3,40,20,0,425,426,5,14,0,0,426,
  	427,3,44,22,0,427,428,3,40,20,0,428,430,1,0,0,0,429,419,1,0,0,0,429,422,
  	1,0,0,0,429,425,1,0,0,0,430,43,1,0,0,0,431,432,3,68,34,0,432,433,5,53,
  	0,0,433,434,3,68,34,0,434,436,5,61,0,0,435,437,3,46,23,0,436,435,1,0,
  	0,0,436,437,1,0,0,0,437,438,1,0,0,0,438,439,5,62,0,0,439,448,1,0,0,0,
  	440,441,3,68,34,0,441,443,5,61,0,0,442,444,3,46,23,0,443,442,1,0,0,0,
  	443,444,1,0,0,0,444,445,1,0,0,0,445,446,5,62,0,0,446,448,1,0,0,0,447,
  	431,1,0,0,0,447,440,1,0,0,0,448,45,1,0,0,0,449,454,3,48,24,0,450,451,
  	5,51,0,0,451,453,3,48,24,0,452,450,1,0,0,0,453,456,1,0,0,0,454,452,1,
  	0,0,0,454,455,1,0,0,0,455,47,1,0,0,0,456,454,1,0,0,0,457,458,3,50,25,
  	0,458,49,1,0,0,0,459,464,3,52,26,0,460,461,5,60,0,0,461,463,3,52,26,0,
  	462,460,1,0,0,0,463,466,1,0,0,0,464,462,1,0,0,0,464,465,1,0,0,0,465,51,
  	1,0,0,0,466,464,1,0,0,0,467,472,3,54,27,0,468,469,5,59,0,0,469,471,3,
  	54,27,0,470,468,1,0,0,0,471,474,1,0,0,0,472,470,1,0,0,0,472,473,1,0,0,
  	0,473,53,1,0,0,0,474,472,1,0,0,0,475,479,3,58,29,0,476,477,3,56,28,0,
  	477,478,3,58,29,0,478,480,1,0,0,0,479,476,1,0,0,0,479,480,1,0,0,0,480,
  	55,1,0,0,0,481,482,7,3,0,0,482,57,1,0,0,0,483,484,5,58,0,0,484,487,3,
  	58,29,0,485,487,3,60,30,0,486,483,1,0,0,0,486,485,1,0,0,0,487,59,1,0,
  	0,0,488,493,3,62,31,0,489,490,7,4,0,0,490,492,3,62,31,0,491,489,1,0,0,
  	0,492,495,1,0,0,0,493,491,1,0,0,0,493,494,1,0,0,0,494,61,1,0,0,0,495,
  	493,1,0,0,0,496,501,3,64,32,0,497,498,7,5,0,0,498,500,3,64,32,0,499,497,
  	1,0,0,0,500,503,1,0,0,0,501,499,1,0,0,0,501,502,1,0,0,0,502,63,1,0,0,
  	0,503,501,1,0,0,0,504,505,5,55,0,0,505,508,3,64,32,0,506,508,3,66,33,
  	0,507,504,1,0,0,0,507,506,1,0,0,0,508,65,1,0,0,0,509,519,3,44,22,0,510,
  	519,5,69,0,0,511,519,5,70,0,0,512,519,5,67,0,0,513,519,5,68,0,0,514,515,
  	5,61,0,0,515,516,3,48,24,0,516,517,5,62,0,0,517,519,1,0,0,0,518,509,1,
  	0,0,0,518,510,1,0,0,0,518,511,1,0,0,0,518,512,1,0,0,0,518,513,1,0,0,0,
  	518,514,1,0,0,0,519,67,1,0,0,0,520,521,7,6,0,0,521,69,1,0,0,0,48,73,82,
  	89,97,104,108,115,126,134,141,148,151,162,170,188,200,212,247,253,258,
  	265,297,301,308,312,319,323,325,341,352,373,393,402,409,416,429,436,443,
  	447,454,464,472,479,486,493,501,507,518
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

std::vector<KodaParser::TopLevelComponentContext *> KodaParser::SystemContext::topLevelComponent() {
  return getRuleContexts<KodaParser::TopLevelComponentContext>();
}

KodaParser::TopLevelComponentContext* KodaParser::SystemContext::topLevelComponent(size_t i) {
  return getRuleContext<KodaParser::TopLevelComponentContext>(i);
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
    setState(73);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::TASK

    || _la == KodaParser::CAPABILITY) {
      setState(70);
      topLevelComponent();
      setState(75);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(76);
    match(KodaParser::EOF);
   
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
  enterRule(_localctx, 2, KodaParser::RuleTopLevelComponent);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(108);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(78);
        match(KodaParser::TASK);
        setState(79);
        match(KodaParser::IDENT);
        setState(80);
        match(KodaParser::LPAREN);
        setState(82);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::IDENT) {
          setState(81);
          argumentList();
        }
        setState(84);
        match(KodaParser::RPAREN);
        setState(85);
        match(KodaParser::LBRACE);
        setState(89);
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
          setState(86);
          statement();
          setState(91);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(92);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(93);
        match(KodaParser::CAPABILITY);
        setState(94);
        match(KodaParser::IDENT);
        setState(95);
        match(KodaParser::LPAREN);
        setState(97);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::IDENT) {
          setState(96);
          argumentList();
        }
        setState(99);
        match(KodaParser::RPAREN);
        setState(100);
        match(KodaParser::LBRACE);
        setState(104);
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
          setState(101);
          statement();
          setState(106);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(107);
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
  enterRule(_localctx, 4, KodaParser::RuleArgumentList);
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
    setState(110);
    argument();
    setState(115);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(111);
      match(KodaParser::COMMA);
      setState(112);
      argument();
      setState(117);
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
  enterRule(_localctx, 6, KodaParser::RuleArgument);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(126);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(118);
      match(KodaParser::IDENT);
      setState(119);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(120);
      match(KodaParser::IDENT);
      setState(121);
      match(KodaParser::REQ);
      setState(122);
      match(KodaParser::IDENT);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(123);
      match(KodaParser::IDENT);
      setState(124);
      match(KodaParser::PRO);
      setState(125);
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
  enterRule(_localctx, 8, KodaParser::RuleStatement);

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
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(128);
        tasksBlock();
        break;
      }

      case KodaParser::VARS: {
        enterOuterAlt(_localctx, 2);
        setState(129);
        varsBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 3);
        setState(130);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 4);
        setState(131);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 5);
        setState(132);
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
        setState(133);
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
  enterRule(_localctx, 10, KodaParser::RuleTasksBlock);
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
    setState(136);
    match(KodaParser::STRATEGY);
    setState(137);
    match(KodaParser::LBRACE);
    setState(139); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(138);
      flow();
      setState(141); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(143);
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
  enterRule(_localctx, 12, KodaParser::RuleFlow);
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
    setState(145);
    match(KodaParser::IDENT);
    setState(151);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(146);
      match(KodaParser::LBRACK);
      setState(148);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(147);
        identList();
      }
      setState(150);
      match(KodaParser::RBRACK);
    }
    setState(153);
    match(KodaParser::COLON);
    setState(154);
    strategy(0);
    setState(155);
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
  enterRule(_localctx, 14, KodaParser::RuleIdentList);
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
    setState(157);
    match(KodaParser::IDENT);
    setState(162);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(158);
      match(KodaParser::COMMA);
      setState(159);
      match(KodaParser::IDENT);
      setState(164);
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
  enterRule(_localctx, 16, KodaParser::RuleVarsBlock);
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
    setState(165);
    match(KodaParser::VARS);
    setState(166);
    match(KodaParser::LBRACE);
    setState(168); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(167);
      variableStatement();
      setState(170); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == KodaParser::IDENT);
    setState(172);
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
  enterRule(_localctx, 18, KodaParser::RuleVariableStatement);

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
    setState(175);
    match(KodaParser::IDENT);
    setState(176);
    match(KodaParser::ASSIGN);
    setState(177);
    expression();
    setState(178);
    match(KodaParser::COLON);
    setState(179);
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
  enterRule(_localctx, 20, KodaParser::RuleActionBlock);
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
    setState(181);
    match(KodaParser::ACTION);
    setState(182);
    match(KodaParser::STRING);
    setState(183);
    match(KodaParser::STRING);
    setState(184);
    match(KodaParser::LBRACE);
    setState(188);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT))) != 0)) {
      setState(185);
      rosDefStatement();
      setState(190);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(191);
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
  enterRule(_localctx, 22, KodaParser::RuleServiceBlock);
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
    match(KodaParser::SERVICE);
    setState(194);
    match(KodaParser::STRING);
    setState(195);
    match(KodaParser::STRING);
    setState(196);
    match(KodaParser::LBRACE);
    setState(200);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT))) != 0)) {
      setState(197);
      rosDefStatement();
      setState(202);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(203);
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
  enterRule(_localctx, 24, KodaParser::RuleTopicBlock);
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
    setState(205);
    match(KodaParser::TOPIC);
    setState(206);
    match(KodaParser::STRING);
    setState(207);
    match(KodaParser::STRING);
    setState(208);
    match(KodaParser::LBRACE);
    setState(212);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TRIGGER)
      | (1ULL << KodaParser::RETURN)
      | (1ULL << KodaParser::ABORT)
      | (1ULL << KodaParser::ERROR)
      | (1ULL << KodaParser::IN)
      | (1ULL << KodaParser::OUT))) != 0)) {
      setState(209);
      rosDefStatement();
      setState(214);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(215);
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
  enterRule(_localctx, 26, KodaParser::RuleRosDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(247);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(217);
        match(KodaParser::TRIGGER);
        setState(218);
        match(KodaParser::COLON);
        setState(219);
        eventDefStatement();
        setState(220);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(222);
        match(KodaParser::RETURN);
        setState(223);
        match(KodaParser::COLON);
        setState(224);
        eventDefStatement();
        setState(225);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(227);
        match(KodaParser::ABORT);
        setState(228);
        match(KodaParser::COLON);
        setState(229);
        eventDefStatement();
        setState(230);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(232);
        match(KodaParser::ERROR);
        setState(233);
        match(KodaParser::COLON);
        setState(234);
        eventDefStatement();
        setState(235);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(237);
        match(KodaParser::IN);
        setState(238);
        match(KodaParser::COLON);
        setState(239);
        eventDefStatement();
        setState(240);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(242);
        match(KodaParser::OUT);
        setState(243);
        match(KodaParser::COLON);
        setState(244);
        eventDefStatement();
        setState(245);
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
  enterRule(_localctx, 28, KodaParser::RuleEventDefStatement);
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
    setState(249);
    match(KodaParser::IDENT);
    setState(250);
    identifier();
    setState(251);
    match(KodaParser::LPAREN);
    setState(253);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::IDENT) {
      setState(252);
      argumentList();
    }
    setState(255);
    match(KodaParser::RPAREN);
    setState(258);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(256);
      match(KodaParser::COLON);
      setState(257);
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
  enterRule(_localctx, 30, KodaParser::RuleEventDefComponentList);
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
    setState(260);
    eventDefComponent();
    setState(263); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(261);
      match(KodaParser::COMMA);
      setState(262);
      eventDefComponent();
      setState(265); 
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
  enterRule(_localctx, 32, KodaParser::RuleEventDefComponent);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(325);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::ACTION:
      case KodaParser::SERVICE:
      case KodaParser::TOPIC: {
        _localctx = _tracker.createInstance<KodaParser::EdcRosEventContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(267);
        rosData();
        setState(268);
        match(KodaParser::IDENT);
        setState(269);
        match(KodaParser::COLON);
        setState(270);
        match(KodaParser::STRING);
        setState(271);
        match(KodaParser::IDENT);
        setState(272);
        match(KodaParser::STRING);
        setState(273);
        match(KodaParser::IDENT);
        setState(274);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::TIMEOUT: {
        _localctx = _tracker.createInstance<KodaParser::EdcTimeoutContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(276);
        match(KodaParser::TIMEOUT);
        setState(277);
        match(KodaParser::NATURAL);
        setState(278);
        timeUnit();
        setState(279);
        match(KodaParser::ARROW);
        setState(280);
        match(KodaParser::IDENT);
        break;
      }

      case KodaParser::ALLOWED: {
        _localctx = _tracker.createInstance<KodaParser::EdcWhenAllowedInContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(282);
        match(KodaParser::ALLOWED);
        setState(283);
        match(KodaParser::IN);
        setState(284);
        whenMode();
        break;
      }

      case KodaParser::REPLY: {
        _localctx = _tracker.createInstance<KodaParser::EdcReplyContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(285);
        match(KodaParser::REPLY);
        setState(286);
        match(KodaParser::IDENT);
        setState(287);
        whenMode();
        break;
      }

      case KodaParser::AFTER: {
        _localctx = _tracker.createInstance<KodaParser::EdcDependsAfterContext>(_localctx);
        enterOuterAlt(_localctx, 5);
        setState(288);
        match(KodaParser::AFTER);
        setState(289);
        match(KodaParser::IDENT);
        break;
      }

      case KodaParser::ONCE: {
        _localctx = _tracker.createInstance<KodaParser::EdcOnceInContext>(_localctx);
        enterOuterAlt(_localctx, 6);
        setState(290);
        match(KodaParser::ONCE);
        setState(291);
        match(KodaParser::IN);
        setState(292);
        whenMode();
        setState(301);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::LBRACE) {
          setState(293);
          match(KodaParser::LBRACE);
          setState(297);
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
            setState(294);
            statement();
            setState(299);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
          setState(300);
          match(KodaParser::RBRACE);
        }
        break;
      }

      case KodaParser::TRIGGER: {
        _localctx = _tracker.createInstance<KodaParser::EdcStartContext>(_localctx);
        enterOuterAlt(_localctx, 7);
        setState(303);
        match(KodaParser::TRIGGER);
        setState(312);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::LBRACE) {
          setState(304);
          match(KodaParser::LBRACE);
          setState(308);
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
            setState(305);
            statement();
            setState(310);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
          setState(311);
          match(KodaParser::RBRACE);
        }
        break;
      }

      case KodaParser::ABORT: {
        _localctx = _tracker.createInstance<KodaParser::EdcResetContext>(_localctx);
        enterOuterAlt(_localctx, 8);
        setState(314);
        match(KodaParser::ABORT);
        setState(323);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::LBRACE) {
          setState(315);
          match(KodaParser::LBRACE);
          setState(319);
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
            setState(316);
            statement();
            setState(321);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
          setState(322);
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
  enterRule(_localctx, 34, KodaParser::RuleRosData);
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
    setState(327);
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
  enterRule(_localctx, 36, KodaParser::RuleWhenMode);
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
  enterRule(_localctx, 38, KodaParser::RuleTimeUnit);
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
    setState(331);
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

tree::TerminalNode* KodaParser::StratRepeatContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

KodaParser::StrategyContext* KodaParser::StratRepeatContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

tree::TerminalNode* KodaParser::StratRepeatContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}

KodaParser::StratRepeatContext::StratRepeatContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratRepeatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratRepeat(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StratEveryContext ------------------------------------------------------------------

tree::TerminalNode* KodaParser::StratEveryContext::EVERY() {
  return getToken(KodaParser::EVERY, 0);
}

tree::TerminalNode* KodaParser::StratEveryContext::NATURAL() {
  return getToken(KodaParser::NATURAL, 0);
}

tree::TerminalNode* KodaParser::StratEveryContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

KodaParser::StrategyContext* KodaParser::StratEveryContext::strategy() {
  return getRuleContext<KodaParser::StrategyContext>(0);
}

tree::TerminalNode* KodaParser::StratEveryContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::StrategyHandlerContext *> KodaParser::StratEveryContext::strategyHandler() {
  return getRuleContexts<KodaParser::StrategyHandlerContext>();
}

KodaParser::StrategyHandlerContext* KodaParser::StratEveryContext::strategyHandler(size_t i) {
  return getRuleContext<KodaParser::StrategyHandlerContext>(i);
}

KodaParser::StratEveryContext::StratEveryContext(StrategyContext *ctx) { copyFrom(ctx); }


std::any KodaParser::StratEveryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitStratEvery(this);
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

tree::TerminalNode* KodaParser::StratSeqContext::ARROW() {
  return getToken(KodaParser::ARROW, 0);
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

KodaParser::StrategyContext* KodaParser::strategy() {
   return strategy(0);
}

KodaParser::StrategyContext* KodaParser::strategy(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  KodaParser::StrategyContext *_localctx = _tracker.createInstance<StrategyContext>(_ctx, parentState);
  KodaParser::StrategyContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 40;
  enterRecursionRule(_localctx, 40, KodaParser::RuleStrategy, precedence);

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
    setState(409);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(334);
      match(KodaParser::JOIN);
      setState(335);
      match(KodaParser::LPAREN);
      setState(336);
      strategy(0);
      setState(339); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(337);
        match(KodaParser::PIPE);
        setState(338);
        strategy(0);
        setState(341); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == KodaParser::PIPE);
      setState(343);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(345);
      match(KodaParser::EITHER);
      setState(346);
      match(KodaParser::LPAREN);
      setState(347);
      strategy(0);
      setState(350); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(348);
        match(KodaParser::PIPE);
        setState(349);
        strategy(0);
        setState(352); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == KodaParser::PIPE);
      setState(354);
      match(KodaParser::RPAREN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<StratLetContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(356);
      match(KodaParser::LET);
      setState(357);
      match(KodaParser::IDENT);
      setState(358);
      match(KodaParser::ASSIGN);
      setState(359);
      eventStatement();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(360);
      match(KodaParser::WITHIN);
      setState(361);
      match(KodaParser::NATURAL);
      setState(362);
      match(KodaParser::DO);
      setState(363);
      strategy(0);
      setState(364);
      match(KodaParser::ELSE);
      setState(365);
      strategy(9);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<StratIfElseContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(367);
      match(KodaParser::IF);
      setState(368);
      expression();
      setState(369);
      match(KodaParser::THEN);
      setState(370);
      strategy(0);
      setState(373);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
      case 1: {
        setState(371);
        match(KodaParser::ELSE);
        setState(372);
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
      setState(375);
      match(KodaParser::REPEAT);
      setState(376);
      match(KodaParser::LPAREN);
      setState(377);
      strategy(0);
      setState(378);
      match(KodaParser::RPAREN);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<StratGuardContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(380);
      match(KodaParser::GUARD);
      setState(381);
      match(KodaParser::LBRACE);
      setState(382);
      expression();
      setState(383);
      match(KodaParser::RBRACE);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<StratEveryContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(385);
      match(KodaParser::EVERY);
      setState(386);
      match(KodaParser::NATURAL);
      setState(387);
      match(KodaParser::LBRACE);
      setState(388);
      strategy(0);
      setState(389);
      match(KodaParser::RBRACE);
      setState(393);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(390);
          strategyHandler(); 
        }
        setState(395);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx);
      }
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<StratEndContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(396);
      match(KodaParser::END);
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<StratRefContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(397);
      identifier();
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<StratTaskContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(398);
      eventStatement();
      setState(402);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(399);
          strategyHandler(); 
        }
        setState(404);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx);
      }
      break;
    }

    case 12: {
      _localctx = _tracker.createInstance<StratParenContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(405);
      match(KodaParser::LPAREN);
      setState(406);
      strategy(0);
      setState(407);
      match(KodaParser::RPAREN);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(416);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<StratSeqContext>(_tracker.createInstance<StrategyContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleStrategy);
        setState(411);

        if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
        setState(412);
        match(KodaParser::ARROW);
        setState(413);
        strategy(14); 
      }
      setState(418);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx);
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
  enterRule(_localctx, 42, KodaParser::RuleStrategyHandler);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(429);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(419);
      match(KodaParser::ON);
      setState(420);
      match(KodaParser::ERROR);
      setState(421);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(422);
      match(KodaParser::ON);
      setState(423);
      match(KodaParser::ABORT);
      setState(424);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(425);
      match(KodaParser::ON);
      setState(426);
      eventStatement();
      setState(427);
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
  enterRule(_localctx, 44, KodaParser::RuleEventStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(447);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(431);
      identifier();
      setState(432);
      match(KodaParser::DOT);
      setState(433);
      identifier();
      setState(434);
      match(KodaParser::LPAREN);
      setState(436);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 8) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 8)) & ((1ULL << (KodaParser::TRIGGER - 8))
        | (1ULL << (KodaParser::RETURN - 8))
        | (1ULL << (KodaParser::ABORT - 8))
        | (1ULL << (KodaParser::ERROR - 8))
        | (1ULL << (KodaParser::IN - 8))
        | (1ULL << (KodaParser::OUT - 8))
        | (1ULL << (KodaParser::MINUS - 8))
        | (1ULL << (KodaParser::NOT - 8))
        | (1ULL << (KodaParser::LPAREN - 8))
        | (1ULL << (KodaParser::NATURAL - 8))
        | (1ULL << (KodaParser::REAL - 8))
        | (1ULL << (KodaParser::IDENT - 8))
        | (1ULL << (KodaParser::STRING - 8)))) != 0)) {
        setState(435);
        exprList();
      }
      setState(438);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(440);
      identifier();
      setState(441);
      match(KodaParser::LPAREN);
      setState(443);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 8) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 8)) & ((1ULL << (KodaParser::TRIGGER - 8))
        | (1ULL << (KodaParser::RETURN - 8))
        | (1ULL << (KodaParser::ABORT - 8))
        | (1ULL << (KodaParser::ERROR - 8))
        | (1ULL << (KodaParser::IN - 8))
        | (1ULL << (KodaParser::OUT - 8))
        | (1ULL << (KodaParser::MINUS - 8))
        | (1ULL << (KodaParser::NOT - 8))
        | (1ULL << (KodaParser::LPAREN - 8))
        | (1ULL << (KodaParser::NATURAL - 8))
        | (1ULL << (KodaParser::REAL - 8))
        | (1ULL << (KodaParser::IDENT - 8))
        | (1ULL << (KodaParser::STRING - 8)))) != 0)) {
        setState(442);
        exprList();
      }
      setState(445);
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
  enterRule(_localctx, 46, KodaParser::RuleExprList);
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
    setState(449);
    expression();
    setState(454);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(450);
      match(KodaParser::COMMA);
      setState(451);
      expression();
      setState(456);
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
  enterRule(_localctx, 48, KodaParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(457);
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
  enterRule(_localctx, 50, KodaParser::RuleExprOr);
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
    setState(459);
    exprAnd();
    setState(464);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(460);
      match(KodaParser::OR);
      setState(461);
      exprAnd();
      setState(466);
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
  enterRule(_localctx, 52, KodaParser::RuleExprAnd);
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
    setState(467);
    exprCmp();
    setState(472);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(468);
      match(KodaParser::AND);
      setState(469);
      exprCmp();
      setState(474);
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
  enterRule(_localctx, 54, KodaParser::RuleExprCmp);
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
    setState(475);
    exprNot();
    setState(479);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::EQ)
      | (1ULL << KodaParser::NEQ)
      | (1ULL << KodaParser::LEQ)
      | (1ULL << KodaParser::GEQ)
      | (1ULL << KodaParser::LT)
      | (1ULL << KodaParser::GT))) != 0)) {
      setState(476);
      compOp();
      setState(477);
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
  enterRule(_localctx, 56, KodaParser::RuleCompOp);
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
  enterRule(_localctx, 58, KodaParser::RuleExprNot);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(486);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(483);
        match(KodaParser::NOT);
        setState(484);
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
        setState(485);
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
  enterRule(_localctx, 60, KodaParser::RuleExprAdd);
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
    setState(488);
    exprMul();
    setState(493);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(489);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(490);
      exprMul();
      setState(495);
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
  enterRule(_localctx, 62, KodaParser::RuleExprMul);
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
    setState(496);
    exprUnary();
    setState(501);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(497);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(498);
      exprUnary();
      setState(503);
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
  enterRule(_localctx, 64, KodaParser::RuleExprUnary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(507);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(504);
        match(KodaParser::MINUS);
        setState(505);
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
        setState(506);
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
  enterRule(_localctx, 66, KodaParser::RuleExprPrimary);

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
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(509);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(510);
      match(KodaParser::IDENT);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(511);
      match(KodaParser::STRING);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(512);
      match(KodaParser::NATURAL);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(513);
      match(KodaParser::REAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(514);
      match(KodaParser::LPAREN);
      setState(515);
      expression();
      setState(516);
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
  enterRule(_localctx, 68, KodaParser::RuleIdentifier);
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
    _la = _input->LA(1);
    if (!(((((_la - 8) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 8)) & ((1ULL << (KodaParser::TRIGGER - 8))
      | (1ULL << (KodaParser::RETURN - 8))
      | (1ULL << (KodaParser::ABORT - 8))
      | (1ULL << (KodaParser::ERROR - 8))
      | (1ULL << (KodaParser::IN - 8))
      | (1ULL << (KodaParser::OUT - 8))
      | (1ULL << (KodaParser::IDENT - 8)))) != 0))) {
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
    case 20: return strategySempred(antlrcpp::downCast<StrategyContext *>(context), predicateIndex);

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
