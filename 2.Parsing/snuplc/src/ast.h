//------------------------------------------------------------------------------
/// @brief SnuPL abstract syntax tree
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/05/22 Bernhard Egger reimplemented TAC generation
/// 2016/03/12 Bernhard Egger adapted to SnuPL/1
/// 2016/04/08 Bernhard Egger assignment 2: AST for SnuPL/-1
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

#ifndef __SnuPL_AST_H__
#define __SnuPL_AST_H__

#include <istream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

#include "scanner.h"
#include "type.h"
#include "symtab.h"
#include "ir.h"
using namespace std;

class CAstStatement;
class CAstExpression;
class CAstFunctionCall;
class CAstConstant;

//------------------------------------------------------------------------------
/// @brief AST base node
///
/// base node class for all node types in the AST
///

class CAstNode {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param token token in input stream (used for error reporting purposes)
    CAstNode(CToken token);
    virtual ~CAstNode(void);

    /// @}

    /// @name properties
    /// @{

    /// @brief return the ID of this node
    int GetID(void) const;

    /// @brief return the token associated with this node
    CToken GetToken(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief return (compute) the type of this node
    virtual const CType* GetType(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const = 0;

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

    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* GetTacAddr(void) const;

    /// @}

  private:
    CToken     _token;              ///< token in input stream that triggered
                                    ///< the creation of the node. Used for
                                    ///< error reporting purposes)
    int        _id;                 ///< id of the node
    static int _global_id;          ///< holds the (global) next id

  protected:
    CTacAddr   *_addr;              ///< result of this node in three-address
                                    ///< code (only set after calling ToTac())
};

/// @name CAstNode output operators
/// @{

/// @brief CAstNode output operator
///
/// @param out output stream
/// @param t reference to CAstNode
/// @retval output stream
ostream& operator<<(ostream &out, const CAstNode &t);

/// @brief CAstNode output operator
///
/// @param out output stream
/// @param t reference to CAstNode
/// @retval output stream
ostream& operator<<(ostream &out, const CAstNode *t);

/// @}

//------------------------------------------------------------------------------
/// @brief AST scope node
///
/// node representing a scope
///

class CAstScope : public CAstNode {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param t token in input stream (used for error reporting purposes)
    /// @param name scope name
    /// @param parent superordinate scope, or NULL if none
    CAstScope(CToken t, const string name, CAstScope *parent=NULL);

    /// @brief destructor
    virtual ~CAstScope(void);

    /// @}

    /// @name scope manipulation/querying
    /// @{

    /// @brief return the type instance
    const string GetName(void) const;

    /// @brief return the superordinate scope
    CAstScope* GetParent(void) const;

    /// @brief return the number of subordinate scopes
    size_t GetNumChildren(void) const;

    /// @brief return the @a i-th subordinate scope
    CAstScope* GetChild(size_t i) const;

    /// @brief get the symbol table for this scope
    CSymtab* GetSymbolTable(void) const;

    /// @brief create a new variable on this scope's level
    virtual CSymbol* CreateVar(const string ident, const CType *type) = 0;

    /// @brief set the statement sequence
    void SetStatementSequence(CAstStatement *statement);

    /// @brief get the statement sequence
    CAstStatement* GetStatementSequence(void) const;

    /// @}

    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);

    virtual CCodeBlock* GetCodeBlock(void) const;

    /// @}

  protected:
    /// @brief set the symbol table for this scope
    void SetSymbolTable(CSymtab *st);


  private:
    /// @brief register a subordinate scope
    /// @param child subordinate scope to add
    void AddChild(CAstScope *child);

    string    _name;                ///< name
    CSymtab   *_symtab;             ///< symbol table
    CAstScope *_parent;             ///< superordinate scope
    CAstStatement* _statseq;        ///< statement sequence
    vector<CAstScope*> _children;   ///< subordinate scopes
    CCodeBlock *_cb;                ///< (entry) code block for this scope
};


//------------------------------------------------------------------------------
/// @brief AST module scope
///
/// node representing a global module scope
///

class CAstModule : public CAstScope {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param t token in input stream (used for error reporting purposes)
    /// @param name module name
    CAstModule(CToken t, const string name);

    /// @}

    /// @name scope manipulation/querying
    /// @{

    /// @brief create a new variable on this scope's level
    virtual CSymbol* CreateVar(const string ident, const CType *type);

    /// @}

    /// @name output
    /// @{

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @}
};


