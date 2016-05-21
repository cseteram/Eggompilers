//------------------------------------------------------------------------------
/// @brief SnuPL intermediate representation
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/14 Bernhard Egger adapted to SnuPL/0
/// 2013/06/06 Bernhard Egger cleanup, added documentation
/// 2014/11/04 Bernhard Egger added opPos
/// 2016/04/01 Bernhard Egger adapted to SnuPL/1
///
/// @section license_section License
/// Copyright (c) 2012-2016 Bernhard Egger
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

#include <iomanip>
#include <cassert>

#include "ir.h"
#include "ast.h"
using namespace std;


//------------------------------------------------------------------------------
// operation names
//
#define OPERATION_STRLEN 8

char EOperationName[][OPERATION_STRLEN] = {
  // binary operators
  // dst = src1 op src2
  "add",                            ///< +  addition
  "sub",                            ///< -  subtraction
  "mul",                            ///< *  multiplication
  "div",                            ///< /  division
  "and",                            ///< && binary and
  "or",                             ///< || binary or

  // unary operators
  // dst = op src1
  "neg",                            ///< -  negation
  "pos",                            ///< +  unary +
  "not",                            ///< !  binary not

  // memory operations
  // dst = src1
  "assign",                         ///< assignment

  // special and pointer operations
  "&()",                            ///< reference: dst = &src1
  "*()",                            ///< dereference: dst = *src1
  "cast",                           ///< type cast: dst = (type)src1

  // unconditional branching
  // goto dst
  "goto",                           ///< dst = target

  // conditional branching
  // if src1 relOp src2 then goto dst
  "=",                              ///< =  equal
  "#",                              ///< #  not equal
  "<",                              ///< <  less than
  "<=",                             ///< <= less or equal
  ">",                              ///< >  bigger than
  ">=",                             ///< >= bigger or equal

  // function call-related operations
  "call",                           ///< call:  dst = call src1
  "return",                         ///< return: return optional src1
  "param",                          ///< parameter: dst = index, src1 = parameter

  // special
  "label",                          ///< jump label; no arguments
  "nop",                            ///< no operation
};

bool IsRelOp(EOperation t)
{
  return (t == opEqual) ||
         (t == opNotEqual) ||
         (t == opLessThan) ||
         (t == opLessEqual) ||
         (t == opBiggerThan) ||
         (t == opBiggerEqual);
}

ostream& operator<<(ostream &out, EOperation t)
{
  out << EOperationName[t];
  return out;
}


//------------------------------------------------------------------------------
// CTac
//
CTac::CTac(void)
{
}

CTac::~CTac(void)
{
}

ostream& operator<<(ostream &out, const CTac &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CTac *t)
{
  return t->print(out);
}


//------------------------------------------------------------------------------
// CTacAddr
//
CTacAddr::CTacAddr(void)
{
}

CTacAddr::~CTacAddr(void)
{
}

//------------------------------------------------------------------------------
// CTacName
//
CTacName::CTacName(const CSymbol *symbol)
  : _symbol(symbol)
{
  assert(symbol != NULL);
}

ostream& CTacName::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << _symbol->GetName();

  return out;
}

const CSymbol* CTacName::GetSymbol(void) const
{
  return _symbol;
}

//------------------------------------------------------------------------------
// CTacConst
//
CTacConst::CTacConst(int value)
  : _value(value)
{
}

int CTacConst::GetValue(void) const
{
  return _value;
}

ostream& CTacConst::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << _value;

  return out;
}


//------------------------------------------------------------------------------
// CTacTemp
//
CTacTemp::CTacTemp(const CSymbol *symbol)
  : CTacName(symbol)
{
}


//------------------------------------------------------------------------------
// CTacReference
//
CTacReference::CTacReference(const CSymbol *symbol)
  : CTacName(symbol)
{
}

ostream& CTacReference::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "@" << _symbol->GetName();

  return out;
}


//------------------------------------------------------------------------------
// CTacInstr
//
CTacInstr::CTacInstr(string name)
  : _id(-1), _op(opNop), _src1(NULL), _src2(NULL), _dst(NULL), _name(name)
{
}

CTacInstr::CTacInstr(EOperation op, CTac *dst, CTacAddr *src1, CTacAddr *src2)
  : _id(-1), _op(op), _src1(src1), _src2(src2), _dst(dst)
{
  if (IsBranch()) {
    CTacLabel *lbl = dynamic_cast<CTacLabel*>(_dst);
    assert(lbl != NULL);
    lbl->AddReference(1);
  }
}

CTacInstr::~CTacInstr(void)
{
  if (IsBranch()) {
    CTacLabel *lbl = dynamic_cast<CTacLabel*>(_dst);
    assert(lbl != NULL);
    lbl->AddReference(-1);
  }
}

