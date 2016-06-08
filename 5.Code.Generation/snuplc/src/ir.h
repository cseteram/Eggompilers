//------------------------------------------------------------------------------
/// @brief SnuPL/0 intermediate representation
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

#ifndef __SnuPL_IR_H__
#define __SnuPL_IR_H__

#include <iostream>
#include <list>
#include <vector>

#include "symtab.h"


//------------------------------------------------------------------------------
/// @brief operations
///
enum EOperation {
  // binary operators
  // dst = src1 op src2
  opAdd=0,                          ///< +  addition
  opSub,                            ///< -  subtraction
  opMul,                            ///< *  multiplication
  opDiv,                            ///< /  division
  opAnd,                            ///< && binary and
  opOr,                             ///< || binary or

  // unary operators
  // dst = op src1
  opNeg,                            ///< -  negation
  opPos,                            ///< +  unary +
  opNot,                            ///< !  binary not

  // memory operations
  // dst = src1
  opAssign,                         ///< assignment

  // special and pointer operations
  opAddress,                        ///< reference: dst = &src1
  opDeref,                          ///< dereference: dst = *src1
  opCast,                           ///< type cast: dst = (type)src1

  // unconditional branching
  // goto dst
  opGoto,                           ///< dst = target

  // conditional branching
  // if src1 relOp src2 then goto dst
  opEqual,                          ///< =  equal
  opNotEqual,                       ///< #  not equal
  opLessThan,                       ///< <  less than
  opLessEqual,                      ///< <= less or equal
  opBiggerThan,                     ///< >  bigger than
  opBiggerEqual,                    ///< >= bigger or equal

  // function call-related operations
  opCall,                           ///< call:  dst = call src1
  opReturn,                         ///< return: return optional src1
  opParam,                          ///< parameter: dst = index,src1 = parameter

  // special
  opLabel,                          ///< jump label; no arguments
  opNop,                            ///< no operation
};

/// @brief returns true if @a op is a relational operation
bool IsRelOp(EOperation t);

/// @brief EOperation output operator
///
/// @param out output stream
/// @param t EOperation
/// @retval output stream
ostream& operator<<(ostream &out, EOperation t);

//------------------------------------------------------------------------------
/// @brief three-address code base class
///
/// base class for three-address code classes
///

class CTac {
  public:
    /// @name constructors/destructors
    /// @{

    CTac(void);
    virtual ~CTac(void);

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream& print(ostream &out, int indent=0) const = 0;

    /// @}
};

/// @name CTac output operators
/// @{

/// @brief CTac output operator
///
/// @param out output stream
/// @param t reference to CTac
/// @retval output stream
ostream& operator<<(ostream &out, const CTac &t);

/// @brief CTac output operator
///
/// @param out output stream
/// @param t reference to CTac
/// @retval output stream
ostream& operator<<(ostream &out, const CTac *t);

/// @}


//------------------------------------------------------------------------------
/// @brief address class
///
/// base class for addresses (variables, constants, temporaries, array accesses)
///

class CTacAddr : public CTac {
  public:
    /// @name constructors/destructors
    /// @{

    CTacAddr(void);
    virtual ~CTacAddr(void);

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream& print(ostream &out, int indent=0) const = 0;

    /// @}
};

class CTacName : public CTacAddr {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param symbol symbol
    CTacName(const CSymbol *symbol);

    /// @}


    /// @name properties
    /// @{

    /// @brief return the symbol
    const CSymbol* GetSymbol(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream& print(ostream &out, int indent=0) const;

    /// @}

  protected:
    const CSymbol *_symbol;          ///< symbol
};

class CTacConst : public CTacAddr {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param value constant value
    CTacConst(int value);

    /// @}


    /// @name properties
    /// @{

    /// @brief return the constant value
    int GetValue(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream& print(ostream &out, int indent=0) const;

    /// @}

  protected:
    int _value;                      ///< constant value
};

class CTacTemp: public CTacName {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    ///
    /// Temp. values must be entered into the symbol table and are then
    /// accessed using their symbol.
    ///
    /// param symbol temporary value
    CTacTemp(const CSymbol *symbol);

