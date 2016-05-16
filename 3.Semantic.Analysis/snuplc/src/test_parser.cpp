//------------------------------------------------------------------------------
/// @brief SNUPL parser test
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2014/11/04 Bernhard Egger print dot command instead of running it
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>

#include "scanner.h"
#include "parser.h"
using namespace std;

int main(int argc, char *argv[])
{
  int i = 1;

  while (i < argc) {
    CScanner *s = new CScanner(new ifstream(argv[i]));
    CParser *p = new CParser(s);

    cout << "parsing '" << argv[i] << "'..." << endl;
    CAstNode *n = p->Parse();

    if (p->HasError()) {
      const CToken *error = p->GetErrorToken();
      cout << "parse error : at " << error->GetLineNumber() << ":"
           << error->GetCharPosition() << " : "
           << p->GetErrorMessage() << endl;
    } else {
      CAstModule *m = dynamic_cast<CAstModule*>(n);
      assert(m != NULL);

      cout << "successfully parsed." << endl
           << "  AST:" << endl;
      m->print(cout, 4);
      cout << endl << endl;

      string outf = string(argv[i]) + ".ast.dot";
      ofstream out(outf.c_str());
      out << "digraph AST {" << endl
          << "  graph [fontname=\"Times New Roman\",fontsize=10];" << endl
          << "  node  [fontname=\"Courier New\",fontsize=10];" << endl
          << "  edge  [fontname=\"Times New Roman\",fontsize=10];" << endl
          << endl;
      m->toDot(out, 2);
      out << "}" << endl;
      out.flush();

      ostringstream cmd;
      cmd << "dot -Tpdf -o" << argv[i] << ".ast.pdf " << argv[i] << ".ast.dot";
      cout << "run the following command to convert the .dot file into a PDF:" << endl
           << "  " << cmd.str() << 
           endl;

      delete m;
    }

    cout << endl << endl;

    i++;
  }

  cout << "Done." << endl;

  return EXIT_SUCCESS;
}
