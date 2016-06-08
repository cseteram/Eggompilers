//------------------------------------------------------------------------------
/// @brief SnuPL/1 parser
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2016/03/09 Bernhard Egger adapted to SnuPL/1
/// 2016/04/08 Bernhard Egger assignment 2: parser for SnuPL/-1
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

#ifndef __SnuPL_PARSER_H__
#define __SnuPL_PARSER_H__

#include "scanner.h"
#include "symtab.h"
#include "ast.h"


//------------------------------------------------------------------------------
/// @brief parser
///
/// parses a module
///
class CParser {
  public:
    /// @brief constructor
    ///
    /// @param scanner  CScanner from which the input stream is read
    CParser(CScanner *scanner);

    /// @brief parse a module
    /// @retval CAstNode program node
    CAstNode* Parse(void);

    /// @name error handling
    ///@{

    /// @brief indicates whether there was an error while parsing the source
    /// @retval true if the parser detected an error
    /// @retval false otherwise
    bool HasError(void) const { return _abort; };

    /// @brief returns the token that caused the error
    /// @retval CToken containing the error token
    const CToken* GetErrorToken(void) const;

    /// @brief returns a human-readable error message
    /// @retval error message
    string GetErrorMessage(void) const;
    ///@}

  private:
    /// @brief sets the token causing a parse error along with a message
    /// @param t token causing the error
    /// @param message human-readable error message
    void SetError(CToken t, const string message);

    /// @brief consume a token given type and optionally store the token
    /// @param type expected token type
    /// @param token If not null, the consumed token is stored in 'token'
    /// @retval true if a token has been consumed
    /// @retval false otherwise
    bool Consume(EToken type, CToken *token=NULL);


    /// @brief initialize symbol table @a s with predefined procedures and
    ///        global variables
    void InitSymbolTable(CSymtab *s);

    /// @name methods for recursive-descent parsing
    /// @{
   
    /// @brief build up AST moudle scope
    /// @retval CAstModule which is created by module
    CAstModule*           module(void);

    /// @brief create and add symbols which are declared variables
    /// @param s AST scope node that variables are declared
    void                  varDeclaration(CAstScope *s);

    /// @brief store all variable's information to create/add symbols to symbol table
    /// @param vars the set of variable's name (which is appeared) is stored 'vars'
    /// @param ttype the variable's type is stored 'ttype'
    /// @param allVars already declared variables in this scope.
    ///        cannot be NULL. so use empty vector if necessary
    void                  varDecl(vector<string> &vars, CAstType* &ttype, vector<string> &allVars);

    /// @brief store all parameter's information to create/add symbols to symbol table
    /// @param vars the set of parameter's name (which is appeared) is stored 'vars'
    /// @param ttype the parameter's type is stored 'ttype'
    /// @param allVars already declared variables in this scope.
    ///        cannot be NULL. so use empty vector if necessary
    void                  varDeclParam(vector<string> &vars, CAstType* &ttype, vector<string> &allVars);

    /// @brief store all variable's information to create/add symbols to symbol table
    /// @param vars the set of variable's name (which is appeared) is stored 'vars'
    /// @param allVars already declared variables in this scope.
    ///        cannot be NULL. so use empty vector if necessary
    void                  varDeclInternal(vector<string> &vars, vector<string> &allVars);

    /// @brief build up AST procedure scope by procedure declaration
    /// @param s AST scope node which is parent of this procedure scope
    /// @retval CAstProcedure which is created by procedure
    CAstProcedure*        procedureDecl(CAstScope *s);

    /// @brief build up AST procedure scope function declaration
    /// @param s AST scope node which is parent of this function scope
    /// @retval CAstProcedure which is created by function
    CAstProcedure*        functionDecl(CAstScope *s);

    /// @brief store all parameter's information to create/add symbols to symbol table
    /// @param paramNames all parameter's name are stored 'paramNames'
    /// @param paramTypes all parameter's name are stored 'paramTypes'.
    ///        paramTypes are one-to-one correspondence with paramNames (equal index)
    void                  formalParam(vector<string> &paramNames, vector<CAstType*> &paramTypes);

    /// @brief create all parameter's symbol and add symbol to parameter and symbol table
    /// @param s AST scope node which parameters are owned
    /// @param symbol Procedure symbol which represents this procedure
    /// @param paramNames all parameter's name which is acquired from 'formalParam'
    /// @param paramTypes all parameter's type which is acquired from 'formalParam'
    void                  AddParameters(CAstScope *s, CSymProc *symbol, vector<string> &paramNames, vector<CAstType*> &paramTypes);

