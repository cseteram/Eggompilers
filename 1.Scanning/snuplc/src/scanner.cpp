//------------------------------------------------------------------------------
/// @brief SnuPL/1 scanner
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2014/09/10 Bernhard Egger assignment 1: scans SnuPL/-1
/// 2016/03/13 Bernhard Egger assignment 1: adapted to modified SnuPL/-1 syntax
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

#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>

#include "scanner.h"
using namespace std;

//------------------------------------------------------------------------------
// token names
//
#define TOKEN_STRLEN 32

char ETokenName[][TOKEN_STRLEN] = {
  "kModule",                        ///< module
  "kBegin",                         ///< begin
  "kEnd",                           ///< end
  "kType",                          ///< boolean or char or integer (type)
  "kBool",                          ///< true or false
  "kIf",                            ///< if
  "kThen",                          ///< then
  "kElse",                          ///< else
  "kWhile",                         ///< while
  "kDo",                            ///< do
  "kReturn",                        ///< return
  "kVar",                           ///< var
  "kProc",                          ///< procedure
  "kFunc",                          ///< function

  "tIdent",                         ///< an identifier
  "tNumber",                        ///< a number
  "tChar",                          ///< a character
  "tString",                        ///< a string
  "tPlusMinus",                     ///< '+' or '-'
  "tMulDiv",                        ///< '*' or '/'
  "tAndOr",                         ///< '&&' or '||'
  "tNot",                           ///< a not operator
  "tRelOp",                         ///< relational operator
  "tAssign",                        ///< assignment operator
  "tSemicolon",                     ///< a semicolon
  "tColon",                         ///< a colon
  "tComma",                         ///< a comma
  "tDot",                           ///< a dot
  "tLBrak",                         ///< a left bracket
  "tRBrak",                         ///< a right bracket
  "tLParen",                        ///< a left paren
  "tRParen",                        ///< a right paren

  "tEOF",                           ///< end of file
  "tIOError",                       ///< I/O error
  "tUndefined",                     ///< undefined
};


//------------------------------------------------------------------------------
// format strings used for printing tokens
//

char ETokenStr[][TOKEN_STRLEN] = {
  "kModule",                        ///< module
  "kBegin",                         ///< begin
  "kEnd",                           ///< end
  "kType (%s)",                     ///< boolean or char or integer
  "kBool (%s)",                     ///< true or false
  "kIf",                            ///< if
  "kThen",                          ///< then
  "kElse",                          ///< else
  "kWhile",                         ///< while
  "kDo",                            ///< do
  "kReturn",                        ///< return
  "kVar",                           ///< var
  "kProc",                          ///< procedure
  "kFunc",                          ///< function

  "tIdent (%s)",                    ///< an identifier
  "tNumber (%s)",                   ///< a number
  "tChar (%s)",                     ///< a character
  "tString (%s)",                   ///< a string
  "tPlusMinus (%s)",                ///< '+' or '-'
  "tMulDiv (%s)",                   ///< '*' or '/'
  "tAndOr (%s)",                    ///< '&&' or '||'
  "tNot",                           ///< a not operator
  "tRelOp (%s)",                    ///< relational operator
  "tAssign",                        ///< assignment operator
  "tSemicolon",                     ///< a semicolon
  "tColon",                         ///< a colon
  "tComma",                         ///< a comma
  "tDot",                           ///< a dot
  "tLBrak",                         ///< a left bracket
  "tRBrak",                         ///< a right bracket
  "tLParen",                        ///< a left paren
  "tRParen",                        ///< a right paren

  "tEOF",                           ///< end of file
  "tIOError",                       ///< I/O error
  "tUndefined (%s)",                ///< undefined
};


//------------------------------------------------------------------------------
// reserved keywords
//
pair<const char*, EToken> Keywords[] =
{
  {"module", kModule},
  {"begin", kBegin},
  {"end", kEnd},
  {"boolean", kType},
  {"char", kType},
  {"integer", kType},
  {"true", kBool},
  {"false", kBool},
  {"if", kIf},
  {"then", kThen},
  {"else", kElse},
  {"while", kWhile},
  {"do", kDo},
  {"return", kReturn},
  {"var", kVar},
  {"procedure", kProc},
  {"function", kFunc}
};



//------------------------------------------------------------------------------
// CToken
//
CToken::CToken()
{
  _type = tUndefined;
  _value = "";
  _line = _char = 0;
}

CToken::CToken(int line, int charpos, EToken type, const string value)
{
  _type = type;
  _value = escape(value);
  _line = line;
  _char = charpos;
}

CToken::CToken(const CToken &token)
{
  _type = token.GetType();
  _value = token.GetValue();
  _line = token.GetLineNumber();
  _char = token.GetCharPosition();
}

CToken::CToken(const CToken *token)
{
  _type = token->GetType();
  _value = token->GetValue();
  _line = token->GetLineNumber();
  _char = token->GetCharPosition();
}

const string CToken::Name(EToken type)
{
  return string(ETokenName[type]);
}

const string CToken::GetName(void) const
{
  return string(ETokenName[GetType()]);
}

ostream& CToken::print(ostream &out) const
{
  int str_len = _value.length();
  str_len = TOKEN_STRLEN + (str_len < 64 ? str_len : 64);

  char *str = (char*) malloc((sizeof (char)) * str_len);
  snprintf(str, str_len, ETokenStr[GetType()], _value.c_str());
  out << dec << _line << ":" << _char << ": " << str;
  free(str);
  return out;
}

