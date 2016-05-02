
type var =
  | Vmodule
  | Vboolean
  | Vtype
  | Vbasetype
  | Vqualident
  | VfactOp
  | VtermOp
  | VrelOp
  | Vfactor
  | Vterm
  | Vsimpleexpr
  | Vexpression
  | Vassignment
  | VsubroutineCall
  | VifStatement
  | VwhileStatement
  | VreturnStatement
  | Vstatement
  | VstatSequence
  | VvarDeclaration
  | VvarDeclSequence
  | VvarDecl
  | VsubroutineDecl
  | VprocedureDecl
  | VfunctionDecl
  | VformalParam
  | VsubroutineBody

let string_of_var v =
  match v with
  | Vmodule -> "module"
  | Vboolean -> "boolean"
  | Vtype -> "type"
  | Vbasetype -> "basetype"
  | Vqualident -> "qualident"
  | VfactOp -> "factOp"
  | VtermOp -> "termOp"
  | VrelOp -> "relOp"
  | Vfactor -> "factor"
  | Vterm -> "term"
  | Vsimpleexpr -> "simpleexpr"
  | Vexpression -> "expression"
  | Vassignment -> "assignment"
  | VsubroutineCall -> "subroutineCall"
  | VifStatement -> "ifStatement"
  | VwhileStatement -> "whileStatement"
  | VreturnStatement -> "returnStatement"
  | Vstatement -> "statement"
  | VstatSequence -> "statSequence"
  | VvarDeclaration -> "varDeclaration"
  | VvarDeclSequence -> "varDeclSequence"
  | VvarDecl -> "varDecl"
  | VsubroutineDecl -> "subroutineDecl"
  | VprocedureDecl -> "procedureDecl"
  | VfunctionDecl -> "functionDecl"
  | VformalParam -> "formalParam"
  | VsubroutineBody -> "subroutineBody"

type rule =
  | L of rule list
  | O of rule list
  | N of rule
  | S of rule
  | V of var
  | T of string

