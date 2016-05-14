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
      if (!_module->TypeCheck(&t, &msg)) SetError(t, msg);
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
  CSymProc *fun;

  // function DIM(array: pointer to array; dim: integer): integer
  fun = new CSymProc("DIM", tm->GetInt());
  fun->AddParam(new CSymParam(0, "arr", tm->GetPointer(tm->GetNull())));
  fun->AddParam(new CSymParam(1, "dim", tm->GetInt()));
  s->AddSymbol(fun);

  // function DOFS(array: pointer to array): integer;
  fun = new CSymProc("DOFS", tm->GetInt());
  fun->AddParam(new CSymParam(0, "arr", tm->GetPointer(tm->GetNull())));
  s->AddSymbol(fun);

  // function ReadInt() : integer;
  fun = new CSymProc("ReadInt", tm->GetInt());
  s->AddSymbol(fun);

  // procedure WriteInt(i: integer);
  fun = new CSymProc("WriteInt", tm->GetNull());
  fun->AddParam(new CSymParam(0, "i", tm->GetInt()));
  s->AddSymbol(fun);
  
  // procedure WriteChar(c: char);
  fun = new CSymProc("WriteChar", tm->GetNull());
  fun->AddParam(new CSymParam(0, "c", tm->GetChar()));
  s->AddSymbol(fun);

  // procedure WriteStr(string: char[]);
  fun = new CSymProc("WriteStr", tm->GetNull());
  fun->AddParam(new CSymParam(0, "str", tm->GetPointer(tm->GetArray(CArrayType::OPEN, tm->GetChar()))));
  s->AddSymbol(fun);

  // procedure WriteLn();
  fun = new CSymProc("WriteLn", tm->GetNull());
  s->AddSymbol(fun);
}

CAstModule* CParser::module(void)
{
  //
  // module ::= "module" ident ";" varDeclaration { subroutineDecl }
  //            "begin" stateSequence "end" ident ".".
  // 
  CToken t;

  // module -> "module" ident ";" ...
  Consume(kModule, &t);
  CToken tModuleIdent = _scanner->Get();
  if (tModuleIdent.GetType() != tIdent)
    SetError(tModuleIdent, "module identifier expected");
  Consume(tSemicolon);

  CAstModule *m = new CAstModule(t, tModuleIdent.GetValue());
  InitSymbolTable(m->GetSymbolTable());

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
    if (t.GetType() != tIdent || t.GetValue() != sub->GetName()) {
      string msg = "subroutine identifier mismatched (\"" + sub->GetName()
                    + "\" != \"" + t.GetValue() + "\")";
      SetError(t, msg);
    }
    Consume(tIdent);
    Consume(tSemicolon);

    tt = _scanner->Peek().GetType();
  }

  // module -> ... "begin" statSequence "end" ...
  Consume(kBegin);
  CAstStatement *statseq = statSequence(m);
  m->SetStatementSequence(statseq);
  Consume(kEnd);

  // module -> ... ident "."
  CToken tModuleIdentClose = _scanner->Get();
  if (tModuleIdent.GetValue() != tModuleIdentClose.GetValue()) {
    string msg = "module identifier not matched (\"" + tModuleIdent.GetValue()
                + "\" != \"" + tModuleIdentClose.GetValue() + "\")";
    SetError(tModuleIdentClose, msg);
  }
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
        CSymbol *var = s->CreateVar(str, ttype->GetType());
        s->GetSymbolTable()->AddSymbol(var);
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

  // vardecl -> ident { "," ident } ":" ...
  varDeclInternal(vars, allVars);

  // varDecl -> ... type
  ttype = type(false);
}

void CParser::varDeclParam(vector<string> &vars, CAstType* &ttype, vector<string> &allVars)
{
  //
  // varDecl ::= ident { "," ident } ":" type.
  //

  // vardecl -> ident { "," ident } ":" ...
  varDeclInternal(vars, allVars);

  // varDecl -> ... type
  ttype = type(true);
}

