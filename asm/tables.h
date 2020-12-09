#include "lex.h"

#define ENUM_STR_ENTRY(e) [e] = #e

char* tok_to_str[] =
{
	ENUM_STR_ENTRY(TOK_ERROR),
	ENUM_STR_ENTRY(TOK_EOL),
	ENUM_STR_ENTRY(TOK_EOF),
	ENUM_STR_ENTRY(TOK_COMMA),
	ENUM_STR_ENTRY(TOK_PLUS),
	ENUM_STR_ENTRY(TOK_MINUS),
	ENUM_STR_ENTRY(TOK_OPEN_PAREN),
	ENUM_STR_ENTRY(TOK_CLOSE_PAREN),
	ENUM_STR_ENTRY(TOK_UNDEFINED),
	ENUM_STR_ENTRY(TOK_LABEL),
	ENUM_STR_ENTRY(TOK_DIRECTIVE),
	ENUM_STR_ENTRY(TOK_LITERAL),
	ENUM_STR_ENTRY(TOK_IDENTIFIER),
	ENUM_STR_ENTRY(TOK_RELOC_FUNC),
};

char* err_to_str[] =
{
	ENUM_STR_ENTRY(ERR_FILE_INPUT),
	ENUM_STR_ENTRY(ERR_UNEXPECTED_CHAR),
	ENUM_STR_ENTRY(ERR_INT_OVERFLOW),
};

char* lit_to_str[] =
{
	ENUM_STR_ENTRY(LIT_DEC),
	ENUM_STR_ENTRY(LIT_HEX),
	ENUM_STR_ENTRY(LIT_BIN),
	ENUM_STR_ENTRY(LIT_CHR),
	ENUM_STR_ENTRY(LIT_STR),
};

typedef enum
{
	DIR_UNDEFINED = -1,
	DIR_ALIGN, DIR_SECTION, DIR_TEXT, DIR_DATA, DIR_RODATA, DIR_BSS,
	DIR_STRING, DIR_ASCIZ, DIR_ASCII,
	DIR_EQU, DIR_OPTION, DIR_BYTE,
	DIR_2BYTE, DIR_HALF, DIR_SHORT,
	DIR_4BYTE, DIR_WORD, DIR_QUAD,
	DIR_ZERO, DIR_SPACE,
	DIR_ORG,
	DIR_LC, // Location Counter

	DIR_COUNT, // Not a real directive
} Directives_t;

const char directives[][16] =
{
	[DIR_ALIGN] = ".align",
	[DIR_SECTION] = ".section",
	[DIR_TEXT] = ".text",
	[DIR_DATA] = ".data",
	[DIR_RODATA] = ".rodata",
	[DIR_BSS] = ".bss",
	[DIR_STRING] = ".string",
	[DIR_ASCIZ] = ".asciz",
	[DIR_ASCII] = ".ascii",
	[DIR_EQU] = ".equ",
	[DIR_OPTION] = ".option",
	[DIR_BYTE] = ".byte",
	[DIR_2BYTE] = ".2byte",
	[DIR_HALF] = ".half",
	[DIR_SHORT] = ".short",
	[DIR_4BYTE] = ".4byte",
	[DIR_WORD] = ".word",
	[DIR_QUAD] = ".quad",
	[DIR_ZERO] = ".zero",
	[DIR_SPACE] = ".space",
	[DIR_ORG] = ".org",
	[DIR_LC] = ".",
};