//------------------------------------------------------------------------------
/// @brief AST procedure scope
///
/// node representing a procedure/function scope
///

class CAstProcedure : public CAstScope {
  public:
    /// @name constructors/destructors
    /// @{

    /// @brief constructor
    /// @param t token in input stream (used for error reporting purposes)
    /// @param name procedure/function name
    /// @param parent superordinate scope (cannot be NULL)
    /// @param symbol symbol representing the procedure/function
    CAstProcedure(CToken t, const string name,
                  CAstScope *parent, CSymProc *symbol);

    /// @}

    /// @name scope manipulation/querying
    /// @{

    /// @brief return the symbol for this procedure/function
    /// @retval CSymProc* symbol representing this procedure/function
    CSymProc* GetSymbol(void) const;

    /// @brief create a new variable on this scope's level
    virtual CSymbol* CreateVar(const string ident, const CType *type);

    /// @}

    /// @name type management
    /// @{

    /// @brief return (compute) the type of this node
    virtual const CType* GetType(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @}

  private:
    CSymProc *_symbol;              ///< corresponding symbol
};


//------------------------------------------------------------------------------
/// @brief AST type node
///
/// node representing a type
///

class CAstType : public CAstNode {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param type CType instance
    CAstType(CToken t, const CType *type);

    /// @}

    /// @brief return the type instance
    const CType* GetType(void) const;

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @}

  private:
    const CType *_type;             ///< type
};


//------------------------------------------------------------------------------
/// @brief AST statement node
///
/// node representing a statement
///

class CAstStatement : public CAstNode {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param token token in input stream (used for error reporting purposes)
    CAstStatement(CToken token);
    virtual ~CAstStatement(void);

    /// @}

    /// @name statement list management
    /// @{

    /// @brief set the next @a statement
    void SetNext(CAstStatement *next);

    /// @brief get the next @a statement
    CAstStatement* GetNext(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const = 0;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *next);

    /// @}


  private:
    CAstStatement* _next;           ///< next statement
};


//------------------------------------------------------------------------------
/// @brief AST assignment statement node
///
/// node representing an assignment statement
///

class CAstStatAssign : public CAstStatement {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param lhs left-hand side of assignment (designator)
    /// @param rhs right-hand side of assignment (expression)
    CAstStatAssign(CToken t, CAstConstant *lhs, CAstExpression *rhs);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the LHS of the assignment
    /// @retval CAstDesignator* LHS of the assignment
    CAstConstant* GetLHS(void) const;

    /// @brief return the RHS of the assignment
    /// @retval CAstExpression* RHS of the assignment
    CAstExpression* GetRHS(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the expression.
    virtual const CType* GetType(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *next);

    /// @}


  private:
    CAstConstant   *_lhs;           ///< LHS (designator)
    CAstExpression *_rhs;           ///< RHS (expression)
};


//------------------------------------------------------------------------------
/// @brief AST procedure call statement node
///
/// node representing a procedure call statement
///

class CAstStatCall : public CAstStatement {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param call function call node
    CAstStatCall(CToken t, CAstFunctionCall *call);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the expression representing the call
    /// @retval CAstFunctionCall* call expression
    CAstFunctionCall* GetCall(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

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


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *next);

    /// @}

  private:
    CAstFunctionCall *_call;        ///< call expression
};


//------------------------------------------------------------------------------
/// @brief AST return statement node
///
/// node representing a return statement
///

class CAstStatReturn : public CAstStatement {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param s enclosing scope (needed for type checking)
    /// @param expr returned expression (or NULL)
    CAstStatReturn(CToken t, CAstScope *scope, CAstExpression *expr);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the enclosing scope
    /// @retval CAstScope* enclosing scope
    CAstScope* GetScope(void) const;

    /// @brief return the expression
    /// @retval CAstExpression* return expression
    CAstExpression* GetExpression(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the expression.
    virtual const CType* GetType(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *next);

    /// @}

  private:
    CAstScope      *_scope;         ///< enclosing scope
    CAstExpression *_expr;          ///< return expression
};


//------------------------------------------------------------------------------
/// @brief AST if-else statement node
///
/// node representing a if-else statement
///

class CAstStatIf : public CAstStatement {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param cond if-else condition (expression)
    /// @param ifBody statement list of if-body
    /// @param elseBody statement list of else-body
    CAstStatIf(CToken t, CAstExpression *cond,
               CAstStatement *ifBody, CAstStatement *elseBody);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the if condition
    /// @retval CAstExpression* if condition
    CAstExpression* GetCondition(void) const;

