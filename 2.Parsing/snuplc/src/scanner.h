//------------------------------------------------------------------------------
/// @brief SnuPL/1 scanner
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2014/09/10 Bernhard Egger assignment 1: scans SnuPL/-1
/// 2016/03/13 Bernhard Egger assignment 1: adapted to modified SnuPL/-1 syntax
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

#ifndef __SnuPL1_SCANNER_H__
#define __SnuPL1_SCANNER_H__

#include <istream>
#include <ostream>
#include <iomanip>
#include <map>

using namespace std;

//------------------------------------------------------------------------------
/// @brief SnuPL/1 token type
///
/// each member of this enumeration represents a token in SnuPL/1
///
enum EToken {
  kModule = 0,                      ///< module
  kBegin,                           ///< begin
  kEnd,                             ///< end
  kType,                            ///< boolean or char or integer
  kBool,                            ///< true or false
  kIf,                              ///< if
  kThen,                            ///< then
  kElse,                            ///< else
  kWhile,                           ///< while
  kDo,                              ///< do
  kReturn,                          ///< return
  kVar,                             ///< var
  kProc,                            ///< procedure
  kFunc,                            ///< function

  tIdent,                           ///< an identifier
  tNumber,                          ///< a number
  tChar,                            ///< a character
  tString,                          ///< a string
  tPlusMinus,                       ///< '+' or '-'
  tMulDiv,                          ///< '*' or '/'
  tAndOr,                           ///< '&&' or '||'
  tNot,                             ///< a not operator
  tRelOp,                           ///< relational operator
  tAssign,                          ///< assignment operator
  tSemicolon,                       ///< a semicolon
  tColon,                           ///< a colon
  tComma,                           ///< a comma
  tDot,                             ///< a dot
  tLBrak,                           ///< a left bracket
  tRBrak,                           ///< a right bracket
  tLParen,                          ///< a left paren
  tRParen,                          ///< a right paren

  tEOF,                             ///< end of file
  tIOError,                         ///< I/O error
  tUndefined,                       ///< undefined
};


//------------------------------------------------------------------------------
/// @brief token
///
/// used to represent a token. Each token has a type (EToken), a value for
/// tokens that in fact subsume a number of terminals, and the exact position
/// in the input stream (line/column).
///
class CToken {
  friend class CScanner;
  public:
    /// @name constructors
    /// @{

    /// @brief default constructor
    CToken();

    /// @brief constructor taking initialization values
    ///
    /// @param line line number in the input stream
    /// @param charpos character position in the input stream
    /// @param type token type
    /// @param value token value
    CToken(int line, int charpos, EToken type, const string value="");

    /// @brief copy contructor
    ///
    /// @param token token to copy
    CToken(const CToken &token);

    /// @brief copy contructor
    ///
    /// @param token token to copy
    CToken(const CToken *token);
    /// @}

    /// @name token attributes
    /// @{

    /// @brief return the name for a given token
    ///
    /// @retval token name
    static const string Name(EToken type);

    /// @brief return the token name
    ///
    /// @retval token name
    const string GetName(void) const;

    /// @brief return the token type
    ///
    /// @retval token type
    EToken GetType(void) const { return _type; };

    /// @brief return the token value
    ///
    /// @retval token value
    string GetValue(void) const { return _value; };
    /// @}

    /// @name stream attributes
    /// @{

    /// @brief return the line number
    ///
    /// @retval line number of the token in the input stream
    int GetLineNumber(void) const { return _line; };

    /// @brief return the character position
    ///
    /// @retval character position of the token in the input stream
    int GetCharPosition(void) const { return _char; };

    /// @}


    /// @brief print the token to an output stream
    ///
    /// @param out output stream
    ostream&  print(ostream &out) const;

    /// @brief escape special characters in a string
    ///
    /// @param text string
    /// @retval escaped string
    static string escape(const string text);

    /// @brief unescape special characters in a string
    ///
    /// @param text escaped string
    /// @retval unescaped string
    static string unescape(const string text);

  private:
    EToken _type;                   ///< token type
    string _value;                  ///< token value
    int    _line;                   ///< input stream position (line)
    int    _char;                   ///< input stream position (character pos)
};

/// @name CToken output operators
/// @{

/// @brief CToken output operator
///
/// @param out output stream
/// @param d reference to CToken
/// @retval output stream
ostream& operator<<(ostream &out, const CToken &t);

/// @brief CToken output operator
///
/// @param out output stream
/// @param d reference to CToken
/// @retval output stream
ostream& operator<<(ostream &out, const CToken *t);

