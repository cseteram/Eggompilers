//------------------------------------------------------------------------------
/// @brief SnuPL backend
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/11/28 Bernhard Egger created
/// 2013/06/09 Bernhard Egger adapted to SnuPL/0
/// 2016/04/04 Bernhard Egger adapted to SnuPL/1
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

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include "backend.h"
using namespace std;


//------------------------------------------------------------------------------
// CBackend
//
CBackend::CBackend(ostream &out)
  : _out(out)
{
}

CBackend::~CBackend(void)
{
}

bool CBackend::Emit(CModule *m)
{
  assert(m != NULL);
  _m = m;

  if (!_out.good()) return false;

  bool res = true;

  try {
    EmitHeader();
    EmitCode();
    EmitData();
    EmitFooter();

    res = _out.good();
  } catch (...) {
    res = false;
  }

  return res;
}

void CBackend::EmitHeader(void)
{
}

void CBackend::EmitCode(void)
{
}

void CBackend::EmitData(void)
{
}

void CBackend::EmitFooter(void)
{
}


//------------------------------------------------------------------------------
// CBackendx86
//
CBackendx86::CBackendx86(ostream &out)
  : CBackend(out), _curr_scope(NULL)
{
  _ind = string(4, ' ');
}

CBackendx86::~CBackendx86(void)
{
}

void CBackendx86::EmitHeader(void)
{
  _out << "##################################################" << endl
       << "# " << _m->GetName() << endl
       << "#" << endl
       << endl;
}

void CBackendx86::EmitCode(void)
{
  _out << _ind << "#-----------------------------------------" << endl
       << _ind << "# text section" << endl
       << _ind << "#" << endl
       << _ind << ".text" << endl
       << _ind << ".align 4" << endl
       << endl
       << _ind << "# entry point and pre-defined functions" << endl
       << _ind << ".global main" << endl
       << _ind << ".extern DIM" << endl
       << _ind << ".extern DOFS" << endl
       << _ind << ".extern ReadInt" << endl
       << _ind << ".extern WriteInt" << endl
       << _ind << ".extern WriteStr" << endl
       << _ind << ".extern WriteChar" << endl
       << _ind << ".extern WriteLn" << endl
       << endl;

  /*
   * forall s in subscopes do
   *   EmitScope(s)
   * EmitScope(program)
   */
  const vector<CScope*> &subscopes = _m->GetSubscopes();
  for (const auto &s : subscopes) {
    SetScope(s);
    EmitScope(s);
  }
  SetScope(_m);
  EmitScope(_m);

  _out << _ind << "# end of text section" << endl
       << _ind << "#-----------------------------------------" << endl
       << endl;
}

void CBackendx86::EmitData(void)
{
  _out << _ind << "#-----------------------------------------" << endl
       << _ind << "# global data section" << endl
       << _ind << "#" << endl
       << _ind << ".data" << endl
       << _ind << ".align 4" << endl
       << endl;

  EmitGlobalData(_m);

  _out << _ind << "# end of global data section" << endl
       << _ind << "#-----------------------------------------" << endl
       << endl;
}

void CBackendx86::EmitFooter(void)
{
  _out << _ind << ".end" << endl
       << "##################################################" << endl;
}

void CBackendx86::SetScope(CScope *scope)
{
  _curr_scope = scope;
}

CScope* CBackendx86::GetScope(void) const
{
  return _curr_scope;
}