    /// @brief return the if-body
    /// @retval CAstStatement* if-body statement sequence
    CAstStatement* GetIfBody(void) const;

    /// @brief return the else-body
    /// @retval CAstStatement* else-body statement sequence
    CAstStatement* GetElseBody(void) const;

    /// @}

    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *next);

    /// @}

  private:
    CAstExpression *_cond;          ///< condition
    CAstStatement *_ifBody;         ///< if body
    CAstStatement *_elseBody;       ///< else body
};


//------------------------------------------------------------------------------
/// @brief AST while statement node
///
/// node representing a while statement
///

class CAstStatWhile : public CAstStatement {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param cond while condition (expression)
    /// @param body statement list of body
    CAstStatWhile(CToken t, CAstExpression *cond, CAstStatement *body);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the if condition
    /// @retval CAstExpression* if condition
    CAstExpression* GetCondition(void) const;

    /// @brief return the body
    /// @retval CAstStatement* body statement sequence
    CAstStatement* GetBody(void) const;

    /// @}

    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *next);

    /// @}

  private:
    CAstExpression *_cond;          ///< condition
    CAstStatement *_body;           ///< body
};


//------------------------------------------------------------------------------
/// @brief AST expression node
///
/// node representing an expression
///

class CAstExpression : public CAstNode {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param token token in input stream (used for error reporting purposes)
    CAstExpression(CToken t);

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const = 0;

    /// @brief return (compute) the type of the expression.
    virtual const CType* GetType(void) const = 0;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}
};


//------------------------------------------------------------------------------
/// @brief AST operation node
///
/// node representing an operation
///

class CAstOperation : public CAstExpression {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param o operation
    CAstOperation(CToken t, EOperation o);

    /// @}

    /// @brief return the operation
    /// @retval EOperation operation
    EOperation GetOperation(void) const;

  private:
    EOperation _oper;               ///< operation
};


//------------------------------------------------------------------------------
/// @brief AST binary operation node
///
/// node representing a binary operation
///

class CAstBinaryOp : public CAstOperation {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param o (binary) operation
    /// @param l left operand
    /// @param r right operand
    CAstBinaryOp(CToken t, EOperation o, CAstExpression *l, CAstExpression *r);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the left operand expression
    /// @retval CAstExpression* left operand expression
    CAstExpression* GetLeft(void) const;

    /// @brief return the right operand expression
    /// @retval CAstExpression* right operand expression
    CAstExpression* GetRight(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the expression.
    virtual const CType* GetType(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}

  private:
    CAstExpression *_left;          ///< left operand
    CAstExpression *_right;         ///< right operand
};


//------------------------------------------------------------------------------
/// @brief AST unary operation node
///
/// node representing a unary operation
///

class CAstUnaryOp : public CAstOperation {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param o (unary) operation
    /// @param e operand
    CAstUnaryOp(CToken t, EOperation o, CAstExpression *e);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the operand expression
    /// @retval CAstExpression* operand expression
    CAstExpression* GetOperand(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the expression.
    virtual const CType* GetType(void) const;

    /// @}

    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}


  private:
    CAstExpression *_operand;       ///< operand
};


//------------------------------------------------------------------------------
/// @brief AST special operation node
///
/// node representing special operations related to (de-)referencing pointers
///

class CAstSpecialOp : public CAstOperation {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param o (unary) operation
    /// @param e operand
    /// @param type type (ignored except for opCast operations)
    CAstSpecialOp(CToken t, EOperation o, CAstExpression *e,
                  const CType *type=NULL);

    /// @}


    /// @name property manipulation
    /// @{

    /// @brief return the operand expression
    /// @retval CAstExpression* operand expression
    CAstExpression* GetOperand(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the expression.
    virtual const CType* GetType(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);

    /// @}


  protected:
    CAstExpression *_operand;       ///< operand
    const CType *_type;             ///< forced type of this op
};

//------------------------------------------------------------------------------
/// @brief AST function call node
///
/// node representing a function call
///

class CAstFunctionCall : public CAstExpression {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param symbol symbol of function to call
    CAstFunctionCall(CToken t, const CSymProc *symbol);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the associated symbol
    const CSymProc* GetSymbol(void) const;

    /// @brief add an argument
    /// @param arg argument
    void AddArg(CAstExpression *arg);

    /// @brief return the number of arguments
    int GetNArgs(void) const;

    /// @brief return the @a index -th argument
    CAstExpression* GetArg(int index) const;