    /// @brief build up subroutine's body and set this to subroutine's statement sequence
    /// @param s AST scope node which owns this subroutine body
    void                  subroutineBody(CAstScope *s);

    /// @brief build up AST statement node by statement sequence
    /// @param s AST scope node which owns this statement sequence
    /// @retval CAstStatement which represents this statement sequence
    CAstStatement*        statSequence(CAstScope *s);

    /// @brief build up AST assignment statement node by assignment statement
    /// @param s AST scope node which owns this assignment
    /// @retval CAstStatAssign which represents this assignment statement
    CAstStatAssign*       assignment(CAstScope *s);

    /// @brief build up AST procedure call statement node by subroutine call (from statement)
    /// @param s AST scope node which owns this subroutine call (from statement)
    /// @retval CAstStatCall which represents this subroutine call statement (from statement)
    CAstStatCall*         subroutineCall(CAstScope *s);

    /// @brief build up AST function call node by function call (from factor)
    /// @param s AST scope node which owns this function call (from factor)
    /// @retval CAstFunctionCall which represents this function call (from factor)
    CAstFunctionCall*     functionCall(CAstScope *s);

    /// @brief build up AST if-else statement node by if-else statement
    /// @param s AST scope node which owns this if-else statement
    /// @retval CAstStatIf which represents this if-else statement
    CAstStatIf*           ifStatement(CAstScope *s);

    /// @brief build up AST while statement node by while statement
    /// @param s AST scope node which owns this while statement
    /// @retval CAstStatWhile which represents this while statement
    CAstStatWhile*        whileStatement(CAstScope *s);

    /// @brief build up AST return statement node by return statement
    /// @param s AST scope node which owns this return statement
    /// @retval CAstStatReturn which represents this return statement
    CAstStatReturn*       returnStatement(CAstScope *s);

    /// @brief build up AST expression node by expression
    /// @param s AST scope node which owns this expression
    /// @retval CAstExpression which represents this expression
    CAstExpression*       expression(CAstScope *s);

    /// @brief build up AST expression node by expression to address type casting
    /// @param s AST scope node which owns this expression -> address type casting
    /// @retval CAstExpression which represents this expression -> address type casting
    CAstExpression*       addressExpression(CAstScope *s);

    /// @brief build up AST expression node by simple expression
    /// @param s AST scope node which owns this simple expression
    /// @retval CAstExpression which represents this simple expression
    CAstExpression*       simpleexpr(CAstScope *s);

    /// @brief build up AST expression node by term
    /// @param s AST scope node which owns this term
    /// @retval CAstExpression which represents this term
    CAstExpression*       term(CAstScope *s);

    /// @brief build up AST expression node by factor
    /// @param s AST scope node which owns this factor
    /// @retval CAstExpression which represents this factor
    CAstExpression*       factor(CAstScope *s);

    /// @brief build up AST type node by given type
    /// @param isParam decide between variables and parameters
    /// @retval CAstType which represents this type value
    CAstType*             type(bool isParam);

    /// @brief build up AST designator node by qualident
    /// @param s AST scope node which owns this qualident
    /// @retval CAstDesignator which represents this qualident
    CAstDesignator*       qualident(CAstScope *s);

    /// @brief build up AST designator node by ident
    /// @param s AST scope node which owns this designator
    /// @retval CAstDesignator which represents this ident
    CAstDesignator*       ident(CAstScope *s);

    /// @brief build up AST constant operand node by given number
    /// @retval CAstConstant which represents this number
    CAstConstant*         number(void);

    /// @brief build up AST constant operand node by given boolean
    /// @retval CAstConstant which represents this boolean value
    CAstConstant*         boolean(void);

    /// @brief build up AST constant operand node by given character
    /// @retval CAstConstant which represents this character value
    CAstConstant*         character(void);

    /// @brief build up AST string constant operand node by given string
    /// @param s AST scope node which owns this string constant
    /// @retval CAstStringConstant which represents this string
    CAstStringConstant*   stringConst(CAstScope *s);

    /// @}


    CScanner     *_scanner;       ///< CScanner instance
    CAstModule   *_module;        ///< root node of the program
    CToken        _token;         ///< current token

    /// @name error handling
    CToken        _error_token;   ///< error token
    string        _message;       ///< error message
    bool          _abort;         ///< error flag

};

#endif // __SnuPL_PARSER_H__
