//------------------------------------------------------------------------------
/// @brief SnuPL/0 parser
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2014/11/04 Bernhard Egger maintain unary '+' signs in the AST
/// 2016/04/01 Bernhard Egger adapted to SnuPL/1 (this is not a joke)
/// 2016/09/28 Bernhard Egger assignment 2: parser for SnuPL/-1
///
/// @section license_section License
/// Copyright (c) 2012-2016, Bernhard Egger
/// All rights reserved.
///
/// Redistribution and use in source and binary forms,  with or without modifi-
/// cation, are permitted provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER  OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE
/// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
/// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT
/// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY
/// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//------------------------------------------------------------------------------

#include <limits.h>
#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <exception>

#include "parser.h"
using namespace std;


//------------------------------------------------------------------------------
// CParser
//
CParser::CParser(CScanner *scanner)
{
  _scanner = scanner;
  _module = NULL;
}

CAstNode* CParser::Parse(void)
{
  _abort = false;

  if (_module != NULL) { delete _module; _module = NULL; }

  try {
    if (_scanner != NULL) _module = module();

    if (_module != NULL) {
      CToken t;
      string msg;
      //if (!_module->TypeCheck(&t, &msg)) SetError(t, msg);
    }
  } catch (...) {
    _module = NULL;
  }

  return _module;
}

const CToken* CParser::GetErrorToken(void) const
{
  if (_abort) return &_error_token;
  else return NULL;
}

string CParser::GetErrorMessage(void) const
{
  if (_abort) return _message;
  else return "";
}

void CParser::SetError(CToken t, const string message)
{
  _error_token = t;
  _message = message;
  _abort = true;
  throw message;
}

bool CParser::Consume(EToken type, CToken *token)
{
  if (_abort) return false;

  CToken t = _scanner->Get();

  if (t.GetType() != type) {
    SetError(t, "expected '" + CToken::Name(type) + "', got '" +
             t.GetName() + "'");
  }

  if (token != NULL) *token = t;

  return t.GetType() == type;
}

void CParser::InitSymbolTable(CSymtab *s)
{
  CTypeManager *tm = CTypeManager::Get();

  // TODO: add predefined functions here
}

CAstModule* CParser::module(void)
{
  //
  // module ::= "module" ident ";" varDeclaration { subroutineDecl }
  //            "begin" stateSequence "end" ident ".".
  // subroutineDecl ::= (procedureDecl | functionDecl) subroutineBody ident ";".
  // procedureDecl ::= "procedure" ident [ formalParam ] ";".
  // functionDecl ::= "function" ident [ formalParam ] ":" type ";".
  
  CToken t;

  // module -> "module" ident ";" ...
  Consume(kModule);
  CToken tModuleIdent = _scanner->Get();
  if (tModuleIdent.GetType() != tIdent)
    SetError(tModuleIdent, "module identifier expected");
  Consume(tSemicolon);

  CAstModule *m = new CAstModule(t, tModuleIdent.GetName());
  // CSymtab *symtab = m->GetSymbolTable();

  // module -> ... varDeclaration ...
  varDeclaration(m);

  // module -> ... { subroutineDecl } ...
  CAstProcedure *sub = NULL;
  EToken tt = _scanner->Peek().GetType();
  while (tt != kBegin) {
    switch (tt) {
      // subroutineDecl -> procedureDecl ...
      case kProc:
        sub = procedureDecl(m);
        break;

      // subroutineDecl -> functionDecl ...
      case kFunc:
        sub = functionDecl(m);
        break;
        
      default:
        SetError(_scanner->Peek(), "invalid subroutine declaration");
        break;
    }

    // subroutineDecl -> ... subroutineBody ...
    subroutineBody(sub);

    // subroutineDecl -> ... ident ";".
    CToken t = _scanner->Peek();
    if (t.GetType() != tIdent || t.GetName() != sub->GetName())
      SetError(t, "subroutine identifier not matched");
    Consume(tIdent);
    Consume(tSemicolon);
  }

  // module -> ... "begin" statSequence "end" ...
  Consume(kBegin);
  CAstStatement *statseq = statSequence(m);
  m->SetStatementSequence(statseq);
  Consume(kEnd);

  // module -> ... ident "."
  CToken tModuleIdentClose = _scanner->Get();
  if (tModuleIdent.GetValue() != tModuleIdentClose.GetValue())
    SetError(tModuleIdentClose, "module identifier not matched");
  Consume(tDot);

  return m;
}

