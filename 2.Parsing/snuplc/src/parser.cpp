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
  // module ::= statSequence  ".".
  //
  //----------------------------------------------------------------------------
  // module ::= "module" ident ";" varDeclaration { subroutineDecl }
  //            "begin" stateSequence "end" ident ".".
  // varDeclaration ::= [ "var" varDeclSequence ";" ].
  // varDeclSequence ::= varDecl { ";" varDecl }.
  // varDecl ::= ident { "," ident } ":" type.
  //
  CToken t;
  CAstModule *m = new CAstModule(t, "module");
  CAstDesignator *id = NULL;
  CAstType *var = NULL;
  CAstProcedure *subs = NULL;
  CAstStatement *statseq = NULL;

  Consume(kModule);
  id = ident();
  Consume(tSemicolon);

  EToken tt = _scanner->Peek().GetType();
  if (tt == kVar) {
    Consume(kVar);

    do {
      CAstType ttype;
      // list l;

      while (true) {
        CToken e = _scanner->Get();
        if (e.GetType() != tIdent) /* ERROR */;

        //
        // list l << e.GetValue();
        //

        tt = _scanner->Peek().GetType();
        if (tt == tColon) break;
        if (tt != tComma) /* ERROR */;
      }

      ttype = type();

      //
      // for e in list l
      //   m->CreateVar(e, ttype);
      //
      
      Consume(tSemicolon);
    } while (/* FOLLOW(this) == FIRST(subroutineDecl) || kBegin */)
  }

  tt = _scanner->Peek().GetType();
  while (tt != kBegin) {
    //
    // subs = subroutineDecl(m) ...?
    //
    tt = _scanner->Peek().GetType();
  }

  Consume(kBegin);
  statseq = statSequence(m);
  Consume(kEnd);
  if (id->GetSymbol() != ident()->GetSymbol()) /* ERROR */;
  Consume(tDot);

  m->SetStatementSequence(statseq);

  return m;
}

CAstStatement* CParser::statSequence(CAstScope *s)
{
  //
  // statSequence ::= [ statement { ";" statement } ].
  // statement ::= assignment.
  // FIRST(statSequence) = { tNumber }
  // FOLLOW(statSequence) = { tDot }
  //
  //----------------------------------------------------------------------------
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

    switch (tt) {
      // statement -> assignment | subroutineCall
      // TODO: left factoring...?
      case tNumber:
        st = assignment(s);
        break;

      case kIf:
        st = /* ifStatement(s) */;
        break;

      case kWhile:
        st = /* whileStatement(s) */;
        break;

      case kReturn:
        st = /* returnStatement(s) */;
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

    Consume(tSemicolon);
  } while (!_abort);
  

  return head;
}

CAstStatAssign* CParser::assignment(CAstScope *s)
{
  //
  // assignment ::= number ":=" expression.
  //
  //----------------------------------------------------------------------------
  // assignment ::= qualident ":=" expression.
  //
  CToken t;

  CAstConstant *lhs = number();
  Consume(tAssign, &t);

  CAstExpression *rhs = expression(s);

  return new CAstStatAssign(t, lhs, rhs);
}

CAstExpression* CParser::expression(CAstScope* s)
{
  //
  // expression ::= simpleexpr [ relOp simpleexpr ].
  //
  CToken t;
  EOperation relop;
  CAstExpression *left = NULL, *right = NULL;

  left = simpleexpr(s);

  if (_scanner->Peek().GetType() == tRelOp) {
    Consume(tRelOp, &t);
    right = simpleexpr(s);

    if (t.GetValue() == "=")       relop = opEqual;
    else if (t.GetValue() == "#")  relop = opNotEqual;
    else SetError(t, "invalid relation.");

    return new CAstBinaryOp(t, relop, left, right);
  } else {
    return left;
  }
}

CAstExpression* CParser::simpleexpr(CAstScope *s)
{
  //
  // simpleexpr ::= term { termOp term }.
  //
  //----------------------------------------------------------------------------
  // simpleexpr ::= ["+"|"-"] term { termOp term }.
  //
  CAstExpression *n = NULL;

  n = term(s);

  while (_scanner->Peek().GetType() == tPlusMinus) {
    CToken t;
    CAstExpression *l = n, *r;

    Consume(tPlusMinus, &t);

    r = term(s);

    n = new CAstBinaryOp(t, t.GetValue() == "+" ? opAdd : opSub, l, r);
  }


  return n;
}

CAstExpression* CParser::term(CAstScope *s)
{
  //
  // term ::= factor { ("*"|"/") factor }.
  //
  //----------------------------------------------------------------------------
  // term ::= factor { ("*"|"/"|"&&") factor }.
  //
  CAstExpression *n = NULL;

  n = factor(s);

  EToken tt = _scanner->Peek().GetType();

  while ((tt == tMulDiv)) {
    CToken t;
    CAstExpression *l = n, *r;

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
  // factor ::= number | "(" expression ")"
  //
  // FIRST(factor) = { tNumber, tLBrak }
  //
  //----------------------------------------------------------------------------
  // factor ::= qualident | number | boolean | char | string
  //          | "(" expression ")" | subroutineCall | "!" factor.
  //
  CToken t;
  EToken tt = _scanner->Peek().GetType();
  CAstExpression *unary = NULL, *n = NULL;

  switch (tt) {
    // factor ::= number
    case tNumber:
      n = number();
      break;

    // factor ::= "(" expression ")"
    case tLParens:
      Consume(tLParens);
      n = expression(s);
      Consume(tRParens);
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

  EToken tt = _scanner->Get().GetType();
  switch (tt) {
    case kBool:
      break;
    case kChar:
      break;
    case kNumber:
      break;
    default:
      break;
  }

  return new CAstType(t, NULL);
}

CAstDesignator* CParser::ident(void)
{
  //
  // ident ::= letter { letter | digit }.
  //
  CToken t;

  Consume(tIdent, &t);

  return new CAstDesignator(t, /* Symbol */);
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
  bool b = false;

  EToken tt = _scanner->Get().GetType();
  switch (tt) {
    case kTrue:
      b = true;
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

  return new CAstConstant(t, CTypeManager::Get()->GetChar(), t.GetValue());
}

CAstStringConstant* CParser::string(CAstScope* s)
{
  //
  // string := '"' { character } '"'.
  //
  CToken t;

  Consume(tString, &t);

  return new CAstStringConstant(t, t.GetValue(), s);
}