typedef enum
{
	INST_UNDEFINED = -1,
	INST_LUI, INST_AUIPC, INST_JAL, INST_JALR, INST_BEQ, INST_BNE, INST_BLT,
	INST_BGE, INST_BLTU, INST_BGEU, INST_LB, INST_LH, INST_LW, INST_LBU, INST_LHU,
	INST_SB, INST_SH, INST_SW, INST_ADDI, INST_SLTI, INST_SLTIU, INST_XORI,
	INST_ORI, INST_ANDI, INST_SLLI, INST_SRLI, INST_SRAI, INST_ADD, INST_SUB,
	INST_SLL, INST_SLT, INST_SLTU, INST_XOR, INST_SRL, INST_SRA, INST_OR, INST_AND,
	INST_FENCE, INST_ECALL, INST_EBREAK,

	INST_COUNT,
	PINST_START,

	PINST_LA, PINST_LB, PINST_LH, PINST_LW, PINST_SB, PINST_SH, PINST_SW,
	PINST_NOP, PINST_LI, PINST_MV, PINST_NOT, PINST_NEG, PINST_SEQZ, PINST_SNEZ,
	PINST_SLTZ, PINST_SGTZ, PINST_BEQZ, PINST_BNEZ, PINST_BLEZ, PINST_BGEZ,
	PINST_BLTZ, PINST_BGTZ, PINST_BGT, PINST_BLE, PINST_BGTU, PINST_BLEU,
	PINST_J, PINST_JAL, PINST_JR, PINST_JALR, PINST_RET, PINST_CALL, PINST_TAIL,
	PINST_FENCE,

	PINST_COUNT,
} Instructions_t;

const char  instructions[][8] =
{
	[INST_LUI] = "lui",
	[INST_AUIPC] = "auipc",
	[INST_JAL] = "jal",
	[INST_JALR] = "jalr",
	[INST_BEQ] = "beq",
	[INST_BNE] = "bne",
	[INST_BLT] = "blt",
	[INST_BGE] = "bge",
	[INST_BLTU] = "bltu",
	[INST_BGEU] = "bgeu",
	[INST_LB] = "lb",
	[INST_LH] = "lh",
	[INST_LW] = "lw",
	[INST_LBU] = "lbu",
	[INST_LHU] = "lhu",
	[INST_SB] = "sb",
	[INST_SH] = "sh",
	[INST_SW] = "sw",
	[INST_ADDI] = "addi",
	[INST_SLTI] = "slti",
	[INST_SLTIU] = "sltiu",
	[INST_XORI] = "xori",
	[INST_ORI] = "ori",
	[INST_ANDI] = "andi",
	[INST_SLLI] = "slli",
	[INST_SRLI] = "srli",
	[INST_SRAI] = "srai",
	[INST_ADD] = "add",
	[INST_SUB] = "sub",
	[INST_SLL] = "sll",
	[INST_SLT] = "slt",
	[INST_SLTU] = "sltu",
	[INST_XOR] = "xor",
	[INST_SRL] = "srl",
	[INST_SRA] = "sra",
	[INST_OR] = "or",
	[INST_AND] = "and",
	[INST_FENCE] = "fence",
	[INST_ECALL] = "ecall",
	[INST_EBREAK] = "ebreak",

	[PINST_LA] = "la",
	[PINST_LB] = "lb",
	[PINST_LH] = "lh",
	[PINST_LW] = "lw",
	[PINST_SB] = "sb",
	[PINST_SH] = "sh",
	[PINST_SW] = "sw",
	[PINST_NOP] = "nop",
	[PINST_LI] = "li",
	[PINST_MV] = "mv",
	[PINST_NOT] = "not",
	[PINST_NEG] = "neg",
	[PINST_SEQZ] = "seqz",
	[PINST_SNEZ] = "snez",
	[PINST_SLTZ] = "sltz",
	[PINST_SGTZ] = "sgtz",
	[PINST_BEQZ] = "beqz",
	[PINST_BNEZ] = "bnez",
	[PINST_BLEZ] = "blez",
	[PINST_BGEZ] = "bgez",
	[PINST_BLTZ] = "bltz",
	[PINST_BGTZ] = "bgtz",
	[PINST_BGT] = "bgt",
	[PINST_BLE] = "ble",
	[PINST_BGTU] = "bgtu",
	[PINST_BLEU] = "bleu",
	[PINST_J] = "j",
	[PINST_JAL] = "jal",
	[PINST_JR] = "jr",
	[PINST_JALR] = "jalr",
	[PINST_RET] = "ret",
	[PINST_CALL] = "call",
	[PINST_TAIL] = "tail",
	[PINST_FENCE] = "fence",

};