void CParser::varDeclInternal(vector<string> &vars, vector<string> &allVars)
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
        SetError(e, "re-declaration variable \"" + e.GetValue() + "\"");
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
  const string &procedureName = e.GetValue();
  if (s->GetSymbolTable()->FindSymbol(procedureName, sGlobal))
    SetError(e, "re-declaration procedure \"" + procedureName + "\"" );

  vector<string> paramNames;
  vector<CAstType*> paramTypes;

  e = _scanner->Peek();
  switch (e.GetType()) {
    case tLParen:
      formalParam(paramNames, paramTypes);
      break;
    case tSemicolon:
      break;
    default:
      SetError(e, "\"(\" or \";\" expected");
      break;
  }

  // procedureDecl -> ... ";"
  Consume(tSemicolon);

  CSymProc *symbol =
    new CSymProc(procedureName, CTypeManager::Get()->GetNull());
  s->GetSymbolTable()->AddSymbol(symbol);

  CAstProcedure *ret = new CAstProcedure(t, procedureName, s, symbol);
  AddParameters(ret, symbol, paramNames, paramTypes);

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
  const string &functionName = e.GetValue();
  if (s->GetSymbolTable()->FindSymbol(functionName, sGlobal))
    SetError(e, "re-declaration function \"" + functionName + "\"");

  vector<string> paramNames;
  vector<CAstType*> paramTypes;
  CAstType* returnType;
  
  e = _scanner->Peek();
  switch (e.GetType()) {
    case tLParen:
      formalParam(paramNames, paramTypes);
      break;
    case tColon:
      break;
    default:
      SetError(e, "\"(\" or \":\" expected");
      break;
  }

  // functionDecl -> ... ":" type ";"
  Consume(tColon);
  returnType = type(false);
  if (returnType->GetType()->IsArray())
    SetError(returnType->GetToken(), "function cannot return array type.");
  Consume(tSemicolon);
  
  CSymProc *symbol = new CSymProc(functionName, returnType->GetType()); 
  s->GetSymbolTable()->AddSymbol(symbol);
  
  CAstProcedure *ret = new CAstProcedure(t, functionName, s, symbol);
  AddParameters(ret, symbol, paramNames, paramTypes);

  return ret;
}

void CParser::formalParam
  (vector<string> &paramNames, vector<CAstType*> &paramTypes)
{
  //
  // formalParam ::= "(" [ varDeclSequence ] ")".
  //

  // formalParam -> "(" ...
  Consume(tLParen);
  
  // formalParam -> ... [ varDeclSequence ] ...
  // varDeclSequence -> varDecl { ";" varDecl }
  CToken e = _scanner->Peek();
  if (e.GetType() == tIdent) {
    do {
      vector<string> l;
      CAstType *ttype;
      varDeclParam(l, ttype, paramNames);

      for (int i = 0; i < (int) l.size() ; i++) {
        if (ttype->GetType()->IsArray()) {
          const CPointerType *ptrtype =
            CTypeManager::Get()->GetPointer(ttype->GetType());
          ttype = new CAstType(ttype->GetToken(), ptrtype);
        }
        paramTypes.push_back(ttype);
      }

      e = _scanner->Peek();
      if (e.GetType() == tRParen) 
        break;
      else Consume(tSemicolon);
    } while (!_abort);
  }

  // formalParam -> ... ")";
  Consume(tRParen);
}