void CBackendx86::EmitScope(CScope *scope)
{
  assert(scope != NULL);

  string label;

  if (scope->GetParent() == NULL) label = "main";
  else label = scope->GetName();

  /* label */
  _out << _ind << "# scope " << scope->GetName() << endl
       << label << ":" << endl;

  /* ComputeStackOffsets(scope) */
  _out << _ind << "# stack offsets:" << endl;
  size_t size = ComputeStackOffsets(scope->GetSymbolTable(), +8, -12);
  _out << endl;

  /* emit function prologue */
  _out << _ind << "# prologue" << endl;
  EmitInstruction("pushl", "%ebp");
  EmitInstruction("movl", "%esp, %ebp");
  EmitInstruction("pushl", "%ebx", "save callee saved registers");
  EmitInstruction("pushl", "%esi");
  EmitInstruction("pushl", "%edi");
  EmitInstruction("subl", "$" + to_string(size) + ", %esp", "make room for locals");

  /* memset local stack area to 0
   * there are 2 different ways depending on total stack offset
   */
  if (size >= 20) {
    _out << endl;
    EmitInstruction("cld", "", "memset local stack area to 0");
    EmitInstruction("xorl", "%eax, %eax");
    EmitInstruction("movl", "$" + to_string(size/4) + ", %ecx");
    EmitInstruction("mov", "%esp, %edi");
    EmitInstruction("rep", "stosl");
  }
  else if (size > 0) {
    _out << endl;
    EmitInstruction("xorl", "%eax, %eax", "memset local stack area to 0");
    for (int i = size - 4; i >= 0; i -= 4)
      EmitInstruction("movl", "%eax, " + to_string(i) + "(%esp)");
  }

  /* emit local data */
  if (scope->GetParent())
    EmitLocalData(scope);
  _out << endl;
 
  /* emit function body */
  _out << _ind << "# function body" << endl;
  const list<CTacInstr*> &instructions = scope->GetCodeBlock()->GetInstr();
  /* forall i in instructions do */
  for (const auto &i : instructions) {
    /* EmitInstruction(i) */
    EmitInstruction(i);
  }
  _out << endl;

  /* emit function epilogue */
  _out << Label("exit") << ":" << endl
       << _ind << "# epilogue" << endl;
  EmitInstruction("addl", "$" + to_string(size) + ", %esp", "remove locals");
  EmitInstruction("popl", "%edi");
  EmitInstruction("popl", "%esi");
  EmitInstruction("popl", "%ebx");
  EmitInstruction("popl", "%ebp");
  EmitInstruction("ret");
  _out << endl;
}

void CBackendx86::EmitGlobalData(CScope *scope)
{
  assert(scope != NULL);

  // emit the globals for the current scope
  CSymtab *st = scope->GetSymbolTable();
  assert(st != NULL);

  bool header = false;

  vector<CSymbol*> slist = st->GetSymbols();

  _out << dec;

  size_t size = 0;

  for (size_t i=0; i<slist.size(); i++) {
    CSymbol *s = slist[i];
    const CType *t = s->GetDataType();

    if (s->GetSymbolType() == stGlobal) {
      if (!header) {
        _out << _ind << "# scope: " << scope->GetName() << endl;
        header = true;
      }

      // insert alignment only when necessary
      if ((t->GetAlign() > 1) && (size % t->GetAlign() != 0)) {
        size += t->GetAlign() - size % t->GetAlign();
        _out << setw(4) << " " << ".align "
             << right << setw(3) << t->GetAlign() << endl;
      }

      _out << left << setw(36) << s->GetName() + ":" << "# " << t << endl;

      if (t->IsArray()) {
        const CArrayType *a = dynamic_cast<const CArrayType*>(t);
        assert(a != NULL);
        int dim = a->GetNDim();

        _out << setw(4) << " "
          << ".long " << right << setw(4) << dim << endl;

        for (int d=0; d<dim; d++) {
          assert(a != NULL);

          _out << setw(4) << " "
            << ".long " << right << setw(4) << a->GetNElem() << endl;

          a = dynamic_cast<const CArrayType*>(a->GetInnerType());
        }
      }

      const CDataInitializer *di = s->GetData();
      if (di != NULL) {
        const CDataInitString *sdi = dynamic_cast<const CDataInitString*>(di);
        assert(sdi != NULL);  // only support string data initializers for now

        _out << left << setw(4) << " "
          << ".asciz " << '"' << sdi->GetData() << '"' << endl;
      } else {
        _out  << left << setw(4) << " "
          << ".skip " << dec << right << setw(4) << t->GetDataSize()
          << endl;
      }

      size += t->GetSize();
    }
  }

  _out << endl;

  // emit globals in subscopes (necessary if we support static local variables)
  vector<CScope*>::const_iterator sit = scope->GetSubscopes().begin();
  while (sit != scope->GetSubscopes().end()) EmitGlobalData(*sit++);
}

