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

#ifndef __SnuPL_BACKEND_H__
#define __SnuPL_BACKEND_H__

#include <iostream>
#include <vector>

#include "symtab.h"
#include "ir.h"

using namespace std;

//------------------------------------------------------------------------------
/// @brief backend base class
///
/// base class for backends
///

class CBackend {
  public:
    /// @name constructors/destructors
    /// @{

    CBackend(ostream &out);
    virtual ~CBackend(void);

    /// @}

    /// @name output method
    /// @{

    virtual bool Emit(CModule *m);

    /// @}

  protected:
    /// @name detailed output methods
    /// @{

    virtual void EmitHeader(void);
    virtual void EmitCode(void);
    virtual void EmitData(void);
    virtual void EmitFooter(void);

    /// @}


    CModule *_m;                    ///< module
    ostream &_out;                  ///< output stream
};


//------------------------------------------------------------------------------
/// @brief x86 backend
///
/// backend for Intel IA32
///
class CBackendx86 : public CBackend {
  public:
    /// @name constructors/destructors
    /// @{

    CBackendx86(ostream &out);
    virtual ~CBackendx86(void);

    /// @}

  protected:
    /// @name detailed output methods
    /// @{

    virtual void EmitHeader(void);
    virtual void EmitCode(void);
    virtual void EmitData(void);
    virtual void EmitFooter(void);

    /// @}

    /// @name additional methods
    /// @{

    /// @brief set the current scope
    void SetScope(CScope *scope);

    /// @brief get the current scope
    CScope* GetScope(void) const;

    /// @brief emit a scope
    virtual void EmitScope(CScope *scope);

    /// @brief emit global data
    virtual void EmitGlobalData(CScope *s);

    /// @brief emit local data
    ///
    /// EmitLocalData() initializes local data (i.e., arrays)
    virtual void EmitLocalData(CScope *s);

    /// @brief emit code for code block @cb
    virtual void EmitCodeBlock(CCodeBlock *cb);

    /// @brief emit instruction @i
    virtual void EmitInstruction(CTacInstr *i);

    /// @brief emit an instruction

    virtual void EmitInstruction(string mnemonic, string args="",
                                 string comment="");

    /// @brief emit a load instruction
    void Load(CTacAddr *src, string dst, string comment="");

    /// @brief emit a store instruction
    void Store(CTac *dst, char src_base, string comment="");

    /// @brief return an operand string for @a op
    /// @param op the operand
    string Operand(const CTac *op);

    /// @brief return an immediate for @a value
    string Imm(int value) const;

    /// @brief return a x86-label for CTaclabel @a label
    string Label(const CTacLabel *label) const;

    /// @brief return a x86-label for string @a label
    string Label(string label) const;

    /// @brief return the condition suffix for a binary comparison operation
    string Condition(EOperation cond) const;

    /// @brief compute the size of operator @t
    int OperandSize(CTac *t) const;

    /// @brief compute the location of local variables, temporaries and
    ///        arguments on the stack. Returns the total size occupied on
    ///        the stack as well as the the number of arguments for this
    ///        scope (if @a nargs is not NULL)
    /// @param symtab symbol table
    /// @param param_ofs offset to parameters from base pointer after epilogue
    /// @param local_ofs offset to local vars from base pointer after epilogue
    size_t ComputeStackOffsets(CSymtab *symtab, int param_ofs, int local_ofs);

    /// @}

    string _ind;                    ///< indentation
    CScope *_curr_scope;            ///< current scope
};




#endif // __SnuPL_BACKEND_H__
