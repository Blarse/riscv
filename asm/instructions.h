
typedef enum InstType
{
	R_TYPE, I_TYPE, S_TYPE, B_TYPE, U_TYPE, J_TYPE
} InstType;

typedef enum InstInputFormat
{
	IN_RD = 1,
	IN_RS1 = 2,
	IN_RS2 = 4,
	IN_IMM = 8,
	IN_RT = 16,
} InstInputFormat;

typedef struct InstTemplate
{
	struct InstTemplate *next;
	union
	{
		struct rtype
		{
			uint32 opcode : 7;
			uint32 rd     : 5;
			uint32 funct3 : 3;
			uint32 rs1    : 5;
			uint32 rs2    : 5;
			uint32 funct7 : 7;
		} rtype;
		struct itype
		{
			uint32 opcode : 7;
			uint32 rd     : 5;
			uint32 funct3 : 3;
			uint32 rs1    : 5;
			uint32 imm12  : 12;
		} itype;
		struct stype
		{
			uint32 opcode : 7;
			uint32 imm5   : 5;
			uint32 funct3 : 3;
			uint32 rs1	  : 5;
			uint32 rs2	  : 5;
			uint32 imm7   : 7;
		} stype;
		struct btype
		{
			uint32 opcode : 7;
			uint32 imm5   : 5;
			uint32 funct3 : 3;
			uint32 rs1    : 5;
			uint32 rs2    : 5;
			uint32 imm7   : 7;
		} btype;
		struct utype
		{
			uint32 opcode : 7;
			uint32 rd     : 5;
			uint32 imm20  : 20;
		} utype;
		struct jtype
		{
			uint32 opcode : 7;
			uint32 rd     : 5;
			uint32 imm20  : 20;
		} jtype;
	};

} InstTemplate;

typedef struct Inst
{
	enum Instructions alt;
	InstType type;
	InstInputFormat format;
	InstTemplate template;

	char name[8];
	//IDEA: Dynamic array of templates
	//InstTemplate templates[]
} Inst;

//InstTemplate pinst_sw_2 = {.next = NULL, .};

Inst i2[] =
{
	[INST_LUI] = {.alt = INST_UNDEFINED, .type = U_TYPE,
		.format = (IN_RD | IN_IMM), .name = "lui",
		.template = {.next = NULL, .utype = {.opcode = 0b0110111}}},

	[INST_SW] = {.alt = PINST_SW, .type = S_TYPE,
		.format = (IN_RS1 | IN_RS2 | IN_IMM), .name = "sw",
		.template = {.next = NULL, .stype = {.opcode = 0b0100011, .funct3 = 0b010}}},

	[PINST_SW] = {.alt = INST_SW, .type = U_TYPE,
		.format = (IN_RD | IN_IMM | IN_RT), .name = "sw",
		.template = {.next = &i2[INST_SW].template, .utype = {.opcode = 0b0010111}}},

	[PINST_NOP] = {.alt = INST_UNDEFINED, .type = I_TYPE, .format = 0, .name = "nop",
		.template = {.next = NULL, .itype = {.opcode = 0b0010011, .rd = 0,
						   .funct3 = 0, .rs1 = 0, .imm12 = 0}}},
};



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
