#include "cst2ast.h"

#include <memory>
#include <stdexcept>
#include <variant>

#include "ast.h"
#include "ast_helpers.h"
#include "logging.h"
#include "typing/helpers.h"

#define ERROR_AND_RETURN(CTX, MSG, RETURN)                \
  do                                                      \
  {                                                       \
    const auto* token = CTX->getStart();                  \
    const size_t line = token->getLine();                 \
    const size_t column = token->getCharPositionInLine(); \
    mErrorListener->addError(line, column, MSG);          \
    return RETURN;                                        \
  } while (false);

#define ERROR_AND_CONTINUE(CTX, MSG)                      \
  do                                                      \
  {                                                       \
    const auto* token = CTX->getStart();                  \
    const size_t line = token->getLine();                 \
    const size_t column = token->getCharPositionInLine(); \
    mErrorListener->addError(line, column, MSG);          \
  } while (false);

namespace koda
{
CST2AST::CST2AST(std::shared_ptr<types::TypeRegistry> registry, CollectingErrorListener* errorListener)
    : KodaBaseVisitor()
    , mTypeRegistry(registry)
    , mErrorListener(errorListener)
{
  mTypeRegistry->registerBuiltinTypes();
}

koda::System CST2AST::build(KodaParser::SystemContext* ctx)
{
  return std::any_cast<koda::System>(visitSystem(ctx));
}

// -------------------------
// Top-level
// -------------------------

std::any CST2AST::visitSystem(KodaParser::SystemContext* ctx)
{
  koda::System sys;
  if (!ctx)
    ERROR_AND_RETURN(ctx, "No KodaParser::SystemContext provided", sys);

  // First, collect all the defined types
  for (auto* declaration : ctx->topLevelDeclaration())
  {
    if (auto* type = declaration->typeDeclaration(); type)
    {
      auto definition = convertTypeDeclaration(type);
      const auto result = mTypeRegistry->add(std::move(definition));
      if (!result.IsSuccess())
        ERROR_AND_CONTINUE(type, result.ErrorMessage());

      continue;
    }

    if (auto* enumeration = declaration->enumDeclaration(); enumeration)
    {
      auto definition = convertEnumDeclaration(enumeration);
      const auto result = mTypeRegistry->add(std::move(definition));
      if (!result.IsSuccess())
        ERROR_AND_CONTINUE(enumeration, result.ErrorMessage());

      continue;
    }

    if (auto* mapping = declaration->mappingDeclaration(); mapping)
      sys.mappings.push_back(std::any_cast<koda::PTypeMapping>(visitMappingDeclaration(mapping)));
  }

  // LOG_DEBUG("Visiting system");
  for (auto* declaration : ctx->topLevelDeclaration())
    if (auto* topLevel = declaration->topLevelComponent(); topLevel)
      sys.components.push_back(std::any_cast<koda::PComponent>(visit(topLevel)));

  // LOG_DEBUG("Done visiting system");
  return sys;
}

std::any CST2AST::visitTopLevelComponent(KodaParser::TopLevelComponentContext* ctx)
{
  auto c = std::make_shared<koda::Component>();
  c->span = spanOf(ctx);

  if (ctx->TASK())
    c->kind = koda::Component::Kind::Task;
  else
    c->kind = koda::Component::Kind::Capability;

  c->name = ctx->IDENT()->getText();

  // LOG_DEBUG("Visiting top level component: {}", c->name);
  if (auto* al = ctx->argumentList())
    c->args = std::any_cast<std::vector<koda::PArgument>>(visit(al));

  for (auto* st : ctx->statement())
    c->statements.push_back(std::any_cast<koda::PStatement>(visit(st)));

  // LOG_DEBUG("Done visiting top level component: {}", c->name);
  return c;
}

std::any CST2AST::visitArgumentList(KodaParser::ArgumentListContext* ctx)
{
  // LOG_DEBUG("Visiting argument list");
  std::vector<koda::PArgument> out;
  for (auto* a : ctx->argument())
    out.push_back(std::any_cast<koda::PArgument>(visit(a)));

  return out;
}

std::any CST2AST::visitArgPlain(KodaParser::ArgPlainContext* ctx)
{
  // LOG_DEBUG("Visiting arg plain");
  auto a = std::make_shared<koda::Argument>();
  a->kind = koda::Argument::Kind::Plain;
  a->a = convertTypeReference(ctx->typeReference());  // Type
  a->b = ctx->IDENT()->getText();                     // Symbol
  a->span = spanOf(ctx);
  return a;
}

std::any CST2AST::visitArgReq(KodaParser::ArgReqContext* ctx)
{
  // LOG_DEBUG("Visiting arg req");
  auto a = std::make_shared<koda::Argument>();
  a->kind = koda::Argument::Kind::Req;
  a->a = convertTypeReference(ctx->typeReference());  // Type
  a->b = ctx->IDENT()->getText();                     // Symbol
  a->span = spanOf(ctx);
  return a;
}

std::any CST2AST::visitArgPro(KodaParser::ArgProContext* ctx)
{
  // LOG_DEBUG("Visiting arg pro");
  auto a = std::make_shared<koda::Argument>();
  a->kind = koda::Argument::Kind::Pro;
  a->a = convertTypeReference(ctx->typeReference());  // Type
  a->b = ctx->IDENT()->getText();                     // Symbol
  a->span = spanOf(ctx);
  return a;
}

std::any CST2AST::visitMappingDeclaration(KodaParser::MappingDeclarationContext* ctx)
{
  auto mapping = std::make_shared<koda::TypeMapping>();

  if (ctx->typeReference())
    mapping->source = convertTypeReference(ctx->typeReference());
  else
    ERROR_AND_RETURN(ctx, "Invalid source in mapping", mapping);

  if (ctx->STRING())
    mapping->destination = ctx->STRING()->getText();
  else
    ERROR_AND_RETURN(ctx, "Invalid destination in mapping", mapping);

  return mapping;
}

// -------------------------
// Statements
// -------------------------

std::any CST2AST::visitStatement(KodaParser::StatementContext* ctx)
{
  // LOG_DEBUG("Visiting statement");
  auto s = std::make_shared<koda::Statement>();
  s->span = spanOf(ctx);

  if (ctx->tasksBlock())
    s->node = std::any_cast<koda::PStrategyBlock>(visit(ctx->tasksBlock()));
  else if (ctx->varsBlock())
    s->node = std::any_cast<koda::PVarsBlock>(visit(ctx->varsBlock()));
  else if (ctx->rosDefStatement())
    s->node = std::any_cast<koda::PRosDef>(visit(ctx->rosDefStatement()));
  else if (ctx->actionBlock())
    s->node = std::any_cast<koda::PActionDef>(visit(ctx->actionBlock()));
  else if (ctx->serviceBlock())
    s->node = std::any_cast<koda::PActionDef>(visit(ctx->serviceBlock()));
  else if (ctx->topicBlock())
    s->node = std::any_cast<koda::PActionDef>(visit(ctx->topicBlock()));
  else
    throw std::runtime_error("Unknown statement kind");

  // LOG_DEBUG("Done visiting statement");

  return s;
}

std::any CST2AST::visitTasksBlock(KodaParser::TasksBlockContext* ctx)
{
  // LOG_DEBUG("Visiting tasks block");
  auto sb = std::make_shared<koda::StrategyBlock>();
  sb->span = spanOf(ctx);

  for (auto* f : ctx->flow())
    sb->flows.push_back(std::any_cast<koda::PFlow>(visit(f)));

  // LOG_DEBUG("Done visiting tasks block");
  return sb;
}

std::any CST2AST::visitFlow(KodaParser::FlowContext* ctx)
{
  // LOG_DEBUG("Visiting flow");
  auto f = std::make_shared<koda::Flow>();
  f->span = spanOf(ctx);
  f->name = ctx->IDENT()->getText();

  for (size_t i = 0; ctx->identList() && i < ctx->identList()->IDENT().size(); ++i)
  {
    auto ident = ctx->identList()->IDENT(i);

    auto arg = std::make_shared<koda::Argument>();
    arg->a = types::TypeReference{};
    arg->b = ident->getText();  // Name only

    // Do we need to make a function out of this?
    auto token = ident->getSymbol();
    arg->span.lineStart = token->getLine();
    arg->span.colStart = token->getCharPositionInLine();
    arg->span.lineEnd = token->getLine();
    arg->span.colEnd = token->getCharPositionInLine() + static_cast<int>(token->getText().size());

    f->args.push_back(arg);
  }

  f->strategy = std::any_cast<koda::PStrategy>(visit(ctx->strategy()));
  // LOG_DEBUG("Done visiting flow");
  return f;
}

std::any CST2AST::visitIdentList(KodaParser::IdentListContext* ctx)
{
  // LOG_DEBUG("Visiting ident list");
  std::vector<std::string> out;
  for (auto* t : ctx->IDENT())
    out.push_back(t->getText());

  return out;
}

std::any CST2AST::visitVarsBlock(KodaParser::VarsBlockContext* ctx)
{
  // LOG_DEBUG("Visiting vars block");
  auto vb = std::make_shared<koda::VarsBlock>();
  vb->span = spanOf(ctx);

  for (auto* v : ctx->variableStatement())
    vb->vars.push_back(std::any_cast<koda::PVarDef>(visit(v)));

  return vb;
}

std::any CST2AST::visitVariableStatement(KodaParser::VariableStatementContext* ctx)
{
  // LOG_DEBUG("Visiting variable statement");
  auto v = std::make_shared<koda::VarDef>();
  v->span = spanOf(ctx);
  v->varType = convertTypeReference(ctx->typeReference());
  v->name = ctx->IDENT()->getText();
  // LOG_DEBUG("Visiting first expression of variable statement");
  v->init = std::any_cast<koda::PExpr>(visit(ctx->expression(0)));
  // LOG_DEBUG("Visiting second expression of variable statement");
  v->fallback = std::any_cast<koda::PExpr>(visit(ctx->expression(1)));

  return v;
}

// Action/service/topic blocks mapped to same IR (ActionDef)
std::any CST2AST::visitActionBlock(KodaParser::ActionBlockContext* ctx)
{
  // LOG_DEBUG("Visiting action block");
  return buildActionLike(ctx, koda::ActionDef::Kind::Action);
}

std::any CST2AST::visitServiceBlock(KodaParser::ServiceBlockContext* ctx)
{
  // LOG_DEBUG("Visiting service block");
  return buildActionLike(ctx, koda::ActionDef::Kind::Service);
}

std::any CST2AST::visitTopicBlock(KodaParser::TopicBlockContext* ctx)
{
  // LOG_DEBUG("Visiting topic block");
  return buildActionLike(ctx, koda::ActionDef::Kind::Topic);
}

// -------------------------
// ROS defs
// -------------------------

std::any CST2AST::visitRosDefStatement(KodaParser::RosDefStatementContext* ctx)
{
  // LOG_DEBUG("Visiting ROS def statement");
  auto r = std::make_shared<koda::RosDef>();
  r->span = spanOf(ctx);
  if (ctx->TRIGGER())
    r->kind = koda::RosDef::Kind::Trigger;
  else if (ctx->RETURN())
    r->kind = koda::RosDef::Kind::Return;
  else if (ctx->ABORT())
    r->kind = koda::RosDef::Kind::Abort;
  else if (ctx->ERROR())
    r->kind = koda::RosDef::Kind::Error;
  else if (ctx->IN())
    r->kind = koda::RosDef::Kind::In;
  else if (ctx->OUT())
    r->kind = koda::RosDef::Kind::Out;
  else
    throw std::runtime_error("Unknown RosDef kind");

  r->def = std::any_cast<koda::PEventDef>(visit(ctx->eventDefStatement()));

  return r;
}

std::any CST2AST::visitEventDefStatement(KodaParser::EventDefStatementContext* ctx)
{
  // LOG_DEBUG("Visiting event def statement");

  auto d = std::make_shared<koda::EventDef>();
  d->span = spanOf(ctx);
  d->typeName = ctx->IDENT()->getText();
  d->name = ctx->identifier()->getText();

  if (ctx->argumentList())
    d->args = std::any_cast<std::vector<koda::PArgument>>(visit(ctx->argumentList()));

  return d;
}

// -------------------------
// Strategy
// -------------------------

std::any CST2AST::visitStratSeq(KodaParser::StratSeqContext* ctx)
{
  // LOG_DEBUG("Visiting Stategy sequence");
  auto value = std::make_shared<koda::Strategy::Seq>();

  // We flatten the tree in case of a sequence of strategies
  for (auto* s : ctx->strategy())
  {
    auto child = std::any_cast<koda::PStrategy>(visit(s));
    // TODO: For now, we don't handle the end, fix this
    if (std::holds_alternative<koda::PEnd>(child->v))
      continue;
    if (auto* paren = std::get_if<koda::PParen>(&child->v))
      if (std::holds_alternative<koda::PEnd>((*paren)->a->v))
        continue;

    // if (std::get_if<koda::PContinue>(&child->v))
    //   continue;

    if (auto* childSeq = std::get_if<koda::PSeq>(&child->v))
      value->alts.insert(value->alts.end(), (*childSeq)->alts.begin(), (*childSeq)->alts.end());
    else
      value->alts.push_back(child);
  }

  // No point in having a sequence if we only have one action in the sequence
  if (value->alts.size() == 1)
    return value->alts.at(0);

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;
  return node;
}

std::any CST2AST::visitStratJoin(KodaParser::StratJoinContext* ctx)
{
  // LOG_DEBUG("Visiting Join sequence");
  auto value = std::make_shared<koda::Strategy::Join>();
  for (auto* s : ctx->strategy())
    value->alts.push_back(std::any_cast<koda::PStrategy>(visit(s)));

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;
  return node;
}

std::any CST2AST::visitStratEither(KodaParser::StratEitherContext* ctx)
{
  // LOG_DEBUG("Visiting Either sequence");
  auto value = std::make_shared<koda::Strategy::Either>();
  for (auto* s : ctx->strategy())
    value->alts.push_back(std::any_cast<koda::PStrategy>(visit(s)));

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;
  return node;
}

std::any CST2AST::visitStratWithin(KodaParser::StratWithinContext* ctx)
{
  // LOG_DEBUG("Visiting Within");
  auto value = std::make_shared<koda::Strategy::Within>();
  value->seconds = std::stoi(ctx->NATURAL()->getText());
  value->a = std::any_cast<koda::PStrategy>(visit(ctx->strategy(0)));
  value->b = std::any_cast<koda::PStrategy>(visit(ctx->strategy(1)));

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;
  return node;
}

std::any CST2AST::visitStratRepeat(KodaParser::StratRepeatContext* ctx)
{
  // LOG_DEBUG("Visiting Repeat");
  auto value = std::make_shared<koda::Strategy::Repeat>();
  value->a = std::any_cast<koda::PStrategy>(visit(ctx->strategy()));

  if (ctx->NATURAL().size() != 2)
    throw std::runtime_error("Missing arguments for repeat block. Expects <iterations> <rate>");

  value->iterations = std::stoi(ctx->NATURAL(0)->getText());
  value->seconds = std::stoi(ctx->NATURAL(1)->getText());
  for (auto h : ctx->strategyHandler())
    value->handlers.push_back(std::any_cast<koda::PStrategyHandler>(visit(h)));

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;
  return node;
}

std::any CST2AST::visitStratEnd(KodaParser::StratEndContext* ctx)
{
  // LOG_DEBUG("Visiting End");
  auto value = std::make_shared<koda::Strategy::End>();
  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;

  return node;
}

std::any CST2AST::visitStratContinue(KodaParser::StratContinueContext* ctx)
{
  // LOG_DEBUG("Visiting Continue");
  auto value = std::make_shared<koda::Strategy::Continue>();
  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;

  return node;
}

std::any CST2AST::visitStratRef(KodaParser::StratRefContext* ctx)
{
  // LOG_DEBUG("Visiting Reference");
  auto value = std::make_shared<koda::Strategy::Ref>();
  value->name = ctx->identifier()->getText();

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;

  return node;
}

std::any CST2AST::visitStratParen(KodaParser::StratParenContext* ctx)
{
  // LOG_DEBUG("Visiting Parenthesis");
  auto value = std::make_shared<koda::Strategy::Paren>();
  value->a = std::any_cast<koda::PStrategy>(visit(ctx->strategy()));

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;

  return node;
}

std::any CST2AST::visitStratTask(KodaParser::StratTaskContext* ctx)
{
  // LOG_DEBUG("Visiting Task");
  auto value = std::make_shared<koda::Strategy::TaskCall>();
  value->call = std::any_cast<koda::PEventCall>(visit(ctx->eventStatement()));
  for (auto h : ctx->strategyHandler())
    value->handlers.push_back(std::any_cast<koda::PStrategyHandler>(visit(h)));

  auto node = std::make_shared<koda::Strategy>();
  node->span = spanOf(ctx);
  node->v = value;

  return node;
}

std::any CST2AST::visitHandlerOnError(KodaParser::HandlerOnErrorContext* ctx)
{
  // LOG_DEBUG("Visiting On error");

  auto value = std::make_shared<koda::StrategyHandler>();
  value->kind = koda::StrategyHandler::Kind::OnError;
  value->body = std::any_cast<koda::PStrategy>(visit(ctx->strategy()));
  value->span = spanOf(ctx);

  return value;
}

std::any CST2AST::visitHandlerOnAbort(KodaParser::HandlerOnAbortContext* ctx)
{
  // LOG_DEBUG("Visiting on abort");
  auto value = std::make_shared<koda::StrategyHandler>();
  value->kind = koda::StrategyHandler::Kind::OnAbort;
  value->body = std::any_cast<koda::PStrategy>(visit(ctx->strategy()));
  value->span = spanOf(ctx);

  return value;
}

std::any CST2AST::visitHandlerOnEmitter(KodaParser::HandlerOnEmitterContext* ctx)
{
  // LOG_DEBUG("Visiting on signal");
  auto value = std::make_shared<koda::StrategyHandler>();
  value->kind = koda::StrategyHandler::Kind::OnEmitter;
  value->emitter = std::any_cast<koda::PEventCall>(visit(ctx->eventStatement()));
  value->body = std::any_cast<koda::PStrategy>(visit(ctx->strategy()));
  if (containsContinue(value->body))
    value->kind = koda::StrategyHandler::Kind::OnEmitterContinue;

  value->span = spanOf(ctx);

  return value;
}

// -------------------------
// Event calls
// -------------------------

std::any CST2AST::visitEvCall(KodaParser::EvCallContext* ctx)
{
  // LOG_DEBUG("Visiting event call");
  auto c = std::make_shared<koda::EventCall>();
  c->span = spanOf(ctx);
  c->receiver = "";
  c->name = ctx->identifier()->getText();

  if (ctx->exprList())
    c->args = std::any_cast<std::vector<koda::PExpr>>(visit(ctx->exprList()));

  return c;
}

std::any CST2AST::visitEvQualifiedCall(KodaParser::EvQualifiedCallContext* ctx)
{
  // LOG_DEBUG("Visiting qualified call");
  auto c = std::make_shared<koda::EventCall>();
  c->span = spanOf(ctx);
  c->receiver = ctx->identifier(0)->getText();
  c->name = ctx->identifier(1)->getText();

  if (ctx->exprList())
    c->args = std::any_cast<std::vector<koda::PExpr>>(visit(ctx->exprList()));

  return c;
}

std::any CST2AST::visitExprList(KodaParser::ExprListContext* ctx)
{
  // LOG_DEBUG("Visiting Expression list");
  std::vector<koda::PExpr> out;
  for (auto* e : ctx->expression())
    out.push_back(std::any_cast<koda::PExpr>(visit(e)));

  return out;
}

// -------------------------
// Expressions
// -------------------------

// Because we factored expression into precedence rules (exprOr -> exprAnd -> ...),
// many nodes are built in the lower-level visit methods. Here are the leaves:

std::any CST2AST::visitIdentifier(KodaParser::IdentifierContext* ctx)
{
  if (ctx->IDENT())
    return unquoteString(ctx->IDENT()->getText());
  else if (ctx->ABORT())
    return unquoteString(ctx->ABORT()->getText());
  else if (ctx->TRIGGER())
    return unquoteString(ctx->TRIGGER()->getText());
  else if (ctx->RETURN())
    return unquoteString(ctx->RETURN()->getText());
  else if (ctx->ERROR())
    return unquoteString(ctx->ERROR()->getText());
  else if (ctx->IN())
    return unquoteString(ctx->IN()->getText());
  else if (ctx->OUT())
    return unquoteString(ctx->OUT()->getText());
  else
    throw std::runtime_error("Unknown Identifier kind");
}

std::any CST2AST::visitExprId(KodaParser::ExprIdContext* ctx)
{
  // LOG_DEBUG("Visiting Expression id");
  auto s = std::make_shared<koda::Expr::Id>();
  s->value = unquoteString(ctx->IDENT()->getText());

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;
  return exp;
}

std::any CST2AST::visitExprString(KodaParser::ExprStringContext* ctx)
{
  // LOG_DEBUG("Visiting Expression string");
  auto s = std::make_shared<koda::Expr::Str>();
  s->value = unquoteString(ctx->STRING()->getText());

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  return exp;
}

std::any CST2AST::visitExprInt(KodaParser::ExprIntContext* ctx)
{
  // LOG_DEBUG("Visiting Expression int");
  auto s = std::make_shared<koda::Expr::Int>();
  s->value = std::stoll(ctx->NATURAL()->getText());

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  return exp;
}

std::any CST2AST::visitExprFloat(KodaParser::ExprFloatContext* ctx)
{
  // LOG_DEBUG("Visiting Expression float");
  auto s = std::make_shared<koda::Expr::Float>();
  s->value = std::stod(ctx->REAL()->getText());

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  return exp;
}

std::any CST2AST::visitExprCall(KodaParser::ExprCallContext* ctx)
{
  // LOG_DEBUG("Visiting Expression call");
  auto s = std::make_shared<koda::Expr::Call>();
  s->value = std::any_cast<koda::PEventCall>(visit(ctx->eventStatement()));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  return exp;
}

std::any CST2AST::visitExprParen(KodaParser::ExprParenContext* ctx)
{
  // LOG_DEBUG("Visiting Expression parenthesis");
  auto s = std::make_shared<koda::Expr::Paren>();
  s->value = std::any_cast<koda::PExpr>(visit(ctx->expression()));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Par");
  return exp;
}

std::any CST2AST::visitExprOr(KodaParser::ExprOrContext* ctx)
{
  koda::PExpr left;
  if (ctx->exprAnd().size() > 0 && ctx->exprAnd(0))
    left = std::any_cast<koda::PExpr>(visit(ctx->exprAnd(0)));

  if (ctx->OR().empty())
    return left;

  // LOG_DEBUG("Visiting Expression Or");
  auto s = std::make_shared<koda::Expr::BinOp>();
  s->operation = koda::Expr::BinOp::Kind::Disjunction;

  s->a = left;
  if (ctx->exprAnd().size() > 1 && ctx->exprAnd(1))
    s->b = std::any_cast<koda::PExpr>(visit(ctx->exprAnd(1)));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Or");
  return exp;
}

std::any CST2AST::visitExprAnd(KodaParser::ExprAndContext* ctx)
{
  koda::PExpr left;
  if (ctx->exprCmp().size() > 0 && ctx->exprCmp(0))
    left = std::any_cast<koda::PExpr>(visit(ctx->exprCmp(0)));

  if (ctx->AND().empty())
    return left;

  // LOG_DEBUG("Visiting Expression And");
  auto s = std::make_shared<koda::Expr::BinOp>();
  s->operation = koda::Expr::BinOp::Kind::Conjunction;

  s->a = left;
  if (ctx->exprCmp().size() > 1 && ctx->exprCmp(1))
    s->b = std::any_cast<koda::PExpr>(visit(ctx->exprCmp(1)));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression And");
  return exp;
}

std::any CST2AST::visitCompOp(KodaParser::CompOpContext* ctx)
{
  if (ctx->EQ())
    return koda::Expr::BinOp::Kind::Equal;
  else if (ctx->NEQ())
    return koda::Expr::BinOp::Kind::NotEqual;
  else if (ctx->LEQ())
    return koda::Expr::BinOp::Kind::LessEqual;
  else if (ctx->GEQ())
    return koda::Expr::BinOp::Kind::GreaterEqual;
  else if (ctx->LT())
    return koda::Expr::BinOp::Kind::LessThan;
  else if (ctx->GT())
    return koda::Expr::BinOp::Kind::GreaterThan;
  else
    throw std::runtime_error("Unknown Comparator kind");
}

std::any CST2AST::visitExprCmp(KodaParser::ExprCmpContext* ctx)
{
  koda::PExpr left;
  if (ctx->exprNot().size() > 0 && ctx->exprNot(0))
    left = std::any_cast<koda::PExpr>(visit(ctx->exprNot(0)));

  if (!ctx->compOp())
    return left;

  // LOG_DEBUG("Visiting Expression Cmp");
  auto s = std::make_shared<koda::Expr::BinOp>();
  s->operation = std::any_cast<koda::Expr::BinOp::Kind>(visit(ctx->compOp()));
  s->a = left;

  if (ctx->exprNot().size() > 1 && ctx->exprNot(1))
    s->b = std::any_cast<koda::PExpr>(visit(ctx->exprNot(1)));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Cmp");
  return exp;
}

std::any CST2AST::visitExprNot(KodaParser::ExprNotContext* ctx)
{
  if (ctx->exprAdd())
    return visit(ctx->exprAdd());

  // LOG_DEBUG("Visiting Expression Not");
  auto s = std::make_shared<koda::Expr::BinOp>();
  s->operation = koda::Expr::BinOp::Kind::Negation;
  if (ctx->exprNot())
    s->a = std::any_cast<koda::PExpr>(visit(ctx->exprNot()));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Not");
  return exp;
}

std::any CST2AST::visitExprAdd(KodaParser::ExprAddContext* ctx)
{
  koda::PExpr left;
  if (ctx->exprMul().size() > 0 && ctx->exprMul(0))
    left = std::any_cast<koda::PExpr>(visit(ctx->exprMul(0)));

  if (ctx->PLUS().empty() && ctx->MINUS().empty())
    return left;

  // LOG_DEBUG("Visiting Expression Add");
  auto s = std::make_shared<koda::Expr::BinOp>();
  if (!ctx->PLUS().empty())
    s->operation = koda::Expr::BinOp::Kind::Addition;
  else if (!ctx->MINUS().empty())
    s->operation = koda::Expr::BinOp::Kind::Subtraction;

  s->a = left;

  if (ctx->exprMul().size() > 1 && ctx->exprMul(1))
    s->b = std::any_cast<koda::PExpr>(visit(ctx->exprMul(1)));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Add");
  return exp;
}

std::any CST2AST::visitExprMul(KodaParser::ExprMulContext* ctx)
{
  koda::PExpr left;
  if (ctx->exprUnary().size() > 0 && ctx->exprUnary(0))
    left = std::any_cast<koda::PExpr>(visit(ctx->exprUnary(0)));

  if (ctx->STAR().empty() && ctx->SLASH().empty())
    return left;

  // LOG_DEBUG("Visiting Expression Mul");
  auto s = std::make_shared<koda::Expr::BinOp>();
  if (!ctx->STAR().empty())
    s->operation = koda::Expr::BinOp::Kind::Multiplication;
  else if (!ctx->SLASH().empty())
    s->operation = koda::Expr::BinOp::Kind::Division;

  s->a = left;
  if (ctx->exprUnary().size() > 1 && ctx->exprUnary(1))
    s->b = std::any_cast<koda::PExpr>(visit(ctx->exprUnary(1)));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Mul");
  return exp;
}

std::any CST2AST::visitExprUnary(KodaParser::ExprUnaryContext* ctx)
{
  if (!ctx->MINUS())
    return visit(ctx->exprPrimary());

  // LOG_DEBUG("Visiting Expression Unary");

  auto s = std::make_shared<koda::Expr::BinOp>();
  s->operation = koda::Expr::BinOp::Kind::Unary;
  if (ctx->exprUnary())
    s->a = std::any_cast<koda::PExpr>(visit(ctx->exprUnary()));

  auto exp = std::make_shared<koda::Expr>();
  exp->span = spanOf(ctx);
  exp->v = s;

  // LOG_DEBUG("Done visiting Expression Unary");
  return exp;
}

template <typename CtxT>
std::any CST2AST::buildActionLike(CtxT* ctx, koda::ActionDef::Kind kind)
{
  auto a = std::make_shared<koda::ActionDef>();
  a->kind = kind;
  a->span = spanOf(ctx);

  // ACTION STRING STRING  { rosDefStatement* }
  a->label1 = unquoteString(ctx->STRING(0)->getText());
  a->label2 = unquoteString(ctx->STRING(1)->getText());

  for (auto* r : ctx->rosDefStatement())
    a->rosDefs.push_back(std::any_cast<koda::PRosDef>(visit(r)));

  for (auto* r : ctx->reqDefStatement())
    if (r->CONSUMES())
      a->consumes = convertTypeReference(r->typeReference());
    else if (r->PRODUCES())
      a->produces = convertTypeReference(r->typeReference());
    else
      LOG_ERROR("Unknown requirement definition");

  return a;
}

bool CST2AST::containsContinue(koda::PStrategy s)
{
  if (!s)
    return false;

  return std::visit(
      [&](auto&& node) -> bool {
        using T = std::decay_t<decltype(node)>;

        // Ref
        if constexpr (std::is_same_v<T, koda::PContinue>)
        {
          return true;
        }
        // Seq / Join / Either
        else if constexpr (std::is_same_v<T, koda::PSeq> || std::is_same_v<T, koda::PJoin> || std::is_same_v<T, koda::PEither>)
        {
          if (!node)
            return false;

          for (const auto& child : node->alts)
            if (child && containsContinue(child))
              return true;

          return false;
        }
        // Unary wrappers
        else if constexpr (std::is_same_v<T, std::shared_ptr<koda::Strategy::Paren>>)
        {
          return node && node->a && containsContinue(node->a);
        }
        // Binary
        else if constexpr (std::is_same_v<T, koda::PWithin>)
        {
          return node && ((node->a && containsContinue(node->a)) || (node->b && containsContinue(node->b)));
        }
        // Repeat
        else if constexpr (std::is_same_v<T, std::shared_ptr<koda::Strategy::Repeat>>)
        {
          if (!node)
            return false;

          if (node->a && containsContinue(node->a))
            return true;

          for (const auto& h : node->handlers)
            if (h && h->body && containsContinue(h->body))
              return true;
          return false;
        }
        // TaskCall
        else if constexpr (std::is_same_v<T, std::shared_ptr<koda::Strategy::TaskCall>>)
        {
          if (!node)
            return false;

          for (const auto& h : node->handlers)
            if (h && h->body && containsContinue(h->body))
              return true;
          return false;
        }
        // End
        else
        {
          return false;
        }
      },
      s->v);
}

types::TypeDefinition CST2AST::convertTypeDeclaration(KodaParser::TypeDeclarationContext* ctx)
{
  if (auto* record = dynamic_cast<KodaParser::TypeRecordContext*>(ctx))
  {
    const types::QualifiedName name = convertQualifiedName(record->qualifiedName(0));
    std::optional<std::string> baseType;
    if (record->EXTENDS() != nullptr)
    {
      const auto baseName = convertQualifiedName(record->qualifiedName(1));
      baseType = baseName.toString();
    }

    std::vector<types::FieldDefinition> fields;
    fields.reserve(record->fieldDeclaration().size());

    for (auto* field : record->fieldDeclaration())
    {
      fields.push_back(types::FieldDefinition{
          .name = field->IDENT()->getText(),
          .type = convertTypeReference(field->typeReference()),
      });
    }

    return types::TypeDefinition::createRecord(name.toString(), fields, baseType.value_or(""), types::makeUuid());
  }

  if (auto* alias = dynamic_cast<KodaParser::TypeAliasContext*>(ctx))
  {
    const auto name = convertQualifiedName(alias->qualifiedName());
    return types::TypeDefinition::createAlias(name.toString(), convertTypeReference(alias->typeReference()), types::makeUuid());
  }

  throw std::runtime_error("Unknown KODA type declaration");
}

types::TypeDefinition CST2AST::convertEnumDeclaration(KodaParser::EnumDeclarationContext* ctx)
{
  const types::QualifiedName name = convertQualifiedName(ctx->qualifiedName());

  // Default enum representation.
  types::EnumUnderlyingKind underlyingType = types::EnumUnderlyingKind::Int32;
  if (ctx->typeReference() != nullptr)
    underlyingType = convertEnumUnderlyingType(ctx->typeReference());

  std::vector<types::EnumValueDefinition> values;
  values.reserve(ctx->enumValue().size());
  for (auto* value : ctx->enumValue())
  {
    std::optional<std::string> enumValue;
    if (value->enumLiteral() != nullptr)
    {
      auto* literal = value->enumLiteral();
      if (literal->STRING() != nullptr)
        enumValue = unquoteString(literal->STRING()->getText());
      else if (literal->NATURAL() != nullptr)
        enumValue = literal->getText();
    }

    values.push_back(types::EnumValueDefinition{
        .name = value->IDENT()->getText(),
        .value = std::move(enumValue),
    });
  }

  return types::TypeDefinition::createEnum(name.toString(), underlyingType, values, koda::types::makeUuid());
}

types::TypeReference CST2AST::convertTypeReference(KodaParser::TypeReferenceContext* ctx)
{
  if (auto* named = dynamic_cast<KodaParser::TypeNamedContext*>(ctx))
  {
    const auto name = convertQualifiedName(named->qualifiedName());
    if (koda::types::isBuiltin(name))
      return types::TypeReference::primitive(koda::types::primitiveKindFromString(name.toString()));
    else
      return types::TypeReference::named(name, name.toId());
  }

  if (auto* list = dynamic_cast<KodaParser::TypeListContext*>(ctx))
    return types::TypeReference::list(convertTypeReference(list->typeReference()));

  if (auto* optional = dynamic_cast<KodaParser::TypeOptionalContext*>(ctx))
    return types::TypeReference::optional(convertTypeReference(optional->typeReference()));

  if (auto* map = dynamic_cast<KodaParser::TypeMapContext*>(ctx))
    return types::TypeReference::map(convertTypeReference(map->typeReference(0)), convertTypeReference(map->typeReference(1)));

  throw std::runtime_error("Unknown type reference: " + ctx->getText());
}

types::QualifiedName CST2AST::convertQualifiedName(KodaParser::QualifiedNameContext* ctx)
{
  const auto name = ctx->getText();
  if (name == "bool")
    return types::BooleanType;
  if (name == "int")
    return types::IntegerType;
  if (name == "uint")
    return types::UnsignedType;
  if (name == "real")
    return types::RealType;
  if (name == "string")
    return types::StringType;
  if (name == "bytes")
    return types::BytesType;
  if (name == "timestamp")
    return types::TimestampType;
  if (name == "duration")
    return types::DurationType;
  if (name == "void")
    return types::VoidType;

  return types::QualifiedName(name);
}

types::EnumUnderlyingKind CST2AST::convertEnumUnderlyingType(KodaParser::TypeReferenceContext* ctx)
{
  const types::TypeReference type = convertTypeReference(ctx);

  if (type.isNamed())
  {
    const auto* definition = mTypeRegistry->resolve(type);
    if (definition == nullptr)
      ERROR_AND_RETURN(ctx, "Unknown enum underlying type '" + type.toString() + "'", types::EnumUnderlyingKind::Unknown);
    if (!definition->isPrimitive())
      ERROR_AND_RETURN(ctx, "Enum underlying type must be primitive", types::EnumUnderlyingKind::Unknown);

    return types::enumKindFromPrimitive(definition->primitive().primitive);
  }
  else if (type.isPrimitive())
  {
    return types::enumKindFromPrimitive(type.primitiveKind());
  }

  ERROR_AND_RETURN(ctx, "Unknown enum underlying type '" + type.toString() + "'", types::EnumUnderlyingKind::Unknown);
}

}  // namespace koda