void CParser::varDeclaration(CAstScope *s)
{
  //
  // varDeclaration ::= [ "var" varDeclSequence ";" ].
  // varDeclSequence ::= varDecl { ";" varDecl }.
  //

  // varDeclaration -> "var" ...
  EToken tt = _scanner->Peek().GetType();
  if (tt == kVar) {
    Consume(kVar);

    // varDeclaration -> ... varDeclSequence ...
    vector<string> allVars;
    do {
      vector<string> l;
      CAstType *ttype;

      // varDeclSequence -> ... varDecl ...
      varDecl(l, ttype, allVars);

      for (const auto &str : l) {
        CSymbol *global_var = s->CreateVar(str, ttype->GetType());
        s->GetSymbolTable()->AddSymbol(global_var);
      }

      // varDeclaration -> ... ";"
      // varDeclSequence -> ... ";" ...
      Consume(tSemicolon);
      tt = _scanner->Peek().GetType();
    } while (tt != kProc && tt != kFunc && tt != kBegin);
  }
}

void CParser::varDecl(vector<string> &vars, CAstType* &ttype, vector<string> &allVars)
{
  //
  // varDecl ::= ident { "," ident } ":" type.
  //
  
  while (!_abort) {
    // varDecl -> ident ...
    CToken e = _scanner->Get();
    if (e.GetType() != tIdent)
      SetError(e, "invalid identifier");
      
    for (const auto &var : allVars) {
      if (var == e.GetValue()) {
        SetError(e, e.GetValue() + " : re-declaration variable");
        return;
      }
    }
    vars.push_back(e.GetValue());
    allVars.push_back(e.GetValue());
    
    e = _scanner->Peek();
    if (e.GetType() == tColon) break;
    else if (e.GetType() != tComma)
      SetError(e, "\":\" or \",\" expected");

    // varDecl -> ... "," ...
    Consume(tComma);
  }

  // varDecl -> ... ":" type
  Consume(tColon);
  ttype = type();
}

CAstProcedure* CParser::procedureDecl(CAstScope *s)
{
  //
  // procedureDecl ::= "procedure" ident [ formalParam ] ";".
  //
  CToken t;

  // procedureDecl -> "procedure" ...
  Consume(kProc, &t);

  // procedureDecl -> ... ident ...
  CToken e = _scanner->Get();
  if (e.GetType() != tIdent)
    SetError(e, "procedure identifier expected");

  // procedureDecl -> ... [ formalParam ] ...
  const string &procedureName = e.GetName();
  if (s->GetSymbolTable()->FindSymbol(procedureName, sGlobal))
    SetError(e, "procedure re-declaration");

  vector<string> paramNames;
  vector<CAstType*> paramTypes;

  e = _scanner->Peek();
  switch (e.GetType()) {
    case tIdent:
      formalParam(paramNames, paramTypes);
      break;
    case tSemicolon:
      break;
    default:
      SetError(e, "tIdent or tSemicolon expected");
      break;
  }

  // procedureDecl -> ... ";"
  Consume(tSemicolon);

  CSymProc *symbol = new CSymProc(procedureName, CTypeManager::Get()->GetNull());
  AddParameters(symbol, paramNames, paramTypes);

  s->GetSymbolTable()->AddSymbol(symbol); // add procedure symbol to global symbol table

  CAstProcedure *ret = new CAstProcedure(t, procedureName, s, symbol);
  for (int i = 0; i < (int) paramNames.size(); i++) {
    // add parameter symbol to local symbol table
    CSymbol *sym = ret->CreateVar(paramNames[i], paramTypes[i]->GetType());
    ret->GetSymbolTable()->AddSymbol(sym);
  }

  return ret;
}