    /// @}

    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return the return type of the call
    virtual const CType* GetType(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}


  private:
    const CSymProc *_symbol;        ///< symbol
    vector<CAstExpression*> _arg;   ///< parameter list
};


//------------------------------------------------------------------------------
/// @brief AST operand node
///
/// node representing an operand
///

class CAstOperand : public CAstExpression {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param token token in input stream (used for error reporting purposes)
    CAstOperand(CToken token);

    /// @}
};


//------------------------------------------------------------------------------
/// @brief AST designator
///
/// node representing a variable operand
///

class CAstDesignator : public CAstOperand {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param symbol variable symbol
    CAstDesignator(CToken t, const CSymbol *symbol);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief return the associated symbol
    const CSymbol* GetSymbol(void) const;

    /// @}

    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the designator.
    virtual const CType* GetType(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}


  protected:
    const CSymbol *_symbol;         ///< symbol
};


//------------------------------------------------------------------------------
/// @brief AST array designator
///
/// node representing an array operand
///

class CAstArrayDesignator : public CAstDesignator {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param symbol variable symbol
    CAstArrayDesignator(CToken t, const CSymbol *symbol);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief add an index
    /// @param idx index expression
    void AddIndex(CAstExpression *idx);

    /// @brief signal that all indices have been added
    ///
    /// This method must be called after all indices for all dimensions
    /// have been added. As a result, the AST computing the address offset
    /// for the access is generated.
    /// This function must only be called once, and no more indices can be
    /// added after calling IndicesComplete().
    void IndicesComplete(void);

    /// @brief return the number of arguments
    int GetNIndices(void) const;

    /// @brief return the @a index-th index expression
    CAstExpression* GetIndex(int index) const;

    /// @}
    /// @}

    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the designator.
    virtual const CType* GetType(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @brief print the node in dot format to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual void toDot(ostream &out, int indent=0) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}


  private:
    bool _done;                     ///< flag indicating all index expressions
                                    ///< have been added
    vector<CAstExpression*> _idx;   ///< index expressions
    CAstExpression *_offset;        ///< address computation expression
};


//------------------------------------------------------------------------------
/// @brief AST constant operand node
///
/// node representing a numerical constant operand node
///

class CAstConstant : public CAstOperand {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param type type of the constant
    /// @param value constant value
    CAstConstant(CToken t, const CType *type, long long value);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief set the constant value
    void SetValue(long long value);

    /// @brief return the constant value
    long long GetValue(void) const;

    /// @brief return the constant value as a string
    string GetValueStr(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the constant
    virtual const CType* GetType(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}


  private:
    const CType *_type;             ///< constant type
    long long _value;               ///< constant value
};


//------------------------------------------------------------------------------
/// @brief AST string constant operand node
///
/// node representing a string constant operand node
///

class CAstStringConstant : public CAstOperand {
  public:
    /// @name constructors/destructors
    /// @{

    /// @param t token in input stream (used for error reporting purposes)
    /// @param value constant value
    /// @param s enclosing scope
    CAstStringConstant(CToken t, const string value, CAstScope *s);

    /// @}

    /// @name property manipulation
    /// @{

    /// @brief set the constant value
    //void SetValue(const string value);

    /// @brief return the constant value
    const string GetValue(void) const;

    /// @brief return the constant value as a string
    const string GetValueStr(void) const;

    /// @}


    /// @name type management
    /// @{

    /// @brief perform type checking
    /// @param t (out, optional) type error at token t
    /// @param msg (out, optional) type error message
    /// @retval true if no type error has been found
    /// @retval false otherwise
    virtual bool TypeCheck(CToken *t, string *msg) const;

    /// @brief return (compute) the type of the constant
    virtual const CType* GetType(void) const;

    /// @}


    /// @name output
    /// @{

    /// @brief print the node to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

    /// @brief return the node's attributes in (dot) string format
    /// @retval string node attributes as a string
    virtual string dotAttr(void) const;

    /// @}


    /// @name transformation into TAC
    /// @{

    virtual CTacAddr* ToTac(CCodeBlock *cb);
    virtual CTacAddr* ToTac(CCodeBlock *cb, CTacLabel *ltrue,CTacLabel *lfalse);

    /// @}


  private:
    static int       _idx;          ///< static counter
    const CType     *_type;         ///< constant type
    CDataInitString *_value;        ///< data initializer (holds string data)
    CSymGlobal      *_sym;          ///< symbol holding the string
};


#endif // __SnuPL_AST_H__
