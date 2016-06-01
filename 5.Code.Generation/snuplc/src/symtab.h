//------------------------------------------------------------------------------
/// @brief SnuPL symbol table
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
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

#ifndef __SnuPL_SYMTAB_H__
#define __SnuPL_SYMTAB_H__

#include <iostream>
#include <map>
#include <vector>

#include "data.h"
#include "type.h"
using namespace std;

//------------------------------------------------------------------------------
/// @brief SnuPL symbol types
///
enum ESymbolType {
  stGlobal,         ///< global symbol
  stLocal,          ///< local symbol
  stParam,          ///< parameter symbol
  stProcedure,      ///< procedure symbol
};

class CSymtab;

//------------------------------------------------------------------------------
/// @brief SnuPL symbol
///
/// base class for all symbols
///
class CSymbol {
  friend class CSymtab;

  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param symboltype symbol type
    /// @param datatype data type of the symbol
    CSymbol(const string name, ESymbolType symboltype, const CType *datatype);

    /// @brief destructor
    virtual ~CSymbol(void);

    /// @}

    /// @name symbol handling
    /// @{

    /// @brief return the symbol's identifier
    /// @retval string name
    string GetName(void) const;

    /// @brief return the symbol's type
    /// @retval ESymbolType symbol type
    ESymbolType GetSymbolType(void) const;

    /// @brief set the symbol's type
    /// @param datatype symbol type
    void SetDataType(const CType *datatype);

    /// @brief return the symbol's type
    /// @retval CType symbol type
    const CType* GetDataType(void) const;

    /// @brief return the symbol table of this symbol
    /// @retval CSymtab symbol table
    CSymtab* GetSymbolTable(void) const;

    /// @}

    /// @name data handling
    /// @{

    /// @brief set the symbol's value (for initialized symbols)
    /// @param data data initializer
    virtual void SetData(const CDataInitializer *data);

    /// @brief return the symbol's data initializer
    /// @retval CDataInitializer data initializer
    virtual const CDataInitializer* GetData(void) const;

    /// @}

    /// @name memory location
    /// @{

    /// @brief set the base register to access this symbol
    void SetBaseRegister(string rbase);

    /// @brief set the offset to access this symbol
    void SetOffset(int offset);

    /// @brief get the base register to access this symbol
    string GetBaseRegister(void) const;

    /// @brief get the offset to access this symbol
    int GetOffset(void) const;

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    /// @name symbol handling
    /// @{

    /// @brief set the symbol table owning this symbol
    /// @param symtab symbol table
    void SetSymbolTable(CSymtab *symtab);

    /// @}

    CSymtab       *_symtab;       ///< symbol table owning this symbol
    string         _name;         ///< name
    ESymbolType    _symboltype;   ///< symbol type
    const CType   *_datatype;     ///< data type
    const CDataInitializer *_data;///< data initializer

    string         _rbase;        ///< base register
    int            _offset;       ///< offset
};

/// @name CSymbol output operators
/// @{

/// @brief CSymbol output operator
///
/// @param out output stream
/// @param s reference to CSymbol
/// @retval output stream
ostream& operator<<(ostream &out, const CSymbol &s);

/// @brief CSymbol output operator
///
/// @param out output stream
/// @param s reference to CSymbol
/// @retval output stream
ostream& operator<<(ostream &out, const CSymbol *s);

/// @}


//------------------------------------------------------------------------------
/// @brief global variable
///
/// class representing global variables
///
class CSymGlobal : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CSymGlobal(const string name, const CType *type);

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
};


//------------------------------------------------------------------------------
/// @brief local variable
///
/// class representing local (i.e., stack-allocated) variables
///
class CSymLocal : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CSymLocal(const string name, const CType *type);

    /// @}

    /// @name property handling
    /// @{

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  protected:
    /// @name constructor/destructor
    /// @{

    /// @brief private constructor invoked by CSymParam
    ///
    /// @param name symbol name (identifier)
    /// @param stype symbol type
    /// @param type data type
    CSymLocal(const string name, ESymbolType stype, const CType *type);

    /// @}
};


//------------------------------------------------------------------------------
/// @brief procedure parameter
///
/// class representing procedure/function parameters
///
class CSymParam : public CSymLocal {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param index parameter index (starting at 0)
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CSymParam(int index, const string name, const CType *type);

    /// @}

    /// @name property handling
    /// @{

    /// @brief return the index of the paramter
    int GetIndex(void) const;

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    int            _index;        ///< index
};


//------------------------------------------------------------------------------
/// @brief procedure symbol
///
/// class representing a procedure/function symbol
///
class CSymProc : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name procedure/function identifier
    /// @param return_type return type (NULL if none)
    CSymProc(const string name, const CType *return_type);

    /// @}

    /// @name property handling
    /// @{

    /// @brief add a parameter
    /// @param param parameter
    void AddParam(CSymParam *param);

    /// @brief return the number of parameters
    /// @retval int number of parameters
    int GetNParams(void) const;

    /// @brief return the @a index-th parameters
    /// @retval CSymParam* parameter
    const CSymParam* GetParam(int index) const;

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    vector<CSymParam*> _param;      ///< parameter list
};


//------------------------------------------------------------------------------
/// @brief search scope enumeration
///
enum EScope {
  sLocal,                         ///< local
  sGlobal,                        ///< global
};

//------------------------------------------------------------------------------
/// @brief SnuPL symbol table
///
/// hierarchical symbol table
///
class CSymtab {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor for top-level symbol table
    CSymtab(void);

    /// @brief constructor for subordinate symbol tables
    /// @param parent superordinate symbol table (must not be NULL)
    CSymtab(CSymtab *parent);

    /// @brief destructor
    virtual ~CSymtab(void);

    ///@}

    /// @name symbol handling
    /// @{

    /// @brief add a symbol to the symbol table
    /// @retval true if the symbol was inserted successfully
    /// @retval false if such a symbol already exists in the local symbol table
    bool AddSymbol(CSymbol *s);

    /// @brief return a symbol with a given name
    /// @param name symbol name (identifier)
    /// @param scope search scope (default: sGlobal)
    /// @retval CSymbol matching symbol or NULL if not found
    const CSymbol* FindSymbol(const string name, EScope scope=sGlobal) const;

    /// @brief return a list of all symbols
    vector<CSymbol*> GetSymbols(void) const;

    /// @}

    /// @brief print the symbol table to an output stream
    ///
    /// @param out output stream
    /// @param indent indentation
    ostream&  print(ostream &out, int indent=0) const;

  private:
    map<string, CSymbol*> _symtab;///< local symbol table
    CSymtab       *_parent;       ///< parent
};

/// @name CSymtab output operators
/// @{

/// @brief CSymtab output operator
///
/// @param out output stream
/// @param d reference to CSymtab
/// @retval output stream
ostream& operator<<(ostream &out, const CSymtab &t);

/// @brief CSymtab output operator
///
/// @param out output stream
/// @param d reference to CSymtab
/// @retval output stream
ostream& operator<<(ostream &out, const CSymtab *t);

/// @}


#endif // __SnuPL_SYMTAB_H__