CAstProcedure* CParser::functionDecl(CAstScope *s)
{
  //
  // funtionDecl ::= "function" ident [ formalParam ] ":" type ";".
  //
  CToken t;

  // functionDecl -> "function" ...
  Consume(kFunc, &t);

  // functionDecl -> ... ident ...
  CToken e = _scanner->Get();
  if (e.GetType() != tIdent)
    SetError(e, "function identifier expected");

  // functionDecl -> ... [ formalParam ] ...
  const string &functionName = e.GetName();
  if (s->GetSymbolTable()->FindSymbol(functionName, sGlobal))
    SetError(e, "function re-declaration");

  vector<string> paramNames;
  vector<CAstType*> paramTypes;
  CAstType* returnType;
  
  e = _scanner->Peek();
  switch (e.GetType()) {
    case tIdent:
      formalParam(paramNames, paramTypes);
      break;
    case tColon:
      break;
    default:
      SetError(e, "tIdent or tColon expected");
      break;
  }

  // functionDecl -> ... ":" type ";"
  Consume(tColon);
  returnType = type();
  Consume(tSemicolon);
  
  CSymProc *symbol = new CSymProc(functionName, returnType->GetType()); 
  AddParameters(symbol, paramNames, paramTypes);
  s->GetSymbolTable()->AddSymbol(symbol); // add function symbol to global symbol table
  
  CAstProcedure *ret = new CAstProcedure(t, functionName, s, symbol);
  for (int i = 0; i < (int) paramNames.size(); i++) {
    // add parameter symbol to local symbol table
    CSymbol *sym = ret->CreateVar(paramNames[i], paramTypes[i]->GetType());
    ret->GetSymbolTable()->AddSymbol(sym);
  }

  return ret;
}

void CParser::formalParam(vector<string> &paramNames, vector<CAstType*> &paramTypes)
{
  // formalParam ::= "(" [ varDeclSequence ] ")".

  // formalParam -> "(" ...
  Consume(tLBrak);
  
  // formalParam -> ... [ varDeclSequence ] ...
  // varDeclSequence ::= varDecl { ";" varDecl }
  
  CToken e = _scanner->Peek();
  if (e.GetType() == tIdent) {
    do {
      vector<string> l;
      CAstType* ttype;
      varDecl(l, ttype, paramNames);

      for (int i = 0; i < (int) l.size() ; i++)
        paramTypes.push_back(ttype);

      if (e.GetType() == tRBrak) break;
      else Consume(tSemicolon);
    } while (_abort);

    // formalParam -> ... ")"
    Consume(tRBrak);
  }

  return;  
}

void CParser::AddParameters(CSymProc *symbol, vector<string> &paramNames, vector<CAstType*> &paramTypes)
{
  int index = 0;

  // add params to symbol 
  for (int i = 0; i < (int) paramNames.size() ; i++) {
    string &paramName = paramNames[i];
    CAstType* paramType = paramTypes[i];

    CSymParam* param = new CSymParam(index++, paramName, paramType->GetType());
    symbol->AddParam(param);
  }

  return;
}

void CParser::subroutineBody(CAstScope *s)
{
  //
  // subroutineBody ::= varDeclaration "begin" statSequence "end".
  //
  varDeclaration(s);
  Consume(kBegin);
  CAstStatement *statseq = statSequence(s);
  Consume(kEnd);

  s->SetStatementSequence(statseq);
}