let apply_rule : var -> rule =
  fun lhs ->
    match lhs with
    | Vmodule ->
        L [
          T "module";
          T "tIdent";
          T ";";
          V VvarDeclaration;
          S (V VsubroutineDecl);
          T "begin";
          V VstatSequence;
          T "end";
          T "tIdent";
          T "."
        ]
    | Vboolean ->
        O [
          T "true";
          T "false"
        ]
    | Vtype ->
        O [
          V Vbasetype;
          L [
            V Vtype;
            T "[";
            T "tNumber";
            T "]"
          ]
        ]
    | Vbasetype ->
        O [
          T "boolean";
          T "char";
          T "integer"
        ]
    | Vqualident ->
        L [
          T "tIdent";
          S (
            L [
              T "[";
               V Vexpression;
               T "]"
            ]
          )
        ]
    | VfactOp ->
        O [
          T "*";
          T "/";
          T "&&"
        ]
    | VtermOp ->
        O [
          T "+";
          T "-";
          T "||"
        ]
    | VrelOp ->
        O [
          T "=";
          T "#";
          T "<";
          T "<=";
          T ">";
          T ">="
        ]
    | Vfactor ->
        O [
          V Vqualident;
          T "tNumber";
          V Vboolean;
          T "tChar";
          T "tString";
          L [
            T "(";
            V Vexpression;
            T ")"
          ];
          V VsubroutineCall;
          L [
            T "!";
            V Vfactor
          ]
        ]
    | Vterm ->
        L [
          V Vfactor;
          S (
            L [
              V VfactOp;
              V Vfactor
            ]
          )
        ]
    | Vsimpleexpr ->
        L [
          N (
            O [
              T "+";
              T "-"
            ]
          );
          V Vterm;
          S (
            L [
              V VtermOp;
              V Vterm
            ]
          )
        ]
    | Vexpression ->
        L [
          V Vsimpleexpr;
          N (
            L [
              V VrelOp;
              V Vsimpleexpr
            ]
          )
        ]
    | Vassignment ->
        L [
          V Vqualident;
          T ":=";
          V Vexpression
        ]
    | VsubroutineCall ->
        L [
          T "tIdent";
          T "(";
          N (
            L [
              V Vexpression;
              S (
                L [
                  T ",";
                  V Vexpression
                ]
              )
            ]
          );
          T ")"
        ]
    | VifStatement ->
        L [
          T "if";
          T "(";
          V Vexpression;
          T ")";
          T "then";
          V VstatSequence;
          N (
            L [
              T "else";
              V VstatSequence
            ]
          );
          T "end"
        ]
    | VwhileStatement ->
        L [
          T "while";
          T "(";
          V Vexpression;
          T ")";
          T "do";
          V VstatSequence;
          T "end"
        ]
    | VreturnStatement ->
        L [
          T "return";
          N (V Vexpression)
        ]
    | Vstatement ->
        O [
          V Vassignment;
          V VsubroutineCall;
          V VifStatement;
          V VwhileStatement;
          V VreturnStatement
        ]
    | VstatSequence ->
        N (
          L [
            V Vstatement;
            S (
              L [
                T ";";
                V Vstatement
              ]
            )
          ]
        )
    | VvarDeclaration ->
        N (
          L [
            T "var";
            V VvarDeclSequence;
            T ";"
          ]
        )
    | VvarDeclSequence ->
        L [
          V VvarDecl;
          S (
            L [
              T ";";
              V VvarDecl
            ]
          )
        ]
    | VvarDecl ->
        L [
          T "tIdent";
          S (
            L [
              T ",";
              T "tIdent"
            ]
          );
          T ":";
          V Vtype
        ]
    | VsubroutineDecl ->
        L [
          O [
            V VprocedureDecl;
            V VfunctionDecl
          ];
          V VsubroutineBody;
          T "tIdent";
          T ";"
        ]
    | VprocedureDecl ->
        L [
          T "procedure";
          T "tIdent";
          N (V VformalParam);
          T ";"
        ]
    | VfunctionDecl ->
        L [
          T "function";
          T "tIdent";
          N (V VformalParam);
          T ":"; V Vtype; T ";"
        ]
    | VformalParam ->
        L [
          T "(";
          N (V VvarDeclSequence);
          T ")"
        ]
    | VsubroutineBody ->
        L [
          V VvarDeclaration;
          T "begin";
          V VstatSequence;
          T "end"
        ]

let vars_list = [
  Vmodule;
  Vboolean;
  Vtype;
  Vbasetype;
  Vqualident;
  VfactOp;
  VtermOp;
  VrelOp;
  Vfactor;
  Vterm;
  Vsimpleexpr;
  Vexpression;
  Vassignment;
  VsubroutineCall;
  VifStatement;
  VwhileStatement;
  VreturnStatement;
  Vstatement;
  VstatSequence;
  VvarDeclaration;
  VvarDeclSequence;
  VvarDecl;
  VsubroutineDecl;
  VprocedureDecl;
  VfunctionDecl;
  VformalParam;
  VsubroutineBody
]

let terms_list = [
  "module";
  "begin";
  "end";
  "true";
  "false";
  "boolean";
  "char";
  "integer";
  "if";
  "then";
  "else";
  "while";
  "do";
  "return";
  "var";
  "procedure";
  "function";

  "tIdent";
  "tNumber";
  "tChar";
  "tString";

  "+"; "-"; "*"; "/";
  "&&"; "||"; "!";
  ":=";
  "="; "#"; "<"; "<="; ">"; ">=";
  ";"; ":";
  ";"; ".";
  "("; ")";
  "["; "]";

  ""; "tEOF"
]

let rec remove_dup l =
  match l with
  | h::t ->
      if List.mem h t
      then remove_dup t
      else h::(remove_dup t)
  | [] -> []

let emptyEnv : var -> string list =
  fun x -> []

let env_first = ref emptyEnv
let env_follow = ref emptyEnv

let cnt_first = ref 0
let cnt_follow = ref 0

let bind : (var -> string list) -> var -> string list -> (var -> string list) =
  fun env x l ->
    fun y -> if x = y then l else env y