void CParser::AddParameters
  (CAstScope *s, CSymProc *symbol, vector<string> &paramNames, vector<CAstType*> &paramTypes)
{
  int cnt = 0;

  // create parameter symbols and add them to symbol table for subroutine
  for (int i = 0; i < (int) paramNames.size() ; i++) {
    string &paramName = paramNames[i];
    CAstType* paramType = paramTypes[i];

    CSymParam* param = new CSymParam(cnt++, paramName, paramType->GetType());
    symbol->AddParam(param);
    s->GetSymbolTable()->AddSymbol(param);
  }
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

  CToken tt = _scanner->Peek();
  while (!_abort && tt.GetType() != kEnd && tt.GetType() != kElse) {
    CAstStatement *st = NULL;

    // stateSequence -> ... statement ...
    tt = _scanner->Peek();
    switch (tt.GetType()) {
      // statement -> assignment | subroutineCall
      case tIdent:
        {
          const CSymbol *sym = s->GetSymbolTable()->FindSymbol(tt.GetValue(), sLocal);
          if (!sym) sym = s->GetSymbolTable()->FindSymbol(tt.GetValue(), sGlobal);
          if (!sym) SetError(tt, "undeclared variable \"" + tt.GetValue() + "\"");

          ESymbolType stype = sym->GetSymbolType();
          if (stype == stProcedure) st = subroutineCall(s);
          else st = assignment(s);
        }
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

    tt = _scanner->Peek();
    if (tt.GetType() == kEnd || tt.GetType() == kElse) break;

    // stateSequence -> ... ";" ...
    Consume(tSemicolon);
  }

  return head;
}

CAstStatAssign* CParser::assignment(CAstScope *s)
{
  //
  // assignment ::= qualident ":=" expression.
  //
  CToken t;

  // assignment -> qualident ...
  CAstDesignator *lhs = qualident(s);

  // assignment -> ... ":=" ...
  Consume(tAssign, &t);

  // assignment -> ... expression
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
  const CSymbol *symbol = s->GetSymbolTable()->FindSymbol(t.GetValue(), sGlobal);
  if (!symbol)
    SetError(t, "undeclared subroutine name");

  CAstFunctionCall *func =
    new CAstFunctionCall(t, dynamic_cast<const CSymProc*>(symbol));
  
  // subroutineCall -> ... "(" ...
  Consume(tLParen);

  while (_scanner->Peek().GetType() != tRParen) {
    // subroutineCall -> ... expression ...
    func->AddArg(addressExpression(s));

    // subroutineCall -> ... "," ...
    if (_scanner->Peek().GetType() == tComma)
        Consume(tComma);
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
  Consume(kIf, &t);
  Consume(tLParen);
  CAstExpression *cond = expression(s);
  Consume(tRParen);
  Consume(kThen);
  CAstStatement *ifBody = statSequence(s);

  // ifStatement -> ... [ "else" stateSequence ] "end"
  CAstStatement *elseBody = NULL;
  if (_scanner->Peek().GetType() == kElse) {
    Consume(kElse);
    elseBody = statSequence(s);
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

  Consume(kWhile, &t);
  Consume(tLParen);
  CAstExpression *cond = expression(s);
  Consume(tRParen);
  Consume(kDo);
  CAstStatement *body = statSequence(s);
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
  Consume(kReturn, &t);

  // returnStatement -> ... expression
  EToken tt = _scanner->Peek().GetType();
  if (tt != kEnd && tt != tSemicolon)
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

CAstExpression* CParser::addressExpression(CAstScope* s)
{
  //
  // addressExpression ::= "&" expression
  // implicit type casting: array to pointer
  //
  CToken t = _scanner->Peek();
  CAstExpression *e = expression(s);

  if (!e->GetType())
    SetError(t, "NULL type");
  else if (e->GetType()->IsArray())
    return new CAstSpecialOp(t, opAddress, e, NULL);

  return e;
}

CAstExpression* CParser::simpleexpr(CAstScope *s)
{
  //
  // simpleexpr ::= [ "+" | "-" ] term { termOp term }.
  // termOp ::= "+" | "-" | "||".
  //
  CToken t;

  // simpleexpr -> [ "+" | "-" ] ...
  bool hasUnary = false;
  if (_scanner->Peek().GetType() == tPlusMinus) {
    hasUnary = true;
    Consume(tPlusMinus, &t);
  }

  // simpleexpr -> ... term ...
  CAstExpression *n = term(s);
  if (hasUnary)
    n = new CAstUnaryOp(t, t.GetValue() == "+" ? opPos : opNeg, n);

  for (CToken tt = _scanner->Peek(); ; tt = _scanner->Peek()) {
    // termOp -> "+" | "-" | "||"
    EOperation eop;
    if (tt.GetValue() == "||") {
      Consume(tAndOr, &tt);
      eop = opOr;
    }
    else if (tt.GetType() == tPlusMinus) {
      Consume(tPlusMinus, &tt);
      eop = tt.GetValue() == "+" ? opAdd : opSub;
    }
    else break;

    // simpleexpr -> ... term ...
    CAstExpression *l = n, *r = term(s);

    n = new CAstBinaryOp(tt, eop, l, r);
  }

  return n;
}

CAstExpression* CParser::term(CAstScope *s)
{
  //
  // term ::= factor { factOp factor }.
  //

  // term -> factor ...
  CAstExpression *n = factor(s);

  for (CToken t = _scanner->Peek(); ; t = _scanner->Peek()) {
    // factOp -> "*" | "/" | "&&"
    EOperation eop;
    if (t.GetValue() == "&&") {
      Consume(tAndOr, &t);
      eop = opAnd;
    }
    else if (t.GetType() == tMulDiv) {
      Consume(tMulDiv, &t);
      eop = t.GetValue() == "*" ? opMul : opDiv;
    }
    else break;

    // term -> ... facter ...
    CAstExpression *l = n, *r = factor(s);

    n = new CAstBinaryOp(t, eop, l, r);
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
  CToken tt = _scanner->Peek();
  CAstExpression *unary = NULL, *n = NULL;

  switch (tt.GetType()) {
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

    // factor -> qualident | subroutineCall
    case tIdent:
      {
        const CSymbol* sym = s->GetSymbolTable()->FindSymbol(tt.GetValue());
        if (sym) {
          ESymbolType stype = sym->GetSymbolType();
          if (stype == stProcedure)
            n = functionCall(s);
          else
            n = qualident(s);
        }
        else 
          SetError(tt, "undeclared variable \"" + tt.GetValue() + "\"");
      }
      break;

    // factor -> "!" factor
    case tNot:
      {
        Consume(tNot, &t);
        CAstExpression *e = factor(s);
        n = new CAstUnaryOp(t, opNot, e);
        break;
      }

    default:
      SetError(_scanner->Peek(), "factor expected.");
      break;
  }

  return n;
}

CAstType* CParser::type(bool isParam)
{
  //
  // type ::= basetype | type "[" [ number ] "]".
  // basetype ::= "boolean" | "char" | "integer".
  //
  CToken t;
  const CType *ttype = NULL;
  vector<long long> index;

  // varDecl -> ... type
  // functionDecl -> ... type ...
  Consume(kType, &t);

  // type -> basetype
  // basetype -> "boolean" | "char" | "integer"
  if (t.GetValue() == "boolean")
    ttype = CTypeManager::Get()->GetBool();
  else if (t.GetValue() == "char")
    ttype = CTypeManager::Get()->GetChar();
  else if (t.GetValue() == "integer")
    ttype = CTypeManager::Get()->GetInt();
  else
    SetError(t, "invalid base type: " + t.GetValue());

  while (_scanner->Peek().GetType() == tLBrak) {
    // type -> ... "[" ...
    Consume(tLBrak);

    // type -> ... number ...
    if (_scanner->Peek().GetType() != tRBrak) {
      long long indexSize = number()->GetValue();

      if (indexSize < 0 || indexSize >= (1LL << 31))
        SetError(t, "declaring array out of index : " + indexSize);
      else
        index.push_back(indexSize);
    }
    else if (!isParam)
      SetError(t, "open index when declare array variable is not allowed.");
    else
      index.push_back(CArrayType::OPEN);


    // type -> ... "]"
    Consume(tRBrak);
  }

  // construct array type
  if (!index.empty()) {
    const CType* innertype = ttype;
    for(int i = (int) index.size() - 1; i >= 0; i--)
      innertype = CTypeManager::Get()->GetArray(index[i], innertype);
    ttype = innertype;
  }

  return new CAstType(t, ttype);
}

CAstDesignator* CParser::qualident(CAstScope *s)
{
  //
  // qualident ::= ident { "[" expression "]" }
  //
  CToken t;

  // qualident -> ident ...
  CAstDesignator *id = ident(s);

  t = _scanner->Peek();
  if (t.GetType() != tLBrak)
    return id;

  const CToken saveToken = id->GetToken();
  const CSymbol* saveSymbol = id->GetSymbol();
  CAstArrayDesignator *arrayId = new CAstArrayDesignator(saveToken, saveSymbol);

  if (!arrayId->GetType()->IsArray()) {
    SetError(t, "access with index which is actually not an array");
    return id;
  }

  free(id);

  while (t.GetType() == tLBrak) {
    // qualident -> ... "[" ...
    Consume(tLBrak);

    // qualident -> ... expression ...
    arrayId->AddIndex(expression(s));

    // qualident -> ... "]" ...
    Consume(tRBrak);
    t = _scanner->Peek();
  }

  arrayId->IndicesComplete();
  return arrayId;
}

CAstDesignator* CParser::ident(CAstScope *s)
{
  //
  // ident ::= letter { letter | digit }.
  //
  CToken t;

  Consume(tIdent, &t);

  CSymtab *symtab = s->GetSymbolTable();
  const CSymbol *symbol = symtab->FindSymbol(t.GetValue(), sLocal);
  if (!symbol) symbol = symtab->FindSymbol(t.GetValue(), sGlobal);
  if (!symbol) SetError(t, "undeclared variable \"" + t.GetValue() + "\"");

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
  if (errno != 0) SetError(t, "invalid number. (" + t.GetValue() + ")");

  long long absv = (v > 0 ? v : (-v));
  if (absv > (1LL << 31)) SetError(t, "invalid number. (" + t.GetValue() + ")");
  // INT_MAX + 1 is handled by TypeCheck

  return new CAstConstant(t, CTypeManager::Get()->GetInt(), v);
}

CAstConstant* CParser::boolean(void)
{
  //
  // boolean ::= "true" | "false".
  //
  CToken t;

  Consume(kBool, &t);
  long long b = (t.GetValue() == "false") ? 0 : 1;

  return new CAstConstant(t, CTypeManager::Get()->GetBool(), b);
}

CAstConstant* CParser::character(void)
{
  //
  // char ::= "'" character "'".
  //
  CToken t;

  Consume(tChar, &t);
  long long ch = (long long) CToken::unescape(t.GetValue())[0];

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