/// @}


//------------------------------------------------------------------------------
/// @brief scanner
///
/// used by CParser to scan (tokenize) SnuPL/1 code
///
class CScanner {
  public:
    /// @name construction/destruction
    /// @{

    /// @brief constructor
    ///
    /// @param in input stream containing the source code
    CScanner(istream *in);

    /// @brief constructor
    ///
    /// @param in input stream containing the source code
    CScanner(string in);

    /// @brief destructor
    ~CScanner();

    /// @}

    /// @brief return and remove the next token from the input stream
    ///
    /// @retval token token
    CToken Get(void);

    /// @brief peek at the next token in the input stream (without removing it)
    ///
    /// @retval token token
    CToken Peek(void) const;

    /// @brief check the status of the scanner
    ///
    /// @retval true if the scanner is in an operating (i.e., normal) state
    /// @retval false if an error has occurred
    bool Good(void) const { return _good; };

    /// @brief get the line number of the next token in the input stream
    ///
    /// @retval line number
    int GetLineNumber(void) const { return _line; };

    /// @brief get the character position of the next token in the input stream
    ///
    /// @retval character position
    int GetCharPosition() const { return _char; };

  private:
    /// @brief initialize list of reserved keywords
    void InitKeywords(void);

    /// @brief scan the next token
    void NextToken(void);

    /// @brief store the current position of the input stream internally
    void RecordStreamPosition();

    /// @brief return the previously recorded input stream position
    ///
    /// @param lineno line number
    /// @param charpos character position
    void GetRecordedStreamPosition(int *lineno, int *charpos);

    /// @brief create and return a new token
    ///
    /// @param type token type
    /// @param token  token value
    /// @retval CToken instance
    CToken* NewToken(EToken type, const string token="");


    /// @name low-level scanner routines
    /// @{

    /// @brief scan the input stream and return the next token
    ///
    /// @retval CToken instance
    CToken* Scan(void);

    /// @brief return the next character from the input stream
    ///
    /// @retval next character in the input stream
    char GetChar(void);

    /// @brief return the next 'n' characters from the input stream
    ///
    /// @param n number of characters to read
    /// @retval string containing the characters read
    string GetChar(int n);

    /// @brief check whether a character is a white character or comment
    ///
    /// @retval true character is white space or comment
    /// @retval false character is not white space or comment
    bool OnRemove(void);

    /// @brief delete one line from the input stream
    void DeleteLine(void);

    /// @brief check if a character is a white character
    ///
    /// @param c character
    /// @retval true character is white space
    /// @retval false character is not white space
    bool IsWhite(char c) const;

    /// @brief check if two characters are representing comment line
    ///
    /// @param c character
    /// @retval true characters are comment
    /// @retval false characters are not comment
    bool IsComment(char c);

    /// @brief scan istream until meet character '\'' and make char token
    ///
    /// @param token token type reference
    /// @param tokval token attribute reference
    void ScanChar(EToken& token, string& tokval);

    /// @brief scan istream until meet character '\"' and make string token
    ///
    /// @param token token type reference
    /// @param tokval token attribute reference
    void ScanString(EToken& token, string& tokval);
    
    /// @brief trim quotation mark from string
    ///
    /// @param tokval string with quotation mark
    void TrimQuotation(string &tokval);

    /// @brief check if a character is printable ASCII character
    ///
    /// @param c character
    /// @retval true character is a printable ASCII character
    /// @retval false character is not a printable ASCII character
    bool IsAsciiChar(char c) const;
    
    /// @brief check if a character is a letter
    ///
    /// @param c character
    /// @retval true character is a letter
    /// @retval false character is not a digit
    bool IsLetter(char c) const;
    
    /// @brief check if a character is a digit
    ///
    /// @param c character
    /// @retval true character is a digit
    /// @retval false character is not a digit
    bool IsDigit(char c) const;

    /// @}


  private:
    static map<string, EToken> keywords;///< reserved keywords with corr. tokens
    istream *_in;                   ///< input stream
    bool    _delete_in;             ///< delete input stream upon destruction
    bool    _good;                  ///< scanner status flag
    int     _line;                  ///< current stream position (line)
    int     _char;                  ///< current stream position (character pos)
    int     _saved_line;            ///< saved stream position (line)
    int     _saved_char;            ///< saved stream position (character pos)
    CToken *_token;                 ///< next token in input stream
};


#endif // __SnuPL0_SCANNER_H__
