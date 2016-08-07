#ifndef _PARSE_H_
#define _PARSE_H_

/*
GRAMMAR:
	<prog>                   -> <line> { <prog> }
	<line>                   -> <labeled_instruction>
	                          | <instruction>
	<labeled_instruction>    -> <label> COLON NEWLINE <instruction>
	<label>                  -> IDENT
	<instruction>            -> <zero_instruction>
	                          | <unary_dest_instruction>
	                          | <unary_any_instruction>
	                          | <binary_instruction>
	                          | <cmp_instruction>
	                          | <jmp_instruction>
	<zero_instruction>       -> NOP
	                          | HALT
	<unary_dest_instruction> -> <unary_dest_name> <dest_arg>
	<unary_dest_name>        -> CLR
	                          | INC
						      | DEC
	<dest_arg>               -> <register>
	                          | <memory>
	<register>               ->  REGISTER
	<memory>                 -> LPAREN <register> RPAREN
	<unary_any_instruction>  -> <unary_any_name> <arg>
	<unary_any_name>         -> FRM
	                          | DIS
	<arg>                    -> <arg_inner>
	                          | <arg_inner> PLUS <arg_inner>
	<arg_inner>              -> <register>
	                          | <memory>
							  | <literal>
	<literal>                -> <hex>
	                          | <decimal>
	<hex>                    -> HEXI
	<decimal>                -> DECI
	<binary_instruction>     -> <binary_instr_name> <dst_arg> COMMA <arg>
	<binary_instr_name>      -> ALM
	                          | ADD
	                          | SUB
	                          | MUL
	                          | DIV
	                          | MOV
	                          | AND
	                          | OR
	                          | NOT
	                          | XOR
	<cmp_instruction>        -> <cmp_instr_name> <arg> COMMA <arg>
	<cmp_instr_name>         -> CMP
	<jmp_instruction>        -> <jmp_instr_name> <label>
	<jmp_instr_name>         -> JEQ
	                          | JGT
	                          | JLT
	                          | JLE
	                          | JGE

TOKENS:
	IDENT    -> [_a-zA-Z][_a-zA-Z0-9]*
		#As long as it doesn't match an instruction, a register, or a decimal
	NOP      -> nop
	HALT     -> halt
	CLR      -> clr
	INC      -> inc
	DEC      -> dec
	REGISTER -> R[1-8] #This is just hard-coded - the "machine" has 8 registers
	LPAREN   -> (
	RPAREN   -> )
	FRM      -> frm
	DIS      -> dis
	PLUS     -> +
	HEXI     -> #[0-9A-F]{4}
	DECI     -> \$[0-9]{5} #Such that its value <= 65535
	COMMA    -> ,
	ALM      -> alm
	ADD      -> add
	SUB      -> sub
	MUL      -> mul
	DIV      -> div
	MOV      -> mov
	AND      -> and
	OR       -> or
	NOT      -> not
	XOR      -> xor
	CMP      -> cmp
	JEQ      -> jeq
	JGT      -> jgt
	JLT      -> jlt
	JLE      -> jle
	JGE      -> jge
*/

#endif
