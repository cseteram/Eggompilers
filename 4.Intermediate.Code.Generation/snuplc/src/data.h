//------------------------------------------------------------------------------
/// @brief SnuPL data initializers
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2016/04/05 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2016, Bernhard Egger
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

#ifndef __SnuPL_DATA_H__
#define __SnuPL_DATA_H__

#include <iostream>
#include <map>
#include <vector>

#include "type.h"
using namespace std;

//------------------------------------------------------------------------------
/// @brief SnuPL data initializer
///
/// base class for all data initializers
///
class CDataInitializer {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    CDataInitializer(void);

    /// @brief destructor
    virtual ~CDataInitializer(void);

    /// @}

    /// @brief print the data initializer to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const = 0;

  private:
};

/// @name CDataInitializer output operators
/// @{

/// @brief CDataInitializer output operator
///
/// @param out output stream
/// @param t reference to CDataInitializer
/// @retval output stream
ostream& operator<<(ostream &out, const CDataInitializer &t);

/// @brief CDataInitializer output operator
///
/// @param out output stream
/// @param t reference to CDataInitializer
/// @retval output stream
ostream& operator<<(ostream &out, const CDataInitializer *t);

/// @}


//------------------------------------------------------------------------------
/// @brief string data initializer
///
/// class representing string data initializers
///
class CDataInitString : public CDataInitializer {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CDataInitString(const string data);

    /// @}

    /// @name data access
    /// @{

    /// @brief get the string data
    /// @retval string string data
    string GetData(void) const;

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    const string   _data;         ///< string data
};


#endif // __SnuPL_DATA_H__