unsigned int CTacInstr::GetId(void) const
{
  return _id;
}

void CTacInstr::SetId(unsigned int id)
{
  _id = id;
}

bool CTacInstr::IsBranch(void) const
{
  return (_op == opGoto) || IsRelOp(_op);
}

EOperation CTacInstr::GetOperation(void) const
{
  return _op;
}

unsigned int CTacInstr::GetNumSrc(void) const
{
  return _src1 == NULL ? 0 : _src2 == NULL ? 1 : 2;
}

CTacAddr* CTacInstr::GetSrc(int index) const
{
  switch (index) {
    case 1: return _src1;
    case 2: return _src2;
  }
  return NULL;
}

CTac* CTacInstr::GetDest(void) const
{
  return _dst;
}

void CTacInstr::SetDest(CTac* dst)
{
  _dst = dst;
}

ostream& CTacInstr::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << right << dec << setw(3) << _id << ": ";

  if (_name == "") {
    bool relop = IsRelOp(GetOperation());

    out << "    " << left << setw(6);
    if (relop) out << "if"; else out << _op;
    out << " ";
    CTacAddr *adr = dynamic_cast<CTacAddr*>(_dst);
    if (adr != NULL) out << _dst << " <- ";
    if (_src1 != NULL) out << _src1;
    if (_src2 != NULL) {
      if (relop) out << " " << _op; else out << ",";
      out << " " << _src2;
    }
    CTacInstr *target = dynamic_cast<CTacInstr*>(_dst);
    if (target != NULL) {
      if (relop) out << " goto ";

      CTacLabel *l = dynamic_cast<CTacLabel*>(target);
      if (l != NULL) out << l->GetLabel();
      else out << target->GetId();
    }
  } else {
    out << "[CTacInstr: '" << _name << "']";
  }

  return out;
}


//------------------------------------------------------------------------------
// CTacLabel
//
CTacLabel::CTacLabel(const string label)
  : CTacInstr(opLabel, NULL), _label(label), _refcnt(0)
{
}

CTacLabel::~CTacLabel(void)
{
}

const string CTacLabel::GetLabel(void) const
{
  return _label;
}

int CTacLabel::AddReference(int ofs)
{
  _refcnt += ofs;
  return _refcnt;
}

int CTacLabel::GetRefCnt(void) const
{
  return _refcnt;
}

ostream& CTacLabel::print(ostream &out, int indent) const
{
  if (true || GetRefCnt() > 0) {
    string ind(indent, ' ');

    out << ind << right << dec << setw(3) << _id << ": "
        << left << _label << ":"
        //<< "  (refcnt: " << _refcnt << ")"
        ;
  }

  return out;
}


//------------------------------------------------------------------------------
// CScope
//
CScope::CScope(CAstNode *ast, CScope *parent)
  : _ast(ast), _parent(parent), _temp_id(0), _label_id(0)
{
  CAstScope *s = dynamic_cast<CAstScope*>(ast);
  assert(s != NULL);

  _name = s->GetName();
  _symtab = s->GetSymbolTable();
  _cb = new CCodeBlock(this);
  s->ToTac(_cb);

  for (size_t i=0; i<s->GetNumChildren(); i++) {
    CProcedure *p = new CProcedure(s->GetChild(i), this);
    _children.push_back(p);
  }
}

CScope::~CScope(void)
{
  delete _cb;
}

string CScope::GetName(void) const
{
  return _name;
}

CScope* CScope::GetParent(void) const
{
  return _parent;
}

const vector<CScope*>& CScope::GetSubscopes(void) const
{
  return _children;
}

CSymtab* CScope::GetSymbolTable(void) const
{
  return _symtab;
}

CCodeBlock* CScope::GetCodeBlock(void) const
{
  return _cb;
}

CTacTemp* CScope::CreateTemp(const CType *type)
{
  ostringstream tmp;
  tmp << "t" << _temp_id++;

  CSymbol *s = new CSymLocal(tmp.str(), type);
  GetSymbolTable()->AddSymbol(s);

  return new CTacTemp(s);
}

CTacLabel* CScope::CreateLabel(const char *hint)
{
  ostringstream tmp;
  tmp << _label_id++;
  if (hint != NULL) tmp << "_" << hint;

  return new CTacLabel(tmp.str());
}

ostream& CScope::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[CScope: " << GetName() << "]]";

  return out;
}

string CScope::dotID(void) const
{
  return GetName();
}

void CScope::toDot(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "// scope '" << dotID() << "'" << endl;

  _cb->toDot(out, indent);
}

ostream& operator<<(ostream &out, const CScope &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CScope *t)
{
  return t->print(out);
}


