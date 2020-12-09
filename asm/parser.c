

int32 parse_directive(char *dir)
{
	for(int32 i = 0; i < DIR_COUNT; ++i)
	{
		if(!strcmp(dir, directives[i]))
			return i;
	}
	return DIR_UNDEFINED;
}

int32 parse_instruction(char *inst)
{
	for(int32 i = 0; i < INST_COUNT; ++i)
	{
		if(!strcmp(inst, instructions[i]))
			return i;
	}
	for(int32 i = PINST_START+1; i < PINST_COUNT; ++i)
	{
		if(!strcmp(inst, instructions[i]))
			return i;
	}
	return INST_UNDEFINED;
}


#if 0
if(!strcmp(current_token.name, ".align"))
{
	next_token(&current_token, input_file);
	if(current_token.type == TOK_LITERAL)
	{
		switch(current_token.value)
		{
		case 2:
			location_counter = ALIGN2(location_counter);
			break;
		case 4;
		location_counter = ALIGN4(location_counter);
		break;
		case 8:
			location_counter = ALIGN8(location_counter);
			break;
		default:
			panic("parser error: wrong .align argument");
		}
	}
	else
	{
		panic("parser error: wrong .align argument");
	}
}
#endif