CAstStatement* CParser::statSequence(CAstScope *s)
{
  //
  // stateSequence ::= [ statement { ";" statement } ].
  // statement ::= assignment | subroutineCall
  //             | ifStatement | whileStatement | returnStatement.
  //
  CAstStatement *head = NULL;
  CAstStatement *tail = NULL;

  do {
    CAstStatement *st = NULL;

    // stateSequence -> ... statement ...
    EToken tt = _scanner->Peek().GetType();
    switch (tt) {
      // statement -> assignment | subroutineCall
      case tIdent:
        // TODO: lookahead 2 tokens
        st = assignment(s);
        break;

      // statement -> ifStatement
      case kIf:
        st = ifStatement(s);
        break;

      // statement -> whileStatement
      case kWhile:
        st = whileStatement(s);
        break;

      // statement -> returnStatement
      case kReturn:
        st = returnStatement(s);
        break;

      default:
        SetError(_scanner->Peek(), "statement expected.");
        break;
    }

    assert(st);

    if (!head) head = st;
    else tail->SetNext(st);

    tail = st;

    tt = _scanner->Peek().GetType();
    if (tt == kEnd) break;

    // stateSequence -> ... ";" ...
    Consume(tSemicolon);
  } while (!_abort);
  
  return head;
}

CAstStatAssign* CParser::assignment(CAstScope *s)
{
  //
  // assignment ::= qualident ":=" expression.
  //
  CToken t;
  CAstDesignator *lhs = NULL;
  
  lhs = qualident(s);
  Consume(tAssign, &t);

  CAstExpression *rhs = expression(s);

  return new CAstStatAssign(t, lhs, rhs);
}

CAstStatCall* CParser::subroutineCall(CAstScope *s)
{
  //
  // subroutineCall ::= ident "(" [ expression { "," expression } ] ")".
  //
  CToken t = _scanner->Peek();
  if (t.GetType() != tIdent)
    SetError(t, "invalid subroutine call");

  CAstFunctionCall *func = functionCall(s);

  return new CAstStatCall(t, func);
}

CAstFunctionCall* CParser::functionCall(CAstScope *s)
{
  //
  // subroutineCall ::= ident "(" [ expression { "," expression } ] ")".
  //
  CToken t;

  // subroutineCall -> ident ...
  Consume(tIdent, &t);
  const CSymProc *symbol = FindSymbol(t.GetValue());
  if (!symbol)
    SetError(t, "undeclared subroutine name");
  CAstFunctionCall *func = new CAstFunctionCall(t, symbol);
  
  // subroutineCall -> ... "(" ...
  Consume(tLParen);

  EToken tt = _scanner->Peek().GetType();
  while (tt != tRParen) {
    // subroutineCall -> ... expression ...
    func->AddArg(expression(s));

    // subroutineCall -> ... "," ...
    EToken tt = _scanner->Peek().GetType();
    if (tt == tComma) Consume(tComma);
  }

  // subroutineCall -> ... ")"
  Consume(tRParen);

  return func;
}

CAstStatIf* CParser::ifStatement(CAstScope *s)
{
  //
  // ifStatement ::= "if" "(" expression ")" "then" stateSequence
  //                 [ "else" stateSequence ] "end".
  //
  CToken t;

  // ifStatement -> "if" "(" expression ")" "then" stateSequence ...
  Consume(kIf);
  Consume(tLParen);
  CAstExpression *cond = expression(s);
  Consume(tRParen);
  Consume(kThen);
  CAstStatement *ifBody = stateSequence(s);

  // ifStatement -> ... [ "else" stateSequence ] "end"
  CAstStatement *elseBody = NULL;
  if (_scanner->Peek().GetType() == kElse) {
    Consume(kElse);
    elseBody = stateSequence(s);
  }
  Consume(kEnd);

  return new CAstStatIf(t, cond, ifBody, elseBody);
}

CAstStatWhile* CParser::whileStatement(CAstScope *s)
{
  //
  // whileStatement ::= "while" "(" expression ")" "do" stateSequence "end".
  //
  CToken t;

  Consume(kWhile);
  Consume(tLParen);
  CAstExpression *cond = expression(s);
  Consume(tRParen);
  Consume(kDo);
  CAstStatement *body = stateSequence(s);
  Consume(kEnd);

  return new CAstStatWhile(t, cond, body);
}