    /// @}
};

class CTacReference: public CTacName {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    ///
    /// other than CTacName/Temp the symbol of a CTacReference is holding a
    /// reference to the storage location
    ///
    /// param symbol value holding the reference
    /// param deref  the symbol behind the reference
    CTacReference(const CSymbol *symbol, const CSymbol *deref);

    /// @}


    /// @name properties
    /// @{

    /// @brief return the symbol
    const CSymbol* GetDerefSymbol(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream& print(ostream &out, int indent=0) const;

    /// @}

  protected:
    const CSymbol *_deref;           ///< symbol this reference is pointing to
};


//------------------------------------------------------------------------------
/// @brief instruction class
///
/// base class for all instructions
///

class CTacInstr : public CTac {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param op operation
    /// @param dst destination operand
    /// @param src1 first source operand
    /// @param src2 second source operand
    CTacInstr(EOperation op, CTac *dst,
              CTacAddr *src1=NULL, CTacAddr *src2=NULL);

    /// @brief constructor (for debugging purposes)
    /// @param name descriptive string
    CTacInstr(string name);

    /// @brief destructor
    virtual ~CTacInstr(void);

    /// @}

    /// @name properties
    /// @{

    /// @brief return the unique instruction id
    unsigned int GetId(void) const;

    /// @brief returns true if this is a branching instruction
    bool IsBranch(void) const;

    /// @brief return the operation
    EOperation GetOperation(void) const;

    /// @brief return the number of source operands
    unsigned int GetNumSrc(void) const;

    /// @brief return source @a index (index = 1/2)
    CTacAddr* GetSrc(int index) const;

    /// @brief return the destination
    CTac* GetDest(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @}

  protected:
    /// @brief set the instruction @a id (unique per procedure)
    void SetId(int unsigned id);

    /// @brief set the destination operand to @a dst
    void SetDest(CTac *dst);

    unsigned int   _id;              ///< unique instruction id
    EOperation     _op;              ///< opcode
    string         _name;            ///< name (for debugging purposes)

    CTacAddr      *_src1;            ///< source operand 1
    CTacAddr      *_src2;            ///< source operand 2
    CTac          *_dst;             ///< destination operand

    friend class CCodeBlock;
};


//------------------------------------------------------------------------------
/// @brief label class
///
/// TAC class for labels
///

class CTacLabel : public CTacInstr {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param label label name
    CTacLabel(const string label);

    /// @brief destructor
    virtual ~CTacLabel(void);

    /// @}


    /// @name properties
    /// @{

    /// @brief return the label
    const string GetLabel(void) const;

    /// @brief increase/decrease the reference counter
    int AddReference(int ofs);

    /// @brief read the reference counter
    int GetRefCnt(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream& print(ostream &out, int indent=0) const;

    /// @}

  protected:
    const string _label;             ///< label
    int _refcnt;                     ///< reference counter
};


//------------------------------------------------------------------------------
/// @brief scope class
///
/// This class represents a scope
///
class CAstNode;
class CCodeBlock;

class CScope {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param ast abstract syntax tree for this scope
    /// @param parent superordinate scope, or NULL if none
    CScope(CAstNode *ast, CScope *parent=NULL);

    /// @brief destructor
    virtual ~CScope(void);

    /// @}


    /// @name properties
    /// @{

    /// @brief return the scope's name
    string GetName(void) const;

    /// @brief return a reference to the parent scope
    CScope* GetParent(void) const;

    /// @brief return a reference to the list of subscopes
    const vector<CScope*>& GetSubscopes(void) const;

    /// @brief return a reference to the symbol table
    CSymtab* GetSymbolTable(void) const;

    /// @brief return the symbol of the scope's declaration
    ///
    /// Only set for procedures/functions; a module will return null.
    virtual CSymbol* GetDeclaration(void) const = 0;

    /// @}


    /// @name block management
    /// @{

    /// @brief get the (first) code block of this scope
    CCodeBlock* GetCodeBlock(void) const;

    /// @}


    /// @name address management
    /// @{

    /// @brief create a new (unique) temporary
    /// @param type type of the temporary
    CTacTemp* CreateTemp(const CType *type);