let add_first : var -> string list -> unit =
  fun x l ->
    let ol = !env_first x in
    let nl = remove_dup (l @ ol) in
    if not (List.length nl = List.length ol)
    then
      let _ = env_first := bind !env_first x nl in
      cnt_first := !cnt_first + 1
    else ()

let add_follow : var -> string list -> unit =
  fun x l ->
    let ol = !env_follow x in
    let nl = remove_dup (l @ ol) in
    if not (List.length nl = List.length ol)
    then
      let _ = env_follow := bind !env_follow x nl in
      cnt_follow := !cnt_follow + 1
    else ()

let first x =
  let rec first_iter x prev_list =
    match x with
    | L l ->
        begin match l with
          | x::t ->
              let hd_list = first_iter x prev_list in
              if List.mem "" hd_list
              then
                let l1 = (List.filter (fun id -> id <> "") hd_list) in
                let l2 = (first_iter (L t) prev_list) in
                remove_dup (l1 @ l2)
              else hd_list
          | [] -> [""]
        end
    | O l ->
        let merge_list =
          fun acc_list x -> acc_list @ (first_iter x prev_list)
        in
        remove_dup (List.fold_left merge_list [] l)
    | N x | S x -> remove_dup (""::(first_iter x prev_list))
    | V v ->
        let result =
          if not (List.mem v prev_list)
          then first_iter (apply_rule v) (v::prev_list)
          else !env_first v
        in
        let _ = add_first v result in
        result
    | T s -> [s]
  in
  first_iter x []

let follow x =
  let rec follow_iter x lhs =
    match x with
    | L l ->
        begin match l with
        | x::t ->
            begin match x with
            | L l -> follow_iter (L (l @ t)) lhs
            | O l -> List.iter (fun x -> follow_iter (L (x::t)) lhs) l
            | N x -> follow_iter (O [L t; L (x::t)]) lhs
            | S x -> follow_iter (O [L t; L (x::t); L (x::x::t)]) lhs
            | V v ->
                if t = []
                then add_follow v (!env_follow lhs)
                else
                  let result = first (L t) in
                  let result_without_eps =
                    if List.mem "" result
                    then
                      let _ = add_follow v (!env_follow lhs) in
                      List.filter (fun id -> id <> "") result
                    else result
                  in
                  let _ = add_follow v result_without_eps in
                  follow_iter (L t) lhs
            | T s -> follow_iter (L t) lhs
            end
        | [] -> ()
        end
    | O l -> List.iter (fun x -> follow_iter x lhs) l
    | N x -> follow_iter x lhs
    | S x -> follow_iter (O [x; L [x; x]]) lhs
    | V v ->
        if v = lhs
        then follow_iter (apply_rule v) lhs
        else add_follow v (!env_follow lhs)
    | T s -> ()
  in
  match x with (V v) -> follow_iter x v | _ -> ()

let rec calc_first () =
  let cnt = !cnt_first in
  let _ = List.iter (fun v -> let _ = first (V v) in ()) vars_list in
  if cnt < !cnt_first then calc_first () else ()

let rec calc_follow () =
  let cnt = !cnt_follow in
  let _ = add_follow Vmodule ["tEOF"] in
  let _ = List.iter (fun v -> follow (V v)) vars_list in
  if cnt < !cnt_follow then calc_follow () else ()

let print_first () =
  List.iter
    (fun v ->
      print_string ("FIRST(" ^ string_of_var v ^ ") =");
      List.iter
        (fun s -> print_string (" [" ^ s ^ "]"))
        (!env_first v);
      print_string "\n"
    )
    vars_list;
  print_string "\n"

let print_follow () =
  List.iter
    (fun v ->
      print_string ("FOLLOW(" ^ string_of_var v ^ ") =");
      List.iter
        (fun s -> print_string (" [" ^ s ^ "]"))
        (!env_follow v);
      print_string "\n"
    )
    vars_list;
  print_string "\n"

let reset () =
  let _ = env_first := emptyEnv in
  let _ = env_follow := emptyEnv in
  let _ = cnt_first := 0 in
  let _ = cnt_follow := 0 in
  let _ = calc_first () in
  calc_follow ()

let _ = reset ()
let _ = print_first ()
let _ = print_follow ()
