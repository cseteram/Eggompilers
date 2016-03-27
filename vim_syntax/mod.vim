" vim syntax file
" for SnuPL/1 language

if exists ("b:current_syntax")
  finish
endif

syn keyword snuDeclare      module procedure function
syn keyword snuKeyword      var return begin end
syn keyword snuCond         if then else
syn keyword snuLoop         while do

syn keyword snuType         integer boolean char
syn keyword snuBool         true false
syn keyword snuTodo         TODO FIXME NOTE contained

syn match   snuParenError   ")"
syn match   snuBrakError    "\]"

syn match   snuNumber       "\<\d\+"
syn match   snuString       "\".*\"" contains=snuEscape
syn match   snuCharacter    "\('.'\|'\\.'\)" contains=snuEscape
syn match   snuEscape       "\\." contained
syn match   snuOperator     "\(:\|<\|>\)="
syn match   snuOperator     "&&\|||"
syn match   snuOperator     "[+\-=*/!<>=#.;]"
syn match   snuComment      "//.*$" contains=snuTodo

syn cluster snuContained    contains=snuEscape,snuTodo

syn region  snuParenRegion  transparent matchgroup=snuBrace start="(" matchgroup=snuBrace end=")" contains=ALLBUT,@snuContained,snuParenError
syn region  snuBrakRegion   transparent matchgroup=snuBrace start="\[" matchgroup=snuBrace end="\]" contains=ALLBUT,@snuContained,snuBrakError


hi link snuDeclare      PreProc
hi link snuKeyword      Keyword
hi link snuCond         Keyword
hi link snuLoop         Keyword

hi link snuType         Type
hi link snuBool         Boolean
hi link snuTodo         Todo

hi link snuParenError   Error
hi link snuBrakError    Error

hi link snuNumber       String
hi link snuString       String
hi link snuCharacter    String
hi link snuEscape       Delimiter
hi link snuOperator     Keyword
hi link snuComment      Comment

hi link snuBrace        Keyword


let b:current_syntax="mod"