string CToken::escape(const string text)
{
  const char *t = text.c_str();
  string s;

  while (*t != '\0') {
    switch (*t) {
      case '\n': s += "\\n";  break;
      case '\t': s += "\\t";  break;
      case '\0': s += "\\0";  break;
      case '\'': s += "\\'";  break;
      case '\"': s += "\\\""; break;
      case '\\': s += "\\\\"; break;
      default :  s += *t;
    }
    t++;
  }

  return s;
}

ostream& operator<<(ostream &out, const CToken &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CToken *t)
{
  return t->print(out);
}


//------------------------------------------------------------------------------
// CScanner
//
map<string, EToken> CScanner::keywords;

CScanner::CScanner(istream *in)
{
  InitKeywords();
  _in = in;
  _delete_in = false;
  _line = _char = 1;
  _token = NULL;
  _good = in->good();
  NextToken();
}

CScanner::CScanner(string in)
{
  InitKeywords();
  _in = new istringstream(in);
  _delete_in = true;
  _line = _char = 1;
  _token = NULL;
  _good = true;
  NextToken();
}

CScanner::~CScanner()
{
  if (_token != NULL) delete _token;
  if (_delete_in) delete _in;
}

void CScanner::InitKeywords(void)
{
  if (keywords.size() == 0) {
    int size = sizeof (Keywords) / sizeof (Keywords[0]);
    for (int i = 0; i < size; i++)
      keywords[Keywords[i].first] = Keywords[i].second;
  }
}

CToken CScanner::Get()
{
  CToken result(_token);

  EToken type = _token->GetType();
  _good = !(type == tIOError);

  NextToken();
  return result;
}

CToken CScanner::Peek() const
{
  return CToken(_token);
}

void CScanner::NextToken()
{
  if (_token != NULL) delete _token;

  _token = Scan();
}

void CScanner::RecordStreamPosition()
{
  _saved_line = _line;
  _saved_char = _char;
}

void CScanner::GetRecordedStreamPosition(int *lineno, int *charpos)
{
  *lineno = _saved_line;
  *charpos = _saved_char;
}

CToken* CScanner::NewToken(EToken type, const string token)
{
  return new CToken(_saved_line, _saved_char, type, token);
}

/* TODO: modify to implement SnuPL/1 */
CToken* CScanner::Scan()
{
  while(OnRemove()) {
    while (_in->good() && IsWhite(_in->peek()))
      GetChar();

    if (_in->good() && IsComment(_in->peek()))
      DeleteLine();
  }

  RecordStreamPosition();

  if (_in->eof()) return NewToken(tEOF);
  if (!_in->good()) return NewToken(tIOError);

  char c = GetChar();
  string tokval(1, c);
  EToken token = tUndefined;

  /* TODO: add cases */
  switch (c) {
    case ':':
      if (_in->peek() == '=') {
        tokval += GetChar();
        token = tAssign;
      }
      else token = tColon;
      break;

    case '+':
    case '-':
      token = tPlusMinus;
      break;

    case '*':
    case '/':
      token = tMulDiv;
      break;

    case '&':
    case '|':
      if (_in->peek() == token) {
        tokval += GetChar();
        token = tAndOr;
      }
      break;

    case '!':
      token = tNot;
      break;

    case '=':
    case '#':
      token = tRelOp;
      break;

    case '<':
    case '>':
      token = tRelop;
      if (_in->peek() == '=')
        tokval += GetChar();
      break;

    case ';':
      token = tSemicolon;
      break;

    case ',':
      token = tComma;
      break;

    case '.':
      token = tDot;
      break;

      /* TODO: proper work please */
    case '\'':
      break;
      
    case '\"':
      break;

    case '[':
      token = tLBrak;
      break;

    case ']':
      token = tRBrak;
      break;

    case '(':
      token = tLParen;
      break;

    case ')':
      token = tRParen;
      break;

    default:
      /* TODO: number */
      if (('0' <= c) && (c <= '9')) {
        token = tNumber;
        // call subroutine to parse integer
      }

      /* TODO: identifier and keyword */
      else if (('a' <= c) && (c <= 'z')) {
        token = tIdent;
        // use map<string, EToken> CScanner::keywords;
      }

      else {
        tokval = "invalid character '";
        tokval += c;
        tokval += "'";
      }
      break;
  }

  return NewToken(token, tokval);
}

char CScanner::GetChar()
{
  char c = _in->get();
  if (c == '\n') {
    _line++;
    _char = 1;
  }
  else _char++;
  return c;
}

string CScanner::GetChar(int n)
{
  string str;
  for (int i = 0; i < n; i++)
    str += GetChar();
  return str;
}

bool CScanner::OnRemove()
{
  return (_in->good() && (IsWhite(_in->peek()) || IsComment(_in->peek())));
}


void CScanner::DeleteLine()
{
  while (_in->good() && (_in->peek() != '\n'))
    GetChar();

  if (_in->good() && (_in->peek() == '\n'))
    GetChar();
}

bool CScanner::IsWhite(char c) const
{
  return ((c == ' ') || (c == '\n') || (c == '\t'));
}

bool CScanner::IsComment(char c)
{
  bool retval = false;

  if (c == '/')
  {
    _in->get();
    if (!_in->good()) return false;
    if (_in->peek() == '/') retval = true;
    _in->unget();
  }

  return retval;
}
