module string;

function foo:
// valid input

 "Hello, world!\n"
 // comment
 "a-zA-Z_\n \t \\ \' \""
 "123" // not a number 123
 "01234//56789_asdav wFC // SAD Df s FS"
 "'/'"
 "'\''"

   "  -*:=<>#_:;,.$@%^&()\\"
  " "
""
"'" "\"\"" "''" "\n" "\t" " "
    "// not a comment" // white spaces "This is not a string"
		"string with \tab" ""

"function"
"module"


// invalid input

"안녕, 세상!\n"
"１" "２""３４５６７"
"＜"
"Ｎｏｔ　ａ　 ｓｔｒｉｎｇ ．\n"
"Nｏｔ　ａ　 ｓｔｒｉｎｇ"
"\nＮｏｔ　ａ　 ｓｔｒｉｎｇ"
"""   
   // comment
"'_\" if (a <= 3) then WriteLine(str); else return a + 5 end \"
          		// comment	"
"Open string