void CBackendx86::EmitLocalData(CScope *scope)
{
  assert(scope != NULL);

  // TODO TODO!
}

void CBackendx86::EmitCodeBlock(CCodeBlock *cb)
{
  assert(cb != NULL);

  const list<CTacInstr*> &instr = cb->GetInstr();
  list<CTacInstr*>::const_iterator it = instr.begin();

  while (it != instr.end()) EmitInstruction(*it++);
}

void CBackendx86::EmitInstruction(CTacInstr *i)
{
  assert(i != NULL);

  ostringstream cmt;
  string mnm;
  cmt << i;

  EOperation op = i->GetOperation();

  switch (op) {
    // binary operators
    // dst = src1 op src2
    case opAdd:
    case opSub:
    case opMul:
    case opDiv:
    case opAnd:
    case opOr:
      Load(i->GetSrc(1), "%eax", cmt.str());
      Load(i->GetSrc(2), "%ebx");
      if (op == opAdd)
        EmitInstruction("addl", "%ebx, %eax");
      else if (op == opSub)
        EmitInstruction("subl", "%ebx, %eax");
      else if (op == opMul)
        EmitInstruction("imull", "%ebx");
      else if (op == opDiv) {
        EmitInstruction("cdq");
        EmitInstruction("idivl", "%ebx");
      }
      else if (op == opAnd) {
        /* opAnd will not be appeared */
        EmitInstruction("andl", "%eax, %ebx");
      }
      else {
        /* opOr will not be appeared */
        EmitInstruction("orl", "%eax, %ebx");
      }

      Store(i->GetDest(), 'a');
      break;

    // unary operators
    // dst = op src1
    case opPos:
    case opNeg:
    case opNot:
      Load(i->GetSrc(1), "%eax", cmt.str());
      if (op == opNeg)
        EmitInstruction("negl", "%eax");
      else if (op == opNot) {
        /* opNot will not be appeared */
        EmitInstruction("notl", "%eax");
      }
      Store(i->GetDest(), 'a');
      break;

    // memory operations
    // dst = src1
    case opAssign:
      Load(i->GetSrc(1), "%eax", cmt.str());
      Store(i->GetDest(), 'a');
      break;

    // pointer operations
    // dst = &src1
    case opAddress:
      EmitInstruction("leal", Operand(i->GetSrc(1)) + ", %eax", cmt.str());
      Store(i->GetDest(), 'a');
      break;
    // dst = *src1
    case opDeref:
      // opDeref not generated for now
      EmitInstruction("# opDeref", "not implemented", cmt.str());
      break;

    // unconditional branching
    // goto dst
    case opGoto:
      EmitInstruction("jmp", Label(dynamic_cast<const CTacLabel*>(i->GetDest())), cmt.str());
      break;

    // conditional branching
    // if src1 relOp src2 then goto dst
    case opEqual:
    case opNotEqual:
    case opLessThan:
    case opLessEqual:
    case opBiggerThan:
    case opBiggerEqual:
      Load(i->GetSrc(1), "%eax", cmt.str());
      Load(i->GetSrc(2), "%ebx");
      EmitInstruction("cmpl", "%ebx, %eax");
      EmitInstruction("j" + Condition(op), Label(dynamic_cast<const CTacLabel*>(i->GetDest())));
      break;

    // function call-related operations
    case opCall:
    {
      const CTacName *fun = dynamic_cast<const CTacName*>(i->GetSrc(1));
      const CSymProc *sym = dynamic_cast<const CSymProc*>(fun->GetSymbol());
      assert(fun != NULL);
      assert(sym != NULL);

      EmitInstruction("call", sym->GetName(), cmt.str());
      if (sym->GetNParams() > 0)
        EmitInstruction("addl", "$" + to_string(4 * sym->GetNParams()) + ", %esp");
      if (i->GetDest())
        Store(i->GetDest(), 'a');
      break;
    }
    case opReturn:
      if (i->GetSrc(1)) {
        Load(i->GetSrc(1), "%eax", cmt.str());
        EmitInstruction("jmp", Label("exit"));
      }
      else
        EmitInstruction("jmp", Label("exit"), cmt.str());
      break;
    case opParam:
      Load(i->GetSrc(1), "%eax", cmt.str());
      EmitInstruction("pushl", "%eax");
      break;

    // special
    case opLabel:
      _out << Label(dynamic_cast<CTacLabel*>(i)) << ":" << endl;
      break;

    case opNop:
      EmitInstruction("nop", "", cmt.str());
      break;

    default:
      EmitInstruction("# ???", "not implemented", cmt.str());
  }
}

