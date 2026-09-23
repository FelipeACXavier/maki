
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
      "flow", "identList", "varsBlock", "variableStatement", "propertiesBlock", 
      "propertyStatement", "actionBlock", "serviceBlock", "topicBlock", 
      "propertyExpr", "propertyHelper", "propertyImplication", "propertyConditionOr", 
      "propertyConditionAnd", "propertyConditionUntil", "propertyConditionUnary", 
      "propertyConsequence", "propertyObservation", "propertyReference", 
      "rosDefStatement", "reqDefStatement", "eventDefStatement", "dataBlock", 
      "strategy", "chooseWhenStatement", "strategyHandler", "eventStatement", 
      "exprList", "expression", "exprOr", "exprAnd", "exprCmp", "compOp", 
      "exprNot", "exprAdd", "exprMul", "exprUnary", "exprPrimary", "recordLiteral", 
      "recordFieldInitializer", "listLiteral", "mapLiteral", "mapFieldInitializer", 
      "identifier"
    },
    std::vector<std::string>{
      "", "'task'", "'capability'", "'type'", "'enum'", "'extends'", "'list'", 
      "'optional'", "'map'", "'mapping'", "'to'", "'annotations'", "'strategy'", 
      "'parameters'", "'properties'", "'action'", "'service'", "'topic'", 
      "'implies'", "'while'", "'always'", "'eventually'", "'next'", "'never'", 
      "'not'", "'if'", "'between'", "'until'", "'is'", "'was'", "'running'", 
      "'started'", "'rejected'", "'stopped'", "'aborted'", "'trigger'", 
      "'return'", "'abort'", "'error'", "'in'", "'out'", "'on'", "'consumes'", 
      "'produces'", "'data'", "'req'", "'pro'", "'success'", "'failure'", 
      "'continue'", "'repeat'", "'join'", "'either'", "'when'", "'choose'", 
      "'within'", "'do'", "'else'", "'then'", "'every'", "'-->'", "'|'", 
      "'=='", "'!='", "'<='", "'>='", "'<'", "'>'", "'='", "'::'", "':'", 
      "','", "';'", "'.'", "'+'", "'-'", "'*'", "'/'", "'!'", "'and'", "'or'", 
      "'('", "')'", "'{'", "'}'", "'['", "']'"
    },
    std::vector<std::string>{
      "", "TASK", "CAPABILITY", "TYPE", "ENUM", "EXTENDS", "LIST", "OPTIONAL", 
      "MAP", "MAPPING", "TO", "ANNOTATIONS", "STRATEGY", "PARAMETERS", "PROPERTIES", 
      "ACTION", "SERVICE", "TOPIC", "IMPLIES", "WHILE", "ALWAYS", "EVENTUALLY", 
      "NEXT", "NEVER", "NEGATION", "IF", "BETWEEN", "UNTIL", "IS", "WAS", 
      "RUNNING", "STARTED", "REJECTED", "STOPPED", "ABORTED", "TRIGGER", 
      "RETURN", "ABORT", "ERROR", "IN", "OUT", "ON", "CONSUMES", "PRODUCES", 
      "DATA", "REQ", "PRO", "SUCCESS", "FAILURE", "CONTINUE", "REPEAT", 
      "JOIN", "EITHER", "WHEN", "CHOOSE", "WITHIN", "DO", "ELSE", "THEN", 
      "EVERY", "ARROW", "PIPE", "EQ", "NEQ", "LEQ", "GEQ", "LT", "GT", "ASSIGN", 
      "DOUBLE_COLON", "COLON", "COMMA", "SEMI", "DOT", "PLUS", "MINUS", 
      "STAR", "SLASH", "NOT", "AND", "OR", "LPAREN", "RPAREN", "LBRACE", 
      "RBRACE", "LBRACK", "RBRACK", "NATURAL", "REAL", "BOOLEAN", "IDENT", 
      "STRING", "ANY", "LINE_COMMENT", "BLOCK_COMMENT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,95,813,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,1,0,5,0,124,8,0,10,0,12,0,
  	127,9,0,1,0,1,0,1,1,1,1,1,1,1,1,3,1,135,8,1,1,2,1,2,1,2,1,2,3,2,141,8,
  	2,1,2,1,2,1,2,5,2,146,8,2,10,2,12,2,149,9,2,1,2,1,2,1,2,1,2,1,2,3,2,156,
  	8,2,1,2,1,2,1,2,5,2,161,8,2,10,2,12,2,164,9,2,1,2,3,2,167,8,2,1,3,1,3,
  	1,3,1,3,3,3,173,8,3,1,3,1,3,5,3,177,8,3,10,3,12,3,180,9,3,1,3,1,3,1,3,
  	1,3,1,3,1,3,1,3,1,3,3,3,190,8,3,1,4,1,4,1,4,1,4,1,4,1,4,3,4,198,8,4,1,
  	5,1,5,1,5,1,5,1,5,5,5,205,8,5,10,5,12,5,208,9,5,3,5,210,8,5,1,5,1,5,1,
  	6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,3,7,222,8,7,1,7,1,7,5,7,226,8,7,10,7,12,
  	7,229,9,7,1,7,1,7,1,8,1,8,1,8,3,8,236,8,8,1,8,1,8,1,9,1,9,3,9,242,8,9,
  	1,9,3,9,245,8,9,1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,1,11,1,11,
  	1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,3,11,270,
  	8,11,1,12,1,12,1,12,5,12,275,8,12,10,12,12,12,278,9,12,1,13,1,13,1,13,
  	5,13,283,8,13,10,13,12,13,286,9,13,1,14,1,14,1,14,1,14,1,14,1,14,1,14,
  	1,14,1,14,3,14,297,8,14,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,3,15,
  	307,8,15,1,16,1,16,1,16,5,16,312,8,16,10,16,12,16,315,9,16,1,16,1,16,
  	1,17,1,17,1,17,3,17,322,8,17,1,17,3,17,325,8,17,1,17,1,17,1,17,1,17,1,
  	18,1,18,1,18,5,18,334,8,18,10,18,12,18,337,9,18,1,19,1,19,1,19,5,19,342,
  	8,19,10,19,12,19,345,9,19,1,19,1,19,1,20,1,20,1,20,1,20,3,20,353,8,20,
  	1,20,1,20,1,21,1,21,1,21,5,21,360,8,21,10,21,12,21,363,9,21,1,21,1,21,
  	1,22,1,22,1,22,1,22,1,22,1,23,1,23,1,23,1,23,1,23,1,23,5,23,378,8,23,
  	10,23,12,23,381,9,23,1,23,1,23,1,24,1,24,1,24,1,24,1,24,1,24,5,24,391,
  	8,24,10,24,12,24,394,9,24,1,24,1,24,1,25,1,25,1,25,1,25,1,25,1,25,5,25,
  	404,8,25,10,25,12,25,407,9,25,1,25,1,25,1,26,1,26,1,27,1,27,1,27,1,27,
  	1,27,1,27,1,27,1,27,1,27,1,27,1,27,3,27,424,8,27,1,28,1,28,1,28,3,28,
  	429,8,28,1,29,1,29,1,29,3,29,434,8,29,1,30,1,30,1,30,3,30,439,8,30,1,
  	31,1,31,1,31,3,31,444,8,31,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,
  	32,1,32,1,32,1,32,1,32,1,32,1,32,3,32,461,8,32,1,33,1,33,1,33,1,33,1,
  	33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,3,33,478,8,33,1,
  	34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,
  	34,1,34,1,34,1,34,3,34,498,8,34,1,35,1,35,1,35,3,35,503,8,35,1,36,1,36,
  	1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,
  	1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,
  	3,36,535,8,36,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,3,37,
  	547,8,37,1,38,1,38,1,38,1,38,3,38,553,8,38,1,38,1,38,1,39,1,39,1,39,5,
  	39,560,8,39,10,39,12,39,563,9,39,1,39,1,39,1,40,1,40,1,40,1,40,1,40,1,
  	40,4,40,573,8,40,11,40,12,40,574,1,40,1,40,1,40,1,40,1,40,1,40,1,40,4,
  	40,584,8,40,11,40,12,40,585,1,40,1,40,1,40,1,40,1,40,1,40,1,40,1,40,1,
  	40,1,40,1,40,1,40,1,40,1,40,1,40,1,40,5,40,604,8,40,10,40,12,40,607,9,
  	40,1,40,1,40,1,40,1,40,1,40,5,40,614,8,40,10,40,12,40,617,9,40,1,40,1,
  	40,1,40,1,40,1,40,1,40,1,40,5,40,626,8,40,10,40,12,40,629,9,40,1,40,3,
  	40,632,8,40,1,40,1,40,1,40,4,40,637,8,40,11,40,12,40,638,5,40,641,8,40,
  	10,40,12,40,644,9,40,1,41,1,41,3,41,648,8,41,1,41,1,41,1,41,1,41,1,42,
  	1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,3,42,664,8,42,1,43,1,43,
  	1,43,1,43,1,43,3,43,671,8,43,1,43,1,43,1,43,1,43,1,43,3,43,678,8,43,1,
  	43,1,43,3,43,682,8,43,1,44,1,44,1,44,5,44,687,8,44,10,44,12,44,690,9,
  	44,1,45,1,45,1,46,1,46,1,46,5,46,697,8,46,10,46,12,46,700,9,46,1,47,1,
  	47,1,47,5,47,705,8,47,10,47,12,47,708,9,47,1,48,1,48,1,48,1,48,3,48,714,
  	8,48,1,49,1,49,1,50,1,50,1,50,3,50,721,8,50,1,51,1,51,1,51,5,51,726,8,
  	51,10,51,12,51,729,9,51,1,52,1,52,1,52,5,52,734,8,52,10,52,12,52,737,
  	9,52,1,53,1,53,1,53,3,53,742,8,53,1,54,1,54,1,54,1,54,1,54,1,54,1,54,
  	1,54,1,54,1,54,1,54,1,54,1,54,1,54,1,54,1,54,3,54,760,8,54,1,55,1,55,
  	1,55,1,55,5,55,766,8,55,10,55,12,55,769,9,55,3,55,771,8,55,1,55,1,55,
  	1,56,1,56,1,56,1,56,1,57,1,57,1,57,1,57,5,57,783,8,57,10,57,12,57,786,
  	9,57,3,57,788,8,57,1,57,1,57,1,58,1,58,1,58,1,58,5,58,796,8,58,10,58,
  	12,58,799,9,58,3,58,801,8,58,1,58,1,58,1,59,1,59,1,59,1,59,1,59,1,59,
  	1,60,1,60,1,60,0,1,80,61,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,
  	34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,
  	80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,
  	120,0,5,2,0,19,19,79,79,1,0,62,67,1,0,74,75,1,0,76,77,3,0,35,40,42,43,
  	90,90,874,0,125,1,0,0,0,2,134,1,0,0,0,4,166,1,0,0,0,6,189,1,0,0,0,8,197,
  	1,0,0,0,10,199,1,0,0,0,12,213,1,0,0,0,14,217,1,0,0,0,16,232,1,0,0,0,18,
  	244,1,0,0,0,20,246,1,0,0,0,22,269,1,0,0,0,24,271,1,0,0,0,26,279,1,0,0,
  	0,28,296,1,0,0,0,30,306,1,0,0,0,32,308,1,0,0,0,34,318,1,0,0,0,36,330,
  	1,0,0,0,38,338,1,0,0,0,40,348,1,0,0,0,42,356,1,0,0,0,44,366,1,0,0,0,46,
  	371,1,0,0,0,48,384,1,0,0,0,50,397,1,0,0,0,52,410,1,0,0,0,54,423,1,0,0,
  	0,56,425,1,0,0,0,58,430,1,0,0,0,60,435,1,0,0,0,62,440,1,0,0,0,64,460,
  	1,0,0,0,66,477,1,0,0,0,68,497,1,0,0,0,70,499,1,0,0,0,72,534,1,0,0,0,74,
  	546,1,0,0,0,76,548,1,0,0,0,78,556,1,0,0,0,80,631,1,0,0,0,82,645,1,0,0,
  	0,84,663,1,0,0,0,86,681,1,0,0,0,88,683,1,0,0,0,90,691,1,0,0,0,92,693,
  	1,0,0,0,94,701,1,0,0,0,96,709,1,0,0,0,98,715,1,0,0,0,100,720,1,0,0,0,
  	102,722,1,0,0,0,104,730,1,0,0,0,106,741,1,0,0,0,108,759,1,0,0,0,110,761,
  	1,0,0,0,112,774,1,0,0,0,114,778,1,0,0,0,116,791,1,0,0,0,118,804,1,0,0,
  	0,120,810,1,0,0,0,122,124,3,2,1,0,123,122,1,0,0,0,124,127,1,0,0,0,125,
  	123,1,0,0,0,125,126,1,0,0,0,126,128,1,0,0,0,127,125,1,0,0,0,128,129,5,
  	0,0,1,129,1,1,0,0,0,130,135,3,4,2,0,131,135,3,6,3,0,132,135,3,14,7,0,
  	133,135,3,20,10,0,134,130,1,0,0,0,134,131,1,0,0,0,134,132,1,0,0,0,134,
  	133,1,0,0,0,135,3,1,0,0,0,136,137,5,1,0,0,137,138,5,90,0,0,138,140,5,
  	81,0,0,139,141,3,26,13,0,140,139,1,0,0,0,140,141,1,0,0,0,141,142,1,0,
  	0,0,142,143,5,82,0,0,143,147,5,83,0,0,144,146,3,30,15,0,145,144,1,0,0,
  	0,146,149,1,0,0,0,147,145,1,0,0,0,147,148,1,0,0,0,148,150,1,0,0,0,149,
  	147,1,0,0,0,150,167,5,84,0,0,151,152,5,2,0,0,152,153,5,90,0,0,153,155,
  	5,81,0,0,154,156,3,26,13,0,155,154,1,0,0,0,155,156,1,0,0,0,156,157,1,
  	0,0,0,157,158,5,82,0,0,158,162,5,83,0,0,159,161,3,30,15,0,160,159,1,0,
  	0,0,161,164,1,0,0,0,162,160,1,0,0,0,162,163,1,0,0,0,163,165,1,0,0,0,164,
  	162,1,0,0,0,165,167,5,84,0,0,166,136,1,0,0,0,166,151,1,0,0,0,167,5,1,
  	0,0,0,168,169,5,3,0,0,169,172,3,24,12,0,170,171,5,5,0,0,171,173,3,24,
  	12,0,172,170,1,0,0,0,172,173,1,0,0,0,173,174,1,0,0,0,174,178,5,83,0,0,
  	175,177,3,8,4,0,176,175,1,0,0,0,177,180,1,0,0,0,178,176,1,0,0,0,178,179,
  	1,0,0,0,179,181,1,0,0,0,180,178,1,0,0,0,181,182,5,84,0,0,182,190,1,0,
  	0,0,183,184,5,3,0,0,184,185,3,24,12,0,185,186,5,68,0,0,186,187,3,22,11,
  	0,187,188,5,72,0,0,188,190,1,0,0,0,189,168,1,0,0,0,189,183,1,0,0,0,190,
  	7,1,0,0,0,191,192,5,90,0,0,192,193,5,70,0,0,193,194,3,22,11,0,194,195,
  	5,72,0,0,195,198,1,0,0,0,196,198,3,10,5,0,197,191,1,0,0,0,197,196,1,0,
  	0,0,198,9,1,0,0,0,199,200,5,11,0,0,200,209,5,83,0,0,201,206,3,12,6,0,
  	202,203,5,71,0,0,203,205,3,12,6,0,204,202,1,0,0,0,205,208,1,0,0,0,206,
  	204,1,0,0,0,206,207,1,0,0,0,207,210,1,0,0,0,208,206,1,0,0,0,209,201,1,
  	0,0,0,209,210,1,0,0,0,210,211,1,0,0,0,211,212,5,84,0,0,212,11,1,0,0,0,
  	213,214,5,91,0,0,214,215,5,70,0,0,215,216,5,91,0,0,216,13,1,0,0,0,217,
  	218,5,4,0,0,218,221,3,24,12,0,219,220,5,70,0,0,220,222,3,22,11,0,221,
  	219,1,0,0,0,221,222,1,0,0,0,222,223,1,0,0,0,223,227,5,83,0,0,224,226,
  	3,16,8,0,225,224,1,0,0,0,226,229,1,0,0,0,227,225,1,0,0,0,227,228,1,0,
  	0,0,228,230,1,0,0,0,229,227,1,0,0,0,230,231,5,84,0,0,231,15,1,0,0,0,232,
  	235,5,90,0,0,233,234,5,68,0,0,234,236,3,18,9,0,235,233,1,0,0,0,235,236,
  	1,0,0,0,236,237,1,0,0,0,237,238,5,72,0,0,238,17,1,0,0,0,239,245,5,91,
  	0,0,240,242,5,75,0,0,241,240,1,0,0,0,241,242,1,0,0,0,242,243,1,0,0,0,
  	243,245,5,87,0,0,244,239,1,0,0,0,244,241,1,0,0,0,245,19,1,0,0,0,246,247,
  	5,9,0,0,247,248,3,22,11,0,248,249,5,10,0,0,249,250,5,91,0,0,250,21,1,
  	0,0,0,251,270,3,24,12,0,252,253,5,6,0,0,253,254,5,66,0,0,254,255,3,22,
  	11,0,255,256,5,67,0,0,256,270,1,0,0,0,257,258,5,7,0,0,258,259,5,66,0,
  	0,259,260,3,22,11,0,260,261,5,67,0,0,261,270,1,0,0,0,262,263,5,8,0,0,
  	263,264,5,66,0,0,264,265,3,22,11,0,265,266,5,71,0,0,266,267,3,22,11,0,
  	267,268,5,67,0,0,268,270,1,0,0,0,269,251,1,0,0,0,269,252,1,0,0,0,269,
  	257,1,0,0,0,269,262,1,0,0,0,270,23,1,0,0,0,271,276,5,90,0,0,272,273,5,
  	69,0,0,273,275,5,90,0,0,274,272,1,0,0,0,275,278,1,0,0,0,276,274,1,0,0,
  	0,276,277,1,0,0,0,277,25,1,0,0,0,278,276,1,0,0,0,279,284,3,28,14,0,280,
  	281,5,71,0,0,281,283,3,28,14,0,282,280,1,0,0,0,283,286,1,0,0,0,284,282,
  	1,0,0,0,284,285,1,0,0,0,285,27,1,0,0,0,286,284,1,0,0,0,287,288,3,22,11,
  	0,288,289,5,90,0,0,289,297,1,0,0,0,290,291,5,90,0,0,291,292,5,45,0,0,
  	292,297,3,22,11,0,293,294,5,90,0,0,294,295,5,46,0,0,295,297,3,22,11,0,
  	296,287,1,0,0,0,296,290,1,0,0,0,296,293,1,0,0,0,297,29,1,0,0,0,298,307,
  	3,32,16,0,299,307,3,38,19,0,300,307,3,42,21,0,301,307,3,46,23,0,302,307,
  	3,48,24,0,303,307,3,50,25,0,304,307,3,72,36,0,305,307,3,78,39,0,306,298,
  	1,0,0,0,306,299,1,0,0,0,306,300,1,0,0,0,306,301,1,0,0,0,306,302,1,0,0,
  	0,306,303,1,0,0,0,306,304,1,0,0,0,306,305,1,0,0,0,307,31,1,0,0,0,308,
  	309,5,12,0,0,309,313,5,83,0,0,310,312,3,34,17,0,311,310,1,0,0,0,312,315,
  	1,0,0,0,313,311,1,0,0,0,313,314,1,0,0,0,314,316,1,0,0,0,315,313,1,0,0,
  	0,316,317,5,84,0,0,317,33,1,0,0,0,318,324,5,90,0,0,319,321,5,85,0,0,320,
  	322,3,36,18,0,321,320,1,0,0,0,321,322,1,0,0,0,322,323,1,0,0,0,323,325,
  	5,86,0,0,324,319,1,0,0,0,324,325,1,0,0,0,325,326,1,0,0,0,326,327,5,70,
  	0,0,327,328,3,80,40,0,328,329,5,72,0,0,329,35,1,0,0,0,330,335,5,90,0,
  	0,331,332,5,71,0,0,332,334,5,90,0,0,333,331,1,0,0,0,334,337,1,0,0,0,335,
  	333,1,0,0,0,335,336,1,0,0,0,336,37,1,0,0,0,337,335,1,0,0,0,338,339,5,
  	13,0,0,339,343,5,83,0,0,340,342,3,40,20,0,341,340,1,0,0,0,342,345,1,0,
  	0,0,343,341,1,0,0,0,343,344,1,0,0,0,344,346,1,0,0,0,345,343,1,0,0,0,346,
  	347,5,84,0,0,347,39,1,0,0,0,348,349,3,22,11,0,349,352,5,90,0,0,350,351,
  	5,68,0,0,351,353,3,90,45,0,352,350,1,0,0,0,352,353,1,0,0,0,353,354,1,
  	0,0,0,354,355,5,72,0,0,355,41,1,0,0,0,356,357,5,14,0,0,357,361,5,83,0,
  	0,358,360,3,44,22,0,359,358,1,0,0,0,360,363,1,0,0,0,361,359,1,0,0,0,361,
  	362,1,0,0,0,362,364,1,0,0,0,363,361,1,0,0,0,364,365,5,84,0,0,365,43,1,
  	0,0,0,366,367,5,90,0,0,367,368,5,70,0,0,368,369,3,52,26,0,369,370,5,72,
  	0,0,370,45,1,0,0,0,371,372,5,15,0,0,372,373,5,91,0,0,373,374,5,91,0,0,
  	374,379,5,83,0,0,375,378,3,74,37,0,376,378,3,72,36,0,377,375,1,0,0,0,
  	377,376,1,0,0,0,378,381,1,0,0,0,379,377,1,0,0,0,379,380,1,0,0,0,380,382,
  	1,0,0,0,381,379,1,0,0,0,382,383,5,84,0,0,383,47,1,0,0,0,384,385,5,16,
  	0,0,385,386,5,91,0,0,386,387,5,91,0,0,387,392,5,83,0,0,388,391,3,74,37,
  	0,389,391,3,72,36,0,390,388,1,0,0,0,390,389,1,0,0,0,391,394,1,0,0,0,392,
  	390,1,0,0,0,392,393,1,0,0,0,393,395,1,0,0,0,394,392,1,0,0,0,395,396,5,
  	84,0,0,396,49,1,0,0,0,397,398,5,17,0,0,398,399,5,91,0,0,399,400,5,91,
  	0,0,400,405,5,83,0,0,401,404,3,74,37,0,402,404,3,72,36,0,403,401,1,0,
  	0,0,403,402,1,0,0,0,404,407,1,0,0,0,405,403,1,0,0,0,405,406,1,0,0,0,406,
  	408,1,0,0,0,407,405,1,0,0,0,408,409,5,84,0,0,409,51,1,0,0,0,410,411,3,
  	54,27,0,411,53,1,0,0,0,412,413,5,25,0,0,413,414,3,58,29,0,414,415,3,66,
  	33,0,415,424,1,0,0,0,416,417,5,26,0,0,417,418,3,58,29,0,418,419,5,79,
  	0,0,419,420,3,58,29,0,420,421,3,66,33,0,421,424,1,0,0,0,422,424,3,56,
  	28,0,423,412,1,0,0,0,423,416,1,0,0,0,423,422,1,0,0,0,424,55,1,0,0,0,425,
  	428,3,58,29,0,426,427,5,18,0,0,427,429,3,56,28,0,428,426,1,0,0,0,428,
  	429,1,0,0,0,429,57,1,0,0,0,430,433,3,60,30,0,431,432,5,80,0,0,432,434,
  	3,58,29,0,433,431,1,0,0,0,433,434,1,0,0,0,434,59,1,0,0,0,435,438,3,62,
  	31,0,436,437,7,0,0,0,437,439,3,60,30,0,438,436,1,0,0,0,438,439,1,0,0,
  	0,439,61,1,0,0,0,440,443,3,64,32,0,441,442,5,27,0,0,442,444,3,64,32,0,
  	443,441,1,0,0,0,443,444,1,0,0,0,444,63,1,0,0,0,445,446,5,24,0,0,446,461,
  	3,52,26,0,447,448,5,20,0,0,448,461,3,52,26,0,449,450,5,21,0,0,450,461,
  	3,52,26,0,451,452,5,23,0,0,452,461,3,52,26,0,453,454,5,22,0,0,454,461,
  	3,52,26,0,455,461,3,68,34,0,456,457,5,81,0,0,457,458,3,52,26,0,458,459,
  	5,82,0,0,459,461,1,0,0,0,460,445,1,0,0,0,460,447,1,0,0,0,460,449,1,0,
  	0,0,460,451,1,0,0,0,460,453,1,0,0,0,460,455,1,0,0,0,460,456,1,0,0,0,461,
  	65,1,0,0,0,462,463,5,20,0,0,463,478,3,52,26,0,464,465,5,21,0,0,465,478,
  	3,52,26,0,466,467,5,23,0,0,467,478,3,52,26,0,468,469,5,22,0,0,469,478,
  	3,52,26,0,470,471,5,26,0,0,471,472,3,58,29,0,472,473,5,79,0,0,473,474,
  	3,58,29,0,474,475,3,66,33,0,475,478,1,0,0,0,476,478,3,56,28,0,477,462,
  	1,0,0,0,477,464,1,0,0,0,477,466,1,0,0,0,477,468,1,0,0,0,477,470,1,0,0,
  	0,477,476,1,0,0,0,478,67,1,0,0,0,479,480,3,70,35,0,480,481,5,28,0,0,481,
  	482,5,30,0,0,482,498,1,0,0,0,483,484,3,70,35,0,484,485,5,31,0,0,485,498,
  	1,0,0,0,486,487,3,70,35,0,487,488,5,29,0,0,488,489,5,32,0,0,489,498,1,
  	0,0,0,490,491,3,70,35,0,491,492,5,33,0,0,492,498,1,0,0,0,493,494,3,70,
  	35,0,494,495,5,29,0,0,495,496,5,34,0,0,496,498,1,0,0,0,497,479,1,0,0,
  	0,497,483,1,0,0,0,497,486,1,0,0,0,497,490,1,0,0,0,497,493,1,0,0,0,498,
  	69,1,0,0,0,499,502,5,90,0,0,500,501,5,73,0,0,501,503,5,90,0,0,502,500,
  	1,0,0,0,502,503,1,0,0,0,503,71,1,0,0,0,504,505,5,35,0,0,505,506,5,70,
  	0,0,506,507,3,76,38,0,507,508,5,72,0,0,508,535,1,0,0,0,509,510,5,36,0,
  	0,510,511,5,70,0,0,511,512,3,76,38,0,512,513,5,72,0,0,513,535,1,0,0,0,
  	514,515,5,37,0,0,515,516,5,70,0,0,516,517,3,76,38,0,517,518,5,72,0,0,
  	518,535,1,0,0,0,519,520,5,38,0,0,520,521,5,70,0,0,521,522,3,76,38,0,522,
  	523,5,72,0,0,523,535,1,0,0,0,524,525,5,39,0,0,525,526,5,70,0,0,526,527,
  	3,76,38,0,527,528,5,72,0,0,528,535,1,0,0,0,529,530,5,40,0,0,530,531,5,
  	70,0,0,531,532,3,76,38,0,532,533,5,72,0,0,533,535,1,0,0,0,534,504,1,0,
  	0,0,534,509,1,0,0,0,534,514,1,0,0,0,534,519,1,0,0,0,534,524,1,0,0,0,534,
  	529,1,0,0,0,535,73,1,0,0,0,536,537,5,42,0,0,537,538,5,70,0,0,538,539,
  	3,22,11,0,539,540,5,72,0,0,540,547,1,0,0,0,541,542,5,43,0,0,542,543,5,
  	70,0,0,543,544,3,22,11,0,544,545,5,72,0,0,545,547,1,0,0,0,546,536,1,0,
  	0,0,546,541,1,0,0,0,547,75,1,0,0,0,548,549,5,90,0,0,549,550,3,120,60,
  	0,550,552,5,81,0,0,551,553,3,26,13,0,552,551,1,0,0,0,552,553,1,0,0,0,
  	553,554,1,0,0,0,554,555,5,82,0,0,555,77,1,0,0,0,556,557,5,44,0,0,557,
  	561,5,83,0,0,558,560,3,40,20,0,559,558,1,0,0,0,560,563,1,0,0,0,561,559,
  	1,0,0,0,561,562,1,0,0,0,562,564,1,0,0,0,563,561,1,0,0,0,564,565,5,84,
  	0,0,565,79,1,0,0,0,566,567,6,40,-1,0,567,568,5,51,0,0,568,569,5,81,0,
  	0,569,572,3,80,40,0,570,571,5,61,0,0,571,573,3,80,40,0,572,570,1,0,0,
  	0,573,574,1,0,0,0,574,572,1,0,0,0,574,575,1,0,0,0,575,576,1,0,0,0,576,
  	577,5,82,0,0,577,632,1,0,0,0,578,579,5,52,0,0,579,580,5,81,0,0,580,583,
  	3,80,40,0,581,582,5,61,0,0,582,584,3,80,40,0,583,581,1,0,0,0,584,585,
  	1,0,0,0,585,583,1,0,0,0,585,586,1,0,0,0,586,587,1,0,0,0,587,588,5,82,
  	0,0,588,632,1,0,0,0,589,590,5,55,0,0,590,591,5,87,0,0,591,592,5,56,0,
  	0,592,593,3,80,40,0,593,594,5,57,0,0,594,595,3,80,40,8,595,632,1,0,0,
  	0,596,597,5,50,0,0,597,598,5,87,0,0,598,599,5,87,0,0,599,600,5,81,0,0,
  	600,601,3,80,40,0,601,605,5,82,0,0,602,604,3,84,42,0,603,602,1,0,0,0,
  	604,607,1,0,0,0,605,603,1,0,0,0,605,606,1,0,0,0,606,632,1,0,0,0,607,605,
  	1,0,0,0,608,632,5,47,0,0,609,632,5,48,0,0,610,632,5,49,0,0,611,615,3,
  	86,43,0,612,614,3,84,42,0,613,612,1,0,0,0,614,617,1,0,0,0,615,613,1,0,
  	0,0,615,616,1,0,0,0,616,632,1,0,0,0,617,615,1,0,0,0,618,619,5,81,0,0,
  	619,620,3,80,40,0,620,621,5,82,0,0,621,632,1,0,0,0,622,623,5,54,0,0,623,
  	627,5,83,0,0,624,626,3,82,41,0,625,624,1,0,0,0,626,629,1,0,0,0,627,625,
  	1,0,0,0,627,628,1,0,0,0,628,630,1,0,0,0,629,627,1,0,0,0,630,632,5,84,
  	0,0,631,566,1,0,0,0,631,578,1,0,0,0,631,589,1,0,0,0,631,596,1,0,0,0,631,
  	608,1,0,0,0,631,609,1,0,0,0,631,610,1,0,0,0,631,611,1,0,0,0,631,618,1,
  	0,0,0,631,622,1,0,0,0,632,642,1,0,0,0,633,636,10,11,0,0,634,635,5,60,
  	0,0,635,637,3,80,40,0,636,634,1,0,0,0,637,638,1,0,0,0,638,636,1,0,0,0,
  	638,639,1,0,0,0,639,641,1,0,0,0,640,633,1,0,0,0,641,644,1,0,0,0,642,640,
  	1,0,0,0,642,643,1,0,0,0,643,81,1,0,0,0,644,642,1,0,0,0,645,647,5,53,0,
  	0,646,648,3,90,45,0,647,646,1,0,0,0,647,648,1,0,0,0,648,649,1,0,0,0,649,
  	650,5,70,0,0,650,651,3,80,40,0,651,652,5,72,0,0,652,83,1,0,0,0,653,654,
  	5,41,0,0,654,655,5,38,0,0,655,664,3,80,40,0,656,657,5,41,0,0,657,658,
  	5,37,0,0,658,664,3,80,40,0,659,660,5,41,0,0,660,661,3,86,43,0,661,662,
  	3,80,40,0,662,664,1,0,0,0,663,653,1,0,0,0,663,656,1,0,0,0,663,659,1,0,
  	0,0,664,85,1,0,0,0,665,666,3,120,60,0,666,667,5,73,0,0,667,668,3,120,
  	60,0,668,670,5,81,0,0,669,671,3,88,44,0,670,669,1,0,0,0,670,671,1,0,0,
  	0,671,672,1,0,0,0,672,673,5,82,0,0,673,682,1,0,0,0,674,675,3,120,60,0,
  	675,677,5,81,0,0,676,678,3,88,44,0,677,676,1,0,0,0,677,678,1,0,0,0,678,
  	679,1,0,0,0,679,680,5,82,0,0,680,682,1,0,0,0,681,665,1,0,0,0,681,674,
  	1,0,0,0,682,87,1,0,0,0,683,688,3,90,45,0,684,685,5,71,0,0,685,687,3,90,
  	45,0,686,684,1,0,0,0,687,690,1,0,0,0,688,686,1,0,0,0,688,689,1,0,0,0,
  	689,89,1,0,0,0,690,688,1,0,0,0,691,692,3,92,46,0,692,91,1,0,0,0,693,698,
  	3,94,47,0,694,695,5,80,0,0,695,697,3,94,47,0,696,694,1,0,0,0,697,700,
  	1,0,0,0,698,696,1,0,0,0,698,699,1,0,0,0,699,93,1,0,0,0,700,698,1,0,0,
  	0,701,706,3,96,48,0,702,703,5,79,0,0,703,705,3,96,48,0,704,702,1,0,0,
  	0,705,708,1,0,0,0,706,704,1,0,0,0,706,707,1,0,0,0,707,95,1,0,0,0,708,
  	706,1,0,0,0,709,713,3,100,50,0,710,711,3,98,49,0,711,712,3,100,50,0,712,
  	714,1,0,0,0,713,710,1,0,0,0,713,714,1,0,0,0,714,97,1,0,0,0,715,716,7,
  	1,0,0,716,99,1,0,0,0,717,718,5,78,0,0,718,721,3,100,50,0,719,721,3,102,
  	51,0,720,717,1,0,0,0,720,719,1,0,0,0,721,101,1,0,0,0,722,727,3,104,52,
  	0,723,724,7,2,0,0,724,726,3,104,52,0,725,723,1,0,0,0,726,729,1,0,0,0,
  	727,725,1,0,0,0,727,728,1,0,0,0,728,103,1,0,0,0,729,727,1,0,0,0,730,735,
  	3,106,53,0,731,732,7,3,0,0,732,734,3,106,53,0,733,731,1,0,0,0,734,737,
  	1,0,0,0,735,733,1,0,0,0,735,736,1,0,0,0,736,105,1,0,0,0,737,735,1,0,0,
  	0,738,739,5,75,0,0,739,742,3,106,53,0,740,742,3,108,54,0,741,738,1,0,
  	0,0,741,740,1,0,0,0,742,107,1,0,0,0,743,760,3,86,43,0,744,760,5,89,0,
  	0,745,760,5,90,0,0,746,760,5,91,0,0,747,760,5,87,0,0,748,760,5,88,0,0,
  	749,760,3,110,55,0,750,760,3,114,57,0,751,760,3,116,58,0,752,753,5,90,
  	0,0,753,754,5,73,0,0,754,760,5,90,0,0,755,756,5,81,0,0,756,757,3,90,45,
  	0,757,758,5,82,0,0,758,760,1,0,0,0,759,743,1,0,0,0,759,744,1,0,0,0,759,
  	745,1,0,0,0,759,746,1,0,0,0,759,747,1,0,0,0,759,748,1,0,0,0,759,749,1,
  	0,0,0,759,750,1,0,0,0,759,751,1,0,0,0,759,752,1,0,0,0,759,755,1,0,0,0,
  	760,109,1,0,0,0,761,770,5,83,0,0,762,767,3,112,56,0,763,764,5,71,0,0,
  	764,766,3,112,56,0,765,763,1,0,0,0,766,769,1,0,0,0,767,765,1,0,0,0,767,
  	768,1,0,0,0,768,771,1,0,0,0,769,767,1,0,0,0,770,762,1,0,0,0,770,771,1,
  	0,0,0,771,772,1,0,0,0,772,773,5,84,0,0,773,111,1,0,0,0,774,775,5,90,0,
  	0,775,776,5,70,0,0,776,777,3,90,45,0,777,113,1,0,0,0,778,787,5,85,0,0,
  	779,784,3,90,45,0,780,781,5,71,0,0,781,783,3,90,45,0,782,780,1,0,0,0,
  	783,786,1,0,0,0,784,782,1,0,0,0,784,785,1,0,0,0,785,788,1,0,0,0,786,784,
  	1,0,0,0,787,779,1,0,0,0,787,788,1,0,0,0,788,789,1,0,0,0,789,790,5,86,
  	0,0,790,115,1,0,0,0,791,800,5,83,0,0,792,797,3,118,59,0,793,794,5,71,
  	0,0,794,796,3,118,59,0,795,793,1,0,0,0,796,799,1,0,0,0,797,795,1,0,0,
  	0,797,798,1,0,0,0,798,801,1,0,0,0,799,797,1,0,0,0,800,792,1,0,0,0,800,
  	801,1,0,0,0,801,802,1,0,0,0,802,803,5,84,0,0,803,117,1,0,0,0,804,805,
  	5,66,0,0,805,806,3,90,45,0,806,807,5,71,0,0,807,808,3,90,45,0,808,809,
  	5,67,0,0,809,119,1,0,0,0,810,811,7,4,0,0,811,121,1,0,0,0,77,125,134,140,
  	147,155,162,166,172,178,189,197,206,209,221,227,235,241,244,269,276,284,
  	296,306,313,321,324,335,343,352,361,377,379,390,392,403,405,423,428,433,
  	438,443,460,477,497,502,534,546,552,561,574,585,605,615,627,631,638,642,
  	647,663,670,677,681,688,698,706,713,720,727,735,741,759,767,770,784,787,
  	797,800
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
    setState(125);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::TASK)
      | (1ULL << KodaParser::CAPABILITY)
      | (1ULL << KodaParser::TYPE)
      | (1ULL << KodaParser::ENUM)
      | (1ULL << KodaParser::MAPPING))) != 0)) {
      setState(122);
      topLevelDeclaration();
      setState(127);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(128);
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
    setState(134);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK:
      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 1);
        setState(130);
        topLevelComponent();
        break;
      }

      case KodaParser::TYPE: {
        enterOuterAlt(_localctx, 2);
        setState(131);
        typeDeclaration();
        break;
      }

      case KodaParser::ENUM: {
        enterOuterAlt(_localctx, 3);
        setState(132);
        enumDeclaration();
        break;
      }

      case KodaParser::MAPPING: {
        enterOuterAlt(_localctx, 4);
        setState(133);
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
    setState(166);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TASK: {
        enterOuterAlt(_localctx, 1);
        setState(136);
        match(KodaParser::TASK);
        setState(137);
        match(KodaParser::IDENT);
        setState(138);
        match(KodaParser::LPAREN);
        setState(140);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::LIST)
          | (1ULL << KodaParser::OPTIONAL)
          | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
          setState(139);
          argumentList();
        }
        setState(142);
        match(KodaParser::RPAREN);
        setState(143);
        match(KodaParser::LBRACE);
        setState(147);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::STRATEGY)
          | (1ULL << KodaParser::PARAMETERS)
          | (1ULL << KodaParser::PROPERTIES)
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
          setState(144);
          statement();
          setState(149);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(150);
        match(KodaParser::RBRACE);
        break;
      }

      case KodaParser::CAPABILITY: {
        enterOuterAlt(_localctx, 2);
        setState(151);
        match(KodaParser::CAPABILITY);
        setState(152);
        match(KodaParser::IDENT);
        setState(153);
        match(KodaParser::LPAREN);
        setState(155);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::LIST)
          | (1ULL << KodaParser::OPTIONAL)
          | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
          setState(154);
          argumentList();
        }
        setState(157);
        match(KodaParser::RPAREN);
        setState(158);
        match(KodaParser::LBRACE);
        setState(162);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << KodaParser::STRATEGY)
          | (1ULL << KodaParser::PARAMETERS)
          | (1ULL << KodaParser::PROPERTIES)
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
          setState(159);
          statement();
          setState(164);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(165);
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
    setState(189);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::TypeRecordContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(168);
      match(KodaParser::TYPE);
      setState(169);
      qualifiedName();
      setState(172);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::EXTENDS) {
        setState(170);
        match(KodaParser::EXTENDS);
        setState(171);
        qualifiedName();
      }
      setState(174);
      match(KodaParser::LBRACE);
      setState(178);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::ANNOTATIONS || _la == KodaParser::IDENT) {
        setState(175);
        fieldDeclaration();
        setState(180);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(181);
      match(KodaParser::RBRACE);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::TypeAliasContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(183);
      match(KodaParser::TYPE);
      setState(184);
      qualifiedName();
      setState(185);
      match(KodaParser::ASSIGN);
      setState(186);
      typeReference();
      setState(187);
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
    setState(197);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IDENT: {
        enterOuterAlt(_localctx, 1);
        setState(191);
        match(KodaParser::IDENT);
        setState(192);
        match(KodaParser::COLON);
        setState(193);
        typeReference();
        setState(194);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ANNOTATIONS: {
        enterOuterAlt(_localctx, 2);
        setState(196);
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
    setState(199);
    match(KodaParser::ANNOTATIONS);
    setState(200);
    match(KodaParser::LBRACE);
    setState(209);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::STRING) {
      setState(201);
      annotation();
      setState(206);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(202);
        match(KodaParser::COMMA);
        setState(203);
        annotation();
        setState(208);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(211);
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
    setState(213);
    match(KodaParser::STRING);
    setState(214);
    match(KodaParser::COLON);
    setState(215);
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
    setState(217);
    match(KodaParser::ENUM);
    setState(218);
    qualifiedName();
    setState(221);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::COLON) {
      setState(219);
      match(KodaParser::COLON);
      setState(220);
      typeReference();
    }
    setState(223);
    match(KodaParser::LBRACE);
    setState(227);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(224);
      enumValue();
      setState(229);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(230);
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
    setState(232);
    match(KodaParser::IDENT);
    setState(235);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(233);
      match(KodaParser::ASSIGN);
      setState(234);
      enumLiteral();
    }
    setState(237);
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
    setState(244);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(239);
        match(KodaParser::STRING);
        break;
      }

      case KodaParser::MINUS:
      case KodaParser::NATURAL: {
        enterOuterAlt(_localctx, 2);
        setState(241);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == KodaParser::MINUS) {
          setState(240);
          match(KodaParser::MINUS);
        }
        setState(243);
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
    setState(246);
    match(KodaParser::MAPPING);
    setState(247);
    typeReference();
    setState(248);
    match(KodaParser::TO);
    setState(249);
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
    setState(269);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IDENT: {
        _localctx = _tracker.createInstance<KodaParser::TypeNamedContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(251);
        qualifiedName();
        break;
      }

      case KodaParser::LIST: {
        _localctx = _tracker.createInstance<KodaParser::TypeListContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(252);
        match(KodaParser::LIST);
        setState(253);
        match(KodaParser::LT);
        setState(254);
        typeReference();
        setState(255);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::OPTIONAL: {
        _localctx = _tracker.createInstance<KodaParser::TypeOptionalContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(257);
        match(KodaParser::OPTIONAL);
        setState(258);
        match(KodaParser::LT);
        setState(259);
        typeReference();
        setState(260);
        match(KodaParser::GT);
        break;
      }

      case KodaParser::MAP: {
        _localctx = _tracker.createInstance<KodaParser::TypeMapContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(262);
        match(KodaParser::MAP);
        setState(263);
        match(KodaParser::LT);
        setState(264);
        typeReference();
        setState(265);
        match(KodaParser::COMMA);
        setState(266);
        typeReference();
        setState(267);
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
    setState(271);
    match(KodaParser::IDENT);
    setState(276);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::DOUBLE_COLON) {
      setState(272);
      match(KodaParser::DOUBLE_COLON);
      setState(273);
      match(KodaParser::IDENT);
      setState(278);
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
    setState(279);
    argument();
    setState(284);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(280);
      match(KodaParser::COMMA);
      setState(281);
      argument();
      setState(286);
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
    setState(296);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ArgPlainContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(287);
      typeReference();
      setState(288);
      match(KodaParser::IDENT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ArgReqContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(290);
      match(KodaParser::IDENT);
      setState(291);
      match(KodaParser::REQ);
      setState(292);
      typeReference();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ArgProContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(293);
      match(KodaParser::IDENT);
      setState(294);
      match(KodaParser::PRO);
      setState(295);
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

KodaParser::PropertiesBlockContext* KodaParser::StatementContext::propertiesBlock() {
  return getRuleContext<KodaParser::PropertiesBlockContext>(0);
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
    setState(306);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::STRATEGY: {
        enterOuterAlt(_localctx, 1);
        setState(298);
        tasksBlock();
        break;
      }

      case KodaParser::PARAMETERS: {
        enterOuterAlt(_localctx, 2);
        setState(299);
        varsBlock();
        break;
      }

      case KodaParser::PROPERTIES: {
        enterOuterAlt(_localctx, 3);
        setState(300);
        propertiesBlock();
        break;
      }

      case KodaParser::ACTION: {
        enterOuterAlt(_localctx, 4);
        setState(301);
        actionBlock();
        break;
      }

      case KodaParser::SERVICE: {
        enterOuterAlt(_localctx, 5);
        setState(302);
        serviceBlock();
        break;
      }

      case KodaParser::TOPIC: {
        enterOuterAlt(_localctx, 6);
        setState(303);
        topicBlock();
        break;
      }

      case KodaParser::TRIGGER:
      case KodaParser::RETURN:
      case KodaParser::ABORT:
      case KodaParser::ERROR:
      case KodaParser::IN:
      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 7);
        setState(304);
        rosDefStatement();
        break;
      }

      case KodaParser::DATA: {
        enterOuterAlt(_localctx, 8);
        setState(305);
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
    setState(308);
    match(KodaParser::STRATEGY);
    setState(309);
    match(KodaParser::LBRACE);
    setState(313);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(310);
      flow();
      setState(315);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(316);
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
    setState(318);
    match(KodaParser::IDENT);
    setState(324);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LBRACK) {
      setState(319);
      match(KodaParser::LBRACK);
      setState(321);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == KodaParser::IDENT) {
        setState(320);
        identList();
      }
      setState(323);
      match(KodaParser::RBRACK);
    }
    setState(326);
    match(KodaParser::COLON);
    setState(327);
    strategy(0);
    setState(328);
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
    setState(330);
    match(KodaParser::IDENT);
    setState(335);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(331);
      match(KodaParser::COMMA);
      setState(332);
      match(KodaParser::IDENT);
      setState(337);
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
    setState(338);
    match(KodaParser::PARAMETERS);
    setState(339);
    match(KodaParser::LBRACE);
    setState(343);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::LIST)
      | (1ULL << KodaParser::OPTIONAL)
      | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
      setState(340);
      variableStatement();
      setState(345);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(346);
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
    setState(348);
    typeReference();
    setState(349);
    match(KodaParser::IDENT);
    setState(352);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::ASSIGN) {
      setState(350);
      match(KodaParser::ASSIGN);
      setState(351);
      expression();
    }
    setState(354);
    match(KodaParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertiesBlockContext ------------------------------------------------------------------

KodaParser::PropertiesBlockContext::PropertiesBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::PropertiesBlockContext::PROPERTIES() {
  return getToken(KodaParser::PROPERTIES, 0);
}

tree::TerminalNode* KodaParser::PropertiesBlockContext::LBRACE() {
  return getToken(KodaParser::LBRACE, 0);
}

tree::TerminalNode* KodaParser::PropertiesBlockContext::RBRACE() {
  return getToken(KodaParser::RBRACE, 0);
}

std::vector<KodaParser::PropertyStatementContext *> KodaParser::PropertiesBlockContext::propertyStatement() {
  return getRuleContexts<KodaParser::PropertyStatementContext>();
}

KodaParser::PropertyStatementContext* KodaParser::PropertiesBlockContext::propertyStatement(size_t i) {
  return getRuleContext<KodaParser::PropertyStatementContext>(i);
}


size_t KodaParser::PropertiesBlockContext::getRuleIndex() const {
  return KodaParser::RulePropertiesBlock;
}


std::any KodaParser::PropertiesBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertiesBlock(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertiesBlockContext* KodaParser::propertiesBlock() {
  PropertiesBlockContext *_localctx = _tracker.createInstance<PropertiesBlockContext>(_ctx, getState());
  enterRule(_localctx, 42, KodaParser::RulePropertiesBlock);
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
    setState(356);
    match(KodaParser::PROPERTIES);
    setState(357);
    match(KodaParser::LBRACE);
    setState(361);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::IDENT) {
      setState(358);
      propertyStatement();
      setState(363);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(364);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyStatementContext ------------------------------------------------------------------

KodaParser::PropertyStatementContext::PropertyStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::PropertyStatementContext::IDENT() {
  return getToken(KodaParser::IDENT, 0);
}

tree::TerminalNode* KodaParser::PropertyStatementContext::COLON() {
  return getToken(KodaParser::COLON, 0);
}

KodaParser::PropertyExprContext* KodaParser::PropertyStatementContext::propertyExpr() {
  return getRuleContext<KodaParser::PropertyExprContext>(0);
}

tree::TerminalNode* KodaParser::PropertyStatementContext::SEMI() {
  return getToken(KodaParser::SEMI, 0);
}


size_t KodaParser::PropertyStatementContext::getRuleIndex() const {
  return KodaParser::RulePropertyStatement;
}


std::any KodaParser::PropertyStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyStatement(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyStatementContext* KodaParser::propertyStatement() {
  PropertyStatementContext *_localctx = _tracker.createInstance<PropertyStatementContext>(_ctx, getState());
  enterRule(_localctx, 44, KodaParser::RulePropertyStatement);

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
    match(KodaParser::IDENT);
    setState(367);
    match(KodaParser::COLON);
    setState(368);
    propertyExpr();
    setState(369);
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
  enterRule(_localctx, 46, KodaParser::RuleActionBlock);
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
    match(KodaParser::ACTION);
    setState(372);
    match(KodaParser::STRING);
    setState(373);
    match(KodaParser::STRING);
    setState(374);
    match(KodaParser::LBRACE);
    setState(379);
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
      setState(377);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(375);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(376);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(381);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(382);
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
  enterRule(_localctx, 48, KodaParser::RuleServiceBlock);
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
    setState(384);
    match(KodaParser::SERVICE);
    setState(385);
    match(KodaParser::STRING);
    setState(386);
    match(KodaParser::STRING);
    setState(387);
    match(KodaParser::LBRACE);
    setState(392);
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
      setState(390);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(388);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(389);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(394);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(395);
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
  enterRule(_localctx, 50, KodaParser::RuleTopicBlock);
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
    setState(397);
    match(KodaParser::TOPIC);
    setState(398);
    match(KodaParser::STRING);
    setState(399);
    match(KodaParser::STRING);
    setState(400);
    match(KodaParser::LBRACE);
    setState(405);
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
      setState(403);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case KodaParser::CONSUMES:
        case KodaParser::PRODUCES: {
          setState(401);
          reqDefStatement();
          break;
        }

        case KodaParser::TRIGGER:
        case KodaParser::RETURN:
        case KodaParser::ABORT:
        case KodaParser::ERROR:
        case KodaParser::IN:
        case KodaParser::OUT: {
          setState(402);
          rosDefStatement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(407);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(408);
    match(KodaParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyExprContext ------------------------------------------------------------------

KodaParser::PropertyExprContext::PropertyExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::PropertyHelperContext* KodaParser::PropertyExprContext::propertyHelper() {
  return getRuleContext<KodaParser::PropertyHelperContext>(0);
}


size_t KodaParser::PropertyExprContext::getRuleIndex() const {
  return KodaParser::RulePropertyExpr;
}


std::any KodaParser::PropertyExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyExpr(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyExprContext* KodaParser::propertyExpr() {
  PropertyExprContext *_localctx = _tracker.createInstance<PropertyExprContext>(_ctx, getState());
  enterRule(_localctx, 52, KodaParser::RulePropertyExpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(410);
    propertyHelper();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyHelperContext ------------------------------------------------------------------

KodaParser::PropertyHelperContext::PropertyHelperContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::PropertyHelperContext::IF() {
  return getToken(KodaParser::IF, 0);
}

std::vector<KodaParser::PropertyConditionOrContext *> KodaParser::PropertyHelperContext::propertyConditionOr() {
  return getRuleContexts<KodaParser::PropertyConditionOrContext>();
}

KodaParser::PropertyConditionOrContext* KodaParser::PropertyHelperContext::propertyConditionOr(size_t i) {
  return getRuleContext<KodaParser::PropertyConditionOrContext>(i);
}

KodaParser::PropertyConsequenceContext* KodaParser::PropertyHelperContext::propertyConsequence() {
  return getRuleContext<KodaParser::PropertyConsequenceContext>(0);
}

tree::TerminalNode* KodaParser::PropertyHelperContext::BETWEEN() {
  return getToken(KodaParser::BETWEEN, 0);
}

tree::TerminalNode* KodaParser::PropertyHelperContext::AND() {
  return getToken(KodaParser::AND, 0);
}

KodaParser::PropertyImplicationContext* KodaParser::PropertyHelperContext::propertyImplication() {
  return getRuleContext<KodaParser::PropertyImplicationContext>(0);
}


size_t KodaParser::PropertyHelperContext::getRuleIndex() const {
  return KodaParser::RulePropertyHelper;
}


std::any KodaParser::PropertyHelperContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyHelper(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyHelperContext* KodaParser::propertyHelper() {
  PropertyHelperContext *_localctx = _tracker.createInstance<PropertyHelperContext>(_ctx, getState());
  enterRule(_localctx, 54, KodaParser::RulePropertyHelper);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(423);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::IF: {
        enterOuterAlt(_localctx, 1);
        setState(412);
        match(KodaParser::IF);
        setState(413);
        propertyConditionOr();
        setState(414);
        propertyConsequence();
        break;
      }

      case KodaParser::BETWEEN: {
        enterOuterAlt(_localctx, 2);
        setState(416);
        match(KodaParser::BETWEEN);
        setState(417);
        propertyConditionOr();
        setState(418);
        match(KodaParser::AND);
        setState(419);
        propertyConditionOr();
        setState(420);
        propertyConsequence();
        break;
      }

      case KodaParser::ALWAYS:
      case KodaParser::EVENTUALLY:
      case KodaParser::NEXT:
      case KodaParser::NEVER:
      case KodaParser::NEGATION:
      case KodaParser::LPAREN:
      case KodaParser::IDENT: {
        enterOuterAlt(_localctx, 3);
        setState(422);
        propertyImplication();
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

//----------------- PropertyImplicationContext ------------------------------------------------------------------

KodaParser::PropertyImplicationContext::PropertyImplicationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::PropertyConditionOrContext* KodaParser::PropertyImplicationContext::propertyConditionOr() {
  return getRuleContext<KodaParser::PropertyConditionOrContext>(0);
}

tree::TerminalNode* KodaParser::PropertyImplicationContext::IMPLIES() {
  return getToken(KodaParser::IMPLIES, 0);
}

KodaParser::PropertyImplicationContext* KodaParser::PropertyImplicationContext::propertyImplication() {
  return getRuleContext<KodaParser::PropertyImplicationContext>(0);
}


size_t KodaParser::PropertyImplicationContext::getRuleIndex() const {
  return KodaParser::RulePropertyImplication;
}


std::any KodaParser::PropertyImplicationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyImplication(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyImplicationContext* KodaParser::propertyImplication() {
  PropertyImplicationContext *_localctx = _tracker.createInstance<PropertyImplicationContext>(_ctx, getState());
  enterRule(_localctx, 56, KodaParser::RulePropertyImplication);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(425);
    propertyConditionOr();
    setState(428);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx)) {
    case 1: {
      setState(426);
      match(KodaParser::IMPLIES);
      setState(427);
      propertyImplication();
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

//----------------- PropertyConditionOrContext ------------------------------------------------------------------

KodaParser::PropertyConditionOrContext::PropertyConditionOrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::PropertyConditionAndContext* KodaParser::PropertyConditionOrContext::propertyConditionAnd() {
  return getRuleContext<KodaParser::PropertyConditionAndContext>(0);
}

tree::TerminalNode* KodaParser::PropertyConditionOrContext::OR() {
  return getToken(KodaParser::OR, 0);
}

KodaParser::PropertyConditionOrContext* KodaParser::PropertyConditionOrContext::propertyConditionOr() {
  return getRuleContext<KodaParser::PropertyConditionOrContext>(0);
}


size_t KodaParser::PropertyConditionOrContext::getRuleIndex() const {
  return KodaParser::RulePropertyConditionOr;
}


std::any KodaParser::PropertyConditionOrContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyConditionOr(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyConditionOrContext* KodaParser::propertyConditionOr() {
  PropertyConditionOrContext *_localctx = _tracker.createInstance<PropertyConditionOrContext>(_ctx, getState());
  enterRule(_localctx, 58, KodaParser::RulePropertyConditionOr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(430);
    propertyConditionAnd();
    setState(433);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
    case 1: {
      setState(431);
      match(KodaParser::OR);
      setState(432);
      propertyConditionOr();
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

//----------------- PropertyConditionAndContext ------------------------------------------------------------------

KodaParser::PropertyConditionAndContext::PropertyConditionAndContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::PropertyConditionUntilContext* KodaParser::PropertyConditionAndContext::propertyConditionUntil() {
  return getRuleContext<KodaParser::PropertyConditionUntilContext>(0);
}

KodaParser::PropertyConditionAndContext* KodaParser::PropertyConditionAndContext::propertyConditionAnd() {
  return getRuleContext<KodaParser::PropertyConditionAndContext>(0);
}

tree::TerminalNode* KodaParser::PropertyConditionAndContext::AND() {
  return getToken(KodaParser::AND, 0);
}

tree::TerminalNode* KodaParser::PropertyConditionAndContext::WHILE() {
  return getToken(KodaParser::WHILE, 0);
}


size_t KodaParser::PropertyConditionAndContext::getRuleIndex() const {
  return KodaParser::RulePropertyConditionAnd;
}


std::any KodaParser::PropertyConditionAndContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyConditionAnd(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyConditionAndContext* KodaParser::propertyConditionAnd() {
  PropertyConditionAndContext *_localctx = _tracker.createInstance<PropertyConditionAndContext>(_ctx, getState());
  enterRule(_localctx, 60, KodaParser::RulePropertyConditionAnd);
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
    propertyConditionUntil();
    setState(438);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx)) {
    case 1: {
      setState(436);
      _la = _input->LA(1);
      if (!(_la == KodaParser::WHILE

      || _la == KodaParser::AND)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(437);
      propertyConditionAnd();
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

//----------------- PropertyConditionUntilContext ------------------------------------------------------------------

KodaParser::PropertyConditionUntilContext::PropertyConditionUntilContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<KodaParser::PropertyConditionUnaryContext *> KodaParser::PropertyConditionUntilContext::propertyConditionUnary() {
  return getRuleContexts<KodaParser::PropertyConditionUnaryContext>();
}

KodaParser::PropertyConditionUnaryContext* KodaParser::PropertyConditionUntilContext::propertyConditionUnary(size_t i) {
  return getRuleContext<KodaParser::PropertyConditionUnaryContext>(i);
}

tree::TerminalNode* KodaParser::PropertyConditionUntilContext::UNTIL() {
  return getToken(KodaParser::UNTIL, 0);
}


size_t KodaParser::PropertyConditionUntilContext::getRuleIndex() const {
  return KodaParser::RulePropertyConditionUntil;
}


std::any KodaParser::PropertyConditionUntilContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyConditionUntil(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyConditionUntilContext* KodaParser::propertyConditionUntil() {
  PropertyConditionUntilContext *_localctx = _tracker.createInstance<PropertyConditionUntilContext>(_ctx, getState());
  enterRule(_localctx, 62, KodaParser::RulePropertyConditionUntil);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(440);
    propertyConditionUnary();
    setState(443);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx)) {
    case 1: {
      setState(441);
      match(KodaParser::UNTIL);
      setState(442);
      propertyConditionUnary();
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

//----------------- PropertyConditionUnaryContext ------------------------------------------------------------------

KodaParser::PropertyConditionUnaryContext::PropertyConditionUnaryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::NEGATION() {
  return getToken(KodaParser::NEGATION, 0);
}

KodaParser::PropertyExprContext* KodaParser::PropertyConditionUnaryContext::propertyExpr() {
  return getRuleContext<KodaParser::PropertyExprContext>(0);
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::ALWAYS() {
  return getToken(KodaParser::ALWAYS, 0);
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::EVENTUALLY() {
  return getToken(KodaParser::EVENTUALLY, 0);
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::NEVER() {
  return getToken(KodaParser::NEVER, 0);
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::NEXT() {
  return getToken(KodaParser::NEXT, 0);
}

KodaParser::PropertyObservationContext* KodaParser::PropertyConditionUnaryContext::propertyObservation() {
  return getRuleContext<KodaParser::PropertyObservationContext>(0);
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::LPAREN() {
  return getToken(KodaParser::LPAREN, 0);
}

tree::TerminalNode* KodaParser::PropertyConditionUnaryContext::RPAREN() {
  return getToken(KodaParser::RPAREN, 0);
}


size_t KodaParser::PropertyConditionUnaryContext::getRuleIndex() const {
  return KodaParser::RulePropertyConditionUnary;
}


std::any KodaParser::PropertyConditionUnaryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyConditionUnary(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyConditionUnaryContext* KodaParser::propertyConditionUnary() {
  PropertyConditionUnaryContext *_localctx = _tracker.createInstance<PropertyConditionUnaryContext>(_ctx, getState());
  enterRule(_localctx, 64, KodaParser::RulePropertyConditionUnary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(460);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NEGATION: {
        enterOuterAlt(_localctx, 1);
        setState(445);
        match(KodaParser::NEGATION);
        setState(446);
        propertyExpr();
        break;
      }

      case KodaParser::ALWAYS: {
        enterOuterAlt(_localctx, 2);
        setState(447);
        match(KodaParser::ALWAYS);
        setState(448);
        propertyExpr();
        break;
      }

      case KodaParser::EVENTUALLY: {
        enterOuterAlt(_localctx, 3);
        setState(449);
        match(KodaParser::EVENTUALLY);
        setState(450);
        propertyExpr();
        break;
      }

      case KodaParser::NEVER: {
        enterOuterAlt(_localctx, 4);
        setState(451);
        match(KodaParser::NEVER);
        setState(452);
        propertyExpr();
        break;
      }

      case KodaParser::NEXT: {
        enterOuterAlt(_localctx, 5);
        setState(453);
        match(KodaParser::NEXT);
        setState(454);
        propertyExpr();
        break;
      }

      case KodaParser::IDENT: {
        enterOuterAlt(_localctx, 6);
        setState(455);
        propertyObservation();
        break;
      }

      case KodaParser::LPAREN: {
        enterOuterAlt(_localctx, 7);
        setState(456);
        match(KodaParser::LPAREN);
        setState(457);
        propertyExpr();
        setState(458);
        match(KodaParser::RPAREN);
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

//----------------- PropertyConsequenceContext ------------------------------------------------------------------

KodaParser::PropertyConsequenceContext::PropertyConsequenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* KodaParser::PropertyConsequenceContext::ALWAYS() {
  return getToken(KodaParser::ALWAYS, 0);
}

KodaParser::PropertyExprContext* KodaParser::PropertyConsequenceContext::propertyExpr() {
  return getRuleContext<KodaParser::PropertyExprContext>(0);
}

tree::TerminalNode* KodaParser::PropertyConsequenceContext::EVENTUALLY() {
  return getToken(KodaParser::EVENTUALLY, 0);
}

tree::TerminalNode* KodaParser::PropertyConsequenceContext::NEVER() {
  return getToken(KodaParser::NEVER, 0);
}

tree::TerminalNode* KodaParser::PropertyConsequenceContext::NEXT() {
  return getToken(KodaParser::NEXT, 0);
}

tree::TerminalNode* KodaParser::PropertyConsequenceContext::BETWEEN() {
  return getToken(KodaParser::BETWEEN, 0);
}

std::vector<KodaParser::PropertyConditionOrContext *> KodaParser::PropertyConsequenceContext::propertyConditionOr() {
  return getRuleContexts<KodaParser::PropertyConditionOrContext>();
}

KodaParser::PropertyConditionOrContext* KodaParser::PropertyConsequenceContext::propertyConditionOr(size_t i) {
  return getRuleContext<KodaParser::PropertyConditionOrContext>(i);
}

tree::TerminalNode* KodaParser::PropertyConsequenceContext::AND() {
  return getToken(KodaParser::AND, 0);
}

KodaParser::PropertyConsequenceContext* KodaParser::PropertyConsequenceContext::propertyConsequence() {
  return getRuleContext<KodaParser::PropertyConsequenceContext>(0);
}

KodaParser::PropertyImplicationContext* KodaParser::PropertyConsequenceContext::propertyImplication() {
  return getRuleContext<KodaParser::PropertyImplicationContext>(0);
}


size_t KodaParser::PropertyConsequenceContext::getRuleIndex() const {
  return KodaParser::RulePropertyConsequence;
}


std::any KodaParser::PropertyConsequenceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyConsequence(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyConsequenceContext* KodaParser::propertyConsequence() {
  PropertyConsequenceContext *_localctx = _tracker.createInstance<PropertyConsequenceContext>(_ctx, getState());
  enterRule(_localctx, 66, KodaParser::RulePropertyConsequence);

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
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(462);
      match(KodaParser::ALWAYS);
      setState(463);
      propertyExpr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(464);
      match(KodaParser::EVENTUALLY);
      setState(465);
      propertyExpr();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(466);
      match(KodaParser::NEVER);
      setState(467);
      propertyExpr();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(468);
      match(KodaParser::NEXT);
      setState(469);
      propertyExpr();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(470);
      match(KodaParser::BETWEEN);
      setState(471);
      propertyConditionOr();
      setState(472);
      match(KodaParser::AND);
      setState(473);
      propertyConditionOr();
      setState(474);
      propertyConsequence();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(476);
      propertyImplication();
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

//----------------- PropertyObservationContext ------------------------------------------------------------------

KodaParser::PropertyObservationContext::PropertyObservationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

KodaParser::PropertyReferenceContext* KodaParser::PropertyObservationContext::propertyReference() {
  return getRuleContext<KodaParser::PropertyReferenceContext>(0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::IS() {
  return getToken(KodaParser::IS, 0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::RUNNING() {
  return getToken(KodaParser::RUNNING, 0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::STARTED() {
  return getToken(KodaParser::STARTED, 0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::WAS() {
  return getToken(KodaParser::WAS, 0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::REJECTED() {
  return getToken(KodaParser::REJECTED, 0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::STOPPED() {
  return getToken(KodaParser::STOPPED, 0);
}

tree::TerminalNode* KodaParser::PropertyObservationContext::ABORTED() {
  return getToken(KodaParser::ABORTED, 0);
}


size_t KodaParser::PropertyObservationContext::getRuleIndex() const {
  return KodaParser::RulePropertyObservation;
}


std::any KodaParser::PropertyObservationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyObservation(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyObservationContext* KodaParser::propertyObservation() {
  PropertyObservationContext *_localctx = _tracker.createInstance<PropertyObservationContext>(_ctx, getState());
  enterRule(_localctx, 68, KodaParser::RulePropertyObservation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(497);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(479);
      propertyReference();
      setState(480);
      match(KodaParser::IS);
      setState(481);
      match(KodaParser::RUNNING);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(483);
      propertyReference();
      setState(484);
      match(KodaParser::STARTED);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(486);
      propertyReference();
      setState(487);
      match(KodaParser::WAS);
      setState(488);
      match(KodaParser::REJECTED);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(490);
      propertyReference();
      setState(491);
      match(KodaParser::STOPPED);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(493);
      propertyReference();
      setState(494);
      match(KodaParser::WAS);
      setState(495);
      match(KodaParser::ABORTED);
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

//----------------- PropertyReferenceContext ------------------------------------------------------------------

KodaParser::PropertyReferenceContext::PropertyReferenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> KodaParser::PropertyReferenceContext::IDENT() {
  return getTokens(KodaParser::IDENT);
}

tree::TerminalNode* KodaParser::PropertyReferenceContext::IDENT(size_t i) {
  return getToken(KodaParser::IDENT, i);
}

tree::TerminalNode* KodaParser::PropertyReferenceContext::DOT() {
  return getToken(KodaParser::DOT, 0);
}


size_t KodaParser::PropertyReferenceContext::getRuleIndex() const {
  return KodaParser::RulePropertyReference;
}


std::any KodaParser::PropertyReferenceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<KodaVisitor*>(visitor))
    return parserVisitor->visitPropertyReference(this);
  else
    return visitor->visitChildren(this);
}

KodaParser::PropertyReferenceContext* KodaParser::propertyReference() {
  PropertyReferenceContext *_localctx = _tracker.createInstance<PropertyReferenceContext>(_ctx, getState());
  enterRule(_localctx, 70, KodaParser::RulePropertyReference);
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
    match(KodaParser::IDENT);
    setState(502);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::DOT) {
      setState(500);
      match(KodaParser::DOT);
      setState(501);
      match(KodaParser::IDENT);
    }
   
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
  enterRule(_localctx, 72, KodaParser::RuleRosDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(534);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::TRIGGER: {
        enterOuterAlt(_localctx, 1);
        setState(504);
        match(KodaParser::TRIGGER);
        setState(505);
        match(KodaParser::COLON);
        setState(506);
        eventDefStatement();
        setState(507);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::RETURN: {
        enterOuterAlt(_localctx, 2);
        setState(509);
        match(KodaParser::RETURN);
        setState(510);
        match(KodaParser::COLON);
        setState(511);
        eventDefStatement();
        setState(512);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ABORT: {
        enterOuterAlt(_localctx, 3);
        setState(514);
        match(KodaParser::ABORT);
        setState(515);
        match(KodaParser::COLON);
        setState(516);
        eventDefStatement();
        setState(517);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::ERROR: {
        enterOuterAlt(_localctx, 4);
        setState(519);
        match(KodaParser::ERROR);
        setState(520);
        match(KodaParser::COLON);
        setState(521);
        eventDefStatement();
        setState(522);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::IN: {
        enterOuterAlt(_localctx, 5);
        setState(524);
        match(KodaParser::IN);
        setState(525);
        match(KodaParser::COLON);
        setState(526);
        eventDefStatement();
        setState(527);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::OUT: {
        enterOuterAlt(_localctx, 6);
        setState(529);
        match(KodaParser::OUT);
        setState(530);
        match(KodaParser::COLON);
        setState(531);
        eventDefStatement();
        setState(532);
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
  enterRule(_localctx, 74, KodaParser::RuleReqDefStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(546);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::CONSUMES: {
        enterOuterAlt(_localctx, 1);
        setState(536);
        match(KodaParser::CONSUMES);
        setState(537);
        match(KodaParser::COLON);
        setState(538);
        typeReference();
        setState(539);
        match(KodaParser::SEMI);
        break;
      }

      case KodaParser::PRODUCES: {
        enterOuterAlt(_localctx, 2);
        setState(541);
        match(KodaParser::PRODUCES);
        setState(542);
        match(KodaParser::COLON);
        setState(543);
        typeReference();
        setState(544);
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
  enterRule(_localctx, 76, KodaParser::RuleEventDefStatement);
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
    setState(548);
    match(KodaParser::IDENT);
    setState(549);
    identifier();
    setState(550);
    match(KodaParser::LPAREN);
    setState(552);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::LIST)
      | (1ULL << KodaParser::OPTIONAL)
      | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
      setState(551);
      argumentList();
    }
    setState(554);
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
  enterRule(_localctx, 78, KodaParser::RuleDataBlock);
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
    match(KodaParser::DATA);
    setState(557);
    match(KodaParser::LBRACE);
    setState(561);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << KodaParser::LIST)
      | (1ULL << KodaParser::OPTIONAL)
      | (1ULL << KodaParser::MAP))) != 0) || _la == KodaParser::IDENT) {
      setState(558);
      variableStatement();
      setState(563);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(564);
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
  size_t startState = 80;
  enterRecursionRule(_localctx, 80, KodaParser::RuleStrategy, precedence);

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
    setState(631);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::JOIN: {
        _localctx = _tracker.createInstance<StratJoinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(567);
        match(KodaParser::JOIN);
        setState(568);
        match(KodaParser::LPAREN);
        setState(569);
        strategy(0);
        setState(572); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(570);
          match(KodaParser::PIPE);
          setState(571);
          strategy(0);
          setState(574); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(576);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::EITHER: {
        _localctx = _tracker.createInstance<StratEitherContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(578);
        match(KodaParser::EITHER);
        setState(579);
        match(KodaParser::LPAREN);
        setState(580);
        strategy(0);
        setState(583); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(581);
          match(KodaParser::PIPE);
          setState(582);
          strategy(0);
          setState(585); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == KodaParser::PIPE);
        setState(587);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::WITHIN: {
        _localctx = _tracker.createInstance<StratWithinContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(589);
        match(KodaParser::WITHIN);
        setState(590);
        match(KodaParser::NATURAL);
        setState(591);
        match(KodaParser::DO);
        setState(592);
        strategy(0);
        setState(593);
        match(KodaParser::ELSE);
        setState(594);
        strategy(8);
        break;
      }

      case KodaParser::REPEAT: {
        _localctx = _tracker.createInstance<StratRepeatContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(596);
        match(KodaParser::REPEAT);
        setState(597);
        match(KodaParser::NATURAL);
        setState(598);
        match(KodaParser::NATURAL);
        setState(599);
        match(KodaParser::LPAREN);
        setState(600);
        strategy(0);
        setState(601);
        match(KodaParser::RPAREN);
        setState(605);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(602);
            strategyHandler(); 
          }
          setState(607);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
        }
        break;
      }

      case KodaParser::SUCCESS: {
        _localctx = _tracker.createInstance<StratSuccessContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(608);
        match(KodaParser::SUCCESS);
        break;
      }

      case KodaParser::FAILURE: {
        _localctx = _tracker.createInstance<StratFailureContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(609);
        match(KodaParser::FAILURE);
        break;
      }

      case KodaParser::CONTINUE: {
        _localctx = _tracker.createInstance<StratContinueContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(610);
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
        setState(611);
        eventStatement();
        setState(615);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(612);
            strategyHandler(); 
          }
          setState(617);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx);
        }
        break;
      }

      case KodaParser::LPAREN: {
        _localctx = _tracker.createInstance<StratParenContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(618);
        match(KodaParser::LPAREN);
        setState(619);
        strategy(0);
        setState(620);
        match(KodaParser::RPAREN);
        break;
      }

      case KodaParser::CHOOSE: {
        _localctx = _tracker.createInstance<StratChooseContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(622);
        match(KodaParser::CHOOSE);
        setState(623);
        match(KodaParser::LBRACE);
        setState(627);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == KodaParser::WHEN) {
          setState(624);
          chooseWhenStatement();
          setState(629);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(630);
        match(KodaParser::RBRACE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(642);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<StratSeqContext>(_tracker.createInstance<StrategyContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleStrategy);
        setState(633);

        if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
        setState(636); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(634);
                  match(KodaParser::ARROW);
                  setState(635);
                  strategy(0);
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(638); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER); 
      }
      setState(644);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx);
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
  enterRule(_localctx, 82, KodaParser::RuleChooseWhenStatement);
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
    setState(645);
    match(KodaParser::WHEN);
    setState(647);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 35) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 35)) & ((1ULL << (KodaParser::TRIGGER - 35))
      | (1ULL << (KodaParser::RETURN - 35))
      | (1ULL << (KodaParser::ABORT - 35))
      | (1ULL << (KodaParser::ERROR - 35))
      | (1ULL << (KodaParser::IN - 35))
      | (1ULL << (KodaParser::OUT - 35))
      | (1ULL << (KodaParser::CONSUMES - 35))
      | (1ULL << (KodaParser::PRODUCES - 35))
      | (1ULL << (KodaParser::MINUS - 35))
      | (1ULL << (KodaParser::NOT - 35))
      | (1ULL << (KodaParser::LPAREN - 35))
      | (1ULL << (KodaParser::LBRACE - 35))
      | (1ULL << (KodaParser::LBRACK - 35))
      | (1ULL << (KodaParser::NATURAL - 35))
      | (1ULL << (KodaParser::REAL - 35))
      | (1ULL << (KodaParser::BOOLEAN - 35))
      | (1ULL << (KodaParser::IDENT - 35))
      | (1ULL << (KodaParser::STRING - 35)))) != 0)) {
      setState(646);
      expression();
    }
    setState(649);
    match(KodaParser::COLON);
    setState(650);
    strategy(0);
    setState(651);
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
  enterRule(_localctx, 84, KodaParser::RuleStrategyHandler);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(663);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnErrorContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(653);
      match(KodaParser::ON);
      setState(654);
      match(KodaParser::ERROR);
      setState(655);
      strategy(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnAbortContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(656);
      match(KodaParser::ON);
      setState(657);
      match(KodaParser::ABORT);
      setState(658);
      strategy(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::HandlerOnEmitterContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(659);
      match(KodaParser::ON);
      setState(660);
      eventStatement();
      setState(661);
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
  enterRule(_localctx, 86, KodaParser::RuleEventStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(681);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::EvQualifiedCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(665);
      identifier();
      setState(666);
      match(KodaParser::DOT);
      setState(667);
      identifier();
      setState(668);
      match(KodaParser::LPAREN);
      setState(670);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 35) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 35)) & ((1ULL << (KodaParser::TRIGGER - 35))
        | (1ULL << (KodaParser::RETURN - 35))
        | (1ULL << (KodaParser::ABORT - 35))
        | (1ULL << (KodaParser::ERROR - 35))
        | (1ULL << (KodaParser::IN - 35))
        | (1ULL << (KodaParser::OUT - 35))
        | (1ULL << (KodaParser::CONSUMES - 35))
        | (1ULL << (KodaParser::PRODUCES - 35))
        | (1ULL << (KodaParser::MINUS - 35))
        | (1ULL << (KodaParser::NOT - 35))
        | (1ULL << (KodaParser::LPAREN - 35))
        | (1ULL << (KodaParser::LBRACE - 35))
        | (1ULL << (KodaParser::LBRACK - 35))
        | (1ULL << (KodaParser::NATURAL - 35))
        | (1ULL << (KodaParser::REAL - 35))
        | (1ULL << (KodaParser::BOOLEAN - 35))
        | (1ULL << (KodaParser::IDENT - 35))
        | (1ULL << (KodaParser::STRING - 35)))) != 0)) {
        setState(669);
        exprList();
      }
      setState(672);
      match(KodaParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::EvCallContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(674);
      identifier();
      setState(675);
      match(KodaParser::LPAREN);
      setState(677);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 35) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 35)) & ((1ULL << (KodaParser::TRIGGER - 35))
        | (1ULL << (KodaParser::RETURN - 35))
        | (1ULL << (KodaParser::ABORT - 35))
        | (1ULL << (KodaParser::ERROR - 35))
        | (1ULL << (KodaParser::IN - 35))
        | (1ULL << (KodaParser::OUT - 35))
        | (1ULL << (KodaParser::CONSUMES - 35))
        | (1ULL << (KodaParser::PRODUCES - 35))
        | (1ULL << (KodaParser::MINUS - 35))
        | (1ULL << (KodaParser::NOT - 35))
        | (1ULL << (KodaParser::LPAREN - 35))
        | (1ULL << (KodaParser::LBRACE - 35))
        | (1ULL << (KodaParser::LBRACK - 35))
        | (1ULL << (KodaParser::NATURAL - 35))
        | (1ULL << (KodaParser::REAL - 35))
        | (1ULL << (KodaParser::BOOLEAN - 35))
        | (1ULL << (KodaParser::IDENT - 35))
        | (1ULL << (KodaParser::STRING - 35)))) != 0)) {
        setState(676);
        exprList();
      }
      setState(679);
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
  enterRule(_localctx, 88, KodaParser::RuleExprList);
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
    setState(683);
    expression();
    setState(688);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::COMMA) {
      setState(684);
      match(KodaParser::COMMA);
      setState(685);
      expression();
      setState(690);
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
  enterRule(_localctx, 90, KodaParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(691);
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
  enterRule(_localctx, 92, KodaParser::RuleExprOr);
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
    setState(693);
    exprAnd();
    setState(698);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::OR) {
      setState(694);
      match(KodaParser::OR);
      setState(695);
      exprAnd();
      setState(700);
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
  enterRule(_localctx, 94, KodaParser::RuleExprAnd);
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
    setState(701);
    exprCmp();
    setState(706);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::AND) {
      setState(702);
      match(KodaParser::AND);
      setState(703);
      exprCmp();
      setState(708);
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
  enterRule(_localctx, 96, KodaParser::RuleExprCmp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(709);
    exprNot();
    setState(713);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx)) {
    case 1: {
      setState(710);
      compOp();
      setState(711);
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
  enterRule(_localctx, 98, KodaParser::RuleCompOp);
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
    setState(715);
    _la = _input->LA(1);
    if (!(((((_la - 62) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 62)) & ((1ULL << (KodaParser::EQ - 62))
      | (1ULL << (KodaParser::NEQ - 62))
      | (1ULL << (KodaParser::LEQ - 62))
      | (1ULL << (KodaParser::GEQ - 62))
      | (1ULL << (KodaParser::LT - 62))
      | (1ULL << (KodaParser::GT - 62)))) != 0))) {
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
  enterRule(_localctx, 100, KodaParser::RuleExprNot);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(720);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(717);
        match(KodaParser::NOT);
        setState(718);
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
        setState(719);
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
  enterRule(_localctx, 102, KodaParser::RuleExprAdd);
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
    setState(722);
    exprMul();
    setState(727);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::PLUS

    || _la == KodaParser::MINUS) {
      setState(723);
      _la = _input->LA(1);
      if (!(_la == KodaParser::PLUS

      || _la == KodaParser::MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(724);
      exprMul();
      setState(729);
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
  enterRule(_localctx, 104, KodaParser::RuleExprMul);
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
    setState(730);
    exprUnary();
    setState(735);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == KodaParser::STAR

    || _la == KodaParser::SLASH) {
      setState(731);
      _la = _input->LA(1);
      if (!(_la == KodaParser::STAR

      || _la == KodaParser::SLASH)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(732);
      exprUnary();
      setState(737);
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
  enterRule(_localctx, 106, KodaParser::RuleExprUnary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(741);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case KodaParser::MINUS: {
        enterOuterAlt(_localctx, 1);
        setState(738);
        match(KodaParser::MINUS);
        setState(739);
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
        setState(740);
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
  enterRule(_localctx, 108, KodaParser::RuleExprPrimary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(759);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 70, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<KodaParser::ExprCallContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(743);
      eventStatement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<KodaParser::ExprBooleanContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(744);
      match(KodaParser::BOOLEAN);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<KodaParser::ExprIdContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(745);
      match(KodaParser::IDENT);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<KodaParser::ExprStringContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(746);
      match(KodaParser::STRING);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<KodaParser::ExprIntContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(747);
      match(KodaParser::NATURAL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<KodaParser::ExprFloatContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(748);
      match(KodaParser::REAL);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<KodaParser::ExprRecordContext>(_localctx);
      enterOuterAlt(_localctx, 7);
      setState(749);
      recordLiteral();
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<KodaParser::ExprListLiteralContext>(_localctx);
      enterOuterAlt(_localctx, 8);
      setState(750);
      listLiteral();
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<KodaParser::ExprMapLiteralContext>(_localctx);
      enterOuterAlt(_localctx, 9);
      setState(751);
      mapLiteral();
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<KodaParser::ExprDataAccessContext>(_localctx);
      enterOuterAlt(_localctx, 10);
      setState(752);
      match(KodaParser::IDENT);
      setState(753);
      match(KodaParser::DOT);
      setState(754);
      match(KodaParser::IDENT);
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<KodaParser::ExprParenContext>(_localctx);
      enterOuterAlt(_localctx, 11);
      setState(755);
      match(KodaParser::LPAREN);
      setState(756);
      expression();
      setState(757);
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
  enterRule(_localctx, 110, KodaParser::RuleRecordLiteral);
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
    setState(761);
    match(KodaParser::LBRACE);
    setState(770);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::IDENT) {
      setState(762);
      recordFieldInitializer();
      setState(767);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(763);
        match(KodaParser::COMMA);
        setState(764);
        recordFieldInitializer();
        setState(769);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(772);
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
  enterRule(_localctx, 112, KodaParser::RuleRecordFieldInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(774);
    match(KodaParser::IDENT);
    setState(775);
    match(KodaParser::COLON);
    setState(776);
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
  enterRule(_localctx, 114, KodaParser::RuleListLiteral);
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
    setState(778);
    match(KodaParser::LBRACK);
    setState(787);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 35) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 35)) & ((1ULL << (KodaParser::TRIGGER - 35))
      | (1ULL << (KodaParser::RETURN - 35))
      | (1ULL << (KodaParser::ABORT - 35))
      | (1ULL << (KodaParser::ERROR - 35))
      | (1ULL << (KodaParser::IN - 35))
      | (1ULL << (KodaParser::OUT - 35))
      | (1ULL << (KodaParser::CONSUMES - 35))
      | (1ULL << (KodaParser::PRODUCES - 35))
      | (1ULL << (KodaParser::MINUS - 35))
      | (1ULL << (KodaParser::NOT - 35))
      | (1ULL << (KodaParser::LPAREN - 35))
      | (1ULL << (KodaParser::LBRACE - 35))
      | (1ULL << (KodaParser::LBRACK - 35))
      | (1ULL << (KodaParser::NATURAL - 35))
      | (1ULL << (KodaParser::REAL - 35))
      | (1ULL << (KodaParser::BOOLEAN - 35))
      | (1ULL << (KodaParser::IDENT - 35))
      | (1ULL << (KodaParser::STRING - 35)))) != 0)) {
      setState(779);
      expression();
      setState(784);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(780);
        match(KodaParser::COMMA);
        setState(781);
        expression();
        setState(786);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(789);
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
  enterRule(_localctx, 116, KodaParser::RuleMapLiteral);
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
    setState(791);
    match(KodaParser::LBRACE);
    setState(800);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == KodaParser::LT) {
      setState(792);
      mapFieldInitializer();
      setState(797);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == KodaParser::COMMA) {
        setState(793);
        match(KodaParser::COMMA);
        setState(794);
        mapFieldInitializer();
        setState(799);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(802);
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
  enterRule(_localctx, 118, KodaParser::RuleMapFieldInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(804);
    match(KodaParser::LT);
    setState(805);
    expression();
    setState(806);
    match(KodaParser::COMMA);
    setState(807);
    expression();
    setState(808);
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
  enterRule(_localctx, 120, KodaParser::RuleIdentifier);
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
    setState(810);
    _la = _input->LA(1);
    if (!(((((_la - 35) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 35)) & ((1ULL << (KodaParser::TRIGGER - 35))
      | (1ULL << (KodaParser::RETURN - 35))
      | (1ULL << (KodaParser::ABORT - 35))
      | (1ULL << (KodaParser::ERROR - 35))
      | (1ULL << (KodaParser::IN - 35))
      | (1ULL << (KodaParser::OUT - 35))
      | (1ULL << (KodaParser::CONSUMES - 35))
      | (1ULL << (KodaParser::PRODUCES - 35))
      | (1ULL << (KodaParser::IDENT - 35)))) != 0))) {
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
    case 40: return strategySempred(antlrcpp::downCast<StrategyContext *>(context), predicateIndex);

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
