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
  // varDeclaration ::= [ "var" varDeclSequence ";" ].
  // varDeclSequence ::= varDecl { ";" varDecl }.
  // varDecl ::= ident { "," ident } ":" type.
  // subroutineDecl ::= (procedureDecl | functionDecl) subroutineBody ident ";".
  //

  // module -> "module" ident ";" ...
  Consume(kModule);
  CToken tModuleIdent = _scanner->Get();
  if (tModuleIdent.GetType() != tIdent)
    SetError(tModuleIdent, "module identifier expected");
  Consume(tSemicolon);

  CToken dummy;
  CAstModule *m = new CAstModule(dummy, tModuleIdent.GetName());
  CSymtab *symtab = m->GetSymbolTable();

  // module -> ... varDeclaration ...
  varDeclaration(m);

  // module -> ... { subroutineDecl } ...
  CAstProcedure *sub = NULL;
  Etoken tt = _scanner->Peek().GetType();
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

    // subroutineDecl -> ... subroutineBody ident ";"
    subroutineBody(sub); // TODO

    CToken t = _scanner->Peek();
    if (t.GetType() != tIdent || t.GetName() != sub->GetName())
      SetError(t, "module identifier not matched");
    Consume(tIdent);
    Consume(tSemicolon);
  }

  // module -> ... "begin" statSequence "end" ...
  Consume(kBegin);
  CAstStatement *statseq = statSequence(m);
  Consume(kEnd);
  m->SetStatementSequence(statseq);

  // module -> ... ident "."
  CToken tModuleIdentClose = _scanner->Get();
  if (tModuleIdent.GetValue() != tModuleIdentClose.GetValue())
    SetError(tModuleIdentClose, "module identifier not matched");
  Consume(tDot);

  return m;
}

void CParser::varDeclaration(CAstScope *s) {
  Etoken tt;

  // varDeclaration -> "var" ...
  tt = _scanner->Peek().GetType();
  if (tt == kVar) {
    Consume(kVar);

    // varDeclaration -> ... varDeclSequence ...
    // varDeclSequence -> varDecl { ";" varDecl }.
    do {
      CAstType *ttype;
      vector<string> l = varDecl(ttype);

      for (const auto &str : l) {
        CSymbol *global_var = s->CreateVar(str, ttype->GetType());
        s->GetSymbolTable()->AddSymbol(global_var);
      }
      
      Consume(tSemicolon);
      tt = _scanner->Peek().GetType();
    } while (tt != kProc && tt != kFunc && tt != kBegin);
  }

  return;
}

vector<string> CParser::varDecl(CAstType *ttype)
{
  vector<string> l;

  // varDecl -> ident { "," ident } ":" type.
  
  while (!_abort) {
    CToken e = _scanner->Get();
    if (e.GetType() != tIdent)
      SetError(e, "invalid identifier");
    
    l.push_back(e.GetValue());
    
    e = _scanner->Peek();
    if (e.GetType() == tColon) break;
    else if (e.GetType() != tComma)
      SetError(e, "',' expected");

    Consume(tComma);
  }

  Consume(tColon);
  ttype = type();

  return l;
}

CAstProcedure* CParser::procedureDecl(CAstScope *s)
{
  //
  // procedureDecl ::= "procedure" ident [ formalParam ] ";".
  // formalParam ::= "(" [ varDeclSequence ] ")".
  //

  CToken t;
  Consume(kProc, &t);
  CToken e = _scanner->Get();
  if (e.GetType() != tIdent)
    SetError(e, "procedure identifier expected");

  const string &procedureName = e.GetName();
  CSymProc *symbol = formalParam(procedureName);
  Consume(tSemicolon);

  return new CAstProcedure(t, procedureName, s, symbol);
}

CAstProcedure* CParser::functionDecl(CAstScope *s)
{
  //
  // funtionDecl ::= "function" ident [ formalParam ] ":" type ";".
  // formalParam ::= "(" [ varDeclSequence ] ")".
  //
  
  CToken t;
  Consume(kFunc, &t);
  CToken e = _scanner->Get();
  if (e.GetType() != tIdent)
    SetError(e, "function identifier expected");

  const string &functionName = e.GetName();
  CSymProc *symbol = formalParam(functionName);
  Consume(tSemicolon);

  return new CAstProcedure(t, functionName, s, symbol);
}

