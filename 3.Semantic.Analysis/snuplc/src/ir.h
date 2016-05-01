//------------------------------------------------------------------------------
/// @brief SnuPL/0 intermediate representation
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/14 Bernhard Egger adapted to SnuPL/0
/// 2013/06/06 Bernhard Egger cleanup, added documentation
/// 2014/11/04 Bernhard Egger added opPos
/// 2016/04/01 Bernhard Egger adapted to SnuPL/1
/// 2016/04/08 Bernhard Egger assignment 2: IR for SnuPL/-1
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
/// @brief address class
///
/// base class for addresses (variables, constants, temporaries, array accesses)
///
class CTacAddr {
};

class CTacLabel {
};


//------------------------------------------------------------------------------
/// @brief code block
///
/// base node class for code blocks
///

class CCodeBlock {
};


/// @}


#endif // __SnuPL_IR_H__