//------------------------------------------------------------------------------
// CModule
//
CModule::CModule(CAstNode *ast)
  : CScope(ast, NULL)
{
}

CModule::~CModule(void)
{
}

CSymbol* CModule::GetDeclaration(void) const
{
  return NULL;
}

ostream& CModule::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[ module: " << GetName() << endl;
  CTypeManager::Get()->print(out, indent+2);
  GetSymbolTable()->print(out, indent+2);
  _cb->print(out, indent+2);

  for (size_t i=0; i<_children.size(); i++) {
    out << endl;
    _children[i]->print(out, indent+2);
  }
  out << ind << "]]";

  return out;
}


//------------------------------------------------------------------------------
// CProcedure
//
CProcedure::CProcedure(CAstNode *ast, CScope *parent)
  : CScope(ast, parent)
{
}

CProcedure::~CProcedure(void)
{
}

CSymbol* CProcedure::GetDeclaration(void) const
{
  CAstProcedure *s = dynamic_cast<CAstProcedure*>(_ast);
  assert(s != NULL);

  return s->GetSymbol();
}

ostream& CProcedure::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[ procedure: " << GetName() << endl;
  GetSymbolTable()->print(out, indent+2);
  _cb->print(out, indent+2);

  for (size_t i=0; i<_children.size(); i++) {
    out << endl << endl;
    _children[i]->print(out, indent+2);
  }
  out << ind << "]]" << endl;

  return out;
}


//------------------------------------------------------------------------------
// CCodeBlock
//
CCodeBlock::CCodeBlock(CScope *owner)
  : _owner(owner), _inst_id(0)
{
  assert(_owner != NULL);
}

CCodeBlock::~CCodeBlock(void)
{
}

string CCodeBlock::GetName(void) const
{
  return _owner->GetName();
}

CScope* CCodeBlock::GetOwner(void) const
{
  return _owner;
}

CTacTemp* CCodeBlock::CreateTemp(const CType *type)
{
  return _owner->CreateTemp(type);
}

CTacLabel* CCodeBlock::CreateLabel(const char *hint)
{
  return _owner->CreateLabel(hint);
}

CTacInstr* CCodeBlock::AddInstr(CTacInstr *instr)
{
  assert(instr != NULL);
  instr->SetId(_inst_id++);
  _ops.push_back(instr);

  return instr;
}

const list<CTacInstr*>& CCodeBlock::GetInstr(void) const
{
  return _ops;
}

void CCodeBlock::CleanupControlFlow(void)
{
  list<CTacInstr*>::iterator it = _ops.begin();

  // 1. pass: delete all branches (absolute/conditional) that jump to the
  //          immediately next instruction. Deleting branch instruction will
  //          automatically decrease the reference count of the target label.
  while (it != _ops.end()) {
    CTacInstr *instr = *it++;

    if (instr->IsBranch()) {
      CTacLabel *lbl = dynamic_cast<CTacLabel*>(instr->GetDest());
      CTacInstr *next = (it == _ops.end() ? NULL : *it);

      if ((lbl != NULL) && (lbl == next)) {
        delete instr;
        it = _ops.erase(--it);
      }
    }
  }

  // 2. pass: remove all labels with reference count 0
  it = _ops.begin();
  while (it != _ops.end()) {
    CTacInstr *instr = *it++;

    CTacLabel *lbl = dynamic_cast<CTacLabel*>(instr);

    if ((lbl != NULL) && (lbl->GetRefCnt() == 0)) {
      delete lbl;
      it = _ops.erase(--it);
    }
  }

  // 3. renumber instructions (we shouldn't do that really, but it's prettier)
  _inst_id = 0;
  it = _ops.begin();
  while (it != _ops.end()) (*it++)->SetId(_inst_id++);
}

ostream& CCodeBlock::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[ " << GetName() << endl;

  list<CTacInstr*>::const_iterator it = _ops.begin();
  while (it != _ops.end()) {
    (*it++)->print(out, indent+2);
    out << endl;
  }

  out << ind << "]]" << endl;

  return out;
}

string CCodeBlock::dotID(void) const
{
  ostringstream o;
  o << _owner->GetName() << "_cb";
  return o.str();
}

string CCodeBlock::dotAttr(void) const
{
  ostringstream o;

  o << " [label=\"" << GetName() << "\\r";

  list<CTacInstr*>::const_iterator it = _ops.begin();
  while (it != _ops.end()) {
    (*it++)->print(o, 0);
    o << "\\l";
  }

  o << "\",shape=box]";

  return o.str();
}

void CCodeBlock::toDot(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << dotID() << dotAttr() << endl;
}

ostream& operator<<(ostream &out, const CCodeBlock &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CCodeBlock *t)
{
  return t->print(out);
}