CSymProc* CParser::formalParam(const string &name)
{
  CTypeManager *tm = CTypeManager::Get();
  CAstType *ttype;
  vector<vector<string> > paramNamesSet;
  vector<CAstType*> paramTypes;

  CToken e = _scanner->Peek();
  if (e.GetType() == tLBrak) {
    Consume(tLBrak);

    e = _scanner->Peek();
    if (e.GetType() != tRBrak) {
      do {
        CAstType *paramType;
        vector<string> paramNames = varDecl(paramType);

        paramNamesSet.push_back(paramNames);
        paramTypes.push_back(paramType);

        e = _scanner->Peek();
        if (e.GetType() == tRBrak)
          break;
        else 
          Consume(tSemicolon);
      } while (_abort);
    }

    Consume(tRBrak);
  }

  e = _scanner->Peek();
  if (e.GetType() == tColon) {
    Consume(tColon);
    ttype = type();
  }
  Consume(tSemicolon);

  // All CTypes must be managed by CTypeManager,
  // since all constructors in CType class are "protected".
  const CType *return_type = (ttype == NULL ? (tm->GetNull()) : (ttype->GetType()));
  CSymProc *symbol = new CSymProc(name, return_type);

  // add params to symbol
  for (int i = 0, j = 0 ; i < (int) paramNamesSet.size() ; i++) {
    vector<string> &paramNames = paramNamesSet[i];
    CAstType* paramType = paramTypes[i];
    
    for (const auto &str : paramNames) {
      CSymParam* param = new CSymParam(j++, str, paramType->GetType());
      symbol->AddParam(param);
    } 
  }

  return symbol;
}

void CParser::subroutineBody(CAstScope *s)
{
  //
  // subroutineBody ::= varDeclaration "begin" statSequence "end".
  // varDeclaration ::= [ "var" varDeclSequence ";" ].
  // varDeclSequence ::= varDecl { ";" varDecl }.
  // varDecl ::= ident { "," ident } ":" type.
  //

  varDeclaration(s);
  Consume(kBegin);
  CAstStatement *statseq = statSequence(s);
  Consume(kEnd);

  s->SetStatementSequence(statseq);

  return;
}

CAstStatement* CParser::statSequence(CAstScope *s)
{
  //
  // stateSequence ::= [ statement { ";" statement } ].
  // statement ::= assignment | subroutineCall
  //             | ifStatement | whileStatement | returnStatement.
  //
  CAstStatement *head = NULL;

  EToken tt = _scanner->Peek().GetType();
  CAstStatement *tail = NULL;

  do {
    CToken t;
    EToken tt = _scanner->Peek().GetType();
    CAstStatement *st = NULL;

    // stateSequence -> ... statement ...
    //
    switch (tt) {
      // statement -> assignment | subroutineCall
      case tNumber:
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
  
  lhs = qualident();
  Consume(tAssign, &t);

  CAstExpression *rhs = expression(s);

  return new CAstStatAssign(t, lhs, rhs);
}

CAstStatCall* CParser::subroutineCall(CAstScope *s)
{
  //
  // subroutineCall ::= ident "(" [ expression { "," expression } ] ")".
  //
  CToken t;

  return new CAstStatCall(t, new CAstFunctionCall(t, symbol));
}

CAstStatIf* CParser::ifStatement(CAstScope *s)
{
  //
  // ifStatement ::= "if" "(" expression ")" "then" stateSequence
  //                 [ "else" stateSequence ] "end".
  //

  return NULL;
}

CAstStatWhile* CParser::whileStatement(CAstScope *s)
{
  //
  // whileStatement ::= "while" "(" expression ")" "do" stateSequence "end".
  //

  return NULL;
}

CAstStatReturn* CParser::returnStatement(CAstScope *s)
{
  //
  // returnStatement ::= "return" [ expression ].
  //

  return NULL;
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
    case tLParens:
      Consume(tLParens);
      n = expression(s);
      Consume(tRParens);
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
      cout << "got " << _scanner->Peek() << endl;
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

  // type -> base type | type ...
  EToken tt = _scanner->Get().GetType();

  // type -> ... "[" [ number ] "]"
  while (!_abort) {
    Consume(tLBrak);

    // TODO

    Consume(tRBrak);
  }

  return new CAstType(t, NULL); // FIXME
}

CAstDesignator* CPaser::qualident(CAstScope *s)
{
  //
  // qualident ::= ident { "[" expression "]" }
  //
  CToken t;

  // qualident -> ident ...
  CAstDesignator* id = ident(s);

  // qualident -> ... "[" expression "]" ...
  while (!_abort) {
    Consume(tLBrak);

    // TODO

    Consume(tRBrak);
  }

  return id; // FIXME
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
