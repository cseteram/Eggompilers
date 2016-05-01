//------------------------------------------------------------------------------
/// @brief SnuPL symbol table
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2016/04/05 Bernhard Egger bugfix in CSymtab::print
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

#include <cassert>
#include <iomanip>

#include "symtab.h"
using namespace std;


//------------------------------------------------------------------------------
// CSymbol
//
CSymbol::CSymbol(const string name, ESymbolType stype, const CType *dtype)
  : _name(name), _symboltype(stype), _datatype(dtype), _data(NULL), 
    _rbase(""), _offset(0), _symtab(NULL)
{
  assert(_name != "");
  assert(_datatype != NULL);
}

CSymbol::~CSymbol(void)
{
}

string CSymbol::GetName(void) const
{
  return _name;
}

ESymbolType CSymbol::GetSymbolType(void) const
{
  return _symboltype;
}

void CSymbol::SetDataType(const CType *datatype)
{
  _datatype = datatype;
}

const CType* CSymbol::GetDataType(void) const
{
  return _datatype;
}

void CSymbol::SetSymbolTable(CSymtab *symtab)
{
  _symtab = symtab;
}

CSymtab* CSymbol::GetSymbolTable(void) const
{
  return _symtab;
}

void CSymbol::SetData(const CDataInitializer *data)
{
  _data = data;
}

const CDataInitializer* CSymbol::GetData(void) const
{
  return _data;
}

void CSymbol::SetBaseRegister(string rbase)
{
  _rbase = rbase;
}

string CSymbol::GetBaseRegister(void) const
{
  return _rbase;
}

void CSymbol::SetOffset(int offset)
{
  _offset = offset;
}

int CSymbol::GetOffset(void) const
{
  return _offset;
}

ostream& CSymbol::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ " << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  out << ind << " ]";
  return out;
}

ostream& operator<<(ostream &out, const CSymbol &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CSymbol *t)
{
  return t->print(out);
}


//------------------------------------------------------------------------------
// CSymGlobal
//
CSymGlobal::CSymGlobal(const string name, const CType *type)
  : CSymbol(name, stGlobal, type)
{
}

ostream& CSymGlobal::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ @" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymLocal
//
CSymLocal::CSymLocal(const string name, const CType *type)
  : CSymbol(name, stLocal, type)
{
}

CSymLocal::CSymLocal(const string name, ESymbolType stype, const CType *type)
  : CSymbol(name, stype, type)
{
}

ostream& CSymLocal::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ $" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  if (GetBaseRegister() != "") {
    out << " " << GetBaseRegister();
    if (GetOffset() >= 0) out << "+";
    out << GetOffset();
  }
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymParam
//
CSymParam::CSymParam(int index, const string name, const CType *type)
  : CSymLocal(name, stParam, type), _index(index)
{
}

ostream& CSymParam::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ %" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  if (GetBaseRegister() != "") {
    out << " " << GetBaseRegister();
    if (GetOffset() >= 0) out << "+";
    out << GetOffset();
  }
  out << ind << " ]";
  return out;
}

int CSymParam::GetIndex(void) const
{
  return _index;
}


//------------------------------------------------------------------------------
// CSymProc
//
CSymProc::CSymProc(const string name, const CType *return_type)
  : CSymbol(name, stProcedure, return_type)
{
}

void CSymProc::AddParam(CSymParam *param)
{
  _param.push_back(param);
}

int CSymProc::GetNParams(void) const
{
  return (int)_param.size();
}

const CSymParam* CSymProc::GetParam(int index) const
{
  assert((index >= 0) && (index < _param.size()));
  return _param[index];
}

ostream& CSymProc::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ *" << GetName() << "(";
  for (size_t i=0; i<_param.size(); i++) {
    const CType *t = _param[i]->GetDataType();
    if (i > 0) out << ",";
    t->print(out);
  }
  out << ") --> ";
  GetDataType()->print(out);
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymtab
//
CSymtab::CSymtab(void)
{
}

CSymtab::CSymtab(CSymtab *parent)
  : _parent(parent)
{
  assert(parent != NULL);
}

CSymtab::~CSymtab(void)
{
  map<string, CSymbol*>::const_iterator it = _symtab.begin();
  while (it != _symtab.end()) delete (*it++).second;
  _symtab.clear();
}

bool CSymtab::AddSymbol(CSymbol *s)
{
  assert(s != NULL);

  // global symbols always get pushed up to the global symbol table
  if ((s->GetSymbolType() == stGlobal) && (_parent != NULL)) {
    return _parent->AddSymbol(s);
  }

  if (!FindSymbol(s->GetName(), sLocal)) {
    _symtab[s->GetName()] = s;
    s->SetSymbolTable(this);
    return true;
  } else {
    return false;
  }
}

const CSymbol* CSymtab::FindSymbol(const string name, EScope scope) const
{
  map<string, CSymbol*>::const_iterator it = _symtab.find(name);

  if (it != _symtab.end()) return (*it).second;
  else {
    if ((scope == sLocal) || (_parent == NULL)) return NULL;
    else return _parent->FindSymbol(name, scope);
  }
}

vector<CSymbol*> CSymtab::GetSymbols(void) const
{
  vector<CSymbol*> _res;

  map<string, CSymbol*>::const_iterator it = _symtab.begin();
  while (it != _symtab.end()) {
    _res.push_back(it->second);
    it++;
  }

  return _res;
}

ostream& CSymtab::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[";
  map<string, CSymbol*>::const_iterator it = _symtab.begin();
  while (it != _symtab.end()) {
    out << endl;

    const CSymbol *s = (*it++).second;
    s->print(out, indent+2);

    const CDataInitializer *di = s->GetData();
    if (di != NULL) {
      out << endl;
      di->print(out, indent+4);
    }
  }
  out << endl << ind << "]]" << endl;

  return out;
}

ostream& operator<<(ostream &out, const CSymtab &s)
{
  return s.print(out);
}

ostream& operator<<(ostream &out, const CSymtab *s)
{
  return s->print(out);
}