CAstStatReturn* CParser::returnStatement(CAstScope *s)
{
  //
  // returnStatement ::= "return" [ expression ].
  //
  CToken t;
  CAstExpression *expr = NULL;

  // returnStatement -> "return" ...
  Consume(kReturn);

  // returnStatement -> ... expression
  if (false /* TODO: FIRST(expression) && !FOLLOW(expression) */)
    expr = expression(s);

  return new CAstStatReturn(t, s, expr);
}

CAstExpression* CParser::expression(CAstScope* s)
{
  //
  // expression ::= simpleexpr [ relOp simpleexpr ].
  // relOp ::= "=" | "#" | "<" | "<=" | ">" | ">=".
  //
  CToken t;
  EOperation relop;
  CAstExpression *left = NULL, *right = NULL;

  // expression -> simpleexpr ...
  left = simpleexpr(s);

  // expression -> ... relOp simpleexpr ...
  if (_scanner->Peek().GetType() == tRelOp) {
    Consume(tRelOp, &t);
    right = simpleexpr(s);

    if (t.GetValue() == "=") relop = opEqual;
    else if (t.GetValue() == "#") relop = opNotEqual;
    else if (t.GetValue() == "<") relop = opLessThan;
    else if (t.GetValue() == "<=") relop = opLessEqual;
    else if (t.GetValue() == ">") relop = opBiggerThan;
    else if (t.GetValue() == ">=") relop = opBiggerEqual;
    else SetError(t, "invalid relation.");

    return new CAstBinaryOp(t, relop, left, right);
  }
  else
    return left;
}

CAstExpression* CParser::simpleexpr(CAstScope *s)
{
  //
  // simpleexpr ::= ["+"|"-"] term { termOp term }.
  // termOp ::= "+" | "-" | "||".
  //
  CAstExpression *n = NULL;

  // TODO: add unary

  // simpleexpr -> ... term ...
  n = term(s);

  // simpleexpr -> ... termOp term ...
  while (_scanner->Peek().GetType() == tPlusMinus) {
    CToken t;
    CAstExpression *l = n, *r;

    // FIXME
    Consume(tPlusMinus, &t);

    r = term(s);

    n = new CAstBinaryOp(t, t.GetValue() == "+" ? opAdd : opSub, l, r);
  }


  return n;
}

CAstExpression* CParser::term(CAstScope *s)
{
  //
  // term ::= factor { factOp factor }.
  //
  CAstExpression *n = NULL;

  // term -> factor ...
  n = factor(s);

  // term -> ... factOp factor ...
  EToken tt = _scanner->Peek().GetType();
  while ((tt == tMulDiv)) {
    CToken t;
    CAstExpression *l = n, *r;

    // FIXME
    Consume(tMulDiv, &t);

    r = factor(s);

    n = new CAstBinaryOp(t, t.GetValue() == "*" ? opMul : opDiv, l, r);

    tt = _scanner->Peek().GetType();
  }

  return n;
}

CAstExpression* CParser::factor(CAstScope *s)
{
  //
  // factor ::= qualident | number | boolean | char | string
  //          | "(" expression ")" | subroutineCall | "!" factor.
  //
  CToken t;
  EToken tt = _scanner->Peek().GetType();
  CAstExpression *unary = NULL, *n = NULL;

  switch (tt) {
    // factor -> number
    case tNumber:
      n = number();
      break;

    // factor -> boolean
    case kBool:
      n = boolean();
      break;

    // factor -> char
    case tChar:
      n = character();
      break;

    // factor -> string
    case tString:
      n = stringConst(s);
      break;

    // factor -> "(" expression ")"
    case tLParen:
      Consume(tLParen);
      n = expression(s);
      Consume(tRParen);
      break;

    // factor -> subroutineCall
    case tIdent:
      n = subroutineCall(s);
      break;

    // factor -> "!" factor
    case tNot:
      Consume(tNot, &t);
      CAstExpression *e = factor(s);
      n = new CAstUnaryOp(t, opNot, e);
      break;

    default:
      SetError(_scanner->Peek(), "factor expected.");
      break;
  }

  return n;
}

