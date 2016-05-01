//------------------------------------------------------------------------------
/// @brief SnuPL intermediate representation
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

ostream& operator<<(ostream &out, EOperation t)
{
  out << EOperationName[t];
  return out;
}
