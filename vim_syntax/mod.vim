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
syn keyword snuTODO         TODO FIXME NOTE contained

syn match   snuParenError   ")"
syn match   snuBrakError    "\]"
syn match   snuNumber       "[-+]\?\d\+"
syn match   snuString       "\".*\"" contains=snuEscape
syn match   snuCharacter    "\('\p'\|'\\\p'\)" contains=snuEscape
syn match   snuEscape       "\\\p" contained
syn match   snuOperator     "\(:\|<\|>\)="
syn match   snuOperator     "&&\|||"
syn match   snuOperator     "[+=*/!<>=#,.;:]"
syn match   snuComment      "//.*$" contains=snuTODO

syn region  snuParen start="(" end=")" fold transparent contains=snuPreProc,snuKeyword,snuCond,snuLoop,snuType,snuBool,snuComment,snuBrakError,snuNumber,snuString,snuCharacter,snuOperator,snuParen,SnuBrak
syn region  snuBrak start="\[" end="\]" fold transparent contains=snuPreProc,snuKeyword,snuCond,snuLoop,snuType,snuBool,snuComment,snuParenError,snuNumber,snuString,snuCharacter,snuOperator,snuParen,SnuBrak


hi link snuDeclare      Macro
hi link snuKeyword      Keyword
hi link snuCond         Keyword
hi link snuLoop         Keyword

hi link snuType         Type
hi link snuBool         Boolean
hi link snuTODO         Todo

hi link snuParenError   Error
hi link snuBrakError    Error
hi link snuComment      Comment
hi link snuNumber       String
hi link snuString       String
hi link snuCharacter    String
hi link snuEscape       Delimiter
hi link snuOperator     Keyword


let b:current_syntax="mod"