CAstType* CPaser::type(void)
{
  //
  // type ::= basetype | type "[" [ number ] "]".
  // basetype ::= "boolean" | "char" | "integer".
  //
  CToken t;
  const CToken *type = NULL;

  EToken tt = _scanner->Peek().GetType();
  switch (tt) {
    // basetype -> "boolean"
    case kBool:
      type = CTypeManager::Get()->GetBool();
      break;

    // basetype -> "char"
    case tChar:
      type = CTypeManager::Get()->GetChar();
      break;

    // basetype -> "integer"
    case tNumber:
      type = CTypeManager::Get()->GetInt();
      break;

    default:
      SetError(_scanner->Peek(), "invalid type");
      break;
  }

  bool isOpenArray = false;
  while (tt == tLBrak) {
    Consume(tLBrak);

    // type -> ... "[" number "]" ...
    if (_scanner->Peek().GetType() != tRBrak) {
      CAstConstant *n = number();
      type = CTypeManager::Get()->GetArray(n->GetValue(), type);
    }
    // type -> ... "[" "]" ...
    else {
      if (!isOpenArray) {
        type = CTypeManager::Get()->GetPointer(type);
        isOpenArray = true;
      }

      type = CTypeManager::Get()->GetArray(OPEN, type);
    }

    Consume(tRBrak);
  }

  return new CAstType(t, type);
}

CAstDesignator* CPaser::qualident(CAstScope *s)
{
  //
  // qualident ::= ident { "[" expression "]" }
  //
  CToken t;

  // qualident -> ident ...
  CAstDesignator *id = ident(s);

  // qualident -> ... { "[" expression "]" }
  EToken tt = _scanner->Peek().GetType();
  while (tt == tLBrak) {
    Consume(tLBrak);
    ((CAstArrayDesignator) id)->AddIndex(expression(s));
    Consume(tRBrak);
  }

  return id;
}

CAstDesignator* CParser::ident(CAstScope *s)
{
  //
  // ident ::= letter { letter | digit }.
  //
  CToken t;

  Consume(tIdent, &t);

  CSymtab *symtab = s.GetSymbolTable();
  EScope scope = s->parent ? sLocal : sGlobal;
  const CSymbol *symbol = symtab.FindSymbol(t.GetValue(), scope);

  if (!symbol) SetError(t, "undeclared variable.");

  return new CAstDesignator(t, symbol);
}

CAstConstant* CParser::number(void)
{
  //
  // number ::= digit { digit }.
  //
  CToken t;

  Consume(tNumber, &t);

  errno = 0;
  long long v = strtoll(t.GetValue().c_str(), NULL, 10);
  if (errno != 0) SetError(t, "invalid number.");

  return new CAstConstant(t, CTypeManager::Get()->GetInt(), v);
}

CAstConstant* CParser::boolean(void)
{
  //
  // boolean ::= "true" | "false".
  //
  CToken t;
  long long b = 0;

  EToken tt = _scanner->Get().GetType();
  switch (tt) {
    case kTrue:
      b = !b;
    case kFalse:
      break;
    default:
      SetError(t, "invalid boolean");
      break;
  }

  return new CAstConstant(t, CTypeManager::Get()->GetBool(), b);
}

CAstConstant* CParser::character(void)
{
  //
  // char ::= "'" character "'".
  //
  CToken t;

  Consume(tChar, &t);
  long long ch = (long long) (t.GetValue()[0]);

  return new CAstConstant(t, CTypeManager::Get()->GetChar(), ch);
}

CAstStringConstant* CParser::stringConst(CAstScope* s)
{
  //
  // string := '"' { character } '"'.
  //
  CToken t;

  Consume(tString, &t);

  return new CAstStringConstant(t, t.GetValue(), s);
}