void CBackendx86::EmitInstruction(string mnemonic, string args, string comment)
{
  _out << left
       << _ind
       << setw(7) << mnemonic << " "
       << setw(23) << args;
  if (comment != "") _out << " # " << comment;
  _out << endl;
}

void CBackendx86::Load(CTacAddr *src, string dst, string comment)
{
  assert(src != NULL);

  string mnm = "mov";
  string mod = "l";

  // set operator modifier based on the operand size
  switch (OperandSize(src)) {
    case 1: mod = "zbl"; break;
    case 2: mod = "zwl"; break;
    case 4: mod = "l"; break;
  }

  // emit the load instruction
  EmitInstruction(mnm + mod, Operand(src) + ", " + dst, comment);
}

void CBackendx86::Store(CTac *dst, char src_base, string comment)
{
  assert(dst != NULL);

  string mnm = "mov";
  string mod = "l";
  string src = "%";

  // compose the source register name based on the operand size
  switch (OperandSize(dst)) {
    case 1: mod = "b"; src += string(1, src_base) + "l"; break;
    case 2: mod = "w"; src += string(1, src_base) + "x"; break;
    case 4: mod = "l"; src += "e" + string(1, src_base) + "x"; break;
  }

  // emit the store instruction
  EmitInstruction(mnm + mod, src + ", " + Operand(dst), comment);
}

string CBackendx86::Operand(const CTac *op)
{
  string operand;

  /* return a string representing op
   * hint : take special care of references (op of type CTacReference)
   */

  /* op : type CTacReference */
  const CTacReference *opRef = dynamic_cast<const CTacReference*>(op);
  if (opRef) {
    // TODO : check that it is true
    const CSymbol *sym = opRef->GetSymbol();
    EmitInstruction("movl", to_string(sym->GetOffset()) + "(" + sym->GetBaseRegister() + "), %edi");
    operand = "(%edi)";

    return operand;
  }

  /* op : type CTacName */
  const CTacName *opName = dynamic_cast<const CTacName*>(op);
  if (opName) {
    const CSymbol *sym = opName->GetSymbol();
    ESymbolType symbolType = sym->GetSymbolType();
    if (symbolType == stGlobal || symbolType == stProcedure)
      operand = sym->GetName();
    else {
      operand = to_string(sym->GetOffset()) + "(" + sym->GetBaseRegister() + ")";
    }
      
    return operand;
  }

  /* op : type CTacConst */
  const CTacConst *opConst = dynamic_cast<const CTacConst*>(op);
  if (opConst) {
    operand = "$" + to_string(opConst->GetValue());
    return operand;
  }

  return operand;
}

string CBackendx86::Imm(int value) const
{
  ostringstream o;
  o << "$" << dec << value;
  return o.str();
}

string CBackendx86::Label(const CTacLabel* label) const
{
  CScope *cs = GetScope();
  assert(cs != NULL);

  ostringstream o;
  o << "l_" << cs->GetName() << "_" << label->GetLabel();
  return o.str();
  return "l_" + cs->GetName() + "_" + label->GetLabel();
}

string CBackendx86::Label(string label) const
{
  CScope *cs = GetScope();
  assert(cs != NULL);

  return "l_" + cs->GetName() + "_" + label;
}

string CBackendx86::Condition(EOperation cond) const
{
  switch (cond) {
    case opEqual:       return "e";
    case opNotEqual:    return "ne";
    case opLessThan:    return "l";
    case opLessEqual:   return "le";
    case opBiggerThan:  return "g";
    case opBiggerEqual: return "ge";
    default:            assert(false); break;
  }
}