    /// @brief create a new (unique) label
    /// @param hint optional descriptive string
    CTacLabel* CreateLabel(const char *hint=NULL);

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node ID in (dot) string format
    /// @retval string node ID as a string
    virtual string dotID(void) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    //virtual string dotAttr(void) const = 0;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}

  protected:
    CAstNode *_ast;                  ///< abstract syntax tree
    string _name;                    ///< name
    CSymtab *_symtab;                ///< symbol table
    CScope *_parent;                 ///< superordinate scope
    vector<CScope*> _children;       ///< list of functions
    CCodeBlock* _cb;                 ///< list of code blocks

    unsigned int _temp_id;           ///< next id for temporaries
    unsigned int _label_id;          ///< next id for labels
};

/// @name CScope output operators
/// @{

/// @brief CScope output operator
///
/// @param out output stream
/// @param t reference to CScope
/// @retval output stream
ostream& operator<<(ostream &out, const CScope &t);

/// @brief CScope output operator
///
/// @param out output stream
/// @param t reference to CScope
/// @retval output stream
ostream& operator<<(ostream &out, const CScope *t);

/// @}


//------------------------------------------------------------------------------
/// @brief module class
///
/// This class represents a module, including the global symbol table,
/// the code for the module body, and the functions.
///

class CModule : public CScope {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param ast abstract syntax tree (must be a CAstModule instance)
    CModule(CAstNode *ast);

    /// @brief destructor
    virtual ~CModule(void);

    /// @}

    /// @name properties
    /// @{

    /// @brief returns NULL
    virtual CSymbol* GetDeclaration(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @}
};


//------------------------------------------------------------------------------
/// @brief procedure class
///
/// This class represents a procedure/function
///

class CProcedure : public CScope {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param ast abstract syntax tree (must be a CAstProcedure instance)
    CProcedure(CAstNode *ast, CScope *parent);

    /// @brief destructor
    virtual ~CProcedure(void);

    /// @}

    /// @name properties
    /// @{

    /// @brief return the symbol of the scope's declaration
    virtual CSymbol* GetDeclaration(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @}
};


//------------------------------------------------------------------------------
/// @brief code block
///
/// base node class for code blocks
///

class CCodeBlock {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param owner scope owning this block
    CCodeBlock(CScope *owner);

    /// @brief destructor
    virtual ~CCodeBlock(void);

    /// @}


    /// @name properties
    /// @{

    /// @brief return the name of thist block
    string GetName(void) const;

    /// @brief return the owner of this block
    CScope* GetOwner(void) const;

    /// @}


    /// @name address management
    /// @{

    /// @brief create a new (unique) temporary
    /// @param type type of the temporary
    CTacTemp* CreateTemp(const CType *type);

    /// @brief create a new (unique) label
    /// @param hint optional descriptive string
    CTacLabel* CreateLabel(const char *hint=NULL);

    /// @}


    /// @name instruction management
    /// @{

    /// @brief append a new @a instr to the list of instructions
    /// @retval CTacInstr* inserted instruction
    CTacInstr* AddInstr(CTacInstr *instr);

    /// @brief return (a reference) to the list of instructions
    const list<CTacInstr*>& GetInstr(void) const;

    /// @brief remove unused/superfluous labels and goto instructions
    void CleanupControlFlow(void);

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node ID in (dot) string format
    /// @retval string node ID as a string
    virtual string dotID(void) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}

  protected:
    CScope *_owner;                  ///< block owner
    list<CTacInstr*> _ops;           ///< operation list
    unsigned int _inst_id;           ///< next id for instructions
};

/// @name CCodeBlock output operators
/// @{

/// @brief CCodeBlock output operator
///
/// @param out output stream
/// @param t reference to CCodeBlock
/// @retval output stream
ostream& operator<<(ostream &out, const CCodeBlock &t);

/// @brief CCodeBlock output operator
///
/// @param out output stream
/// @param t reference to CCodeBlock
/// @retval output stream
ostream& operator<<(ostream &out, const CCodeBlock *t);

/// @}


#endif // __SnuPL_IR_H__