int CBackendx86::OperandSize(CTac *t) const
{
  int size = 4;

  /* compute the size for operand t of type CTacName
   * Hint: you need to take special care of references (incl. references to pointers!)
   *       and arrays. Compare your output to that of the reference implementation
   *       if you are not sure.
   */

  /* t : type CTacReference */
  CTacReference *tRef = dynamic_cast<CTacReference*>(t);
  if (tRef != NULL) {
    const CSymbol *deref = tRef->GetDerefSymbol();
    const CType *dtype = deref->GetDataType();
    if (dtype->IsPointer() || dtype->IsArray()) {
      if (dtype->IsPointer())
        dtype = dynamic_cast<const CPointerType*>(dtype)->GetBaseType();
      
      const CType *basetype = dynamic_cast<const CArrayType*>(dtype)->GetBaseType();
      size = basetype->GetDataSize();
    }
    else
      size = dtype->GetDataSize();
    return size;
  }

  /* t : type CTacName */
  CTacName *tName = dynamic_cast<CTacName*>(t);
  if (tName != NULL) {
    size = tName->GetSymbol()->GetDataType()->GetDataSize();
    return size;
  }

  /* t : type CTacConst 
   * constant has the size = 4
   */
  return size;
}

size_t CBackendx86::ComputeStackOffsets(CSymtab *symtab,
                                        int param_ofs,int local_ofs)
{
  assert(symtab != NULL);
  vector<CSymbol*> slist = symtab->GetSymbols();

  size_t size = 0;

  /* foreach local symbol l in slist do
   *   compute aligned offset on stack and store in symbol l
   *   set base register to %ebp
   */
  for (const auto &l : slist) {
    if (l->GetSymbolType() != stLocal)
      continue;

    const CType* datatype = l->GetDataType();
    if (datatype->IsInt() || datatype->IsPointer()) {
      if (local_ofs % 4) {
        int padding = 4 + local_ofs % 4; /* local_ofs < 0 */
        size += padding;
        local_ofs += -padding;
      }

      size += 4;
      local_ofs += -4;
      l->SetOffset(local_ofs);
      l->SetBaseRegister("%ebp");
    }
    else if (datatype->IsChar() || datatype->IsBoolean()) {
      size += 1;
      local_ofs += -1;
      l->SetOffset(local_ofs);
      l->SetBaseRegister("%ebp");
    }
    else if (datatype->IsArray()) {
      if ((local_ofs - datatype->GetSize()) % 4) {
        int padding = 4 + (local_ofs - datatype->GetSize()) % 4;
        size += padding;
        local_ofs += -padding;
      }

      size += datatype->GetSize();
      local_ofs += -datatype->GetSize();
      l->SetOffset(local_ofs);
      l->SetBaseRegister("%ebp");
    }
  }

  /* foreach parameter p in slist do
   *   compute offset on stack and store in symbol p
   *   set base register to %ebp
   */
  for (const auto &p : slist) {
    if (p->GetSymbolType() != stParam)
      continue;

    /* parameter doesn't require alignment rule.
     * its offset is always (start_param_ofs + 4 * index)
     */
    int offset = param_ofs + 4 * dynamic_cast<CSymParam*>(p)->GetIndex();
    p->SetOffset(offset);
    p->SetBaseRegister("%ebp");    
  }

  /* align size */
  if (local_ofs % 4) {
    int padding = 4 + local_ofs % 4;
    size += padding;
    local_ofs += -padding;
  }

  /* dump stack frame to assembly file */
  // TODO : it should equals to snuplc reference...
  for (const auto &s : slist) {
    ESymbolType stype = s->GetSymbolType();
    if (stype != stLocal && stype != stParam)
      continue;

    _out << _ind << "#" << setw(7) << std::right << s->GetOffset() << "(" << s->GetBaseRegister() << ")"
         << setw(4) << s->GetDataType()->GetSize() << setw(2) << s << endl;
  }

  return size;
}
